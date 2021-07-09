/*
 * Copyright (c) 2016.  Jefferson Lab (JLab). All rights reserved. Permission
 * to use, copy, modify, and distribute  this software and its documentation for
 * educational, research, and not-for-profit purposes, without fee and without a
 * signed licensing agreement.
 *
 * IN NO EVENT SHALL JLAB BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL
 * INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING
 * OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF JLAB HAS
 * BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * JLAB SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE. THE ERSAP SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY,
 * PROVIDED HEREUNDER IS PROVIDED "AS IS". JLAB HAS NO OBLIGATION TO PROVIDE
 * MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * This software was developed under the United States Government license.
 * For more information contact author at gurjyan@jlab.org
 * Department of Experimental Nuclear Physics, Jefferson Lab.
 */

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
