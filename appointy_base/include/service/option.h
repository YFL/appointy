#ifndef _CHOICE_H_
#define _CHOICE_H_

#include <string>

#include <appointy_time.h>
#include <price.h>

namespace appointy
{

struct Option
{
public:
    const uint32_t id;
    const std::string text;
    const Price price;
    const Time duration;

public:
    Option(uint32_t id, const std::string &text, const Price &price, const Time &duration);

public:
    auto operator==(const Option &rhs) const noexcept -> bool;
    auto operator!=(const Option &rhs) const noexcept -> bool;

public:
    auto to_string() const -> std::string;
    auto to_json() const -> nlohmann::json;
};

} // namespace appointy

#endif // _CHOICE_H_