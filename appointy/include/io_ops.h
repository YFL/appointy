// Basically loading and storing the model from configs and into databases

#ifndef _IO_OPS_H_
#define _IO_OPS_H_

#include <string>
#include <json.hpp>

#include <service.h>

namespace appointy
{

auto open_file_to_string(const std::string &path_to_file) -> std::string;

auto load_services_from_json(const std::string &path_to_json) -> std::vector<Service>;

auto store_services(const std::vector<Service> &services, const std::string &connection_string, const std::string &db_name) -> bool;

auto load_services(const std::string &connection_string, const std::string &db_name) -> std::vector<Service>;

} // namespace appointy

#endif // _IO_OPS_H_