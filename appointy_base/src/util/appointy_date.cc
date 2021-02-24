#include <appointy_date.h>

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

Date::Date(const std::time_t date) :
    _date {date}
{

}

Date::Date(int year, int month, int day) :
    _date {}
{
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

auto Date::year() const -> int
{
    return std::localtime(&_date)->tm_year + 1900;
}

auto Date::month() const -> int
{
    return std::localtime(&_date)->tm_mon + 1;
}

auto Date::day() const -> int
{
    return std::localtime(&_date)->tm_mday;
}

auto Date::date() const -> std::time_t
{
    return _date;
}

auto Date::is_leap() const -> bool
{
    return !(year() % 4);
}

auto Date::days_in_year() const -> int
{
    int days = 0;
    for(int i = 1; i < month(); i++)
    {
        days += days_in_month(i, is_leap());
    }
    
    return days + day();
}

auto Date::to_string() const -> std::string
{
    std::string ret {std::to_string(this->year()) + "."};
    ret += ::std::to_string(this->month()) + ".";
    ret += ::std::to_string(this->day());

    return ret;
}

auto Date::to_json() const -> json
{
    json j = _date;

    return j;
}

} // namespace appointy