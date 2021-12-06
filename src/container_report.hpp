

#ifndef ERSAP_CONTAINER_REPORT_HPP
#define ERSAP_CONTAINER_REPORT_HPP

#include "concurrent_utils.hpp"
#include "constants.hpp"

#include <memory>
#include <string>
#include <vector>

namespace ersap {

class Base;
class ServiceReport;

class ContainerReport
{
public:
    using vector_type = util::CopyOnWriteVector<ServiceReport>;
    using element_type =  vector_type::pointer_type;
    using range_type = vector_type::range_type;


public:
    ContainerReport(const std::string& name, const std::string& author);

public:
    std::string name() { return name_; };

    std::string author() { return author_; };

    std::string lang() { return constants::cpp_lang; };

    std::string start_time() { return start_time_; };

public:
    void add_service(const element_type& service);

    void remove_service(const element_type& service);

    range_type services() const;

private:
    std::string name_;
    std::string author_;
    std::string start_time_;

    vector_type services_;
};

} // end namespace ersap

#endif // end of include guard: ERSAP_CONTAINER_REPORT_HPP
