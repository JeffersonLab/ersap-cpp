

#ifndef ERSAP_COMPONENT_HPP
#define ERSAP_COMPONENT_HPP

#include "constants.hpp"

#include <xmsg/xmsg.h>

#include <memory>
#include <string>

namespace ersap {

namespace util {

std::string make_name(const std::string& host, int port, const std::string& lang);

std::string make_name(const std::string& dpe,
                      const std::string& container);

std::string make_name(const std::string& dpe,
                      const std::string& container,
                      const std::string& engine);

} // end namespace util

class Component
{
public:
    static Component dpe(const xmsg::ProxyAddress& address)
    {
        return dpe(address, constants::cpp_lang);
    }

    static Component dpe(const xmsg::ProxyAddress& address, const std::string& lang)
    {
        auto name = util::make_name(address.host(), address.pub_port(), lang);
        return Component{name, address, [](auto& n) {
            return xmsg::Topic::build("dpe", n);
        }};
    }

    static Component container(const Component& dpe, const std::string& name)
    {
        return Component{dpe, name, [](auto& n) {
            return xmsg::Topic::build("container", n);
        }};
    }

    static Component service(const Component& container,
                             const std::string& name)
    {
        return Component{container, name, [](auto& n) {
            return xmsg::Topic::raw(n);
        }};
    }


    // static Component service()
    // {

    // }

private:
    template<typename F>
    Component(const std::string& name,
              const xmsg::ProxyAddress& addr,
              F topic_gen)
      : name_{name}
      , addr_{addr}
      , topic_{topic_gen(name_)}
    {
        // nop
    }

    template<typename F>
    Component(const Component& parent,
              const std::string& name,
              F topic_gen)
      : name_{util::make_name(parent.name(), name)}
      , addr_{parent.addr()}
      , topic_{topic_gen(name_)}
    {
        // nop
    }

public:
    const std::string& name() const { return name_; }

    const xmsg::ProxyAddress& addr() const { return addr_; }

    const xmsg::Topic& topic() const { return topic_; }

private:
    std::string name_;
    xmsg::ProxyAddress addr_;
    xmsg::Topic topic_;
};

} // end namespace ersap

#endif // end of include guard: ERSAP_COMPONENT_HPP
