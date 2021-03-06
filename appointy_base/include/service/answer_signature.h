#ifndef _ANSWER_SIGNATURE_H_
#define _ANSWER_SIGNATURE_H_

#include <string>

#include <json.hpp>

namespace appointy
{

class AnswerSignatureType
{
public:
    enum Type : uint8_t
    {
        SINGLE,
        MANY,
        INT,
        DOUBLE
    };

public:
    AnswerSignatureType() = default;
    AnswerSignatureType(Type t) : _type(t) {}
    AnswerSignatureType(const AnswerSignatureType &copy) = default;
    AnswerSignatureType(AnswerSignatureType &&move) = default;

public:
    auto operator=(const AnswerSignatureType &copy) -> AnswerSignatureType & = default;
    auto operator=(AnswerSignatureType &&move) -> AnswerSignatureType & = default;

public:
    auto operator==(const AnswerSignatureType &other) const -> bool { return this->_type == other._type; }
    auto operator==(Type t) const -> bool { return this->_type == t; }
    auto operator!=(const AnswerSignatureType &other) const -> bool { return !(*this == other); }
    auto operator!=(Type t) const -> bool { return !(*this == t); }

public:
    explicit operator Type() const { return _type; }
    explicit operator std::string() const { switch(_type) { case SINGLE: return std::string {"single"}; case MANY: return std::string {"many"}; case INT: return std::string {"int"}; case DOUBLE: return std::string {"double"}; } return "Error"; }
    explicit operator bool() const = delete;

private:
    Type _type;
};

struct AnswerSignature
{
public:
    const std::string id;
    const AnswerSignatureType type;

public:
    virtual ~AnswerSignature() = default;

public:
    virtual auto operator==(const AnswerSignature &rhs) const noexcept -> bool = 0;
    virtual auto operator!=(const AnswerSignature &rhs) const noexcept -> bool = 0;

protected:
    AnswerSignature(const std::string &id, AnswerSignatureType type);

public:
    virtual auto to_string() const -> std::string = 0;
    virtual auto to_json() const noexcept -> nlohmann::json = 0;
};

} // namespace appointy

#endif // _ANSWER_SIGNATURE_H_