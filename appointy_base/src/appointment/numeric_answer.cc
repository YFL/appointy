#include <numeric_answer.h>

namespace appointy
{

template<>
NumericAnswer<int>::NumericAnswer(const std::string &answer_signature_id, int number) :
    Answer {answer_signature_id, AnswerType::INT},
    number {number}
{

}

template<>
NumericAnswer<double>::NumericAnswer(const std::string &answer_signature_id, double number) :
    Answer {answer_signature_id, AnswerType::DOUBLE},
    number {number}
{
    
}

} // namespace appointy
