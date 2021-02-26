#ifndef _APPOINTY_DATE_H_
#define _APPOINTY_DATE_H_

#include <string>
#include <ctime>

#include <json.hpp>

namespace appointy
{

struct Date
{
public:
    Date(const std::time_t date) noexcept;
    Date(int year, int month, int day);

public:
    auto operator==(const Date &other) const -> bool;
    auto operator!=(const Date &other) const -> bool;
    auto operator<(const Date &other) const -> bool;
    auto operator>(const Date &other) const -> bool;
    auto operator<=(const Date &other) const -> bool;
    auto operator>=(const Date &other) const -> bool;
    auto operator+(int days) const -> Date;
    auto operator++(int days) const -> Date;

public:
    auto year() const noexcept -> int;
    auto month() const noexcept -> int;
    auto day() const noexcept -> int;

public:
    auto date() const noexcept -> std::time_t;

public:
    auto is_leap() const noexcept -> bool;

public:
    auto days_in_year() const noexcept -> int;

public:
    auto to_string() const noexcept -> std::string;
    auto to_json() const noexcept -> nlohmann::json;

public:
    const std::time_t _date;
};

} // namespace appointy

#endif // _APPOINTY_DATE_H_