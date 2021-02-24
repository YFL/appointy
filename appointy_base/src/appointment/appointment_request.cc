#include <appointment_request.h>

#include <json.hpp>

namespace appointy
{

using json = nlohmann::json;

AppointmentRequest::AppointmentRequest(const Date &first, const Date &last, const Time &from, const Time &until, const std::string  &service_id, const std::vector<std::shared_ptr<Answer>> &answers) :
    first_date {first},
    last_date {last},
    interval_start {from},
    interval_end {until},
    service_id {service_id},
    answers {answers}
{

}

auto AppointmentRequest::to_string() const -> std::string
{
    auto ret = this->first_date.to_string() + "\n";
    ret += this->last_date.to_string() + "\n";
    ret += this->interval_start.to_string() + "\n";
    ret += this->interval_end.to_string() + "\n";
    ret += this->service_id + "\n";
    
    for(auto &a : this->answers)
    {
        ret += a->to_string() + "\n"; 
    }

    return ret;
}

auto AppointmentRequest::to_json() const -> json
{
    json j = "{}"_json;
    j["first_date"] = this->first_date.to_json();
    j["last_date"] = this->last_date.to_json();
    j["interval_start"] = this->interval_start.to_json();
    j["interval_end"] = this->interval_end.to_json();
    j["service_id"] = this->service_id;
    auto &json_answers = j["answers"] = "[]"_json;
    for(auto &a : answers)
    {
        json_answers.push_back(a->to_json());
    }

    return j;
}

} // namespace appointy