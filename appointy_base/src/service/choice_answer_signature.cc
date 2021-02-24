#include <choice_answer_signature.h>

namespace appointy
{

using json = nlohmann::json;

ChoiceAnswerSignature::ChoiceAnswerSignature(const std::string &id, ChoiceType type, const std::vector<Option> &options) :
    AnswerSignature {id, static_cast<AnswerSignatureType>((AnswerSignatureType::Type)(uint8_t)type)},
    options {options}
{

}

auto ChoiceAnswerSignature::to_string() const -> std::string
{
    std::string ret {this->id + "\n"};
    std::string type(this->type);
    ret += type + "\n";
    ret += "options:\n";
    for(auto &option : this->options)
    {
        ret += option.to_string() + "\n";
    }
    ret.pop_back();

    return ret;
}

auto ChoiceAnswerSignature::to_json() const -> json
{
    json j = "{}"_json;
    j["id"] = this->id;
    j["type"] = (std::string)this->type;
    j["options"] = "[]"_json;
    auto &json_options = j["options"];
    for(auto &o : options)
    {
        json_options.push_back(o.to_json());
    }

    return j;
}

} // namespace appointy