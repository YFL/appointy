#ifndef _NUMERIC_ANSWER_SIGNATURE_H_
#define _NUMERIC_ANSWER_SIGNATURE_H_

#include <optional>

#include <answer_signature.h>
#include <appointy_time.h>
#include <price.h>

namespace appointy
{

template <typename NumericType>
class NumericAnswerSignature : public AnswerSignature
{
public:
    const std::optional<NumericType> min;
    const std::optional<NumericType> max;
    const std::optional<NumericType> default_value;
    const Price price;
    const Time duration;

public:
    NumericAnswerSignature(const std::string &id, const std::optional<NumericType> &min, const std::optional<NumericType> &max, const std::optional<NumericType> &default_value, const Time &duration, const Price &price);

public:
    auto operator==(const AnswerSignature &rhs) const noexcept -> bool override;
    auto operator!=(const AnswerSignature &rhs) const noexcept -> bool override;

public:
    virtual auto to_string() const -> std::string override
    {
        std::string ret {this->id + "\n" + (std::string)this->type + "\n"};
        ret += this->min ? "min: " + std::to_string(this->min.value()) + "\n" : "";
        ret += this->max ? "max: " + std::to_string(this->max.value()) + "\n" : "";
        ret += this->default_value ? "default: " + std::to_string(this->default_value.value()) + "\n" : "";
        ret += price.to_string() + "\n";
        
        return ret + duration.to_string() + "\n";
    }
    
    virtual auto to_json() const noexcept -> nlohmann::json override
    {
        nlohmann::json j = "{}"_json;
        j["_id"] = this->id;
        j["type"] = this->type == AnswerSignatureType::INT ? "int" : "double";
        if(this->min)
        {
            j["min"] = this->min.value();
        }
        if(this->max)
        {
            j["max"] = this->max.value();
        }
        if(this->default_value)
        {
            j["default"] = this->default_value.value();
        }
        j["price"] = this->price.to_json();
        j["duration"] = this->duration.to_json();

        return j;
    }
};

} // namespace appointy

#endif // _NUMERIC_ANSWER_H_