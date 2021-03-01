#ifndef _CHOICE_ANSWER_SIGNATURE_H_
#define _CHOICE_ANSWER_SIGNATURE_H_

#include <vector>

#include <json.hpp>

#include <answer_signature.h>
#include <option.h>

namespace appointy
{

class ChoiceType
{
public:
    enum Type : uint8_t
    {
        SINGLE,
        MANY
    };

public:
    ChoiceType() = default;
    ChoiceType(Type t) : _type(t) {}
    ChoiceType(const ChoiceType &copy) = default;
    ChoiceType(ChoiceType &&move) = default;

public:
    auto operator=(const ChoiceType &copy) -> ChoiceType & = default;
    auto operator=(ChoiceType &&move) -> ChoiceType & = default;

public:
    operator Type() const { return _type; }
    operator std::string() const { switch(_type) { case SINGLE: return "single"; case MANY: return "many"; } }
    explicit operator bool() const = delete;

private:
    Type _type;
};

class ChoiceAnswerSignature : public AnswerSignature
{
public:
    const std::vector<Option> options;

public:
    explicit ChoiceAnswerSignature(const std::string &id, ChoiceType choice_type, const std::vector<Option> &options);

public:
    virtual auto operator==(const AnswerSignature &rhs) const noexcept -> bool override;
    virtual auto operator!=(const AnswerSignature &rhs) const noexcept -> bool override;

public:
    virtual auto to_string() const -> std::string override;
    virtual auto to_json() const -> nlohmann::json override;
};

} // namespace appointy

#endif // _CHOICE_ANSWER_H_