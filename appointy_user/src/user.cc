#include <user.h>

namespace appointy
{

User::User(const std::string &id, const std::string &username, const std::string &email, const std::string &phone) :
    _id {id},
    username {username},
    email {email},
    phone {phone}
{

}

auto User::to_json() const noexcept -> nlohmann::json
{
    auto json = "{}"_json;
    json["_id"] = _id;
    json["username"] = username;
    json["email"] = email;
    json["phone"] = phone;
}

} // namespace appointy