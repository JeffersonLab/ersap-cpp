

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
