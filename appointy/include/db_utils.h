#ifndef _DB_UTILS_H_
#define _DB_UTILS_H_

#include <bsoncxx/document/value.hpp>

#include <service.h>

namespace appointy
{

namespace util
{

auto set_up_services_collection(const std::string &connection_string, const std::string &db_name) -> void;

auto service_to_bson(const Service &service) -> bsoncxx::document::value;

} // namespace util

} // namespace appointy

#endif // _DB_UTILS_H_