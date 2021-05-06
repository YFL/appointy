#include <request_handling.h>

#include <iostream>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/query_exception.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/replace.hpp>
#include <mongocxx/uri.hpp>

#include <appointy_exception.h>
#include <choice_answer.h>
#include <choice_answer_signature.h>
#include <io_ops.h>
#include <json_parser.h>
#include <numeric_answer.h>
#include <numeric_answer_signature.h>

namespace appointy
{

using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;

template <template<class, class> class Collection, typename T, typename Allocator>
auto add(const Collection<T, Allocator> &collection, const T &init)
{
    if(collection.begin() == collection.end())
    {
        return init;
    }
    T begin = *collection.begin();
    Collection<T, Allocator> new_collection {collection.begin() + 1, collection.end()};

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

auto create_detail(const std::shared_ptr<Answer> &answer, const Service &service)
{
    auto question_it = std::find_if(service.questions.begin(), service.questions.end(), [&answer](auto question) { return question.answer_signature->id == answer->answer_signature_id; });
    if(question_it == service.questions.end())
    {
        throw Exception {std::string {"Couldn't find the question based on i'ts answer_signature's id "} + answer->answer_signature_id};
    }

    if(answer->answer_type == AnswerType::CHOICE)
    {
        auto choice_answer = dynamic_cast<ChoiceAnswer &>(*answer);

        auto options = dynamic_cast<ChoiceAnswerSignature &>(*question_it->answer_signature).options;
        auto texts = std::vector<std::string, std::allocator<std::string>> {};
        texts.reserve(choice_answer.ids.size());

        // We assume, that the data in the database and gotten from the database is correct, meaning that there will allways be a match
        // in the if statement in the predicate
        std::transform(choice_answer.ids.begin(), choice_answer.ids.end(), std::back_inserter(texts),
            [&options](auto id)
            {
                for(auto option : options)
                {
                    if(option.id == id)
                    {
                        return option.text;
                    }
                }

                // This path should never be accessed
                // Check the comment right before the transform call
                return std::string {""};
            });
        
        return std::pair<std::string, std::variant<std::vector<std::string, std::allocator<std::string>>, int, double>> {question_it->text, texts};
    }
    else if(answer->answer_type == AnswerType::INT)
    {
        return std::make_pair<>(question_it->text, std::variant<std::vector<std::string>, int, double> {dynamic_cast<NumericAnswer<int> &>(*answer).number});
    }
    else
    {
        return std::make_pair<>(question_it->text, std::variant<std::vector<std::string>, int, double> {dynamic_cast<NumericAnswer<double> &>(*answer).number});
    }
}

auto compute_estimated_duration_of_config(const ServiceConfiguration &config, const std::string &connection_string, const std::string &db_name) -> ConfigCompletionTime
{
    auto service = get_service_from_db(config.service_id, connection_string, db_name);

    auto durations = std::vector<Time> {};
    for(auto it = config.configuration.begin(); it != config.configuration.end(); it++)
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

    auto config_duration = add(durations, service.duration);

    auto completion_times = load_config_completion_times(config, connection_string, db_name);
    
    if(completion_times.size())
    {
        auto sum = long {config_duration.to_seconds()};
        for(auto &completion_time : completion_times)
        {
            sum += completion_time.completion_time.to_seconds();
        }

        auto total_seconds = sum / (completion_times.size() + 1);

        auto minutes = static_cast<int>(total_seconds / 60);
        auto seconds = static_cast<int>(total_seconds % 60);
        auto hours = static_cast<int>(minutes / 60);
        minutes = minutes % 60;

        return ConfigCompletionTime {config, {hours, minutes, seconds}};
    }

    return ConfigCompletionTime {config, config_duration};
}

auto offer_appointments(const AppointmentConfiguration &r, const std::string &connection_string, const std::string &services_db_name, const std::string &appointments_db_name) -> std::vector<AppointmentOffer>
{
    auto uri = mongocxx::uri {connection_string};
    auto client = mongocxx::client {uri};
    auto services_collection = client[services_db_name]["Services"];
    auto appointments_collection = client[appointments_db_name]["Appointments"];

    if(r.interval_start > r.interval_end)
    {
        throw Exception {"The interval's start is bigger then it's end"};
    }

    auto total_duration = compute_estimated_duration_of_config(r.configuration, connection_string, services_db_name).completion_time;

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
        int days = r.last_date.days_in_year() - r.first_date.days_in_year() + 1;
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
            gaps.push_back(AppointmentOffer {appointments.front().date, r.interval_start, appointments.front().start - r.interval_start, r});
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
                gaps.push_back(AppointmentOffer {current_date, appointments[i].end, appointments[i + 1].start - appointments[i].end, r});
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

auto book_appointment(const Appointment &appointment, const std::string &db_connection_string, const std::string &services_db_name, const std::string &appointments_db_name) -> bool
{
    mongocxx::uri uri {db_connection_string};
    mongocxx::client client {uri};

    mongocxx::collection appointments_collection {client[appointments_db_name]["Appointments"]};

    auto appointment_offers = offer_appointments(appointment.configuration, db_connection_string, services_db_name, appointments_db_name);

    for(auto &offer : appointment_offers)
    {
        if(offer.date == appointment.date && offer.start <= appointment.start && offer.start + offer.duration >= appointment.end)
        {
            bsoncxx::document::view_or_value document {bsoncxx::from_json(appointment.to_json().dump())};

            auto result = appointments_collection.insert_one(document);

            return result.value().result().inserted_count() == 1 ? true : false;
        }
    }

    throw Exception {"Other appointments booked in that time interval"};
}

auto list_appointments(const Date &start_date, const Date &end_date, const Time &from, const Time &until, const std::string &db_connection_string, const std::string &appointments_db_name) -> std::vector<Appointment>
{
    auto client = mongocxx::client {mongocxx::uri {db_connection_string}};
    auto appointments_collection = client[appointments_db_name]["Appointments"];

    auto doc = document {};
    auto filter = doc << "$and" << open_array
        << open_document << "date"
            << open_document
                << "$gte" << start_date.date()
                << "$lte" << end_date.date()
            << close_document
        << close_document
        << open_document << "start"
            << open_document
                << "$gte" << from.time()
                << "$lte" << until.time()
            << close_document
        << close_document
    << close_array << finalize;

    auto cursor = appointments_collection.find(filter.view());

    try
    {
        std::vector<Appointment> ret;
        for(auto it = cursor.begin(); it != cursor.end(); it++)
        {
            ret.push_back(JSON_Parser::parse_appointment(nlohmann::json::parse(bsoncxx::to_json(*it))));
        }

        return ret;
    }
    catch(const mongocxx::query_exception &e)
    {
        throw Exception {std::string {"An error occurred while fetching the appointments from the database: "} + e.what()};
    }
}

auto get_appointment_details(const Appointment &appointment, const std::string &db_connection_string, const std::string &services_db_name) -> AppointmentDetail
{
    auto service = get_service_from_db(appointment.configuration.configuration.service_id, db_connection_string, services_db_name);

    auto config_data = std::vector<std::pair<std::string, std::variant<std::vector<std::string>, int, double>>> {};
    config_data.reserve(appointment.configuration.configuration.configuration.size());
    auto bound = std::bind(create_detail, std::placeholders::_1, service);
    std::transform(appointment.configuration.configuration.configuration.begin(), appointment.configuration.configuration.configuration.end(), std::back_inserter(config_data), bound);

    return AppointmentDetail {service.id, service.name, config_data};
}

auto remove_appointment(const std::string &appointment_id, const std::string &db_connection_string, const std::string &appointments_db_name) -> void
{
    auto client = mongocxx::client {mongocxx::uri {db_connection_string}};
    auto appointments_collection = client[appointments_db_name]["Appointments"];

    auto doc = document {};
    auto filter = doc << "_id" << bsoncxx::oid {appointment_id} << finalize;

    auto appointment = appointments_collection.find_one_and_delete(filter.view());
    if(!appointment)
    {
        throw Exception {"The appointment hasn't been found by the id " + appointment_id};
    }
}

auto update_appointment(const std::string &appointment_id, const Appointment &new_appointment, const std::string &db_connection_string, const std::string &appointments_db_name) -> void
{
    auto client = mongocxx::client {mongocxx::uri {db_connection_string}};
    auto appointments_collection = client[appointments_db_name]["Appointments"];

    auto doc = document {};
    auto filter = doc << "_id" << bsoncxx::oid {appointment_id} << finalize;
    auto replaced = appointments_collection.replace_one(filter.view(), bsoncxx::from_json(new_appointment.to_json().dump()));
    
    if(replaced)
    {
        if(replaced.value().modified_count() != 1)
        {
            throw Exception {"The number of modified documents isn't equal to one: " + std::to_string(replaced.value().modified_count())};
        }
    }
    else
    {
        throw Exception {"Some error occurred while attempting to update the document"};
    }
}

} // namespace appointy