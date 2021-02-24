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
    Date(const std::time_t date);
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
    auto year() const -> int;
    auto month() const -> int;
    auto day() const -> int;

public:
    auto date() const -> std::time_t;

public:
    auto is_leap() const -> bool;

public:
    auto days_in_year() const -> int;

public:
    auto to_string() const -> std::string;
    auto to_json() const -> nlohmann::json;

public:
    const std::time_t _date;
};

} // namespace appointy

#endif // _APPOINTY_DATE_H_