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

#include "service.hpp"

#include "data_utils.hpp"
#include "logging.hpp"


static tp::ThreadPoolOptions thread_pool_options(int pool_size, int queue_size)
{
    tp::ThreadPoolOptions opts;
    opts.setThreadCount(static_cast<size_t>(pool_size));
    opts.setQueueSize(static_cast<size_t>(queue_size));
    return opts;
}


namespace ersap {

Service::Service(const Component& self,
                 const Component& frontend,
                 const ServiceParameters& params)
  : Base{self, frontend}
  , loader_{params.engine_lib}
  , thread_pool_{thread_pool_options(params.pool_size, 1024)}
  , sys_config_{std::make_shared<ServiceConfig>()}
  , report_{std::make_shared<ServiceReport>(name(), params,
                                            loader_->author(),
                                            loader_->version(),
                                            loader_->description())}
  , service_{std::make_unique<ServiceEngine>(self, frontend,
                                             loader_.get(),
                                             report_.get(),
                                             sys_config_.get())}
{
    LOGGER->info("created service = %s pool_size = %d", name(), params.pool_size);
}


Service::~Service()
{
    try {
        stop();
    } catch (...) {
        // nop
    }
}


void Service::start()
{
    std::unique_lock<std::mutex> lock{mutex_};
    sub_ = subscribe(self().topic(), connect(), [this](auto& msg) {
        this->callback(msg);
    });
    register_as_subscriber(self().topic(), "");
    LOGGER->info("started service = %s", name());
}


void Service::stop()
{
    std::unique_lock<std::mutex> lock{mutex_};
    if (sub_) {
        try {
            unsubscribe(std::move(sub_));
            deregister_as_subscriber(self().topic());
            LOGGER->info("removed service = %s", name());
        } catch (const std::exception& e) {
            LOGGER->error("could not remove service = %s", name());
        }
    }
}


void Service::setup(xmsg::Message& msg)
{
    auto m = std::make_unique<xmsg::Message>(std::move(msg));
    thread_pool_.post([s=service_.get(), m=std::move(m)]() { s->setup(*m); });
}


void Service::configure(xmsg::Message& msg)
{
    auto m = std::make_unique<xmsg::Message>(std::move(msg));
    thread_pool_.post([s=service_.get(), m=std::move(m)](){
        try {
            s->configure(*m);
        } catch (const std::exception& e) {
            LOGGER->error("%s configure: unhandled exception %s", s->name(), e.what());
        } catch (...) {
            LOGGER->error("%s configure: unexpected exception", s->name());
        }
    });
}


void Service::execute(xmsg::Message& msg)
{
    auto m = std::make_unique<xmsg::Message>(std::move(msg));
    thread_pool_.post([s=service_.get(), m=std::move(m)]() {
        try {
            s->execute(*m);
        } catch (const std::exception& e) {
            LOGGER->error("%s execute: unhandled exception %s", s->name(), e.what());
        } catch (...) {
            LOGGER->error("%s execute: unexpected exception", s->name());
        }
    });
}


void Service::callback(xmsg::Message& msg)
{
    std::unique_lock<std::mutex> lock{cb_mutex_};
    try {
        auto meta = msg.meta();
        if (!meta->has_action()) {
            setup(msg);
        } else if (meta->action() == xmsg::proto::Meta::CONFIGURE) {
            configure(msg);
        } else {
            execute(msg);
        }
    } catch (std::exception& e) {
        LOGGER->error("%s callback: %s", name(), e.what());
    } catch (...) {
        LOGGER->error("%s callback: unexpected exception", name());
    }
}


std::shared_ptr<ServiceReport> Service::report() const
{
    return report_;
}

} // end namespace ersap
