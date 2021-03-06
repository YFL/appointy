#include <appointment_offer.h>

namespace appointy
{

using json = nlohmann::json;

AppointmentOffer::AppointmentOffer(const Date &date, const Time &start, const Time &duration, const AppointmentConfiguration &configuration) :
    date {date},
    start {start},
    duration {duration},
    configuration {configuration}
{

}

auto AppointmentOffer::to_json() const noexcept -> json
{
    json j = "{}"_json;
    j["date"] = this->date.to_json();
    j["start"] = this->start.to_json();
    j["duration"] = this->duration.to_json();
    j["configuration"] = this->configuration.to_json();

    return j;
}

auto AppointmentOffer::to_string() const -> std::string
{
    auto s = this->date.to_string() + "\n";
    s += this->start.to_string() + "\n";
    s += this->duration.to_string() + "\n";

    return s + this->configuration.to_string() + "\n";
}

} // namespace appointy