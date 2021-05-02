#ifndef _APPOINTMENT_DETAIL_H_
#define _APPOINTMENT_DETAIL_H_

#include <string>
#include <vector>
#include <variant>

#include <json.hpp>

namespace appointy
{

struct AppointmentDetail
{
    const std::string service_id;
    const std::string service_name;
    const std::vector<std::pair<std::string, std::variant<std::vector<std::string>, int, double>>> configuration_data;

public:
    AppointmentDetail(std::string service_id, const std::string &service_name, const std::vector<std::pair<std::string, std::variant<std::vector<std::string>, int, double>>> &configuration_data) noexcept;

public:
    auto to_json() const noexcept -> nlohmann::json;
};

} // namespace appointy

#endif // _APPOINTMENT_DETAIL_H_