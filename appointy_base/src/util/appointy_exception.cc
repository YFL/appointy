#include <appointy_exception.h>

namespace appointy
{

Exception::Exception(const std::string &msg) :
    std::exception(),
    msg(msg)
{
    
}

auto Exception::what() const noexcept -> const char *
{
    return msg.c_str();
}

} // namespace appointy