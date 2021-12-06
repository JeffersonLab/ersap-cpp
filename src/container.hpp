

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
