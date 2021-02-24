#ifndef _ANSWER_H_
#define _ANSWER_H_

#include <cstdint>

#include <json.hpp>

namespace appointy
{

class AnswerType
{
public:
    enum Type : uint8_t
    {
        CHOICE,
        INT,
        DOUBLE
    };

public:
    AnswerType() = default;
    AnswerType(Type t) : _type(t) { }
    AnswerType(const AnswerType &copy) = default;
    AnswerType(AnswerType &&move) = default;

public:
    auto operator=(const AnswerType &copy) -> AnswerType & = default;
    auto operator=(AnswerType &&move) -> AnswerType & = default;

public:
    operator Type() const { return _type; }
    operator std::string() const { switch((Type)*this) { case CHOICE: return "choice"; case INT: return "int"; case DOUBLE: return "double"; } return "Error"; }
    explicit operator bool() const = delete;

public:
    auto to_string() const -> std::string;

private:
    Type _type;
};

struct Answer
{
public:
    const std::string answer_signature_id;
    const AnswerType answer_type;

public:
    virtual ~Answer() = default;

public:
    virtual auto to_string() const -> std::string = 0;
    virtual auto to_json() const -> nlohmann::json = 0;

protected:
    Answer(const std::string &answer_signature_id, AnswerType answer_type);
};

} // namespace appointy

#endif // _ANSWER_H_