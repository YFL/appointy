#include <io_ops.h>

#include <algorithm>
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

auto load_services_from_json(const std::string &ptj) -> std::vector<Service>
{
    try
    {
        auto json = nlohmann::json::parse(open_file_to_string(ptj), nullptr, true, true);
        auto services = std::vector<Service> {};

        if(!json.is_array())
        {
            throw Exception {"The root object is not an array. Even if there's only one service, it has to be added to an array"};
        }
        services.reserve(json.size());

        for(auto &json_service : json)
        {
            services.push_back(JSON_Parser::parse_service(json_service));
        }

        return services;
    }
    catch(const nlohmann::detail::parse_error &e)
    {
        throw Exception {"Couldn't parse the services from the file: " + ptj};
    }
}

auto store_services(const std::vector<Service> &services, const std::string &connection_string, const std::string &db_name) -> bool
{
    mongocxx::uri uri {connection_string};
    mongocxx::client client {uri};
    mongocxx::database db {client[db_name]};
    mongocxx::collection services_coll {db["Services"]};

    std::vector<bsoncxx::document::view_or_value> documents;
    for(auto &service : services)
    {
        documents.push_back(util::service_to_bson(service));
    }

    auto result = services_coll.insert_many(documents.begin(), documents.end());

    if(result.has_value())
    {
        if(result.value().inserted_count() != services.size())
        {
            return false;
        }

        return true;
    }

    return false;
}

auto load_services(const std::string &connection_string, const std::string &db_name) -> std::vector<Service>
{
    std::vector<Service> services;

    mongocxx::uri uri {connection_string};
    mongocxx::client client {uri};
    mongocxx::collection collection {client[db_name]["Services"]};

    auto cursor = collection.find({});

    for(auto doc : cursor)
    {
        services.push_back(JSON_Parser::parse_service(nlohmann::json::parse(bsoncxx::to_json(doc), nullptr, true, true)));
    }

    return services;
}

} // namespace appointy