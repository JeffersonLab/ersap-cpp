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

#include <ersap/stdlib/json_utils.hpp>

#include <ersap/engine_data.hpp>

namespace {

const json11::Json& get_value(const json11::Json& obj, const std::string& key)
{
    using ersap::stdlib::JsonError;
    if (key.empty()) {
        throw std::invalid_argument{"empty key parameter"};
    }
    const auto& val = obj[key];
    if (val.is_null()) {
        throw JsonError{"JSON key \"" + key + "\" is not present"};
    }
    return val;
}

}


namespace ersap {
namespace stdlib {

json11::Json parse_json(const EngineData& input)
{
    return parse_json(data_cast<std::string>(input));
}


json11::Json parse_json(const std::string& str)
{
    auto err = std::string{};
    auto obj = json11::Json::parse(str, err);
    if (obj.is_null()) {
        throw JsonError{err};
    }
    return obj;
}


bool has_key(const json11::Json& obj, const std::string& key)
{
    return !obj[key].is_null();
}


bool get_bool(const json11::Json& obj, const std::string& key)
{
    const auto& val = get_value(obj, key);
    if (val.type() == json11::Json::Type::BOOL) {
        return val.bool_value();
    }
    throw JsonError{"JSON key \"" + key + "\" is not a boolean"};
}


int get_int(const json11::Json& obj, const std::string& key)
{
    const auto& val = get_value(obj, key);
    if (val.type() == json11::Json::Type::NUMBER) {
        return val.int_value();
    }
    throw JsonError{"JSON key \"" + key + "\" is not a number"};
}


int get_double(const json11::Json& obj, const std::string& key)
{
    const auto& val = get_value(obj, key);
    if (val.type() == json11::Json::Type::NUMBER) {
        return val.number_value();
    }
    throw JsonError{"JSON key \"" + key + "\" is not a number"};
}


const std::string& get_string(const json11::Json& obj, const std::string& key)
{
    const auto& val = get_value(obj, key);
    if (val.type() == json11::Json::Type::STRING) {
        return val.string_value();
    }
    throw JsonError{"JSON key \"" + key + "\" is not a string"};
}


const json11::Json& get_array(const json11::Json& obj, const std::string& key)
{
    const auto& val = get_value(obj, key);
    if (val.type() == json11::Json::Type::ARRAY) {
        return val;
    }
    throw JsonError{"JSON key \"" + key + "\" is not an array"};
}


const json11::Json& get_object(const json11::Json& obj, const std::string& key)
{
    const auto& val = get_value(obj, key);
    if (val.type() == json11::Json::Type::OBJECT) {
        return val;
    }
    throw JsonError{"JSON key \"" + key + "\" is not an array"};
}


void check_json(const json11::Json& obj, const json11::Json::shape& shape)
{
    auto err = std::string{};
    if (!obj.has_shape(shape, err)) {
        throw JsonError{err};
    }
}

} // end namespace stdlib
} // end namespace ersap
