#ifndef _CONFIG_COMPLETION_TIME_
#define _CONFIG_COMPLETION_TIME_

#include <vector>

#include <service_configuration.h>
#include <appointy_time.h>

namespace appointy
{

struct ConfigCompletionTime
{
    const ServiceConfiguration configuration;
    const Time completion_time;

public:
    ConfigCompletionTime(const ServiceConfiguration &config, const Time &comletion_time) noexcept;

public:
    auto to_json() const noexcept -> nlohmann::json;
};

} // namespace appointy

#endif // _CONFIG_COMPLETION_TIME