#ifndef _SERVICE_CONFIGURATION_H_
#define _SERVICE_CONFIGURATIN_H_

#include <answer.h>

namespace appointy
{

struct ServiceConfiguration
{
    const std::string service_id;
    const std::vector<std::shared_ptr<Answer>> configuration;

public:
    ServiceConfiguration(const std::string &service_id, const std::vector<std::shared_ptr<Answer>> &configuration) noexcept;

public:
    auto to_json() const noexcept -> nlohmann::json;
};

} // namespace appointy

#endif // _SERVICE_CONFIGURATION_H_