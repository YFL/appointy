#include <choice_answer.h>

namespace appointy
{

using json = nlohmann::json;

ChoiceAnswer::ChoiceAnswer(const std::string &answer_signature_id, const std::vector<uint32_t> &ids) :
    Answer {answer_signature_id, AnswerType::CHOICE},
    ids {ids}
{
    
}

auto ChoiceAnswer::to_string() const -> std::string
{
    auto ret = std::string {"choice\n"};
    ret += "answer_signature_id: " + this->answer_signature_id + "\n";
    ret += "choices: ";
    for(auto id : this->ids)
    {
        ret += std::to_string(id) + " ";
    }

    return ret + "\n";
}

auto ChoiceAnswer::to_json() const -> json
{
    json j = "{}"_json;
    j["answer_signature_id"] = this->answer_signature_id;
    j["type"] = "choice";
    auto &choices_json = j["choices"] = "[]"_json;

    for(auto id : ids)
    {
        choices_json.push_back(id);
    }

    return j;
}

} // namespace appointy