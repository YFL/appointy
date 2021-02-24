#include <price.h>

#include <json.hpp>

namespace appointy
{

using json = nlohmann::json;

Price::Price(int unit, int hundredth) :
    unit {unit},
    hundredth {hundredth}
{
    
}

Price::Price(const Price &copy) :
    unit {copy.unit},
    hundredth {copy.hundredth}
{

}

Price::Price(Price &&move) :
    Price(move)
{

}

auto Price::operator+(const Price &other) const -> Price
{
    int _unit = unit + other.unit;
    int _hundredth = hundredth + other.hundredth;
    _unit += _hundredth / 100;
    _hundredth %= 100; 

    return Price {_unit, _hundredth};
}

auto Price::operator-(const Price &other) const -> Price
{
    int _hundredth = hundredth - other.hundredth;
    int _unit = unit - other.unit;
    _unit += _hundredth / 100;
    _hundredth %= 100;

    return Price {_unit, _hundredth};
}

auto Price::to_string() const -> std::string
{
    std::string ret {std::to_string(this->unit) + "."};

    return ret + (this->hundredth < 10 ? "0" + std::to_string(this->hundredth) : std::to_string(this->hundredth));
}

auto Price::to_json() const -> json
{
    json j = "{}"_json;
    j["main"] = this->unit;
    j["hundredth"] = this->hundredth;

    return j;
}

} // namespace appointy