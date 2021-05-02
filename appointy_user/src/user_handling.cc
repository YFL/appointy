#include <user_handling.h>

#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>

#include <appointy_exception.h>

namespace appointy
{

auto generate_user(const std::string &db_connection_string, const std::string &db_name) -> std::string
{
    auto client = mongocxx::client {mongocxx::uri {db_connection_string}};
    auto users_collection = client[db_name]["Users"];

    auto result = users_collection.insert_one({});

    if(result)
    {
        return result.value().inserted_id().get_oid().value.to_string();
    }

    throw Exception {"Some error occurred while generating a new user"};
}

auto link_appointment_to_user(const std::string &appointment_id, const std::string &user_id, const std::string &db_connection_string, const std::string &db_name) -> void
{
    auto client = mongocxx::client {mongocxx::uri {db_connection_string}};
    auto users_appointments_collection = client[db_name]["Users_Appointments"];

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

} // namespace appointy