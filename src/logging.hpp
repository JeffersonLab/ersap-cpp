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

#ifndef ERSAP_LOGGER_HPP
#define ERSAP_LOGGER_HPP

#include "utils.hpp"

#include <iostream>
#include <mutex>

namespace ersap {
namespace log {

class Logger
{
public:
    static Logger* instance()
    {
        static Logger logger;
        return &logger;
    }

public:
    template<typename... Args>
    void info(Args&&... args)
    {
        auto time = util::get_current_time();
        std::unique_lock<std::mutex> lock{m_};

        std::cout << time << ": ";
        tprintf(std::cout, std::forward<Args>(args)...);
        std::cout << std::endl;
    }

    template<typename... Args>
    void error(Args&&... args)
    {
        auto time = util::get_current_time();
        std::unique_lock<std::mutex> lock{m_};

        std::cerr << time << ": ";
        tprintf(std::cerr, std::forward<Args>(args)...);
        std::cerr << std::endl;
    }

private:
    void tprintf(std::ostream& out, const char* format)
    {
        out << format;
    }

    template<typename T, typename... Targs>
    void tprintf(std::ostream& out, const char* format, T&& value, Targs&&... Fargs)
    {
        for ( ; *format != '\0'; format++ ) {
            if ( *format == '%' ) {
                out << value;
                tprintf(out, format+2, std::forward<Targs>(Fargs)...);
                return;
            }
            out << *format;
        }
    }

private:
    std::mutex m_;
};

} // end namespace log

log::Logger* const LOGGER = log::Logger::instance();

} // end namespace ersap

#endif // end of include guard: ERSAP_LOGGER_HPP
