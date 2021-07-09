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

#include <ersap/engine_data_type.hpp>

#include <xmsg/constants.h>
#include <xmsg/proto/data.h>

#include <memory>

namespace {

namespace vl { // variable lenght integers (not provided by xmsg helpers)

using xmsg::proto::Data;
using xmsg::proto::detail::set_repeated;

template <typename T>
inline void set_value(Data& /*data*/, const T& /*value*/)
        { static_assert(sizeof(T) == 0, "Unsupported data type"); }

inline void set_value(Data& data, std::int32_t value)
        { data.set_vlsint32(value); }
inline void set_value(Data& data, std::int64_t value)
        { data.set_vlsint64(value); }

inline void set_value(Data& data, const std::vector<std::int32_t>& value)
        { set_repeated(data.mutable_vlsint32a(), value); }
inline void set_value(Data& data, const std::vector<std::int64_t>& value)
        { set_repeated(data.mutable_vlsint64a(), value); }

template <typename T>
inline T get_value(const Data& /*data*/)
        { static_assert(sizeof(T) == 0, "Unsupported data type"); return T{}; }

template<> inline std::int32_t get_value(const Data& data)
        { return data.vlsint32(); }
template<> inline std::int64_t get_value(const Data& data)
        { return data.vlsint64(); }

template<> inline std::vector<std::int32_t> get_value(const Data& data)
        { auto& a = data.vlsint32a(); return {a.begin(), a.end()}; }
template<> inline std::vector<std::int64_t> get_value(const Data& data)
        { auto& a = data.vlsint64a(); return {a.begin(), a.end()}; }
}

// ---------------------------------------------------------------------------

template<typename T>
class PrimitiveSerializer : public ersap::Serializer
{
public:
    std::vector<std::uint8_t> write(const ersap::any& data) const override
    {
        const T& value = ersap::any_cast<const T&>(data);
        const auto xdata = xmsg::proto::make_data(value);
        return xmsg::proto::serialize_data(xdata);
    }

    ersap::any read(const std::vector<std::uint8_t>& buffer) const override
    {
        const auto xdata = xmsg::proto::parse_data(buffer);
        return { xmsg::proto::parse_data<T>(xdata) };
    }
};


template<typename T>
class VariableLengthSerializer : public ersap::Serializer
{
public:
    std::vector<std::uint8_t> write(const ersap::any& data) const override
    {
        const T& value = ersap::any_cast<const T&>(data);
        auto xdata = xmsg::proto::Data{};
        vl::set_value(xdata, value);
        return xmsg::proto::serialize_data(xdata);
    }

    ersap::any read(const std::vector<std::uint8_t>& buffer) const override
    {
        const auto xdata = xmsg::proto::parse_data(buffer);
        return { vl::get_value<T>(xdata) };
    }
};


class RawBytesSerializer : public ersap::Serializer
{
public:
    std::vector<std::uint8_t> write(const ersap::any& data) const override
    {
        return ersap::any_cast<std::vector<std::uint8_t>>(data);
    }

    std::vector<std::uint8_t> write(ersap::any&& data) const override
    {
        return ersap::any_cast<std::vector<std::uint8_t>>(std::move(data));
    }

    ersap::any read(const std::vector<std::uint8_t>& buffer) const override
    {
        return { buffer };
    }

    ersap::any read(std::vector<std::uint8_t>&& buffer) const override
    {
        return { std::move(buffer) };
    }
};


class NativeSerializer : public ersap::Serializer
{
public:
    std::vector<std::uint8_t> write(const ersap::any& data) const override
    {
        const auto& value = ersap::any_cast<const xmsg::proto::Data&>(data);
        return xmsg::proto::serialize_data(value);
    }

    ersap::any read(const std::vector<std::uint8_t>& buffer) const override
    {
        return { xmsg::proto::parse_data(buffer) };
    }
};


class StringSerializer : public ersap::Serializer
{
public:
    std::vector<std::uint8_t> write(const ersap::any& data) const override
    {
        const auto& value = ersap::any_cast<const std::string&>(data);
        return { std::begin(value), std::end(value) };
    }

    ersap::any read(const std::vector<std::uint8_t>& buffer) const override
    {
        return { std::string{std::begin(buffer), std::end(buffer)} };
    }
};

// ---------------------------------------------------------------------------

template<typename T> auto s_primitive()
{
    return std::make_unique<PrimitiveSerializer<T>>();
}

template<typename T> auto s_vector()
{
    return std::make_unique<PrimitiveSerializer<std::vector<T>>>();
}

template<typename T> auto vl_primitive()
{
    return std::make_unique<VariableLengthSerializer<T>>();
}

template<typename T> auto vl_vector()
{
    return std::make_unique<VariableLengthSerializer<std::vector<T>>>();
}

} // end namespace

// ---------------------------------------------------------------------------

namespace ersap {

namespace type {

namespace mt = xmsg::mimetype;

const EngineDataType SINT32 { mt::single_sint32, vl_primitive<std::int32_t>() };
const EngineDataType SINT64 { mt::single_sint64, vl_primitive<std::int64_t>() };
const EngineDataType SFIXED32 { mt::single_sfixed32, s_primitive<std::int32_t>() };
const EngineDataType SFIXED64 { mt::single_sfixed64, s_primitive<std::int64_t>() };
const EngineDataType FLOAT { mt::single_float, s_primitive<float>() };
const EngineDataType DOUBLE { mt::single_double, s_primitive<double>() };
const EngineDataType STRING { mt::single_string, s_primitive<std::string>() };

const EngineDataType BYTES { "binary/bytes", std::make_unique<RawBytesSerializer>() };

const EngineDataType ARRAY_SINT32 { mt::array_sint32, vl_vector<std::int32_t>() };
const EngineDataType ARRAY_SINT64 { mt::array_sint64, vl_vector<std::int64_t>() };
const EngineDataType ARRAY_SFIXED32 { mt::array_sfixed32, s_vector<std::int32_t>() };
const EngineDataType ARRAY_SFIXED64 { mt::array_sfixed64, s_vector<std::int64_t>() };
const EngineDataType ARRAY_FLOAT { mt::array_float, s_vector<float>() };
const EngineDataType ARRAY_DOUBLE { mt::array_double, s_vector<double>() };
const EngineDataType ARRAY_STRING { mt::array_string, s_vector<std::string>() };

const EngineDataType JSON { "application/json", std::make_unique<StringSerializer>() };
const EngineDataType NATIVE { "native", std::make_unique<NativeSerializer>() };

} // end namespace type
} // end namespace ersap
