#ifndef _APPOINTMENT_H_
#define _APPOINTMENT_H_

#include <appointy_date.h>
#include <appointy_time.h>
#include <appointment_configuration.h>

namespace appointy
{

struct Appointment
{
public:
    const std::string id;
    const Date date;
    const Time start;
    const Time end;
    const AppointmentConfiguration configuration;

public:
    Appointment(const std::string &id, const Date &date, const Time &start, const Time &end, const AppointmentConfiguration &configuration);

public:
    auto to_string() const -> std::string;
    auto to_json() const noexcept -> nlohmann::json;
};
    
} // namespace appointy

#endif // _APPOINTMENT_H_