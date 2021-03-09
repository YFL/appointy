#include <iostream>

#include <mongocxx/instance.hpp>

#include <io_ops.h>
#include <appointment_request.h>
#include <json_parser.h>
#include <request_handling.h>

auto main() -> int
{
    using namespace appointy;

    auto instance = mongocxx::instance {};

    auto services = load_services_from_json("./services_examples_test.json");

    set_up_services_collection("mongodb://localhost", "appointy_db");

    //store_services(services, "mongodb://localhost", "appointy_db");

    auto services_loaded = load_services("mongodb://localhost", "appointy_db");
    for(auto &s : services_loaded)
    {
        std::cout << s.to_json().dump() << std::endl;
    }

    auto appointment_request_str = open_file_to_string("./appointment_request_test3.json");
    auto appointment_request = JSON_Parser::parse_appointment_request(nlohmann::json::parse(appointment_request_str));

    auto appointment_offers = accept_appointment_request(appointment_request, "mongodb://localhost", "appointy_db");

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

    auto appointment = JSON_Parser::parse_appointment(nlohmann::json::parse(open_file_to_string("./appointment_test2.json")));

    std::cout << book_appointment(appointment, "mongodb://localhost", "appointy_db") << std::endl;

    return 0;
}
