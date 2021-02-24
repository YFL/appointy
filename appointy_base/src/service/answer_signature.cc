#include <answer_signature.h>

#include <iostream>

namespace appointy
{

AnswerSignature::AnswerSignature(const std::string &id, AnswerSignatureType type) :
    id {id},
    type {type}
{
    
}

} // namespace appointy