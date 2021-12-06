

#include "component.hpp"

namespace ersap {
namespace util {

std::string make_name(const std::string& host, int port, const std::string& lang)
{
    auto name = host;
    if (port != constants::cpp_port) {
        name += constants::port_sep + std::to_string(port);
    }
    name += constants::lang_sep + lang;
    return name;
}


std::string make_name(const std::string& dpe,
                      const std::string& container)
{
    return dpe + ":" + container;
}


std::string make_name(const std::string& dpe,
                      const std::string& container,
                      const std::string& engine)
{
    return dpe + ":" + container + ":" + engine;
}

} // end namespace util
} // end namespace ersap
