#ifndef _QUESTION_H_
#define _QUESTION_H_

#include <memory>

#include <answer_signature.h>

namespace appointy
{

struct Question
{
public:
    const std::string text;
    const std::shared_ptr<AnswerSignature> answer_signature;

public:
    Question(const std::string &text, std::shared_ptr<AnswerSignature> answer);

public:
    auto to_string() const -> std::string;
    auto to_json() const -> nlohmann::json;

};

} // namespace appointy

#endif // _QUESTION_H_