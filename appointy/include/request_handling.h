#ifndef _REQUEST_HANDLING_H_
#define _REQUEST_HANDLING_H_

#include <appointment.h>
#include <appointment_request.h>
#include <appointment_offer.h>

namespace appointy
{

auto accept_appointment_request(const AppointmentRequest &request, const std::string &connection_string, const std::string &db_name) -> std::vector<AppointmentOffer>;
auto book_appointment(const Appointment &booking_request, const std::string &db_connection_string, const std::string &db_name) -> bool;

} // namespace appointy

#endif // _REQUEST_HANDLING_H_