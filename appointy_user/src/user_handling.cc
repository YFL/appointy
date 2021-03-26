#include <user_handling.h>

#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>

namespace appointy
{

auto register_user(const User &user, const std::string &password, const std::string &connection_string, const std::string &db_name) noexcept -> bool
{
    auto client = mongocxx::client {mongocxx::uri {connection_string}};
    auto collection = client["Users"];
}

} // namespace appointy