#include <io_ops.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/view_or_value.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>

#include <appointy_exception.h>
#include <db_utils.h>
#include <json_parser.h>

namespace appointy
{

auto open_file_to_string(const std::string &sfp) -> std::string
{
    std::ifstream file {sfp};
    if(!file.is_open())
    {
        throw Exception {"Couldn't open file " + sfp + " " + strerror(errno)};
    }

    std::stringstream ss;
    std::string line;
    while(std::getline(file, line))
    {
        ss << line << "\n";
    }

    return ss.str();
}

auto load_services_from_json(const std::string &ptj) -> std::string
{
    return open_file_to_string(ptj);
}

auto set_up_services_collection(const std::string &connection_string, const std::string &db_name) -> void
{
    mongocxx::uri uri {connection_string};
    mongocxx::client client {uri};
    mongocxx::collection services {client[db_name]["Services"]};

    auto keys_builder = bsoncxx::builder::stream::document {};
    auto keys = keys_builder << "name" << 1 << bsoncxx::builder::stream::finalize;
    auto options = mongocxx::options::index {};
    options.name("name_unique");
    options.unique(true);
    services.create_index(bsoncxx::document::view_or_value {keys}, bsoncxx::document::view_or_value {options});
}

auto store_services(const std::string &services, const std::string &connection_string, const std::string &db_name) -> bool
{
    using json = nlohmann::json;

    json j;
    try
    {
        j = json::parse(services, nullptr, true, true);
    }
    catch(const json::parse_error &e)
    {
        throw Exception {"Couldn't parse services:\n" + services + "\nWith error: " + e.what()};
    }

    auto size = j.size();
    std::vector<Service> in_mem_services;
    for(auto &service_json : j)
    {
        in_mem_services.push_back(JSON_Parser::parse_service(service_json));
    }

    mongocxx::uri uri {connection_string};
    mongocxx::client client {uri};
    mongocxx::database db {client[db_name]};
    mongocxx::collection services_coll {db["Services"]};

    std::vector<bsoncxx::document::view_or_value> documents;
    for(auto &service : in_mem_services)
    {
        documents.push_back(util::service_to_bson(service));
    }

    auto client_session = client.start_session();

    client_session.start_transaction();

    auto result = services_coll.insert_many(client_session, documents.begin(), documents.end());

    if(result.has_value())
    {
        if(result.value().inserted_count() != size)
        {
            client_session.abort_transaction();

            return false;
        }
        client_session.commit_transaction();

        return true;
    }

    client_session.abort_transaction();

    return false;
}

auto load_services(const std::string &connection_string, const std::string &db_name) -> std::vector<std::string>
{
    std::vector<std::string> services;

    mongocxx::uri uri {connection_string};
    mongocxx::client client {uri};
    mongocxx::collection collection {client[db_name]["Services"]};

    auto cursor = collection.find({});

    for(auto doc : cursor)
    {
        services.push_back(bsoncxx::to_json(doc));
    }

    return services;
}

} // namespace appointy