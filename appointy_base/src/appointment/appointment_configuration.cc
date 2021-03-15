#include <appointment_configuration.h>

#include <json.hpp>

namespace appointy
{

using json = nlohmann::json;

AppointmentConfiguration::AppointmentConfiguration(const Date &first, const Date &last, const Time &from, const Time &until, const ServiceConfiguration &configuration) :
    first_date {first},
    last_date {last},
    interval_start {from},
    interval_end {until},
    configuration {configuration}
{

}

auto AppointmentConfiguration::to_string() const -> std::string
{
    auto ret = this->first_date.to_string() + "\n";
    ret += this->last_date.to_string() + "\n";
    ret += this->interval_start.to_string() + "\n";
    ret += this->interval_end.to_string() + "\n";
    ret += this->configuration.service_id + "\n";
    
    for(auto &a : this->configuration.configuration)
    {
        ret += a->to_string() + "\n"; 
    }

    return ret;
}

auto AppointmentConfiguration::to_json() const noexcept -> json
{
    json j = "{}"_json;
    j["first_date"] = this->first_date.to_json();
    j["last_date"] = this->last_date.to_json();
    j["interval_start"] = this->interval_start.to_json();
    j["interval_end"] = this->interval_end.to_json();
    j["configuration"] = this->configuration.to_json();

    return j;
}

} // namespace appointy