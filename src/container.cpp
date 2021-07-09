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

#include "container.hpp"

#include "logging.hpp"

#include <cstdlib>

namespace {
std::string default_author()
{
    auto* author = std::getenv("USER");
    return author ? author : "ersap";
}
} // end namespace


namespace ersap {

Container::Container(const Component& self,
                     const Component& frontend,
                     const std::string& description)
  : Base{self, frontend}
  , report_{std::make_shared<ContainerReport>(name(), default_author())}
  , description_{description}
  , running_{false}
{
    // nop
}

Container::~Container()
{
    try {
        stop();
    } catch (...) {
        // nop
    }
}


void Container::start()
{
    std::unique_lock<std::mutex> lock{mutex_};
    register_as_subscriber(self().topic(), description_);
    running_ = true;
    LOGGER->info("started container = %s", name());
}


void Container::stop()
{
    std::unique_lock<std::mutex> lock{mutex_};
    if (running_) {
        running_ = false;
        remove_services();
        try {
            deregister_as_subscriber(self().topic());
            LOGGER->info("removed container = %s", name());
        } catch (const std::exception& e) {
            LOGGER->error("could not remove container = %s", name());
        }
    }
}


void Container::add_service(const ServiceParameters& params)
{
    auto name = params.engine_name;
    auto serv_comp = Component::service(self(), name);
    auto service = services_.insert(name, serv_comp, frontend(), params);
    if (service) {
        try {
            service->start();
            report_->add_service(service->report());
        } catch (const std::exception& e) {
            service->stop();
            services_.remove(name);
            throw;
        }
    } else {
        LOGGER->error("service = %s already exists. No new service is deployed",
                      serv_comp.name());
    }
}


bool Container::remove_service(const std::string& engine_name)
{
    auto service = services_.remove(engine_name);
    if (service) {
        service->stop();
        report_->remove_service(service->report());
        return true;
    }
    return false;
}


void Container::remove_services()
{
    services_.for_each([](auto s) { s->stop(); });
    services_.clear();
}


std::shared_ptr<ContainerReport> Container::report() const
{
    return report_;
}

} // end namespace ersap
