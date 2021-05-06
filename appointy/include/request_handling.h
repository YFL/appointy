#ifndef _REQUEST_HANDLING_H_
#define _REQUEST_HANDLING_H_

#include <appointment.h>
#include <appointment_configuration.h>
#include <appointment_detail.h>
#include <appointment_offer.h>
#include <config_completion_time.h>
#include <duration_request.h>
#include <service_configuration.h>

namespace appointy
{

auto compute_estimated_duration_of_config(const ServiceConfiguration &config, const std::string &connection_string, const std::string &db_name) -> ConfigCompletionTime;

auto offer_appointments(const AppointmentConfiguration &configuration, const std::string &connection_string, const std::string &services_db_name, const std::string &appointments_db_name) -> std::vector<AppointmentOffer>;

/**
 * @return The id of the inserted appointment
 * @throw appointy::Exception - if there's already an appointment booked to that date and time or when the insertion failed
 */
auto book_appointment(const Appointment &appointment, const std::string &db_connection_string, const std::string &services_db_name, const std::string &appointments_db_name) -> std::string;

auto list_appointments(const Date &start_date, const Date &end_date, const Time &from, const Time &until, const std::string &db_connection_string, const std::string &appointments_db_name) -> std::vector<Appointment>;

auto get_appointment_details(const Appointment &appointment, const std::string &db_connection_string, const std::string &services_db_name) -> AppointmentDetail;

auto remove_appointment(const std::string &appointment_id, const std::string &db_connection_string, const std::string &appointments_db_name) -> void;

auto update_appointment(const std::string &appointment_id, const Appointment &new_appointment, const std::string &db_connection_string, const std::string &db_name) -> void;

} // namespace appointy

#endif // _REQUEST_HANDLING_H_