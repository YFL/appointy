#include <appointy_time.h>

#include <string>

#include <appointy_exception.h>

namespace appointy
{

using json = nlohmann::json;

Time::Time(const std::time_t time) noexcept :
    _time {time}
{

}

Time::Time(int hours, int minutes, int seconds) :
    _time {}
{
    if(hours < 0 || hours > 23)
    {
        throw Exception {"Hours must be at least 0"};
    }
    if(minutes < 0 || minutes > 59)
    {
        throw Exception {"Minutes must be from the interval <0, 59>"};
    }
    if(seconds < 0 || seconds > 59)
    {
        throw Exception {"Seconds must be from the interval <0, 59>"};
    }
    std::tm time;
    std::memset(&time, 0, sizeof(std::tm));
    time.tm_hour = hours;
    time.tm_min = minutes;
    time.tm_sec = seconds;

    const_cast<std::time_t &>(_time) = std::mktime(&time);
}

auto Time::operator==(const Time &t) const -> bool
{
    return this->_time == t._time;
}

auto Time::operator!=(const Time &t) const -> bool
{
    return this->_time != t._time;
}

auto Time::operator<(const Time &t) const -> bool
{
    return this->_time < t._time;
}

auto Time::operator>(const Time &t) const -> bool
{
    return this->_time > t._time;
}

auto Time::operator<=(const Time &t) const -> bool
{
    return this->_time <= t._time;
}

auto Time::operator>=(const Time &t) const -> bool
{
    return this->_time >= t._time;
}

auto Time::operator+(const Time &other) const -> Time
{
    int _seconds = seconds() + other.seconds();
    int _minutes = minutes() + other.minutes() + _seconds / 60;
    _seconds %= 60;
    int _hours = hours() + other.hours() + _minutes / 60;
    _minutes %= 60;

    return Time {_hours, _minutes, _seconds};
}

auto Time::operator-(const Time &other) const -> Time
{
    Time *left = nullptr;
    Time *right = nullptr;
    if(*this > other)
    {
        left = const_cast<Time *>(this);
        right = const_cast<Time *>(&other);
    }
    else
    {
        left = const_cast<Time *>(&other);
        right = const_cast<Time *>(this);
    }
    
    int _hours = left->hours() - right->hours();
    int _minutes = left->minutes() - right->minutes();
    int _seconds = left->seconds() - right->seconds();
    if(_seconds < 0)
    {
        --_minutes;
        _seconds = 60 + _seconds;
    }
    if(_minutes < 0)
    {
        _hours--;
        _minutes = 60 + _minutes;
    }

    return Time {_hours, _minutes, _seconds};
}

auto Time::hours() const -> int
{
    return std::localtime(&_time)->tm_hour;
}

auto Time::minutes() const -> int
{
    return std::localtime(&_time)->tm_min;
}

auto Time::seconds() const -> int
{
    return std::localtime(&_time)->tm_sec;
}

auto Time::time() const -> std::time_t
{
    return _time;
}

auto Time::to_seconds() const -> int
{
    return seconds() + 60 * minutes() + 3600 * hours();
}

auto Time::to_string() const -> std::string
{
    std::string ret;
    ret += std::to_string(this->hours()) + ":";
    ret += std::to_string(this->minutes()) + ":";
    
    return ret + std::to_string(this->seconds());
}

auto Time::to_json() const noexcept -> json
{
    json j = _time;

    return j;
}

} // namespace appointy