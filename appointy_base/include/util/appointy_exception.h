#ifndef _APPOINTY_EXCEPTION_H_
#define _APPOINTY_EXCEPTION_H_

#include <exception>
#include <string>

namespace appointy
{

class Exception : public std::exception
{
public:
    Exception(const std::string &what);

public:
    auto what() const noexcept -> const char * override;

private:
    std::string msg;
};

} // namespace appointy

#endif // _APPOINTY_EXCEPTION_H_