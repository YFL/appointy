#ifndef _JSON_PARSER_H_
#define _JSON_PARSER_H_

#include <json.hpp>

#include <answer.h>
#include <answer_signature.h>
#include <appointment.h>
#include <appointment_configuration.h>
#include <appointy_exception.h>
#include <appointy_date.h>
#include <appointy_time.h>
#include <choice_answer_signature.h>
#include <config_completion_time.h>
#include <numeric_answer_signature.h>
#include <price.h>
#include <question.h>
#include <service.h>
#include <service_configuration.h>

namespace appointy
{

class JSON_Parser
{
public:
    static auto parse_service(const nlohmann::json &service) -> Service;
    static auto parse_question(const nlohmann::json &question) -> Question;
    static auto parse_answer_signature(const nlohmann::json &answer_signature) -> AnswerSignature *;
    static auto parse_choice_answer_signature(const nlohmann::json &answer_signature) -> ChoiceAnswerSignature *;
    template <typename T>
    static auto parse_numeric_answer_signature(const nlohmann::json &answer_signature) -> NumericAnswerSignature<T> *
    {
        std::optional<T> min, max, def;
        min = max = def = std::nullopt;
        auto id = std::string {};
        try
        {
            auto id_json = answer_signature.at("_id");
            if(id_json.is_object())
            {
                id = id_json.at("$oid");
            }
            else
            {
                id = id_json;
            }
            
        }
        catch(const nlohmann::detail::out_of_range &)
        {
            // Intentionally left blank
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
    static auto parse_option(const nlohmann::json &option) -> Option;
    static auto parse_date(const nlohmann::json &date) -> Date;
    static auto parse_time(const nlohmann::json &time) -> Time;
    static auto parse_price(const nlohmann::json &price) -> Price;

public:
    static auto parse_answer(const nlohmann::json &answer) -> Answer *;
    static auto parse_choices(const nlohmann::json &choices) -> std::vector<uint32_t>;
    static auto parse_appointment_configuration(const nlohmann::json &configuration) -> AppointmentConfiguration;
    static auto parse_appointment(const nlohmann::json &appointment) -> Appointment;
    static auto parse_service_configuration(const nlohmann::json &service_configuration) -> ServiceConfiguration;
    static auto parse_config_completion_time(const nlohmann::json &config_completion_time) -> ConfigCompletionTime;
};

} // namespace appointy

#endif // _JSON_PARSER_H_