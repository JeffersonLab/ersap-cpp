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
