

#include "base.hpp"

#include "constants.hpp"
#include "data_utils.hpp"

#include <xmsg/constants.h>

#include <algorithm>
#include <functional>
#include <iostream>

namespace {
    xmsg::RegAddress get_fe_address(const ersap::Component& fe)
    {
        int reg_port = fe.addr().pub_port() + ersap::constants::reg_port_shift;
        return { fe.addr().host(), reg_port };
    }
}


namespace ersap {

Base::Base(const Component& self, const Component& frontend)
  : xmsg::xMsg{self.name(), self.addr(), get_fe_address(frontend)}
  , self_{self}
  , frontend_{frontend}
{
    // nop
}


Base::~Base()
{
    // nothing
}


void Base::send(const Component& component, const std::string& data)
{
    auto msg = util::build_request(component.topic(), data);
    auto con = connect(component.addr());
    publish(con, msg);
}


void Base::send(const Component& component, xmsg::Message& msg)
{
    auto con = connect(component.addr());
    publish(con, msg);
}


void Base::send_response(const xmsg::Message& msg,
                         const std::string& data,
                         xmsg::proto::Meta::Status status)
{
    auto meta = std::make_unique<xmsg::proto::Meta>();
    meta->set_datatype(xmsg::mimetype::single_string);
    meta->set_author(name());
    meta->set_status(status);
    auto res = xmsg::Message{msg.replyto(), std::move(meta),
                             std::vector<uint8_t>{data.begin(), data.end()}};

    auto con = connect();
    publish(con, res);
}

} // end namespace ersap
