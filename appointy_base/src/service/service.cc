#include <service.h>

#include <json.hpp>

#include <appointy_exception.h>

namespace appointy
{

using json = nlohmann::json;

//---------------------------------------members------------------------------------------

Service::Service(const std::string &id, const std::string &name, const Price &price, const Time &duration, const std::vector<Question> &questions) :
    id {id},
    name {name},
    price {price},
    duration {duration},
    questions {questions}
{

}

auto Service::to_string() const -> std::string
{
    std::string ret {this->id + "\n"};
    ret += this->name + "\n";
    ret += this->price.to_string() + "\n";
    ret += this->duration.to_string() + "\n\nquestions:\n";
    for(auto &question : this->questions)
    {
        ret += question.to_string() + "\n";
    }
    ret.pop_back();

    return ret;
}

auto Service::to_json() const -> json
{
    json j = "{}"_json;
    j["name"] = this->name;
    j["id"] = this->id;
    j["duration"] = this->duration.to_json();
    j["price"] = this->price.to_json();
    j["questions"] = "[]"_json;
    for(auto &q : this->questions)
    {
        j["questions"].push_back(q.to_json());
    }

    return j;
}

} // namespace appointy
