

#include "container_report.hpp"

#include "utils.hpp"

#include <algorithm>
#include <atomic>

namespace ersap {

ContainerReport::ContainerReport(const std::string& name,
                                 const std::string& author)
  : name_{name}
  , author_{author}
  , start_time_{util::get_current_time()}
{
    // nop
}


void ContainerReport::add_service(const element_type& service)
{
    services_.add(service);
}


void ContainerReport::remove_service(const element_type& service)
{
    services_.remove(service);
}


ContainerReport::range_type ContainerReport::services() const
{
    return services_.view();
}

}
