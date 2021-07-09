//
// Created by Vardan Gyurjyan on 1/23/21.
//

#include "ersap/stdlib/streaming_service.hpp"
#include <ersap/stdlib/json_utils.hpp>

#include "service_utils.hpp"

#include <iostream>
#include <stdexcept>

namespace {

    const std::string CONF_STREAM_ACTION = "action";

    const std::string CONF_ACTION_CONNECT = "open";
    const std::string CONF_ACTION_DISCONNECT = "close";

    const std::string REQUEST_STREAM_NEXT = "next";
    const std::string REQUEST_NEXT_STREAM_REC = "next-rec";
    const std::string REQUEST_FRAME_ORDER = "order";
    const std::string REQUEST_FRAME_NUMBER = "count";

} // namespace

namespace ersap {
    namespace stdlib {

        // Impl class definition
        class StreamingService::Impl {
        public:
            explicit Impl(StreamingService *service);
            ~Impl();

        public:
            void connect(const json11::Json &config_data);
            void disconnect();

        private:
            int get_value(const json11::Json &config_data, const std::string &key,
                          int def_val, int min_val, int max_val);

        public:
            void get_next_frame(EngineData &output);
            void get_byte_order(EngineData &output);
            void get_event_count(EngineData &output);

        private:
            static std::string get_order(Endian endian);

        public:
            void reset();

        private:
            std::string stream_names[4] = {"stream1","stream2","stream3","stream4"};
            int stream_ports[4] = {0,0,0,0};
            int current_event_ = 0;
            int event_count_ = 0;

            std::set<int> processing_events_;

        private:
            StreamingService *service_;
        };


        // StreamingService service constructor
        StreamingService::StreamingService()
                : impl_{std::make_unique<Impl>(this)} {}

        // StreamingService destructor
        StreamingService::~StreamingService() = default;

        // Impl constructor
        StreamingService::Impl::Impl(StreamingService *service)
                : service_{service} {}

        // Impl default destructor
        StreamingService::Impl::~Impl() = default;


        // overriding base ersap:Engine class methods
        EngineData StreamingService::configure(EngineData &input) {
            if (input.mime_type() == type::JSON) {
                try {
                    auto data = parse_json(data_cast<std::string>(input));
                    const auto &action = get_string(data, CONF_STREAM_ACTION);
                    if (action == CONF_ACTION_CONNECT) {
                        impl_->connect(data);
                    } else if (action == CONF_ACTION_DISCONNECT) {
                        impl_->disconnect();
                    } else {
                        std::cerr << name() << " config: invalid \"" << CONF_STREAM_ACTION
                                  << "\" value: \"" << action << "\"" << std::endl;
                    }
                } catch (const bad_any_cast &e) {
                    std::cerr << name() << " config: " << "input data is not JSON"
                              << std::endl;
                } catch (const std::exception &e) {
                    std::cerr << name() << " config: " << e.what() << std::endl;
                }
            } else {
                std::cerr << name() << " config: wrong mime-type " << input.mime_type()
                          << std::endl;
            }
            return {};
        }

        EngineData StreamingService::execute(EngineData &input) {
            auto output = EngineData();

            const auto &dt = input.mime_type();
            if (dt == type::STRING) {
                const auto &request = data_cast<std::string>(input);
                if (request == REQUEST_STREAM_NEXT || request == REQUEST_NEXT_STREAM_REC) {
                    impl_->get_next_frame(output);
                } else if (request == REQUEST_FRAME_ORDER) {
                    impl_->get_byte_order(output);
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

        EngineData StreamingService::execute_group(const std::vector<EngineData> &) {
            return {};
        }

        std::vector<EngineDataType> StreamingService::input_data_types() const {
            return {type::JSON, type::STRING};
        }

        std::vector<EngineDataType> StreamingService::output_data_types() const {
            return {get_data_type(), type::STRING, type::SFIXED32};
        }

        std::set<std::string> StreamingService::states() const {
            return {};
        }

        void StreamingService::reset() {
            impl_->reset();
        }


        // overriding TdsIml class methods
        void StreamingService::Impl::connect(const json11::Json &config_data) {

            // get stream port numbers from the configuration data
            for (int i : {0,1,2,3}) {
            stream_ports[i] = get_int(config_data, stream_names[i]);
                std::cout << service_->name() << " request to connect to the stream, port = " << stream_ports[i]
                          << std::endl;
        }
            // connect to the streams
            try {
                for (int p : stream_ports) {
                service_->connect(p, config_data);
            }
                std::cout << service_->name() << " connected " << std::endl;
            } catch (const StreamingServiceError &e) {
                std::cerr << service_->name() << " could not connect to streams " << e.what()
                          << std::endl;
            }
        }

        void StreamingService::Impl::disconnect() {
            std::cout << service_->name() << " request to disconnect from streams" << std::endl;
            service_->disconnect();
            std::cout << service_->name() << " disconnected " << std::endl;
        }


        int StreamingService::Impl::get_value(const json11::Json &config_data,
                                              const std::string &key,
                                              int def_val,
                                              int min_val,
                                              int max_val) {
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


        void StreamingService::Impl::get_next_frame(EngineData &output) {
            try {
                auto event_id = current_event_++;
                output.set_communication_id(event_id);

                auto event = service_->process_frame(event_id);
                output.set_data(service_->get_data_type(), std::move(event));
                output.set_description("data");

                processing_events_.insert(event_id);
            } catch (const StreamingServiceError &e) {
                std::ostringstream msg;
                msg << "Error getting the next frame " << current_event_
                    << " from streams " << "\n\n" << e.what();
                util::set_error(output, msg.str(), 1);
            }
        }


        void StreamingService::Impl::get_byte_order(EngineData &output) {
                try {
                    auto order = get_order(service_->get_byte_order());
                    output.set_data(type::STRING, order);
                    output.set_description("byte order");
                } catch (const StreamingServiceError &e) {
                    std::ostringstream msg;
                    msg << "Error requesting byte-order of a stream " << "\n\n" << e.what();
                    util::set_error(output, msg.str(), 1);
                }
        }


        std::string StreamingService::Impl::get_order(Endian endian) {
            switch (endian) {
                case Endian::Little:
                    return "LITTLE_ENDIAN";
                case Endian::Big:
                default:
                    return "BIG_ENDIAN";
            }
        }

        void StreamingService::Impl::get_event_count(EngineData &output) {
                output.set_data(type::SFIXED32, event_count_);
                output.set_description("event count");
        }

        void StreamingService::Impl::reset() {
                disconnect();
        }

    } // namespace stdlib
} // end namespace ersap
