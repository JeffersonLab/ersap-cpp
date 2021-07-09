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

#ifndef ERSAP_ENGINE_DATA_HPP
#define ERSAP_ENGINE_DATA_HPP

#include <ersap/any.hpp>
#include <ersap/engine_status.hpp>

#include <memory>
#include <string>
#include <type_traits>

namespace xmsg {
namespace proto {
class Meta;
} // end namespace proto
} // end namespace xmsg

namespace ersap {

class EngineDataType;

class EngineData final
{
public:
    EngineData();

    EngineData(const EngineData& rhs);
    EngineData& operator=(const EngineData& rhs);

    EngineData(EngineData&& rhs) noexcept;
    EngineData& operator=(EngineData&& rhs) noexcept;

    ~EngineData();

public:
    const std::string& mime_type() const;

    const any& data() const
    {
        return data_;
    }

    any& data()
    {
        return data_;
    }

    template<typename S, typename T>
    void set_data(S&& type, T&& data)
    {
        set_mime_type(std::forward<S>(type));
        set_value(std::forward<T>(data));
    }

    bool has_data()
    {
        return data_.has_value();
    }

private:
    void set_mime_type(const std::string& mime_type);
    void set_mime_type(const EngineDataType& data_type);

    template<typename T>
    void set_value(T&& data) { data_ = std::forward<T>(data); }

    void set_value(const char* data) { data_ = std::string{data}; }

public:
    const std::string& description() const;

    void set_description(const std::string& description);

    EngineStatus status() const;

    int status_severity() const;

    void set_status(EngineStatus status);

    void set_status(EngineStatus status, int severity);

public:
    const std::string& engine_state() const;

    void set_engine_state(const std::string& state);

    const std::string& engine_name() const;

    const std::string& engine_version() const;

public:
    long communication_id() const;

    void set_communication_id(long id);

    const std::string& composition() const;

    long execution_time() const;

private:
    friend class EngineDataAccessor;
    using Meta = xmsg::proto::Meta;

    EngineData(any&& data, std::unique_ptr<Meta>&& meta);

    any data_;
    std::unique_ptr<Meta> meta_;
};


template<typename T>
T& data_cast(EngineData& data)
{
    using V = std::add_lvalue_reference_t<T>;
    return any_cast<V>(data.data());
}


template<typename T>
const T& data_cast(const EngineData& data)
{
    using V = std::add_lvalue_reference_t<std::add_const_t<T>>;
    return any_cast<V>(data.data());
}


} // end namespace ersap

#endif // end of include guard: ERSAP_ENGINE_DATA_HPP
