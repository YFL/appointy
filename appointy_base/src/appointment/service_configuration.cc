#include <service_configuration.h>

namespace appointy
{

ServiceConfiguration::ServiceConfiguration(const std::string &service_id, const std::vector<std::shared_ptr<Answer>> &config) noexcept :
    service_id {service_id},
    configuration {config}
{

}

auto ServiceConfiguration::to_json() const noexcept -> nlohmann::json
{
    auto json = "{}"_json;
    json["service_id"] = service_id;
    json["configuration"] = "[]"_json;
    for(auto &answer : configuration)
    {
        json["configuration"].push_back(answer->to_json());
    }

    return json;
}

} // namespace appointy