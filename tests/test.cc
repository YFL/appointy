#include <iostream>

#include <mongocxx/instance.hpp>

#include <db_utils.h>
#include <io_ops.h>
#include <appointment_configuration.h>
#include <json_parser.h>
#include <request_handling.h>
#include <user_handling.h>

auto main() -> int
{
    using namespace appointy;

    auto instance = mongocxx::instance {};

    auto appointments = list_appointments({2021, 3, 17}, {2021, 3, 17}, {8, 0, 0}, {20, 0, 0}, "mongodb://localhost", "appointy_db");

    for(auto &a : appointments)
    {
        std::cout << "appointment id: " << a.id << std::endl;
        auto detail = get_appointment_details(a, "mongodb://localhost", "appointy_db");
        std::cout << detail.to_json().dump() << std::endl;
    }

    update_appointment("608ecace1a338e6f9f4474cb", JSON_Parser::parse_appointment(nlohmann::json::parse(open_file_to_string("./appointment_update_test_1.json"))),"mongodb://localhost", "appointy_db");

    auto user = generate_user("mongodb://localhost", "appointy_users_db");

    std::cout << "newly generated user's name: " << user << std::endl;

    link_appointment_to_user("608ecace1a338e6f9f4474cb", user, "mongodb://localhost/", "appointy_users_appointments_db");

    auto users_appointments = list_user_appointments(user, "mongodb://localhost/", "appointy_db", "appointy_users_appointments_db");

    std::cout << "User " << user << "'s appointments:\n";
    for(auto &ua : users_appointments)
    {
        std::cout << get_appointment_details(ua, "mongodb://localhost", "appointy_db").to_json().dump() << std::endl;
    }

    return 0;

    auto services = load_services_from_json("./services_examples_test.json");

    // util::set_up_services_collection("mongodb://localhost", "appointy_db");

    // store_services(services, "mongodb://localhost", "appointy_db");

    auto services_loaded = load_services("mongodb://localhost", "appointy_db");
    for(auto &s : services_loaded)
    {
        std::cout << s.to_json().dump() << std::endl;
    }

    auto appointment_config_str = open_file_to_string("./appointment_configuration_test_2.json");
    auto appointment_config = JSON_Parser::parse_appointment_configuration(nlohmann::json::parse(appointment_config_str));

    std::cout << "An answer: " << appointment_config.configuration.configuration.front()->to_json().dump()  << "\n\n" << std::endl;

    auto estimated_duration = compute_estimated_duration_of_config(appointment_config.configuration, "mongodb://localhost", "appointy_db", "appointy_db");

    std::cout << "estimated duration: " << estimated_duration.to_json().dump() << std::endl;

    auto appointment_offers = offer_appointments(appointment_config, "mongodb://localhost", "appointy_db", "appointy_db");

    std::cout << "\n\n\nappointment_offers.size(): " << appointment_offers.size() << "[ \n";
    for(auto i = std::vector<AppointmentOffer>::size_type {0}; i < appointment_offers.size(); i++)
    {
        std::cout << appointment_offers[i].to_json();
        if(i < appointment_offers.size() - 1)
        {
            std::cout << ',';
        }
        std::cout << std::endl;
    }
    std::cout << "]" << std::endl;

    auto appointment = JSON_Parser::parse_appointment(nlohmann::json::parse(open_file_to_string("./appointment_test_1.json")));

    std::cout << book_appointment(appointment, "mongodb://localhost", "appointy_db", "appointy_db") << std::endl;

    auto config_completion_time = ConfigCompletionTime {appointment.configuration.configuration, {1,32,0}};

    store_config_completion_time(config_completion_time, "mongodb://localhost", "appointy_db");

    auto completion_times = load_config_completion_times(appointment.configuration.configuration, "mongodb://localhost", "appointy_db");
    std::cout << "completion_times.size(): " << completion_times.size();
    for(auto &completion_time : completion_times)
    {
        std::cout << "\n\n\n" << completion_time.to_json().dump() << std::endl;
    }

    return 0;
}
