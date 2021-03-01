#include <numeric_answer_signature.h>

namespace appointy
{

template<>
NumericAnswerSignature<int>::NumericAnswerSignature(const std::string &id, const std::optional<int> &min, const std::optional<int> &max, const std::optional<int> &default_value, const Time &duration, const Price &price) :
    AnswerSignature {id, AnswerSignatureType::INT},
    min {min},
    max {max},
    default_value {default_value},
    price {price},
    duration {duration}
{

}

template<>
NumericAnswerSignature<double>::NumericAnswerSignature(const std::string &id, const std::optional<double> &min, const std::optional<double> &max, const std::optional<double> &default_value, const Time &duration, const Price &price) :
    AnswerSignature(id, AnswerSignatureType::DOUBLE),
    min(min),
    max(max),
    default_value(default_value),
    price(price),
    duration(duration)
{

}

template<>
auto NumericAnswerSignature<int>::operator==(const AnswerSignature &rhs) const noexcept -> bool
{
    auto *nas = dynamic_cast<const NumericAnswerSignature<int> *>(&rhs);

    return nas && id == nas->id && type == nas->type && min == nas->min && max == nas->max && default_value == nas->default_value && price == nas->price && duration == nas->duration;
}

template<>
auto NumericAnswerSignature<double>::operator==(const AnswerSignature &rhs) const noexcept -> bool
{
    auto *nas = dynamic_cast<const NumericAnswerSignature<double> *>(&rhs);

    return nas && id == nas->id && type == nas->type && min == nas->min && max == nas->max && default_value == nas->default_value && price == nas->price && duration == nas->duration;
}

template<>
auto NumericAnswerSignature<int>::operator!=(const AnswerSignature &rhs) const noexcept -> bool
{
    return !(*this == rhs);
}

template<>
auto NumericAnswerSignature<double>::operator!=(const AnswerSignature &rhs) const noexcept -> bool
{
    return !(*this == rhs);
}

} // namespace appointy