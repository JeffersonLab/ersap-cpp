

#ifndef ERSAP_ENGINE_METRICS_PUBLISHER_HPP
#define ERSAP_ENGINE_METRICS_PUBLISHER_HPP

#include <string>

namespace ersap {

/**
 * Allows a user Engine to publish arbitrary key-value metrics to the Monitor FE
 * during execute(). Call publish() any number of times inside execute(); metrics
 * are batched and sent as a single JSON message after execute() returns.
 *
 * Only active when ERSAP_MONITOR_FE is set. No-op otherwise.
 *
 * Example:
 * @code
 * EngineData MyEngine::execute(EngineData& input)
 * {
 *     double rate = compute_rate();
 *     EngineMetricsPublisher::publish("event_rate_hz", rate);
 *     EngineMetricsPublisher::publish("total_events",  total_++);
 *     return input;
 * }
 * @endcode
 */
class EngineMetricsPublisher
{
public:
    static void publish(const std::string& key, double value);

    static void publish(const std::string& key, long long value);

    // Called by ServiceEngine before execute() — clears the thread-local buffer
    static void clear();

    // Called by ServiceEngine after execute() — returns JSON and clears buffer
    static std::string drain_as_json();

private:
    EngineMetricsPublisher() = delete;
};

} // namespace ersap

#endif // end of include guard: ERSAP_ENGINE_METRICS_PUBLISHER_HPP
