#include <option.h>

#include <json.hpp>

namespace appointy
{

using json = nlohmann::json;

Option::Option(uint32_t id, const std::string &text, const Price &p, const Time &duration) :
    id {id},
    text {text},
    price {p},
    duration {duration}
{

}

auto Option::operator==(const Option &rhs) const noexcept -> bool
{
    return id == rhs.id && text == rhs.text && price == rhs.price && duration == rhs.duration;
}

auto Option::operator!=(const Option &rhs) const noexcept -> bool
{
    return !(*this == rhs);
}

auto Option::to_string() const -> std::string
{
    std::string ret {std::to_string(this->id) + "\n"};
    ret += this->text + "\n";
    ret += this->price.to_string() + "\n";

    return ret + this->duration.to_string() + "\n";
}

auto Option::to_json() const noexcept -> json
{
    json j = "{}"_json;
    j["id"] = this->id;
    j["text"] = this->text;
    j["price"] = this->price.to_json();
    j["duration"] = this->duration.to_json();

    return j;
}

} // namespace appointy