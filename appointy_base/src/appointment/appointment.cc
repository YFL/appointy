#include <appointment.h>

#include <json.hpp>

namespace appointy
{

using json = nlohmann::json;

Appointment::Appointment(const std::string &id, const Date &d, const Time &start, const Time &end, const AppointmentConfiguration &configuration) :
    id {id},
    date {d},
    start {start},
    end {end},
    configuration {configuration}
{

}

auto Appointment::to_string() const -> std::string
{
    std::string ret {this->id + "\n"};
    ret += this->date.to_string() + "\n";
    ret += this->start.to_string() + "\n";
    ret += end.to_string() + "\n";

    return ret + configuration.to_string(); 
}

auto Appointment::to_json() const noexcept -> json
{
    json j = "{}"_json;
    if(!id.empty())
    {
        j["id"] = this->id;
    }
    j["date"] = this->date.to_json();
    j["start"] = this->start.to_json();
    j["end"] = this->end.to_json();
    j["configuration"] = this->configuration.to_json();

    return j;
}

} // namespace appointy