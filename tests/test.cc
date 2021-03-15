#include <iostream>

#include <mongocxx/instance.hpp>

#include <db_utils.h>
#include <io_ops.h>
#include <appointment_configuration.h>
#include <json_parser.h>
#include <request_handling.h>

auto main() -> int
{
    using namespace appointy;

    auto instance = mongocxx::instance {};

    auto services = load_services_from_json("./services_examples_test.json");

    // util::set_up_services_collection("mongodb://localhost", "appointy_db");

    // store_services(services, "mongodb://localhost", "appointy_db");

    auto services_loaded = load_services("mongodb://localhost", "appointy_db");
    for(auto &s : services_loaded)
    {
        std::cout << s.to_json().dump() << std::endl;
    }

    auto appointment_request_str = open_file_to_string("./appointment_request_test5.json");
    auto appointment_request = JSON_Parser::parse_appointment_configuration(nlohmann::json::parse(appointment_request_str));

    auto estimated_duration = duration_of_config({appointment_request.configuration.service_id, appointment_request.configuration.configuration}, "mongodb://localhost", "appointy_db");

    std::cout << estimated_duration.to_json().dump() << std::endl;

    auto appointment_offers = offer_appointments(appointment_request, "mongodb://localhost", "appointy_db");

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

    auto appointment = JSON_Parser::parse_appointment(nlohmann::json::parse(open_file_to_string("./appointment_test4.json")));

    std::cout << book_appointment(appointment, "mongodb://localhost", "appointy_db") << std::endl;

    return 0;
}
