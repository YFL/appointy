#ifndef _JSON_PARSER_H_
#define _JSON_PARSER_H_

#include <json.hpp>

#include <answer.h>
#include <answer_signature.h>
#include <appointment.h>
#include <appointment_request.h>
#include <appointy_exception.h>
#include <appointy_date.h>
#include <appointy_time.h>
#include <choice_answer_signature.h>
#include <numeric_answer_signature.h>
#include <price.h>
#include <question.h>
#include <service.h>

namespace appointy
{

using json = nlohmann::json;

class JSON_Parser
{
public:
    static auto parse_service(const json &service) -> Service;
    static auto parse_question(const json &question) -> Question;
    static auto parse_answer_signature(const json &answer_signature) -> AnswerSignature *;
    static auto parse_choice_answer_signature(const json &answer_signature) -> ChoiceAnswerSignature *;
    template <typename T>
    static auto parse_numeric_answer_signature(const json &answer_signature) -> NumericAnswerSignature<T> *
    {
        std::optional<T> min, max, def;
        min = max = def = std::nullopt;
        std::string id;
        try
        {
            id = answer_signature.at("id");
        }
        catch(const nlohmann::detail::out_of_range &)
        {
            id = "0";
        }
        
        try
        {
            min = answer_signature.at("min");
        }
        catch(const nlohmann::detail::out_of_range &)
        {
            // min is an optional parameter
        }

        try
        {
            max = answer_signature.at("max");
        }
        catch(const nlohmann::detail::out_of_range &)
        {
            // max is an optional parameter
        }
        
        try
        {
            def = answer_signature.at("default");
        }
        catch(const nlohmann::detail::out_of_range &)
        {
            // default is an optional parameter
        }
        
        try
        {
            Time d = parse_time(answer_signature.at("duration"));

            try
            {
                return new NumericAnswerSignature<T>(id, min, max, def, d, parse_price(answer_signature.at("price")));
            }
            catch(const nlohmann::detail::out_of_range &)
            {
                throw Exception("No price in answer_signature\n" + answer_signature.dump());
            }
        }
        catch(const nlohmann::detail::out_of_range &)
        {
            throw Exception("No duration in answer_signature\n" + answer_signature.dump());
        }
    }
    static auto parse_option(const json &option) -> Option;
    static auto parse_date(const json &date) -> Date;
    static auto parse_time(const json &time) -> Time;
    static auto parse_price(const json &price) -> Price;

public:
    static auto parse_answer(const json &answer) -> Answer *;
    static auto parse_choices(const json &choices) -> std::vector<uint32_t>;
    static auto parse_appointment_request(const json &request) -> AppointmentRequest;
    static auto parse_appointment(const json &appointment) -> Appointment;
};

} // namespace appointy

#endif // _JSON_PARSER_H_