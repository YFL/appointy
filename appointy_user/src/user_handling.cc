#include <user_handling.h>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/query_exception.hpp>
#include <mongocxx/uri.hpp>

#include <appointy_exception.h>
#include <json_parser.h>

namespace appointy
{

auto generate_user(const std::string &db_connection_string, const std::string &users_db_name) -> std::string
{
    auto client = mongocxx::client {mongocxx::uri {db_connection_string}};
    auto users_collection = client[users_db_name]["Users"];

    auto result = users_collection.insert_one({});

    if(result)
    {
        return result.value().inserted_id().get_oid().value.to_string();
    }

    throw Exception {"Some error occurred while generating a new user"};
}

auto link_appointment_to_user(const std::string &appointment_id, const std::string &user_id, const std::string &db_connection_string, const std::string &users_appointments_db_name) -> void
{
    auto client = mongocxx::client {mongocxx::uri {db_connection_string}};
    auto users_appointments_collection = client[users_appointments_db_name]["Users_Appointments"];

    auto doc = bsoncxx::builder::stream::document {} << "user" << bsoncxx::oid {user_id} << "appointment" << bsoncxx::oid {appointment_id} << bsoncxx::builder::stream::finalize;

    auto result = users_appointments_collection.insert_one(doc.view());
    if(result)
    {
        if(result.value().result().inserted_count() != 1)
        {
            throw Exception {"Couldn't link the appointment to the user: appointment id: " + appointment_id + " user id: " + user_id};
        }
    }
}

auto list_user_appointments(const std::string &user_id, const std::string &db_connection_string, const std::string &appointments_db_name, const std::string &users_appointments_db_name) -> std::vector<Appointment>
{
    auto client = mongocxx::client {mongocxx::uri {db_connection_string}};
    auto users_appointments_collection = client[users_appointments_db_name]["Users_Appointments"];
    auto appointments_collection = client[appointments_db_name]["Appointments"];

    auto doc = bsoncxx::builder::stream::document {};
    auto filter = doc << "user" << bsoncxx::oid {user_id} << bsoncxx::builder::stream::finalize;

    auto cursor = users_appointments_collection.find(filter.view());

    try
    {
        auto appointments = std::vector<Appointment> {};
        for(auto it = cursor.begin(); it != cursor.end(); it++)
        {
            auto appointment_id_elem = it->operator[]("appointment");
            if(!appointment_id_elem || appointment_id_elem.type() != bsoncxx::type::k_oid)
            {
                throw Exception {"The document found is either not a document, or it doesn't contain the appointment's ID"};
            }

            auto appointment_oid = appointment_id_elem.get_oid().value;
            auto doc = bsoncxx::builder::stream::document {};
            auto filter = doc << "_id" << appointment_oid << bsoncxx::builder::stream::finalize;
            auto result = appointments_collection.find_one(filter.view());

            if(!result)
            {
                throw Exception {"The appointment couldn't be found by ID: " + appointment_oid.to_string()};
            }

            appointments.push_back(JSON_Parser::parse_appointment(nlohmann::json::parse(bsoncxx::to_json(result.value()))));
        }

        return appointments;
    }
    catch(const mongocxx::query_exception &e)
    {
        throw Exception {std::string {"Couldn't find the appointments linked to the user based on user's ID: "} + e.what()};
    }
}

} // namespace appointy