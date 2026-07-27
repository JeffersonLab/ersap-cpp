
#include <ersap/engine_metrics_publisher.hpp>

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <vector>

namespace {

struct MetricEntry
{
    std::string key;
    bool is_double;
    double dval;
    long long ival;
};

thread_local std::vector<MetricEntry> buffer;

} // end anonymous namespace


namespace ersap {

void EngineMetricsPublisher::publish(const std::string& key, double value)
{
    buffer.push_back({key, true, value, 0});
}


void EngineMetricsPublisher::publish(const std::string& key, long long value)
{
    buffer.push_back({key, false, 0.0, value});
}


void EngineMetricsPublisher::clear()
{
    buffer.clear();
}


std::string EngineMetricsPublisher::drain_as_json()
{
    if (buffer.empty()) {
        return "";
    }
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer{sb};
    writer.StartObject();
    for (auto& e : buffer) {
        writer.Key(e.key.c_str());
        if (e.is_double) {
            writer.Double(e.dval);
        } else {
            writer.Int64(e.ival);
        }
    }
    writer.EndObject();
    buffer.clear();
    return sb.GetString();
}

} // namespace ersap
