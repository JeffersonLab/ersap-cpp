

#include "dpe.hpp"

#include "base.hpp"
#include "concurrent_map.hpp"
#include "constants.hpp"
#include "container.hpp"
#include "data_utils.hpp"
#include "dpe_report.hpp"
#include "json_report.hpp"
#include "logging.hpp"
#include "utils.hpp"

#include <xmsg/proxy.h>
#include <xmsg/xmsg.h>

#include <condition_variable>
#include <mutex>
#include <thread>


namespace ersap {

class ReportService;

class DpeException : public std::runtime_error
{
public:
    DpeException(const std::string& msg) : std::runtime_error{msg} {}

    DpeException(const char* msg) : std::runtime_error{msg} {}
};


class Dpe::DpeImpl : public Base
{
public:
    DpeImpl(const xmsg::ProxyAddress& local,
            const xmsg::ProxyAddress& frontend,
            const DpeConfig& config);

    ~DpeImpl() override;

public:
    void start_proxy();

    void stop_proxy();

    void print_startup();

    void subscribe();

    void unsubscribe();

    void start_reports();

    void stop_reports();

public:
    void start_container(util::RequestParser& parser);

    void stop_container(util::RequestParser& parser);

    void stop_containers();

    void start_service(util::RequestParser& parser);

    void stop_service(util::RequestParser& parser);

    void callback(xmsg::Message& msg);

public:
    xmsg::Topic topic();

private:
    std::mutex dpe_mutex_;
    std::mutex cb_mutex_;

    std::unique_ptr<xmsg::sys::Proxy> proxy_;
    std::unique_ptr<xmsg::Subscription> sub_;
    util::ConcurrentMap<std::string, Container> containers_;

    DpeConfig config_;
    DpeReport report_;
    std::unique_ptr<ReportService> report_service_;
};


class ReportService
{
public:
    ReportService(Base& base, DpeConfig& config, DpeReport& report);

    ~ReportService();

public:
    void start();

    void stop();

public:
    std::string alive_report() { return report_.alive_report(); }

    std::string json_report() { return json_report_.generate(report_); }

private:
    void run();

    xmsg::Message alive_message()
    {
        return build_message(constants::dpe_alive, alive_report());
    }

    xmsg::Message report_message()
    {
        return build_message(constants::dpe_report, json_report());
    }

    xmsg::Message build_message(const std::string& topic_prefix,
                                const std::string& json);

private:
    bool wait(int time_out)
    {
        auto duration = std::chrono::milliseconds{time_out};
        std::unique_lock<std::mutex> lock{m_};
        return !cv_.wait_for(lock, duration, [this]() { return interrupt_; });
    }

    void interrupt()
    {
        std::unique_lock<std::mutex> lock{m_};
        interrupt_ = true;
        cv_.notify_one();
    }

private:
    std::thread thread_;
    std::condition_variable cv_;
    std::mutex m_;

    bool interrupt_ = false;

    Base& base_;
    DpeConfig& config_;
    DpeReport& report_;

    JsonReport json_report_;
};


Dpe::Dpe(bool is_frontend,
         const xmsg::ProxyAddress& local,
         const xmsg::ProxyAddress& frontend,
         const DpeConfig& config)
  : dpe_{std::make_unique<DpeImpl>(local, frontend, config)}
{
    // nop
}


Dpe::~Dpe()
{
    stop();
}


void Dpe::start()
{
    dpe_->start_proxy();
    dpe_->subscribe();
    dpe_->start_reports();
    dpe_->print_startup();
}


void Dpe::stop()
{
    dpe_->stop_reports();
    dpe_->unsubscribe();
    dpe_->stop_containers();
    dpe_->stop_proxy();
    LOGGER->info("shutdown DPE");
}


Dpe::DpeImpl::DpeImpl(const xmsg::ProxyAddress& local,
                 const xmsg::ProxyAddress& frontend,
                 const DpeConfig& config)
  : Base{Component::dpe(local),
         Component::dpe(frontend, constants::java_lang)}
  , proxy_{std::make_unique<xmsg::sys::Proxy>(local)}
  , config_(config)
  , report_{*this, config_}
  , report_service_{std::make_unique<ReportService>(*this, config_, report_)}
{
    // nop
}


Dpe::DpeImpl::~DpeImpl() = default;


void Dpe::DpeImpl::start_proxy()
{
    proxy_->start();
}


void Dpe::DpeImpl::stop_proxy()
{
    proxy_->stop();
}


void Dpe::DpeImpl::stop_containers()
{
    containers_.for_each([](auto s) { s->stop(); });
    containers_.clear();
}


void Dpe::DpeImpl::print_startup()
{
    std::cout << "=========================================" << std::endl;
    std::cout << "                 ERSAP DPE               " << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << " Name             = " << name() << std::endl;
    std::cout << " Date             = " << util::get_current_time() << std::endl;
    std::cout << " Version          = 4.3" << std::endl;
    std::cout << " Lang             = " << "C++" << std::endl;
    if (!config_.description.empty()) {
        std::cout << " Description      = " << config_.description << std::endl;
    }
    std::cout << std::endl;
    std::cout << " Proxy Host       = " << self().addr().host() << std::endl;
    std::cout << " Proxy Port       = " << self().addr().pub_port() << std::endl;
    std::cout << std::endl;
    std::cout << " FrontEnd Host    = " << frontend().addr().host() << std::endl;
    std::cout << " FrontEnd Port    = " << frontend().addr().pub_port() << std::endl;
    std::cout << " FrontEnd Lang    = " << "Java" << std::endl;
    std::cout << "=========================================" << std::endl;
}


void Dpe::DpeImpl::subscribe()
{
    std::unique_lock<std::mutex> lock{dpe_mutex_};
    sub_ = Base::subscribe(topic(), connect(), [this](auto& msg) {
        this->callback(msg);
    });
    Base::register_as_subscriber(topic(), config_.description);
}


void Dpe::DpeImpl::unsubscribe()
{
    std::unique_lock<std::mutex> lock{dpe_mutex_};
    if (sub_) {
        Base::unsubscribe(std::move(sub_));
        Base::deregister_as_subscriber(topic());
    }
}


void Dpe::DpeImpl::start_reports()
{
    report_service_->start();
}


void Dpe::DpeImpl::stop_reports()
{
    report_service_->stop();
}


void Dpe::DpeImpl::start_container(util::RequestParser& parser)
{
    auto name = parser.next_string();
    auto pool_size = parser.next_integer();
    auto description = parser.next_string();
    if (pool_size <= 0) {
        pool_size = 1;
    }

    auto cont_comp = Component::container(self(), name);
    auto container = containers_.insert(name, cont_comp, frontend(), "");
    if (container) {
        try {
            container->start();
            report_.add_container(container->report());
        } catch (const std::exception& e) {
            container->stop();
            containers_.remove(name);
            throw DpeException{"could not start container " + cont_comp.name() +
                               ": " + e.what()};
        }
    } else {
        throw util::InvalidRequest{
                "container = " + name + " already exists. " +
                "No new container is created."};
    }
}


void Dpe::DpeImpl::start_service(util::RequestParser& parser)
{
    auto container_name = parser.next_string();
    auto engine_name = parser.next_string();
    auto engine_lib = parser.next_string();
    auto pool_size = parser.next_integer();
    auto description = parser.next_string();
    auto initial_state = parser.next_string();
    if (pool_size <= 0) {
        pool_size = 1;
    }

    ServiceParameters service_params = {
        engine_name, engine_lib, initial_state, description, pool_size
    };

    auto service_name = util::make_name(name(), container_name, engine_name);
    auto container = containers_.find(container_name);
    if (container) {
        try {
            container->add_service(service_params);
        } catch (const std::exception& e) {
            throw DpeException{"could not start service " + service_name +
                               ": " + e.what()};
        }
    } else {
        throw util::InvalidRequest{
                "could not start service = " + service_name +
                ": missing container"};
    }
}


void Dpe::DpeImpl::stop_service(util::RequestParser& parser)
{
    auto container_name = parser.next_string();
    auto engine_name = parser.next_string();
    auto service_name = util::make_name(name(), container_name, engine_name);

    auto container = containers_.find(container_name);
    if (!container) {
        throw util::InvalidRequest{
                "could not stop service = " + service_name +
                ": missing container"};
    }
    bool removed = container->remove_service(engine_name);
    if (!removed) {
        throw util::InvalidRequest{
                "could not stop service = " + service_name +
                ": service doesn't exist"};
    }
}


void Dpe::DpeImpl::stop_container(util::RequestParser& parser)
{
    auto name = parser.next_string();
    auto container = containers_.remove(name);
    if (!container) {
        auto container_name = xmsg::Topic::build(DpeImpl::name(), name).str();
        throw util::InvalidRequest{"could not stop container = " + container_name +
                                   ": container doesn't exist"};
    }
    report_.remove_container(container->report());
    container->stop();
}


xmsg::Topic Dpe::DpeImpl::topic()
{
    return xmsg::Topic::build("dpe", name());
}


void Dpe::DpeImpl::callback(xmsg::Message& msg)
{
    std::unique_lock<std::mutex> lock{cb_mutex_};
    try {
        auto parser = util::RequestParser::build(msg);
        auto cmd = parser.next_string();
        auto response = parser.request();

        if (cmd == constants::start_container) {
            start_container(parser);
        } else if (cmd == constants::stop_container) {
            stop_container(parser);
        } else if (cmd == constants::start_service) {
            start_service(parser);
        } else if (cmd == constants::stop_service) {
            stop_service(parser);
        } else if (cmd == constants::report_json ||
                   cmd == constants::report_runtime) { // keep it to not break existing clients
            response = report_service_->json_report();
        } else {
            LOGGER->info("Received data %s", util::parse_message(msg));
        }
        if (msg.has_replyto()) {
            send_response(msg, response, xmsg::proto::Meta::INFO);
        }
    } catch (std::exception& e) {
        LOGGER->error(e.what());
        if (msg.has_replyto()) {
            send_response(msg, e.what(), xmsg::proto::Meta::ERROR);
        }
    } catch (...) {
        LOGGER->error("%s callback: unexpected exception", name());
    }
}


ReportService::ReportService(Base& base,
                             DpeConfig& config,
                             DpeReport& report)
  : base_{base}
  , config_{config}
  , report_{report}
{
    // nop
}


ReportService::~ReportService()
{
    stop();
}


void ReportService::start()
{
    thread_ = std::thread{[this]() {
        std::this_thread::sleep_for(std::chrono::seconds{5});
        run();
    }};
}


void ReportService::stop()
{
    if (thread_.joinable()) {
        interrupt();
        thread_.join();
    }
}


void ReportService::run()
{
    try {
        auto fe_addr = base_.frontend().addr();
        auto con = base_.connect(fe_addr);
        while (true) {
            auto alive_msg = alive_message();
            auto json_msg = report_message();
            base_.publish(con, alive_msg);
            base_.publish(con, json_msg);
            if (!wait(config_.report_period)) {
                break;
            }
        }
    } catch (std::exception& e) {
        LOGGER->error(e.what());
    }
};


xmsg::Message ReportService::build_message(const std::string& topic_prefix,
                                           const std::string& data)
{
    auto topic = xmsg::Topic::build(topic_prefix,
                                    config_.session,
                                    base_.name());
    return xmsg::Message{std::move(topic), type::JSON.mime_type(),
                         std::vector<std::uint8_t>{data.begin(), data.end()}};
}

} // end namespace ersap
