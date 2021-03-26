#ifndef _USER_H_
#define _USER_H_

#include <string>

#include <json.hpp>

namespace appointy
{

struct User
{
    const std::string _id;
    const std::string username;
    const std::string email;
    const std::string phone;

public:
    User(const std::string &id, const std::string &username, const std::string &email, const std::string &phone) noexcept;

public:
    auto to_json() const noexcept -> nlohmann::json;
};

} // namespace appointy

#endif // _USER_H_