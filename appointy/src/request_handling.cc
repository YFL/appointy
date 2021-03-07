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

auto accept_appointment_request(const AppointmentRequest &r, const std::string &connection_string, const std::string &db_name) -> std::vector<AppointmentOffer>
{
    using nlohmann::json;
    mongocxx::uri uri {connection_string};
    mongocxx::client client {uri};
    mongocxx::collection services_collection {client[db_name]["Services"]};
    mongocxx::collection appointments_collection {client[db_name]["Appointments"]};
    // std::string query = "SELECT `date`, start_time, finish_time FROM Appointments WHERE `date` >= $first AND `date` <= $last_date AND start_time <= $interval_end AND finish_time >= $interval_start ORDER BY `date` ASC, start_time ASC;";

    document filter;
    auto filter_value = filter << "$and" << open_array 
        << open_document << "date" 
            << open_document
                << "$gte" << r.first_date.date()
                << "$lte" << r.last_date.date()
            << close_document
        << close_document
        << open_document << "start_time"
            << open_document
                << "$lte" << r.interval_end.time()
            << close_document
        << close_document
        << open_document << "finish_time"
            << open_document
                << "$gte" << r.interval_start.time()
            << close_document
        << close_document
    << close_array << finalize;
            

    std::vector<Appointment> appointments;

    auto cursor = appointments_collection.find(bsoncxx::document::view_or_value {filter_value}, mongocxx::options::find().sort(document {} << "date" << 1 << "start_time" << 1 << finalize));

    for(auto it = cursor.begin(); it != cursor.end(); it++)
    {
        appointments.push_back(JSON_Parser::parse_appointment(json::parse(bsoncxx::to_json(*it))));
    }

    std::vector<AppointmentOffer> gaps;
    
    if(appointments.empty())
    {
        int days = r.last_date.days_in_year() - r.first_date.days_in_year();
        gaps.push_back(AppointmentOffer {r.first_date, r.interval_start, r.interval_end - r.interval_start, r});
        for(int i = 1; i < days; i++)
        {
            // TODO not all gaps added. If the gap ends before the requested end time and there is enough time to add another gap, than we shall do it.
            gaps.push_back(AppointmentOffer {gaps[i-1].date++, r.interval_start, r.interval_end - r.interval_start, r});
        }

        return gaps;
    }

    // query = "SELECT duration FROM Services WHERE id = $id;";

    filter = {};
    filter_value = filter << "_id" << bsoncxx::oid(r.service_id) << finalize;

    auto service_doc_opt = services_collection.find_one(bsoncxx::document::view_or_value {filter_value});

    if(!service_doc_opt)
    {
        throw Exception("No service found with the given id");
    }

    auto service_doc = service_doc_opt.value();

    auto duration_elem = *(service_doc.view().find("duration"));

    Time base_duration {duration_elem.get_int64()};

    std::vector<Time> durations;

    //for(auto &answer : r.answers)
    for(auto it = r.answers.begin(); it != r.answers.end(); it++)
    {
        auto answer = *it;
        if(answer->answer_type == AnswerType::CHOICE)
        {
            ChoiceAnswer &a = dynamic_cast<ChoiceAnswer &>(*answer);

            for(int id : a.ids)
            {
                // query = "SELECT duration FROM Choice_Answer_Signatures WHERE id = $id;";

                filter = {};
                filter_value = filter << "questions.answer_signature._id" << bsoncxx::oid {a.answer_signature_id} << finalize;

                auto projection_doc = document {} << "duration" << 1 << finalize;
                auto options = mongocxx::options::find {};
                options.projection({projection_doc});

                auto result = services_collection.find_one({filter_value}, {options});

                if(result.has_value())
                {
                    durations.push_back(JSON_Parser::parse_time(bsoncxx::to_json(result.value().view())));
                }
                else
                {
                    throw Exception {"Couldn't find the answer_signature by id: " + answer->answer_signature_id};
                }
            }
        }
        else
        {
            //query = "SELECT duration FROM Value_Answer_Signatures WHERE answer_signature_id = (SELECT Answer_Signatures.id FROM Answer_Signatures WHERE question_id = $id);";

            filter_value = document {} << "questions.answer_signature._id" << bsoncxx::oid {answer->answer_signature_id} << finalize;
            auto projection = document {} << "duration" << 1 << finalize;
            auto options = mongocxx::options::find {};
            options.projection({projection});

            auto result = services_collection.find_one({filter_value}, {options});

            if(result.has_value())
            {
                auto duration = JSON_Parser::parse_time(bsoncxx::to_json(result.value().view()));

                if(answer->answer_type == AnswerType::INT)
                {
                    NumericAnswer<int> &a = dynamic_cast<NumericAnswer<int> &>(*answer);
                    durations.push_back(duration * a.number);
                }
                else
                {
                    NumericAnswer<double> &a = dynamic_cast<NumericAnswer<double> &>(*answer);
                    durations.push_back(duration * a.number);
                }
            }
        }
    }

    Time total_duration = add(durations, base_duration);

    if(appointments.front().start > r.interval_start)
    {
        if(r.interval_start - appointments.front().start >= total_duration)
        {
            gaps.push_back(AppointmentOffer {appointments.front().date, r.interval_start, total_duration, r});
        }
    }

    int i = 0;
    while(i < appointments.size() - 1)
    {
        Date current_date = appointments[i].date;
        while(appointments[i].date == current_date && i < appointments.size() - 1)
        {
            if(appointments[i + 1].start - appointments[i].end >= total_duration)
            {
                gaps.push_back(AppointmentOffer {current_date, appointments[i].end, total_duration, r});
            }
            i++;
        }
        i++;
    }

    if(appointments.back().end < r.interval_end && r.interval_end - appointments.back().end >= total_duration)
    {
        gaps.push_back(AppointmentOffer {appointments.back().date, appointments.back().end, total_duration, r});
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