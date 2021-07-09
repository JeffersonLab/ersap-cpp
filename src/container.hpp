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

#ifndef ERSAP_CONTAINER_HPP
#define ERSAP_CONTAINER_HPP

#include "base.hpp"

#include "concurrent_map.hpp"
#include "container_report.hpp"
#include "service.hpp"

#include <mutex>

namespace ersap {

class Container : public Base
{
public:
    Container(const Component& self,
              const Component& frontend,
              const std::string& description);

    Container(const Container&) = delete;
    Container& operator=(const Container&) = delete;

    ~Container() override;

public:
    void start();

    void stop();

public:
    void add_service(const ServiceParameters& params);

    bool remove_service(const std::string& engine_name);

    void remove_services();

public:
    std::shared_ptr<ContainerReport> report() const;

private:
    std::mutex mutex_;

    util::ConcurrentMap<std::string, Service> services_;
    std::shared_ptr<ContainerReport> report_;
    std::string description_;
    bool running_;
};

} // end namespace ersap

#endif // end of include guard: ERSAP_CONTAINER_HPP
