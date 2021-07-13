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

#ifndef ERSAP_ENGINE_DATA_HELPER_HPP
#define ERSAP_ENGINE_DATA_HELPER_HPP

#include <ersap/engine_data.hpp>
#include <ersap/engine_data_type.hpp>
#include <ersap/engine_status.hpp>

#include <xmsg/message.h>

#include <memory>
#include <string>
#include <iostream>

namespace ersap {

class EngineDataAccessor final
{
public:
    EngineData create(any&& data, std::unique_ptr<xmsg::proto::Meta>&& meta)
    {
        return EngineData{std::move(data), std::move(meta)};
    }

    const xmsg::proto::Meta* view_meta(const EngineData& data)
    {
        return data.meta_.get();
    }


    xmsg::proto::Meta* view_meta(EngineData& data)
    {
        return data.meta_.get();
    }

    std::unique_ptr<xmsg::proto::Meta>& move_meta(EngineData& data)
    {
        return data.meta_;
    }

    xmsg::Message serialize(const EngineData& data,
                            const xmsg::Topic& topic,
                            const std::vector<EngineDataType>& data_types)
    {
        using Msg = xmsg::Message;

        auto& mime_type = data.meta_->datatype();
        for (auto&& dt : data_types) {
            if (dt.mime_type() == mime_type) {
                try {
                    auto bb = dt.serializer()->write(data.data_);
                    auto mm = xmsg::proto::copy_meta(*data.meta_);
                    return Msg{topic, std::move(mm), std::move(bb)};
                } catch (const std::exception& e) {
                    throw std::runtime_error{"could not serialize " + mime_type + ": " + e.what()};
                }
            }
        }
        if (mime_type == type::STRING.mime_type()) {
            auto bb = type::STRING.serializer()->write(data.data_);
            auto mm = xmsg::proto::copy_meta(*data.meta_);
            return Msg{topic, std::move(mm), std::move(bb)};
        }
        throw std::runtime_error{"unsupported output mime-type = " + mime_type};
    }


    EngineData deserialize(const xmsg::Message& msg,
                           const std::vector<EngineDataType>& data_types)
    {
        auto* metadata = msg.meta();
        auto& mime_type = metadata->datatype();
        for (auto&& dt : data_types) {
std::cout << dt.mime_type() << " - " << mime_type << "\n"; //vg 07.11
            if (dt.mime_type() == mime_type) {
                try {
                    auto user_data = dt.serializer()->read(msg.data());
                    auto user_meta = xmsg::proto::copy_meta(*metadata);
                    return create(std::move(user_data), std::move(user_meta));
                } catch (const std::exception& e) {
                    throw std::runtime_error{"could not deserialize " + mime_type + ": " + e.what()};
                }
            }
        }
        throw std::runtime_error{"unsupported input mime-type = " + mime_type};
    }
};

} // end namespace ersap

#endif // end of include guard: ERSAP_ENGINE_DATA_HELPER_HPP
