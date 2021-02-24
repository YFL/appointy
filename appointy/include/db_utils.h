#ifndef _DB_UTILS_H_
#define _DB_UTILS_H_

#include <bsoncxx/json.hpp>
#include <bsoncxx/document/view_or_value.hpp>

#include <service.h>

namespace appointy
{

namespace util
{

    auto service_to_bson(const Service &service) -> bsoncxx::document::value;

} // namespace util

} // namespace appointy

#endif // _DB_UTILS_H_