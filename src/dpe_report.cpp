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

#include "dpe_report.hpp"

#include "base.hpp"
#include "json_utils.hpp"
#include "utils.hpp"

#include <cstdlib>

static std::string get_ersap_home()
{
    auto ersap_home = std::getenv("ERSAP_HOME");
    if (!ersap_home) {
        throw std::runtime_error{"Missing ERSAP_HOME env variable"};
    }
    return {ersap_home};
}


static std::string get_alive_report(const std::string& name,
                                    int cores,
                                    const std::string& ersap_home)
{
    using namespace ersap::util;

    Buffer buffer;
    Writer writer{buffer};

    writer.StartObject();
    put(writer, "name", name);
    put(writer, "n_cores", cores);
    put(writer, "ersap_home", ersap_home);
    writer.EndObject();

    return buffer.GetString();
}


namespace ersap {

DpeReport::DpeReport(Base& base, DpeConfig& config)
  : name_{base.name()}
  , start_time_{util::get_current_time()}
  , ersap_home_{get_ersap_home()}
  , config_{config}
{
    alive_report_ = get_alive_report(name(), core_count(), ersap_home());
}


std::string DpeReport::ersap_home() const
{
    return ersap_home_;
}


int DpeReport::core_count() const
{
    return config_.max_cores;
}


long DpeReport::memory_size() const
{
    return 1;
}


long DpeReport::memory_usage() const
{
    return 1;
}


double DpeReport::cpu_usage() const
{
    return 1.0;
}


double DpeReport::load() const
{
    return 1.0;
}


void DpeReport::add_container(const element_type& container)
{
    containers_.add(container);
}


void DpeReport::remove_container(const element_type& container)
{
    containers_.remove(container);
}


DpeReport::range_type DpeReport::containers() const
{
    return containers_.view();
}

} // end namespace ersap
