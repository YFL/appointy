#include <answer.h>

#include <appointy_exception.h>

namespace appointy
{

Answer::Answer(const std::string &answer_signature_id, AnswerType answer_type) :
    answer_signature_id {answer_signature_id},
    answer_type {answer_type}
{

}

} // namespace appointy