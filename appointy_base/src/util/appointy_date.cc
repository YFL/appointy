#include <appointy_date.h>
#include <appointy_exception.h>

namespace appointy
{

using json = nlohmann::json;

auto days_in_month = [](int index, bool is_leap) -> int
{
    std::unordered_map<int, int> days_in_months
    {
        {1, 31}, {3, 31}, {4, 30}, {5, 31}, {6, 30}, {7, 31}, {8, 31}, {9, 30}, {10, 31}, {11, 30}, {12, 31}
    };

    if(index == 2)
    {
        if(is_leap)
        {
            return 29;
        }

        return 28;
    }

    return days_in_months[index];
};

Date::Date(const std::time_t date) noexcept :
    _date {date}
{

}

Date::Date(int year, int month, int day) :
    _date {}
{
    if(year < 1900)
    {
        throw Exception {"Year must be at least 1900"};
    }
    if(month < 1 || month > 12)
    {
        throw Exception {"Month must be from interval <1, 12>"};
    }
    if(day > days_in_month(month, !(year % 4)) || day < 1)
    {
        throw Exception {"The day doesn't correspond to the number of days in the given month"};
    }
    std::tm date;
    std::memset(&date, 0, sizeof(std::tm));
    date.tm_year = year - 1900;
    date.tm_mon = month - 1;
    date.tm_mday = day;
    
    const_cast<std::time_t &>(_date) = std::mktime(&date);
}

auto Date::operator==(const Date &d) const -> bool
{
    return this->_date == d._date;
}

auto Date::operator!=(const Date &d) const -> bool
{
    return this->_date != d._date;
}

auto Date::operator<(const Date &d) const -> bool
{
    return this->_date < d._date;
}

auto Date::operator>(const Date &d) const -> bool
{
    return this->_date > d._date;
}

auto Date::operator<=(const Date &d) const -> bool
{
    return this->_date <= d._date;
}

auto Date::operator>=(const Date &d) const -> bool
{
    return this->_date >= d._date;
}

auto Date::operator+(int days) const -> Date
{
    int _day = day() + days;
    int _month = month();
    
    int DIM;
    while((DIM = days_in_month(_month, is_leap())) < _day)
    {
        _day -= DIM;
        _month++;
    }

    int _year = year();
    _year += _month / 12;
    _month = _month % 12;

    return Date {_year, _month, _day};
}

auto Date::operator++(int days) const -> Date
{
    return *this + 1;
}

auto Date::year() const noexcept -> int
{
    return std::localtime(&_date)->tm_year + 1900;
}

auto Date::month() const noexcept -> int
{
    return std::localtime(&_date)->tm_mon + 1;
}

auto Date::day() const noexcept -> int
{
    return std::localtime(&_date)->tm_mday;
}

auto Date::date() const noexcept -> std::time_t
{
    return _date;
}

auto Date::is_leap() const noexcept -> bool
{
    return !(year() % 4);
}

auto Date::days_in_year() const noexcept -> int
{
    int days = 0;
    for(int i = 1; i < month(); i++)
    {
        days += days_in_month(i, is_leap());
    }
    
    return days + day();
}

auto Date::to_string() const noexcept -> std::string
{
    std::string ret {std::to_string(this->year()) + "."};
    ret += ::std::to_string(this->month()) + ".";
    ret += ::std::to_string(this->day());

    return ret;
}

auto Date::to_json() const noexcept -> json
{
    json j = _date;

    return j;
}

} // namespace appointy