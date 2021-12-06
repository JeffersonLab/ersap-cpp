

#include "data_utils.hpp"

#include <ersap/engine_data_type.hpp>

#include <xmsg/message.h>

#include <sstream>
#include <stdexcept>
#include <string>

namespace ersap {
namespace util {

xmsg::Message build_request(const xmsg::Topic& topic, const std::string& data)
{
    return xmsg::Message{topic, type::STRING.mime_type(),
                         std::vector<uint8_t>{data.begin(), data.end()}};
}


std::string parse_message(const xmsg::Message& msg)
{
    auto& data = msg.data();
    return std::string{data.begin(), data.end()};
}

} // end namespace util
} // end namespace ersap
