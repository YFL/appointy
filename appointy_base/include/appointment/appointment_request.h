#ifndef _APPOINTMENT_REQUEST_H_
#define _APPOINTMENT_REQUEST_H_

#include <vector>
#include <memory>

#include <answer.h>
#include <appointy_date.h>
#include <appointy_time.h>
#include <service_configuration.h>

namespace appointy
{

struct AppointmentRequest
{
public:
    const Date first_date;
    const Date last_date;
    const Time interval_start;
    const Time interval_end;
    const ServiceConfiguration configuration;

public:
    AppointmentRequest(const Date &first, const Date &last, const Time &from, const Time &until, const ServiceConfiguration &configuration);

public:
    auto to_string() const -> std::string;
    auto to_json() const noexcept -> nlohmann::json;
};

} // namespace appointy

#endif // _APPOINTMENT_REQUEST_H_