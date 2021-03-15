// Basically loading and storing the model from configs and into databases

#ifndef _IO_OPS_H_
#define _IO_OPS_H_

#include <string>
#include <json.hpp>

#include <config_completion_time.h>
#include <service.h>
#include <service_configuration.h>

namespace appointy
{

auto open_file_to_string(const std::string &path_to_file) -> std::string;

auto load_services_from_json(const std::string &path_to_json) -> std::vector<Service>;

auto store_services(const std::vector<Service> &services, const std::string &connection_string, const std::string &db_name) -> bool;

auto load_services(const std::string &connection_string, const std::string &db_name) -> std::vector<Service>;

auto store_config_completion_time(const ConfigCompletionTime &config_completion_time, const std::string &connection_string, const std::string &db_name) -> bool;

auto load_config_completion_times(const ServiceConfiguration &configuration, const std::string &connection_string, const std::string &db_name) -> std::vector<ConfigCompletionTime>;

} // namespace appointy

#endif // _IO_OPS_H_