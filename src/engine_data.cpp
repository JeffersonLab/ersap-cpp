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

#include <ersap/engine_data.hpp>
#include <ersap/engine_data_type.hpp>

#include <xmsg/proto/meta.h>

namespace ersap {

EngineData::EngineData()
  : data_{}
  , meta_{std::make_unique<Meta>()}
{
    // nop
}


EngineData::EngineData(any&& data, std::unique_ptr<Meta>&& meta)
  : data_{std::move(data)}
  , meta_{std::move(meta)}
{
    // nop
}


EngineData::EngineData(const EngineData& rhs)
  : data_{rhs.data_}
  , meta_{xmsg::proto::copy_meta(*rhs.meta_)}
{
    // nop
}


EngineData& EngineData::operator=(const EngineData& rhs)
{
    data_ = rhs.data_;
    meta_ = xmsg::proto::copy_meta(*rhs.meta_);
    return *this;
}


EngineData::EngineData(EngineData&&) noexcept = default;
EngineData& EngineData::operator=(EngineData&&) noexcept = default;

EngineData::~EngineData() = default;


const std::string& EngineData::mime_type() const
{
    return meta_->datatype();
}


void EngineData::set_mime_type(const std::string& mime_type)
{
    meta_->set_datatype(mime_type);
}


void EngineData::set_mime_type(const EngineDataType& data_type)
{
    meta_->set_datatype(data_type.mime_type());
}


const std::string& EngineData::description() const
{
    return meta_->description();
}


EngineStatus EngineData::status() const
{
    auto status = meta_->status();
    switch (status) {
        case xmsg::proto::Meta::INFO:
            return EngineStatus::INFO;
        case xmsg::proto::Meta::WARNING:
            return EngineStatus::WARNING;
        case xmsg::proto::Meta::ERROR:
            return EngineStatus::ERROR;
        default:
            throw std::domain_error{"unexpected meta status"};
    }
}


int EngineData::status_severity() const
{
    return meta_->severityid();
}


void EngineData::set_status(EngineStatus status)
{
    set_status(status, 1);
}


void EngineData::set_status(EngineStatus status, int severity)
{
    switch (status) {
        case EngineStatus::INFO:
            meta_->set_status(xmsg::proto::Meta::INFO);
            break;
        case EngineStatus::WARNING:
            meta_->set_status(xmsg::proto::Meta::WARNING);
            break;
        case EngineStatus::ERROR:
            meta_->set_status(xmsg::proto::Meta::ERROR);
            break;
        default:
            throw std::domain_error{"unexpected engine status"};
    }
    meta_->set_severityid(severity);
}


void EngineData::set_description(const std::string& description)
{
    meta_->set_description(description);
}


const std::string& EngineData::engine_state() const
{
    return meta_->senderstate();
}


void EngineData::set_engine_state(const std::string& state)
{
    meta_->set_senderstate(state);
}


const std::string& EngineData::engine_name() const
{
    return meta_->author();
}


const std::string& EngineData::engine_version() const
{
    return meta_->version();
}


long EngineData::communication_id() const
{
    return meta_->communicationid();
}


void EngineData::set_communication_id(long id)
{
    meta_->set_communicationid(id);
}


const std::string& EngineData::composition() const
{
    return meta_->composition();
}


long EngineData::execution_time() const
{
    return meta_->executiontime();
}

} // end namespace ersap
