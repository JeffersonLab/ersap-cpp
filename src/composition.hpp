

#ifndef ERSAP_COMPOSITION_COMPILER_HPP
#define ERSAP_COMPOSITION_COMPILER_HPP

#include <list>
#include <set>
#include <string>

namespace ersap {
namespace composition {

class SimpleCompiler
{
public:
    SimpleCompiler(std::string service_name);

    void compile(const std::string& composition);

    std::set<std::string> outputs();

private:
    std::string service_name_;
    std::list<std::string> prev_;
    std::list<std::string> next_;
};

} // end namespace composition
} // end namespace ersap

#endif // end of include guard: ERSAP_COMPOSITION_COMPILER_HPP
