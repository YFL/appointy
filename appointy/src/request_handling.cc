#include <request_handling.h>

#include <iostream>

#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <appointy_exception.h>
#include <choice_answer.h>
#include <numeric_answer.h>
#include <json_parser.h>

namespace appointy
{

using mongocxx::uri;
using mongocxx::client;
using mongocxx::collection;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::document;

template <typename Collection, typename T>
auto add(Collection collection, const T &init)
{
    if(collection.begin() == collection.end())
    {
        return init;
    }
    T begin = *collection.begin();
    Collection new_collection {collection.begin() + 1, collection.end()};

    return add(new_collection, init + begin);
}

auto get_service_from_db(const std::string &service_id, const std::string &connection_string, const std::string &db_name)
{
    auto uri = mongocxx::uri {connection_string};
    auto client = mongocxx::client {uri};
    auto services_collection = client[db_name]["Services"];

    auto filter = document {} << "_id" << bsoncxx::oid(service_id) << finalize;

    auto service_doc_optional = services_collection.find_one(filter.view());
    if(!service_doc_optional)
    {
        throw Exception {"The service couldn't be found by the given id " + service_id};
    }

    return JSON_Parser::parse_service(nlohmann::json::parse(bsoncxx::to_json(service_doc_optional.value().view())));
}

auto duration_of_config(const Service &service, const std::vector<std::shared_ptr<Answer>> &answers)
{
    auto durations = std::vector<Time> {};
    for(auto it = answers.begin(); it != answers.end(); it++)
    {
        auto &answer = *it;

        auto answer_signature = std::shared_ptr<AnswerSignature> {nullptr};

        for(auto &question : service.questions)
        {
            if(question.answer_signature->id == answer->answer_signature_id)
            {
                answer_signature = question.answer_signature;
                break;
            }
        }

        if(!answer_signature)
        {
            throw Exception {"Couldn't find the answer signature based on the asnwer signature id: " + answer->answer_signature_id};
        }

        if(answer->answer_type == AnswerType::CHOICE)
        {
            auto a = dynamic_cast<ChoiceAnswer &>(*answer);
            auto as_cast = dynamic_cast<ChoiceAnswerSignature &>(*answer_signature);
            for(int id : a.ids)
            {
                for(auto &option : as_cast.options)
                {
                    if(option.id == id)
                    {
                        durations.push_back(option.duration);
                    }
                }
            }
        }
        else if(answer->answer_type == AnswerType::INT)
        {
            auto as_cast = dynamic_cast<NumericAnswerSignature<int> &>(*answer_signature);
            auto a = dynamic_cast<NumericAnswer<int> &>(*answer);
            durations.push_back(as_cast.duration * a.number);
        }
        else
        {
            auto as_cast = dynamic_cast<NumericAnswerSignature<double> &>(*answer_signature);
            auto a = dynamic_cast<NumericAnswer<double> &>(*answer);
            durations.push_back(as_cast.duration * a.number);
        }
    }

    return add(durations, service.duration);
}

auto accept_estimated_duration_request(const DurationRequest &r, const std::string &connection_string, const std::string &db_name) -> Time
{
    auto service = get_service_from_db(r.service_id, connection_string, db_name);

    // TODO correction based on historical completion times
    
    return duration_of_config(service, r.configuration);
}

auto accept_appointment_request(const AppointmentRequest &r, const std::string &connection_string, const std::string &db_name) -> std::vector<AppointmentOffer>
{
    auto uri = mongocxx::uri {connection_string};
    auto client = mongocxx::client {uri};
    auto services_collection = client[db_name]["Services"];
    auto appointments_collection = client[db_name]["Appointments"];

    if(r.interval_start > r.interval_end)
    {
        throw Exception {"The interval's start is bigger then it's end"};
    }

    // collecting the durations
    auto total_duration = accept_estimated_duration_request({r.service_id, r.answers}, connection_string, db_name);
    
    if(r.interval_end - r.interval_start < total_duration)
    {
        throw Exception {"The requested interval is smaller then the completion time of this configuration"};
    }

    auto filter = document {} << "$and" << open_array 
        << open_document << "date" 
            << open_document
                << "$gte" << r.first_date.date()
                << "$lte" << r.last_date.date()
            << close_document
        << close_document
        << open_document << "start"
            << open_document
                << "$lte" << r.interval_end.time()
            << close_document
        << close_document
        << open_document << "end"
            << open_document
                << "$gte" << r.interval_start.time()
            << close_document
        << close_document
    << close_array << finalize;
            

    auto appointments = std::vector<Appointment> {};

    auto cursor = appointments_collection.find(filter.view(), mongocxx::options::find().sort(document {} << "date" << 1 << "start_time" << 1 << finalize));

    for(auto it = cursor.begin(); it != cursor.end(); it++)
    {
        appointments.push_back(JSON_Parser::parse_appointment(nlohmann::json::parse(bsoncxx::to_json(*it))));
    }

    auto gaps = std::vector<AppointmentOffer> {};
    
    if(appointments.empty())
    {
        int days = r.last_date.days_in_year() - r.first_date.days_in_year();
        gaps.push_back(AppointmentOffer {r.first_date, r.interval_start, r.interval_end - r.interval_start, r});
        for(int i = 1; i < days; i++)
        {
            // it seems to me, that the following TODO line is not reasonable.
            // reasoning: The requested time is not the same as the completion time of the requested service.
            // So if the gap is as big as the requested time, than there can be no more gaps following it, because
            // the client can not get the service after the requested time.
            //
            // So the problem with the todo is, that we don't return a gap of the size of the requested service's
            // completion time, but the size of the requested interval, yet the todo thinks it's the other way around
            // TODO not all gaps added. If the gap ends before the requested end time and there is enough time to add another gap, than we shall do it.
            gaps.push_back(AppointmentOffer {gaps[i-1].date++, r.interval_start, r.interval_end - r.interval_start, r});
        }

        return gaps;
    }
    
    // check whether there is enough time before the first 
    // booked appointment's start from the requested interval's
    // start IF the first booked appointment is starting
    // later then the requested interval
    if(appointments.front().start > r.interval_start)
    {
        if(r.interval_start - appointments.front().start >= total_duration)
        {
            gaps.push_back(AppointmentOffer {appointments.front().date, r.interval_start, total_duration, r});
        }
    }

    auto i = 0u;
    while(i < appointments.size() - 1)
    {
        auto current_date = appointments[i].date;
        while(appointments[i].date == current_date && i < appointments.size() - 1)
        {
            if(appointments[i + 1].start - appointments[i].end >= total_duration)
            {
                gaps.push_back(AppointmentOffer {current_date, appointments[i].end, total_duration, r});
            }
            i++;
        }
    }

    if(appointments.back().end < r.interval_end && r.interval_end - appointments.back().end >= total_duration)
    {
        gaps.push_back(AppointmentOffer {appointments.back().date, appointments.back().end, r.interval_end - appointments.back().end, r});
    }

    auto date = appointments.back().date++;

    // The + 1 means: add the subtracted day back (f.e.: 31 - 11 = 20, but if we wann include 11 too, then it changes to 31 - 10)
    // the above numbers are days of january, so past days in the given year (including the current day)
    for(auto i = int {0}; i < r.last_date.days_in_year() - date.days_in_year() + 1; i++)
    {
        gaps.push_back(AppointmentOffer {date + i, r.interval_start, r.interval_end - r.interval_start, r});
    }

    return gaps;
}

auto book_appointment(const Appointment &appointment, const std::string &db_connection_string, const std::string &db_name) -> bool
{
    mongocxx::uri uri {db_connection_string};
    mongocxx::client client {uri};

    mongocxx::collection collection {client[db_name]["Appointments"]};

    auto appointment_offers = accept_appointment_request(appointment.request, db_connection_string, db_name);

    for(auto &offer : appointment_offers)
    {
        if(offer.date == appointment.date && offer.start <= appointment.start && offer.start + offer.duration >= appointment.end)
        {
            bsoncxx::document::view_or_value document {bsoncxx::from_json(appointment.to_json().dump())};

            auto result = collection.insert_one(document);

            return result.value().result().inserted_count() == 1 ? true : false;
        }
    }

    throw Exception {"Other appointments booked in that time interval"};
}

} // namespace appointy