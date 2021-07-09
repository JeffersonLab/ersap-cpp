/*
 * Copyright (c) 2017.  Jefferson Lab (JLab). All rights reserved. Permission
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

#ifndef ERSAP_STD_SERVICE_UTILS_HPP
#define ERSAP_STD_SERVICE_UTILS_HPP

#include <ersap/engine_data.hpp>

namespace ersap {
namespace util {

/**
 * Sets the given engine data with an error status.
 *
 * @param output the engine data that will be returned by the service
 * @param msg a description for the error
 */
inline void set_error(EngineData& output, const std::string& msg)
{
    output.set_description(msg);
    output.set_status(ersap::EngineStatus::ERROR, 1);
}

/**
 * Sets the given engine data with an error status.
 *
 * @param output the engine data that will be returned by the service
 * @param msg a description for the error
 * @param severity the severity of the error, as a positive integer
 */
inline void set_error(EngineData& output, const std::string& msg, int severity)
{
    output.set_description(msg);
    output.set_status(ersap::EngineStatus::ERROR, severity);
}

} // end namespace util
} // end namespace ersap

#endif // end of include guard: ERSAP_STD_SERVICE_UTILS_HPP
