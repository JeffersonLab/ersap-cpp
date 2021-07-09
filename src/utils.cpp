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

#include "utils.hpp"

#include "constants.hpp"

#include <xmsg/constants.h>
#include <xmsg/topic.h>

#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace {

const char SEPARATOR = ':';

int get_port(const std::string& full_name, int index)
{
    switch (full_name[index]) {
        case 'j':
        case 'J':
            return ersap::constants::java_port;
        case 'c':
        case 'C':
            return ersap::constants::cpp_port;
        case 'p':
        case 'P':
            return ersap::constants::python_port;
        default:
            throw std::invalid_argument{"invalid language:" + full_name};
    }
}
}


namespace ersap {
namespace util {

std::string get_current_time()
{
    time_t now;
    std::time(&now);
    char buf[sizeof "2001-01-01 00:00:00"];
    std::strftime(buf, sizeof buf, "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return buf;
}


std::string get_dpe_name(const std::string& canonical_name)
{
    return xmsg::Topic::raw(canonical_name).domain();
}


std::string get_container_name(const std::string& canonical_name)
{
    return xmsg::Topic::raw(canonical_name).subject();
}


std::string get_container_canonical_name(const std::string& canonical_name)
{
    auto first = canonical_name.find(SEPARATOR);
    if (first == std::string::npos) {
        throw std::invalid_argument{"not a container or service name"};
    }
    auto second = canonical_name.find(SEPARATOR, first + 1);
    if (second == std::string::npos) {
        return canonical_name;
    }
    return canonical_name.substr(0, second);
}


std::string get_engine_name(const std::string& canonical_name)
{
    return xmsg::Topic::raw(canonical_name).type();
}


std::string get_dpe_host(const std::string& canonical_name)
{
    auto port_sep = canonical_name.find(constants::port_sep);
    if (port_sep == std::string::npos) {
        auto lang_sep = canonical_name.find(constants::lang_sep);
        return canonical_name.substr(0, lang_sep);
    }
    return canonical_name.substr(0, port_sep);
}

int get_dpe_port(const std::string& canonical_name)
{
    auto port_sep = canonical_name.find(constants::port_sep);
    auto lang_sep = canonical_name.find(constants::lang_sep);
    if (port_sep == std::string::npos) {
        return get_port(canonical_name, lang_sep + 1);
    }
    std::string port = canonical_name.substr(port_sep + 1, lang_sep);
    return std::stoi(port);
}


std::string get_dpe_lang(const std::string& canonical_name)
{
    auto dpe_name = get_dpe_name(canonical_name);
    return dpe_name.substr(dpe_name.find(constants::lang_sep) + 1);
}


int get_default_port(const std::string& lang)
{
    return get_port(lang, 0);
}


} // end namespace util
} // end namespace ersap
