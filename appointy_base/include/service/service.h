#ifndef _SERVICE_H_
#define _SERVICE_H_

#include <vector>
#include <string>

#include <sqlite3.h>

#include <appointy_time.h>
#include <price.h>
#include <question.h>

namespace appointy
{

struct Service
{
public:
    const std::string id;
    const std::string name;
    const Price price;
    const Time duration;
    const std::vector<Question> questions;

public:
    Service(const std::string &id, const std::string &name, const Price &price, const Time &duration, const std::vector<Question> &questions);

public:
    auto to_string() const -> std::string;
    auto to_json() const -> nlohmann::json;
};

} // namespace appointy

#endif // _SERVICE_H_