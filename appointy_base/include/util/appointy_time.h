#ifndef _APPOINTY_TIME_H_
#define _APPOINTY_TIME_H_

#include <ctime>

#include <json.hpp>

namespace appointy
{

struct Time
{
public:
    Time(const std::time_t time) noexcept;
    Time(int hours, int minutes, int seconds);

public:
    auto operator==(const Time &other) const -> bool;
    auto operator!=(const Time &other) const -> bool;
    auto operator<(const Time &other) const -> bool;
    auto operator>(const Time &other) const -> bool;
    auto operator<=(const Time &other) const -> bool;
    auto operator>=(const Time &other) const -> bool;
    auto operator+(const Time &other) const -> Time;
    auto operator-(const Time &other) const -> Time;
    template <typename IntOrDouble>
    auto operator*(IntOrDouble number) const -> Time
    {
        int _seconds = to_seconds() * number;
        int _minutes = _seconds / 60;
        _seconds %= 60;
        int _hours = _minutes / 60;
        _minutes %= 60;

        return Time {_hours, _minutes, _seconds};
    }

    template <typename IntOrDouble>
    auto operator/(IntOrDouble hours) const -> Time
    {
        return (double)1/hours * *this;
    }

public:
    auto hours() const -> int;
    auto minutes() const -> int;
    auto seconds() const -> int;

public:
    auto time() const -> std::time_t;

public:
    auto to_seconds() const -> int;

public:
    auto to_string() const -> std::string;
    auto to_json() const noexcept -> nlohmann::json;

private:
    const std::time_t _time;
};

} // namespace appointy

#endif // _APPOINTY_TIME_H_