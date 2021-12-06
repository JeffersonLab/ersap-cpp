

#ifndef ERSAP_DPE_HPP
#define ERSAP_DPE_HPP

#include "dpe_config.hpp"

#include <memory>

namespace xmsg {
    class ProxyAddress;
} // end namespace xmsg

namespace ersap {

class Dpe final
{
public:
    Dpe(bool is_frontend,
        const xmsg::ProxyAddress& local,
        const xmsg::ProxyAddress& frontend,
        const DpeConfig& config);

    ~Dpe();

public:
    void start();

    void stop();

private:
    class DpeImpl;
    std::unique_ptr<DpeImpl> dpe_;
};

} // end namespace ersap

#endif // end of include guard: ERSAP_DPE_HPP
