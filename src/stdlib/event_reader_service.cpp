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

#include <ersap/stdlib/event_reader_service.hpp>

#include <ersap/stdlib/json_utils.hpp>

#include "service_utils.hpp"

#include <iostream>
#include <mutex>
#include <stdexcept>


namespace {

const std::string CONF_ACTION = "action";
const std::string CONF_FILENAME = "file";

const std::string CONF_ACTION_OPEN = "open";
const std::string CONF_ACTION_CLOSE = "close";

const std::string CONF_EVENTS_SKIP = "skip";
const std::string CONF_EVENTS_MAX = "max";

const std::string REQUEST_NEXT = "next";
const std::string REQUEST_NEXT_REC = "next-rec";
const std::string REQUEST_FRAME_ORDER = "order";
const std::string REQUEST_FRAME_NUMBER = "count";

const std::string NO_NAME = "";
const std::string NO_FILE = "No open file";
const std::string END_OF_FILE = "End of file";

const int EOF_NOT_FROM_WRITER = 0;
const int EOF_WAITING_REC = -1;

} // namespace


namespace ersap {
namespace stdlib {

class EventReaderService::Impl
{
public:
    Impl(EventReaderService* service);
    ~Impl();

public:
    void open_file(const json11::Json& config_data);
    void close_file(const json11::Json& config_data);

    bool has_file() { return service_->has_file(); }
    void close_file();

private:
    void set_limits(const json11::Json& config_data);
    int get_value(const json11::Json& config_data, const std::string& key,
                  int def_val, int min_val, int max_val);

public:
    void get_next_event(const EngineData& input, EngineData& output);
    void get_file_byte_order(EngineData& output);
    void get_event_count(EngineData& output);

private:
    bool is_rec_request(const EngineData& input);
    void return_next_event(EngineData& output);
    std::string get_order(Endian endian);

public:
    void reset();

private:
    std::string file_name_ = NO_NAME;
    std::string open_error_ = NO_FILE;

    int current_event_;
    int last_event_;
    int event_count_;

    std::set<int> processing_events_;
    int eof_request_count_;

private:
    EventReaderService* service_;

    std::mutex mutex_;
};


EventReaderService::EventReaderService()
  : impl_{std::make_unique<Impl>(this)}
{
    // nop
}

EventReaderService::~EventReaderService() = default;


EventReaderService::Impl::Impl(EventReaderService* service)
  : service_{service}
{
    // nop
}

EventReaderService::Impl::~Impl() = default;


EngineData EventReaderService::configure(EngineData& input)
{
    if (input.mime_type() == type::JSON) {
        try {
            auto data = parse_json(data_cast<std::string>(input));
            auto action = get_string(data, CONF_ACTION);
            if (action == CONF_ACTION_OPEN) {
                impl_->open_file(data);
            } else if (action == CONF_ACTION_CLOSE) {
                impl_->close_file(data);
            } else {
                std::cerr << name() << " config: invalid \"" << CONF_ACTION
                          << "\" value: \"" << action << "\"" << std::endl;
            }
        } catch (const bad_any_cast& e) {
            std::cerr << name() << " config: " << "input data is not JSON"
                      << std::endl;
        } catch (const std::exception& e) {
            std::cerr << name() << " config: " << e.what() << std::endl;
        }
    } else {
        std::cerr << name() << " config: wrong mime-type " << input.mime_type()
                  << std::endl;
    }
    return {};
}


void EventReaderService::Impl::open_file(const json11::Json& config_data)
{
    std::unique_lock<std::mutex> lock{mutex_};

    if (has_file()) {
        close_file();
    }

    file_name_ = get_string(config_data, CONF_FILENAME);
    std::cout << service_->name() << " request to open file " << file_name_
              << std::endl;
    try {
        service_->open_file(file_name_, config_data);
        set_limits(config_data);
        std::cout << service_->name() << " opened file " << file_name_
                  << std::endl;
    } catch (const EventReaderError& e) {
        std::cerr << service_->name() << " could not open file " << e.what()
                  << std::endl;
        file_name_ = NO_NAME;
    }
}


void EventReaderService::Impl::set_limits(const json11::Json& config_data)
{
    event_count_ = service_->read_event_count();
    int skip_events = get_value(config_data, CONF_EVENTS_SKIP,
                                0, 0, event_count_);
    if (skip_events != 0) {
        std::cout << service_->name() << " config: skip first " << skip_events
                  << " events" << std::endl;
    }
    current_event_ = skip_events;

    int rem_events = event_count_ - skip_events;
    int max_events = get_value(config_data, CONF_EVENTS_MAX,
                               rem_events, 0, rem_events);
    if (max_events != rem_events) {
        std::cout << service_->name() << " config: read first " << max_events
                  << " events" << std::endl;
    }
    last_event_ = skip_events + max_events;

    processing_events_.clear();
    eof_request_count_ = 0;
}


int EventReaderService::Impl::get_value(const json11::Json& config_data,
                                        const std::string& key,
                                        int def_val,
                                        int min_val,
                                        int max_val)
{
    auto json_val = config_data[key];
    if (!json_val.is_null()) {
        if (json_val.is_number()) {
            int value = json_val.int_value();
            if (value >= min_val && value <= max_val) {
                return value;
            }
        }
        std::cerr << service_->name() << " config: "
                << "invalid value for '" << key << "': " << json_val.dump()
                << std::endl;
    }
    return def_val;
}


void EventReaderService::Impl::close_file(const json11::Json& config_data)
{
    std::unique_lock<std::mutex> lock{mutex_};

    file_name_ = get_string(config_data, CONF_FILENAME);
    std::cout << service_->name() << " request to close file " << file_name_
              << std::endl;
    if (has_file()) {
        close_file();
    } else {
        std::cerr << service_->name() << " file " << file_name_ << " not open"
                  << std::endl;
    }
    open_error_ = NO_FILE;
    file_name_ = NO_NAME;
}


void EventReaderService::Impl::close_file()
{
    service_->close_file();
    std::cout << service_->name() << " closed file " << file_name_ << std::endl;
}


EngineData EventReaderService::execute(EngineData& input)
{
    auto output = EngineData();

    const auto& dt = input.mime_type();
    if (dt == type::STRING) {
        const auto& request = data_cast<std::string>(input);
        if (request == REQUEST_NEXT || request == REQUEST_NEXT_REC) {
            impl_->get_next_event(input, output);
        } else if (request == REQUEST_FRAME_ORDER) {
            impl_->get_file_byte_order(output);
        } else if (request == REQUEST_FRAME_NUMBER) {
            impl_->get_event_count(output);
        } else {
            util::set_error(output, "Wrong input data: " + request);
        }
    } else {
        util::set_error(output, "Wrong input type: " + dt);
    }

    return output;
}


inline bool EventReaderService::Impl::is_rec_request(const EngineData& input)
{
    return data_cast<std::string>(input) == REQUEST_NEXT_REC;
}


void EventReaderService::Impl::get_next_event(const EngineData& input,
                                              EngineData& output)
{
    std::unique_lock<std::mutex> lock{mutex_};

    bool from_rec = is_rec_request(input);
    if (from_rec) {
        processing_events_.erase(input.communication_id());
    }
    if (!has_file()) {
        util::set_error(output, open_error_, 1);
    } else if (current_event_ < last_event_) {
        return_next_event(output);
    } else {
        util::set_error(output, END_OF_FILE, 1);
        if (from_rec) {
            if (processing_events_.empty()) {
                eof_request_count_++;
                util::set_error(output, END_OF_FILE, eof_request_count_ + 1);
                output.set_data(type::SFIXED32, eof_request_count_);
            } else {
                output.set_data(type::SFIXED32, EOF_WAITING_REC);
            }
        } else {
            output.set_data(type::SFIXED32, EOF_NOT_FROM_WRITER);
        }
    }
}


void EventReaderService::Impl::return_next_event(EngineData& output)
{
    try {
        auto event_id = current_event_++;
        output.set_communication_id(event_id);

        auto event = service_->read_event(event_id);
        output.set_data(service_->get_data_type(), std::move(event));
        output.set_description("data");

        processing_events_.insert(event_id);
    } catch (const EventReaderError& e) {
        std::ostringstream msg;
        msg << "Error requesting event " << current_event_
            << " from file " << file_name_
            << "\n\n" << e.what();
        util::set_error(output, msg.str(), 1);
    }
}


void EventReaderService::Impl::get_file_byte_order(EngineData& output)
{
    std::unique_lock<std::mutex> lock{mutex_};

    if (has_file()) {
        try {
            auto order = get_order(service_->read_byte_order());
            output.set_data(type::STRING, order);
            output.set_description("byte order");
        } catch (const EventReaderError& e) {
            std::ostringstream msg;
            msg << "Error requesting byte-order from file " << file_name_
                << "\n\n" << e.what();
            util::set_error(output, msg.str(), 1);
        }
    } else {
        util::set_error(output, open_error_, 1);
    }
}


std::string EventReaderService::Impl::get_order(Endian endian)
{
    switch (endian) {
        case Endian::Little:
            return "LITTLE_ENDIAN";
        case Endian::Big:
        default:
            return "BIG_ENDIAN";
    }
}


void EventReaderService::Impl::get_event_count(EngineData& output)
{
    std::unique_lock<std::mutex> lock{mutex_};

    if (has_file()) {
        output.set_data(type::SFIXED32, event_count_);
        output.set_description("event count");
    } else {
        util::set_error(output, open_error_, 1);
    }
}


EngineData EventReaderService::execute_group(const std::vector<EngineData>&)
{
    return {};
}


std::vector<EngineDataType> EventReaderService::input_data_types() const
{
    return {type::JSON, type::STRING};
}


std::vector<EngineDataType> EventReaderService::output_data_types() const
{
    return {get_data_type(), type::STRING, type::SFIXED32};
}


std::set<std::string> EventReaderService::states() const
{
    return {};
}


void EventReaderService::reset()
{
    impl_->reset();
}


void EventReaderService::Impl::reset()
{
    std::unique_lock<std::mutex> lock{mutex_};
    if (has_file()) {
        close_file();
    }
}

} // namespace stdlib
} // end namespace ersap
