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

#ifndef ERSAP_DATA_UTILS_HPP
#define ERSAP_DATA_UTILS_HPP

#include "constants.hpp"

#include <xmsg/message.h>

#include <sstream>
#include <stdexcept>
#include <string>

namespace ersap {
namespace util {

xmsg::Message build_request(const xmsg::Topic& topic, const std::string& data);

std::string parse_message(const xmsg::Message& msg);


class InvalidRequest : public std::logic_error
{
public:
    InvalidRequest(const std::string& msg) : std::logic_error{msg} {}

    InvalidRequest(const char* msg) : std::logic_error{msg} {}
};


class RequestParser
{
public:
    static RequestParser build(const xmsg::Message& msg)
    {
        const auto& mt = msg.datatype();
        if (mt == "text/string") {
            return { msg.meta(), parse_message(msg) };
        }
        throw InvalidRequest{"invalid mime-type = " + mt};
    }

private:
    RequestParser(const xmsg::proto::Meta* meta, std::string&& data)
      : meta_{meta}
      , data_{std::move(data)}
      , ss_{data_}
    {
        // nop
    }

public:
    RequestParser(const RequestParser& other)
      : meta_{other.meta_}
      , data_{other.data_}
      , ss_{data_}
    {
        // nop
    }

    RequestParser(RequestParser&& other)
      : meta_{other.meta_}
      , data_{std::move(other.data_)}
      , ss_{data_}
    {
        other.ss_.clear();
    }

public:
    std::string next_string()
    {
        std::string item;
        if (std::getline(ss_, item, constants::data_sep[0])) {
            if (item.empty()) {
                throw invalid_request();
            }
            return item;
        }
        throw invalid_request();
    }

    std::string next_string(const std::string& default_value)
    {
        std::string item;
        if (std::getline(ss_, item, constants::data_sep[0])) {
            if (!item.empty()) {
                return item;
            }
        }
        return default_value;
    }

    int next_integer()
    {
        try {
            return std::stoi(next_string());
        } catch (const std::out_of_range& e) {
            throw invalid_request();
        } catch (const std::invalid_argument& e) {
            throw invalid_request();
        }
    }

    const std::string& request() const
    {
        return data_;
    }

private:
    InvalidRequest invalid_request()
    {
        std::string msg = "invalid request";
        if (meta_->has_author()) {
            msg += " from author = " + meta_->author();
        }
        msg += ": " + data_;
        return InvalidRequest{msg};
    }

private:
    const xmsg::proto::Meta* meta_;
    std::string data_;
    std::stringstream ss_;
};


} // end namespace util
} // end namespace ersap

#endif // end of include guard: ERSAP_DATA_UTILS_HPP
