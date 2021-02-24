#include <appointment.h>

#include <json.hpp>

namespace appointy
{

using json = nlohmann::json;

Appointment::Appointment(const std::string &id, const Date &d, const Time &start, const Time &end, const AppointmentRequest &request) :
    id {id},
    date {d},
    start {start},
    end {end},
    request {request}
{

}

auto Appointment::to_string() const -> std::string
{
    std::string ret {this->id + "\n"};
    ret += this->date.to_string() + "\n";
    ret += this->start.to_string() + "\n";
    ret += end.to_string() + "\n";

    return ret + request.to_string(); 
}

auto Appointment::to_json() const -> json
{
    json j = "{}"_json;
    j["id"] = this->id;
    j["date"] = this->date.to_json();
    j["start"] = this->start.to_json();
    j["end"] = this->end.to_json();
    j["request"] = this->request.to_json();

    return j;
}

} // namespace appointy