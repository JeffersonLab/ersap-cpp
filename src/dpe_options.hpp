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

#ifndef ERSAP_DPE_OPTIONS_HPP
#define ERSAP_DPE_OPTIONS_HPP

#include "dpe_config.hpp"

#include <optionparser.h>

#include <xmsg/address.h>

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

namespace ersap {
namespace option {

using namespace ::option;

struct Arg : public ::option::Arg
{
    static void print_error(const char* msg1, const Option& opt, const char* msg2)
    {
        fprintf(stderr, "%s", msg1);
        fwrite(opt.name, opt.namelen, 1, stderr);
        fprintf(stderr, "%s", msg2);
    }

    static option::ArgStatus Unknown(const Option& option, bool msg)
    {
        if (msg) {
            print_error("Unknown option ", option, "\n");
        }
        return option::ARG_ILLEGAL;
    }

    static option::ArgStatus Required(const Option& option, bool msg)
    {
        if (option.arg != nullptr) {
            return option::ARG_OK;
        }
        if (msg) {
            print_error("Option ", option, " requires an argument\n");
        }
        return option::ARG_ILLEGAL;
    }
};

enum OptionIndex {
    UNKNOWN, HOST, PORT, FE_HOST, FE_PORT,
    SESSION, DESC, POOL, CORES, REPORT,
    CTX_SK, CTX_IO,
    HELP
};

constexpr Descriptor usage[] =
{
    { UNKNOWN, 0, "", "", Arg::Unknown,             "Usage: c_dpe [options]\n\n"
                                                    "  Options:" },
    { HOST, 0, "", "host", Arg::Required,           "  --host <hostname>         \tuse given host for this DPE" },
    { PORT, 0, "", "port", Arg::Required,           "  --port <port>             \tuse given port for this DPE" },
    { FE_HOST, 0, "", "fe-host", Arg::Required,     "  --fe-host <hostname>      \tthe host used by the remote front-end" },
    { FE_PORT, 0, "", "fe-port", Arg::Required,     "  --fe-port <port>          \tthe port used by the remote front-end" },
    { SESSION, 0, "", "session", Arg::Required,     "  --session <id>            \tthe session ID of this DPE" },
    { DESC, 0, "", "description", Arg::Required,    "  --description <string>    \ta short description of this DPE\n\n"
                                                    "  Config options:" },
    { POOL, 0, "", "poolsize", Arg::Required,       "  --poolsize <size>         \tsize of thread pool to handle requests" },
    { CORES, 0, "", "max-cores", Arg::Required,     "  --max-cores <cores>       \thow many cores can be used by a service" },
    { REPORT, 0, "", "report", Arg::Required,       "  --report <seconds>        \tthe period to publish reports\n\n"
                                                    "  Advanced options:" },
    { CTX_SK, 0, "", "max-sockets", Arg::Required,  "  --max-sockets <sockets>   \tmaximum number of allowed ZMQ sockets" },
    { CTX_IO, 0, "", "io-threads", Arg::Required,   "  --io-threads <threads>    \tsize of ZMQ thread pool to handle I/O" },
    { HELP, 0, "h", "help", Arg::None,              "" },
    { 0, 0, nullptr, nullptr, nullptr, nullptr}
};


class DpeOptionsParser final
{
public:
    bool parse(int argc, char* argv[])
    {
        argc -= static_cast<int>(argc > 0);
        argv += static_cast<int>(argc > 0);
        Stats stats(usage, argc, argv);

        options_.resize(stats.options_max);
        buffer_.resize(stats.buffer_max);

        parser_ = Parser(usage, argc, argv, &options_[0], &buffer_[0]);

        if (parser_.nonOptionsCount() > 0) {
            std::cerr << "Invalid command line arguments." << std::endl;
            return false;
        }

        if (parser_.error()) {
            return false;
        }

        if (has_help()) {
            return true;
        }

        if (options_[FE_HOST] == nullptr) {
            if (options_[FE_PORT] == nullptr) {
                std::cerr << "The remote front-end host is required." << std::endl;
            } else {
                std::cerr << "Missing front-end host argument." << std::endl;
            }
            return false;
        }

        parse_options();
        return true;
    }

    bool has_help()
    {
        return options_[HELP] != nullptr;
    }

    void print_help()
    {
        option::printUsage(std::cout, usage);
    }

private:
    void parse_options()
    {
        const auto default_host = std::string{"localhost"};
        const auto default_port = 7781;
        const auto default_fe_port = 7771;

        // Act as front-end by default but if feHost or fePort are passed
        // act as a worker DPE with remote front-end
        bool fe = options_[FE_HOST] == nullptr && options_[FE_PORT] == nullptr;

        // Get local DPE address
        auto local_host = value_of(HOST, default_host);
        auto local_port = value_of(PORT, default_port);
        local_addr_ = xmsg::ProxyAddress{local_host, local_port};

        if (fe) {
            // Get local FE address (use same local DPE address)
            fe_addr_ = local_addr_;
        } else {
            // Get remote FE address
            auto fe_host = value_of(FE_HOST, default_host);
            auto fe_port = value_of(FE_PORT, default_fe_port);
            fe_addr_ = xmsg::ProxyAddress{fe_host, fe_port};
        }

        config_ = {
            value_of(SESSION, ""),
            value_of(DESC, ""),
            value_of(POOL, 2),
            value_of(CORES, std::thread::hardware_concurrency()),
            parse_report_period()
        };

        max_sockets_ = value_of(CTX_SK, 1024);
        io_threads_ = value_of(CTX_IO, 1);
    }

    std::string value_of(int opt, const std::string& val)
    {
        auto arg = options_[opt].arg;
        if (arg != nullptr) {
            return std::string{arg};
        }
        return val;
    }

    int value_of(int opt, int val)
    {
        auto arg = options_[opt].arg;
        if (arg != nullptr) {
            return std::stoi(arg);
        }
        return val;
    }

    int parse_report_period()
    {
        using namespace std::chrono;
        auto default_period = 10'000;
        auto ms = milliseconds{default_period};
        auto s = value_of(REPORT, duration_cast<seconds>(ms).count());
        return duration_cast<milliseconds>(seconds{s}).count();
    }

public:
    xmsg::ProxyAddress local_address() const
    {
        return local_addr_;
    }

    xmsg::ProxyAddress frontend_address() const
    {
        return fe_addr_;
    }

    DpeConfig config() const
    {
        return config_;
    }

    int max_sockets() const
    {
        return max_sockets_;
    }

    int io_threads() const
    {
        return io_threads_;
    }

private:
    std::vector<Option> options_;
    std::vector<Option> buffer_;

    Parser parser_;

    xmsg::ProxyAddress local_addr_;
    xmsg::ProxyAddress fe_addr_;

    DpeConfig config_;

    int max_sockets_;
    int io_threads_;
};

} // end namespace option
} // end namespace ersap

#endif // end of include guard: ERSAP_DPE_OPTIONS_HPP
