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

#include <ersap/stdlib/event_writer_service.hpp>

#include <ersap/stdlib/json_utils.hpp>

#include "service_utils.hpp"

#include <iostream>
#include <mutex>
#include <sstream>
#include <stdexcept>


namespace {

const std::string CONF_ACTION = "action";
const std::string CONF_FILENAME = "file";
const std::string CONF_ORDER = "order";

const std::string CONF_ACTION_OPEN = "open";
const std::string CONF_ACTION_CLOSE = "close";
const std::string CONF_ACTION_SKIP = "skip";

const std::string OUTPUT_NEXT = "next-rec";
const std::string EVENT_SKIP = "skip";

const std::string NO_NAME = "";
const std::string NO_FILE = "No open file";

} // namespace


namespace ersap {
namespace stdlib {


class EventWriterService::Impl
{
public:
    Impl(EventWriterService* service);
    ~Impl();

public:
    void open_file(const json11::Json& config_data);
    void close_file(const json11::Json& config_data);

    bool has_file() { return service_->has_file(); }
    void close_file();
    void skip_all();

public:
    void write_event(const EngineData& input, EngineData& output);

public:
    void reset();

private:
    std::string file_name_ = NO_NAME;
    std::string open_error_ = NO_FILE;

public:
    bool skip_events_ = false;
    int event_counter_ = 0;

private:
    EventWriterService* service_;

    std::mutex mutex_;
};


EventWriterService::EventWriterService()
  : impl_{std::make_unique<Impl>(this)}
{
    // nop
}

EventWriterService::~EventWriterService() = default;


EventWriterService::Impl::Impl(EventWriterService* service)
  : service_{service}
{
    // nop
}

EventWriterService::Impl::~Impl() = default;


EngineData EventWriterService::configure(EngineData& input)
{
    if (input.mime_type() == type::JSON) {
        try {
            auto data = parse_json(input);
            auto action = get_string(data, CONF_ACTION);
            if (action == CONF_ACTION_OPEN) {
                impl_->open_file(data);
            } else if (action == CONF_ACTION_CLOSE) {
                impl_->close_file(data);
            } else if (action == CONF_ACTION_SKIP) {
                impl_->skip_all();
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
        std::cerr << name() << " config: wrong mime-type: " << input.mime_type()
                  << std::endl;
    }
    return {};
}


void EventWriterService::Impl::open_file(const json11::Json& config_data)
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
        event_counter_ = 0;
        std::cout << service_->name() << " opened file " << file_name_
                  << std::endl;
    } catch (const EventWriterError& e) {
        std::cerr << service_->name() << " could not open file " << e.what()
                  << std::endl;
        file_name_ = NO_NAME;
        event_counter_ = 0;
    }
}


void EventWriterService::Impl::close_file(const json11::Json& config_data)
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
    event_counter_ = 0;
}


void EventWriterService::Impl::close_file()
{
    service_->close_file();
    std::cout << service_->name() << " closed file " << file_name_ << std::endl;
}


void EventWriterService::Impl::skip_all()
{
    std::unique_lock<std::mutex> lock{mutex_};

    if (has_file()) {
        skip_events_ = true;
        std::cout << service_->name() << " skipping all events" << std::endl;
    } else {
        std::cout << service_->name() << " file " << file_name_
                  << " is already open" << std::endl;
    }
}


EngineData EventWriterService::execute(EngineData& input)
{
    auto output = EngineData();

    if (input.mime_type() == type::STRING) {
        util::set_error(output, "Wrong input type: " + input.mime_type());
        return output;
    }

    impl_->write_event(input, output);

    return output;
}


void EventWriterService::Impl::write_event(const EngineData& input,
                                           EngineData& output)
{
    if (skip_events_ || input.description() == EVENT_SKIP) {
        output.set_data(type::STRING, OUTPUT_NEXT);
        output.set_description("event skipped");
        return;
    }

    std::unique_lock<std::mutex> lock{mutex_};
    if (has_file()) {
        try {
            service_->write_event(input.data());
            event_counter_++;
            output.set_data(type::STRING, OUTPUT_NEXT);
            output.set_description("event saved");
        } catch (const EventWriterError& e) {
            std::ostringstream msg;
            msg << "Error saving event to file " << file_name_ << "\n\n"
                << e.what();
            util::set_error(output, msg.str());
        }
    } else {
        util::set_error(output, open_error_);
    }
}


EngineData EventWriterService::execute_group(const std::vector<EngineData>&)
{
    return {};
}


std::vector<EngineDataType> EventWriterService::input_data_types() const
{
    return {get_data_type(), type::JSON};
}


std::vector<EngineDataType> EventWriterService::output_data_types() const
{
    return {type::STRING};
}


std::set<std::string> EventWriterService::states() const
{
    return {};
}


void EventWriterService::reset()
{
    impl_->reset();
}


void EventWriterService::Impl::reset()
{
    std::unique_lock<std::mutex> lock{mutex_};
    if (has_file()) {
        close_file();
    }
}


EventWriterService::Endian
EventWriterService::parse_byte_order(const json11::Json& opts)
{
    if (has_key(opts, CONF_ORDER)) {
        auto byte_order = get_string(opts, CONF_ORDER);
        if (byte_order == "BIG_ENDIAN") {
            return Endian::Big;
        }
    }
    return Endian::Little;
}

} // namespace stdlib
} // end namespace ersap
