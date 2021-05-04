#ifndef _USER_HANDLING_H_
#define _USER_HANDLING_H_

#include <appointment.h>

namespace appointy
{
    
auto generate_user(const std::string &connection_string, const std::string &db_name) -> std::string;

auto link_appointment_to_user(const std::string &appointment_id, const std::string &user_id, const std::string &db_connection_string, const std::string &db_name) -> void;

auto list_user_appointments(const std::string &user_id, const std::string &db_connection_string, const std::string &db_name) -> std::vector<Appointment>;

auto n_most_frequent_appointments(const std::string &user_id, uint32_t n, const std::string &connection_string, const std::string &db_name) -> std::vector<Appointment>;

} // namespace appointy

#endif // _USER_HANDLING_H_