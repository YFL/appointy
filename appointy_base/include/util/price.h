#ifndef _PRICE_H_
#define _PRICE_H_

#include <string>

#include <json.hpp>

namespace appointy
{

struct Price
{
public:
    const int unit;
    const int hundredth;

public:
    Price(int unit, int hundredth);
    Price (const Price &copy);
    Price (Price &&move);

public:
    auto operator+(const Price &other) const -> Price;
    auto operator-(const Price &other) const -> Price;
    auto operator==(const Price &rhs) const noexcept -> bool;
    auto operator!=(const Price &rhs) const noexcept -> bool;

public:
    auto to_string() const -> std::string;
    auto to_json() const noexcept -> nlohmann::json;
};


} // namespace appointy

#endif // _PRICE_H_