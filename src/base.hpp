

#ifndef ERSAP_BASE_HPP
#define ERSAP_BASE_HPP

#include "component.hpp"

#include <xmsg/xmsg.h>

namespace ersap {

class Base : public xmsg::xMsg
{
public:
    Base(const Component& self, const Component& frontend);

    ~Base() override;

public:
    void send(const Component& component, const std::string& data);

    void send(const Component& component, xmsg::Message& msg);

    void send_response(const xmsg::Message& msg,
                       const std::string& data,
                       xmsg::proto::Meta::Status status);

public:
    const Component& self() { return self_; }

    const Component& frontend() { return frontend_; }

private:
    Component self_;
    Component frontend_;
};

} // end namespace ersap

#endif // end of include guard: ERSAP_BASE_HPP
