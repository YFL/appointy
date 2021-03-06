#ifndef _CHOICE_ANSWER_H_
#define _CHOICE_ANSWER_H_

#include <cstdint>
#include <string>
#include <vector>

#include <json.hpp>

#include <answer.h>

namespace appointy
{

struct ChoiceAnswer : public Answer
{
public:
    const std::vector<uint32_t> ids;
    
public:
    ChoiceAnswer(const std::string &answer_signature_id, const std::vector<uint32_t> &ids);

public:
    virtual auto to_string() const -> std::string override;
    virtual auto to_json() const noexcept  -> nlohmann::json override;
};

} // namespace appointy

#endif // _CHOICE_ANSWER_H_