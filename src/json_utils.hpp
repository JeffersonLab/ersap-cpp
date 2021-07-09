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

#ifndef ERSAP_JSON_UTILS_HPP
#define ERSAP_JSON_UTILS_HPP

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <string>

namespace ersap {
namespace util {

using Buffer = rapidjson::StringBuffer;
using Writer = rapidjson::Writer<rapidjson::StringBuffer>;

inline void put(Writer& writer, const char* key, const std::string& value)
{
    writer.Key(key);
    writer.String(value);
}

inline void put(Writer& writer, const char* key, int value)
{
    writer.Key(key);
    writer.Int(value);
}


inline void put(Writer& writer, const char* key, long value)
{
    writer.Key(key);
    writer.Int64(value);
}


inline void put(Writer& writer, const char* key, double value)
{
    writer.Key(key);
    writer.Double(value);
}

} // end namespace util
} // end namespace ersap

#endif // end of include guard: ERSAP_JSON_UTILS_HPP
