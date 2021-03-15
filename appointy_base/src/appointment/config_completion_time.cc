#include <config_completion_time.h>

namespace appointy
{

ConfigCompletionTime::ConfigCompletionTime(const ServiceConfiguration &config, const Time &completion_time) noexcept :
    configuration {config},
    completion_time {completion_time}
{
    
}

auto ConfigCompletionTime::to_json() const noexcept -> nlohmann::json
{
    auto json = "{}"_json;
    json["configuration"] = configuration.to_json();

    return json;
}

} // namespace appointy