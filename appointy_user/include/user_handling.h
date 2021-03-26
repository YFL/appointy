#ifndef _USER_HANDLING_H_
#define _USER_HANDLING_H_

#include <appointment.h>

#include <user.h>

namespace appointy
{
    
auto register_user(const User &user, const std::string &password, const std::string &connection_string, const std::string &db_name) noexcept -> bool;

auto verify_password(const std::string &username, const std::string &password, const std::string &connection_string, const std::string &db_name) -> bool;

auto link_appointment_to_user(const Appointment &appointment, const std::string &user_id) -> bool;

auto n_most_frequent_appointments(const std::string &user_id, uint32_t n, const std::string &connection_string, const std::string &db_name) -> std::vector<Appointment>;

} // namespace appointy

#endif // _USER_HANDLING_H_