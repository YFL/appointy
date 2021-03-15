#ifndef _REQUEST_HANDLING_H_
#define _REQUEST_HANDLING_H_

#include <appointment.h>
#include <appointment_request.h>
#include <appointment_offer.h>
#include <config_completion_time.h>
#include <duration_request.h>
#include <service_configuration.h>

namespace appointy
{

auto duration_of_config(const ServiceConfiguration &config, const std::string &connection_string, const std::string &db_name) -> ConfigCompletionTime;

auto accept_appointment_request(const AppointmentRequest &request, const std::string &connection_string, const std::string &db_name) -> std::vector<AppointmentOffer>;

auto book_appointment(const Appointment &appointment, const std::string &db_connection_string, const std::string &db_name) -> bool;

auto store_config_completion_time(const ConfigCompletionTime &config_completion_time, const std::string &connection_string, const std::string &db_name) -> bool;

auto load_config_completion_times(const ServiceConfiguration &configuration, const std::string &connection_string, const std::string &db_name) -> std::vector<ConfigCompletionTime>;

} // namespace appointy

#endif // _REQUEST_HANDLING_H_