#ifndef _DURATION_REQUEST_H_
#define _DURATION_REQUEST_H_

#include <string>

#include <json.hpp>

#include <answer.h>

namespace appointy
{

struct DurationRequest
{
    const std::string service_id;
    const std::vector<std::shared_ptr<Answer>> configuration;

public:
    DurationRequest(const std::string &service_id, const std::vector<std::shared_ptr<Answer>> &configuration) noexcept;

public:
    auto to_json() const noexcept -> nlohmann::json;
};

} // namespace appointy

#endif // _DURATION_REQUEST_H_