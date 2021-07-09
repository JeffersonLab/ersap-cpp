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
