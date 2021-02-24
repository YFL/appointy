#include <json_parser.h>

#include <choice_answer.h>
#include <numeric_answer.h>

namespace appointy
{

auto JSON_Parser::parse_service(const json &service) -> Service
{
    std::string id {"0"};
    try
    {
        id = service.at("id");
    }
    catch(const nlohmann::detail::out_of_range &)
    {
        
    }
    
    std::string name;
    try
    {
        name = service.at("name");
    }
    catch(const nlohmann::detail::out_of_range &)
    {
        throw Exception {"No name in service\n" + service.dump()};
    }

    try
    {
        Time duration = JSON_Parser::parse_time(service.at("duration"));
        
        try
        {
            Price price = JSON_Parser::parse_price(service.at("price"));

            std::vector<Question> questions;
            try
            {
                auto &questions_json = service.at("questions");
                for(auto &question : questions_json)
                {
                    try
                    {
                        questions.push_back(JSON_Parser::parse_question(question));
                    }
                    catch(const Exception &e)
                    {
                        throw Exception {"Couldn't parse question:\n" + question.dump() + "\nWithError: " + e.what()};
                    }
                }

                return Service {id, name, price, duration, questions};
            }
            catch(const nlohmann::detail::out_of_range &)
            {
                // intentionally left blank: questions are optional
            }

            return Service {id, name, price, duration, questions};
        }
        catch(const nlohmann::detail::out_of_range &)
        {
            throw Exception {"No price in service\n" + service.dump()};
        }
    }
    catch(const nlohmann::detail::out_of_range &)
    {
        throw Exception {"No duration in service\n" + service.dump()};
    }
    catch(const Exception &e)
    {
        throw Exception {"Couldn't parse service:\n" + service.dump() + "\nWith error: " + e.what()};
    }
}

auto JSON_Parser::parse_question(const json &question) -> Question
{
    std::string text;
    try
    {
        text = question.at("text");
    }
    catch(const nlohmann::detail::out_of_range &)
    {
        throw Exception("No text in question\n" + question.dump());
    }

    try
    {
        return Question {text, std::shared_ptr<AnswerSignature>(parse_answer_signature(question.at("answer_signature")))};
    }
    catch(const nlohmann::detail::out_of_range &)
    {
        throw Exception("No answer signiture in question\n" + question.dump());
    }
}

auto JSON_Parser::parse_answer_signature(const json &answer_signature) -> AnswerSignature *
{
    try
    {
        std::string type = answer_signature.at("type");
        if(type == "single" || type == "many")
        {
            return parse_choice_answer_signature(answer_signature);
        }
        else if(type == "int")
        {
            return parse_numeric_answer_signature<int>(answer_signature);
        }
        else if(type == "double")
        {
            return parse_numeric_answer_signature<double>(answer_signature);
        }
        else
        {
            throw Exception("The type specified is invalid\n" + answer_signature.dump());
        }
    }
    catch(nlohmann::detail::out_of_range &)
    {
        throw Exception("No type in answer_signature\n" + answer_signature.dump());
    }
}

auto JSON_Parser::parse_choice_answer_signature(const json &answer_signature) -> ChoiceAnswerSignature *
{
    std::string type_string;
    try
    {
        type_string = answer_signature.at("type");
    }
    catch(const nlohmann::detail::out_of_range &)
    {
        throw Exception("No type in answer_signature\n" + answer_signature.dump());
    }

    if(type_string != "single" && type_string != "many")
    {
        throw Exception("answer_signature's type is not a choice type" + answer_signature.dump());
    }

    ChoiceType type = type_string == "single" ? ChoiceType::SINGLE : ChoiceType::MANY;

    std::vector<Option> options;
    try
    {
        auto options_json = answer_signature.at("options");
        for(auto &option : options_json)
        {
            options.push_back(parse_option(option));
        }
    }
    catch(const nlohmann::detail::out_of_range &)
    {
        throw Exception("No options in answer_signature\n" + answer_signature.dump());
    }

    return new ChoiceAnswerSignature("0", type, options);
}

auto JSON_Parser::parse_option(const json &option) -> Option
{
    std::string text;
    try
    {
        text = option.at("text");
    }
    catch(const nlohmann::detail::out_of_range &)
    {
        throw Exception("No text in option\n" + option.dump());
    }

    std::unique_ptr<Price> p;
    try
    {
        p = std::unique_ptr<Price>(new Price {JSON_Parser::parse_price(option["price"])});
    }
    catch(const nlohmann::detail::out_of_range &)
    {
        throw Exception("No price in option\n" + option.dump());
    }

    try
    {
        return Option {0u, text, *p, JSON_Parser::parse_time(option["duration"])};
    }
    catch(const nlohmann::detail::out_of_range&)
    {
        throw Exception("No duration in option\n" + option.dump());
    }
}

auto JSON_Parser::parse_date(const json &date) -> Date
{
    if(date.is_number_integer())
    {
        return Date {date};
    }
    try
    {
        int year = date.at("year");
        try
        {
            int month = date.at("month");

            try
            {
                return {year, month, date.at("day")};
            }
            catch(const nlohmann::detail::out_of_range &)
            {
                throw Exception("No day in date" + date.dump());
            }
        }
        catch(const nlohmann::detail::out_of_range &)
        {
            throw Exception ("No month in date: " + date.dump());
        }
    }
    catch(const nlohmann::detail::out_of_range &)
    {
        throw Exception("No year in date: " + date.dump());
    }
}

auto JSON_Parser::parse_time(const json &time) -> Time
{
    int hours, mins;
    if(time.is_number_integer())
    {
        return Time {time};
    }
    try
    {
        hours = time.at("hours");
    }
    catch(const nlohmann::detail::out_of_range&)
    {
        throw Exception("No hours in duration\n" + time.dump());
    }
    try
    {
        mins = time.at("minutes");
    }
    catch(const nlohmann::detail::out_of_range&)
    {
        throw Exception("No minutes in duration\n" + time.dump());
    }
    try
    {
        return Time {hours, mins, time.at("seconds")};
    }
    catch(const nlohmann::detail::out_of_range&)
    {
        throw Exception("No seconds in duration\n" + time.dump());
    }
}

auto JSON_Parser::parse_price(const json &price) -> Price
{
    int main, hundredth;
    try
    {
        main = price.at("main");
    }
    catch(const nlohmann::detail::out_of_range&)
    {
        throw Exception("No main in price\n" + price.dump());
    }
    try
    {
        return Price {main, price.at("hundredth")};
    }
    catch(const nlohmann::detail::out_of_range&)
    {
        throw Exception("No hundredth in price\n" + price.dump());
    }
}

auto JSON_Parser::parse_answer(const json &answer) -> Answer *
{
    std::string type;
    try
    {
        type = answer.at("type");
    }
    catch(const nlohmann::detail::out_of_range &)
    {
        throw Exception("No type in answer: " + answer.dump());
    }

    std::string answer_signature_id;
    try
    {
        answer_signature_id = answer.at("answer_signature_id");
    }
    catch(const nlohmann::detail::out_of_range &)
    {
        throw Exception("No answer_signature_id in answer: " + answer.dump());
    }

    if(type == "choice")
    {
        try
        {

            return new ChoiceAnswer {answer_signature_id, parse_choices(answer.at("choices"))};
        }
        catch(const nlohmann::detail::out_of_range &)
        {
            throw Exception("No choices in answer: " + answer.dump());
        }
    }

    if(type == "int")
    {
        try
        {
            return new NumericAnswer<int> {answer_signature_id, answer.at("number")};
        }
        catch(const nlohmann::detail::out_of_range &)
        {
            throw Exception("No number in answer: " + answer.dump());
        }
    }

    if(type == "double")
    {
        try
        {
            return new NumericAnswer<int> {answer_signature_id, answer.at("number")};
        }
        catch(const nlohmann::detail::out_of_range &)
        {
            throw Exception("No number in answer: " + answer.dump());
        }
    }

    throw Exception("Invalid type in answer: " + answer.dump());
}

auto JSON_Parser::parse_choices(const json &choices) -> std::vector<uint32_t>
{
    std::vector<uint32_t> ret;
    for(auto &c : choices)
    {
        ret.push_back(c);
    }

    return ret;
}

auto JSON_Parser::parse_appointment_request(const json &request) -> AppointmentRequest
{
    try
    {
        Date first_date = parse_date(request.at("first_date"));
        try
        {
            Date last_date = parse_date(request.at("last_date"));
            
            try
            {
                Time interval_start = parse_time(request.at("interval_start"));

                try
                {
                    Time interval_end = parse_time(request.at("interval_end"));
                    std::string service_id;
                    try
                    {
                        service_id = request.at("service_id");
                    }
                    catch(const nlohmann::detail::out_of_range &)
                    {
                        throw Exception("No service id in request: " + request.dump());
                    }

                    std::vector<std::shared_ptr<Answer>> answers;
                    try
                    {
                        auto &answers_json = request.at("answers");
                        for(auto &answer : answers_json)
                        {
                            answers.emplace_back(parse_answer(answer));
                        }
                    }
                    catch(const nlohmann::detail::out_of_range &)
                    {
                        throw Exception("No answers in request: " + request.dump());
                    }

                    return {first_date, last_date, interval_start, interval_end, service_id, answers};
                }
                catch(const nlohmann::detail::out_of_range &)
                {
                    throw Exception("No interval_end in request: " + request.dump());
                }
            }
            catch(const nlohmann::detail::out_of_range &)
            {
                throw Exception("No interval_start in request: " + request.dump());
            }
        }
        catch(const nlohmann::detail::out_of_range &)
        {
            throw Exception("No last_date in request: " + request.dump());
        }
    }
    catch(const nlohmann::detail::out_of_range &)
    {
        throw Exception("No first_date in request: " + request.dump());
    }
}

auto JSON_Parser::parse_appointment(const json &appointment) -> Appointment
{
    std::string id {"0"};
    try
    {
        id = appointment.at("id");
    }
    catch(const nlohmann::detail::out_of_range &)
    {
        
    }
    
    try
    {
        Date date = JSON_Parser::parse_date(appointment.at("date"));

        try
        {
            Time start = JSON_Parser::parse_time(appointment.at("start"));

            try
            {
                Time end = JSON_Parser::parse_time(appointment.at("end"));

                try
                {
                    return Appointment {id, date, start, end, JSON_Parser::parse_appointment_request(appointment.at("request"))};
                }
                catch(const nlohmann::detail::out_of_range &)
                {
                    throw Exception {"No request in appointment:\n" + appointment.dump()};
                }
                catch(const Exception &e)
                {
                    throw Exception {"Couldn't parse appointment request from appointment:\n" + appointment.dump() + "\nWith error: " + e.what()};
                }
            }
            catch(const nlohmann::detail::out_of_range &)
            {
                throw Exception {"No ending time in appointment:\n" + appointment.dump()};
            }
        }
        catch(const nlohmann::detail::out_of_range &)
        {
            throw Exception {"No start time in appointment:\n" + appointment.dump()};
        }
        
    }
    catch(const nlohmann::detail::out_of_range &)
    {
        throw Exception {"No date in appointment:\n" + appointment.dump()};
    }
}

} // namespace appointy
