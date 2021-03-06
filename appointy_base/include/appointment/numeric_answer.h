#ifndef _NUMERIC_ANSWER_H_
#define _NUMERIC_ANSWER_H_

#include <answer.h>

namespace appointy
{

template <typename NumericType>
struct NumericAnswer : public Answer
{
public:
    NumericType number;

public:
    NumericAnswer(const std::string &answer_signature_id, NumericType number);

public:
    virtual auto to_string() const -> std::string override
    {
        auto ret = (std::string)this->answer_type + "\n";
        ret += this->answer_signature_id + "\n";

        return ret + std::to_string(this->number) + "\n";
    }

    virtual auto to_json() const noexcept -> nlohmann::json override
    {
        nlohmann::json j = "{}"_json;
        j["answer_signature_id"] = this->answer_signature_id;
        j["type"] = std::string(this->answer_type);
        j["number"] = this->number;

        return j;
    }
};

} // naemspace appointy

#endif // _NUMERIC_ANSWER_H_
