#include <appointment_detail.h>

namespace appointy
{

AppointmentDetail::AppointmentDetail(std::string service_id, const std::string &service_name, const std::vector<std::pair<std::string, std::variant<std::vector<std::string>, int, double>>> &configuration_data) noexcept :
    service_id {service_id},
    service_name {service_name},
    configuration_data {configuration_data}
{

}

auto AppointmentDetail::to_json() const noexcept -> nlohmann::json
{
    auto json = "{}"_json;
    json["service_id"] = service_id;
    json["service_name"] = service_name;
    json["configuration_data"] = "{}"_json;
    auto conf_data = json["configuration_data"];
    for(auto &p : configuration_data)
    {
        if(p.second.index() == 0)
        {
            conf_data[p.first] = std::get<std::vector<std::string>>(p.second);
        }
        else if(p.second.index() == 1)
        {
            conf_data[p.first] = std::get<int>(p.second);
        }
        else if(p.second.index() == 2)
        {
            conf_data[p.first] = std::get<double>(p.second);
        }
    }

    return json;
}

} // namespace appointy