#include <db_utils.h>

#include <json.hpp>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/oid.hpp>

#include <appointy_exception.h>
#include <choice_answer_signature.h>
#include <numeric_answer_signature.h>

namespace appointy
{

namespace util
{

using nlohmann::json;

auto question_to_bson(const Question &question) -> bsoncxx::document::value;
auto answer_signature_to_bson(const std::shared_ptr<AnswerSignature> &answer_signature) -> bsoncxx::document::value;
auto option_to_bson(const Option &option) -> bsoncxx::document::value;

auto service_to_bson(const Service &service) -> bsoncxx::document::value
{
    bsoncxx::builder::stream::document document;
    document << "name" << service.name << "price" << bsoncxx::from_json(service.price.to_json().dump());
    document << "duration" << std::stol(service.duration.to_json().dump());
    bsoncxx::builder::stream::array questions;
    for(auto &question : service.questions)
    {
        questions << question_to_bson(question);
    }

    return document << "questions" << questions << bsoncxx::builder::stream::finalize;
}

auto question_to_bson(const Question &question) -> bsoncxx::document::value
{
    bsoncxx::builder::stream::document document;

    return document << "text" << question.text << "answer_signature" << answer_signature_to_bson(question.answer_signature) << bsoncxx::builder::stream::finalize;
}

auto answer_signature_to_bson(const std::shared_ptr<AnswerSignature> &answer_signature) -> bsoncxx::document::value
{
    bsoncxx::builder::stream::document document;
    document << "type" << (std::string)answer_signature->type;
    document << "_id" << bsoncxx::oid {};
    if(answer_signature->type == AnswerSignatureType::SINGLE || answer_signature->type == AnswerSignatureType::MANY)
    {
        auto &as = dynamic_cast<ChoiceAnswerSignature &>(*answer_signature);
        bsoncxx::builder::stream::array options_bson_array;
        for(auto &option : as.options)
        {
            options_bson_array << option_to_bson(option);
        }

        return document << "options" << options_bson_array << bsoncxx::builder::stream::finalize;
    }
    else if(answer_signature->type == AnswerSignatureType::INT)
    {
        auto &as = dynamic_cast<NumericAnswerSignature<int> &>(*answer_signature);
        document << "price" << bsoncxx::from_json(as.price.to_json().dump()) << "duration" << std::stol(as.duration.to_json().dump());
        if(as.min)
        {
            document << "min" << as.min.value();
        }
        if(as.max)
        {
            document << "max" << as.max.value();
        }
        if(as.default_value)
        {
            document << "default_value" << as.default_value.value();
        }

        return document << bsoncxx::builder::stream::finalize;
    }
    else
    {
        auto &as = dynamic_cast<NumericAnswerSignature<double> &>(*answer_signature);
        document << "price" << bsoncxx::from_json(as.price.to_json().dump()) << "duration" << std::stol(as.duration.to_json().dump());
        if(as.min)
        {
            document << "min" << as.min.value();
        }
        if(as.max)
        {
            document << "max" << as.max.value();
        }
        if(as.default_value)
        {
            document << "default_value" << as.default_value.value();
        }

        return document << bsoncxx::builder::stream::finalize;
    }
}

auto option_to_bson(const Option &option) -> bsoncxx::document::value
{
    bsoncxx::builder::stream::document document;

    return document << "id" << static_cast<int32_t>(option.id) << "text" << option.text << "price" << bsoncxx::from_json(option.price.to_json().dump()) << "duration" << std::stol(option.duration.to_json().dump()) << bsoncxx::builder::stream::finalize;
}

} // namespace util

} // namespace appointy