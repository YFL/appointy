#include <question.h>

#include <json.hpp>

namespace appointy
{

using json = nlohmann::json;

Question::Question(const std::string &text, std::shared_ptr<AnswerSignature> answer) :
    text {text},
    answer_signature {answer}
{

}

auto Question::to_string() const -> std::string
{
    return this->text + "\n" + this->answer_signature->to_string();
}

auto Question::to_json() const -> json
{
    json j = "{}"_json;
    j["text"] = this->text;
    j["answer_signature"] = this->answer_signature->to_json();

    return j;
}

} // namespace appointy