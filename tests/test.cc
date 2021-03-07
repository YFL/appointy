#include <iostream>

#include <mongocxx/instance.hpp>

#include <io_ops.h>
#include <appointment_request.h>
#include <json_parser.h>
#include <request_handling.h>

auto main() -> int
{
    using namespace appointy;

    mongocxx::instance instance;

    auto services = load_services_from_json("./services_examples_test.json");

    set_up_services_collection("mongodb://localhost", "appointy_db");

    //store_services(services, "mongodb://localhost", "appointy_db");

    auto services_loaded = load_services("mongodb://localhost", "appointy_db");
    for(auto &s : services_loaded)
    {
        std::cout << s.to_json().dump() << std::endl;
    }

    auto appointment_request_str = open_file_to_string("./appointment_request_test2.json");
    auto appointment_request = JSON_Parser::parse_appointment_request(nlohmann::json::parse(appointment_request_str));

    auto appointment_offers = accept_appointment_request(appointment_request, "mongodb://localhost", "appointy_db");

    std::cout << "\n\n\n";
    for(auto offer : appointment_offers)
    {
        std::cout << offer.to_json() << std::endl;
    }

    auto appointment = JSON_Parser::parse_appointment(nlohmann::json::parse(open_file_to_string("./appointment_test.json")));

    std::cout << book_appointment(appointment, "mongodb://localhost", "appointy_db") << std::endl;

    return 0;
}
