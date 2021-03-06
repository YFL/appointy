#ifndef _APPOINTMENT_OFFER_H_
#define _APPOINTMENT_OFFER_H_

#include <answer.h>
#include <appointment_request.h>
#include <appointy_date.h>
#include <appointy_time.h>

namespace appointy
{

struct AppointmentOffer
{
public:
    const Date date;
    const Time start;
    const Time duration;
    const AppointmentRequest request;

public:
    AppointmentOffer(const Date &date, const Time &start, const Time &duration, const AppointmentRequest &request);

public:
    auto to_json() const noexcept -> nlohmann::json;
    auto to_string() const -> std::string;
};

} // namespace appointy

#endif // _APPOINTMENT_OFFER_H_