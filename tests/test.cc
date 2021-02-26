#include <iostream>

#include <mongocxx/instance.hpp>

#include <io_ops.h>
#include <appointment_request.h>
#include <request_handling.h>

auto main() -> int
{
    using namespace appointy;

    mongocxx::instance instance;

    auto services = load_services_from_json("./services_examples_test.json");

    set_up_services_collection("mongodb://localhost", "appointy_db");

    store_services(services, "mongodb://localhost", "appointy_db");

    auto services_loaded = load_services("mongodb://localhost", "appointy_db");
    for(auto &s : services_loaded)
    {
        std::cout << s << std::endl;
    }

    auto appointment_request = open_file_to_string("./request_examples.json");

    auto appointment_offers = accept_appointment_request(appointment_request, "mongodb://localhost", "appointy_db");

    std::cout << "\n\n\n";
    for(auto offer : appointment_offers)
    {
        std::cout << offer.to_string() << std::endl;
    }

    return 0;
}
