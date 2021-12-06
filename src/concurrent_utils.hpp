

#ifndef ERSAP_CONCURRENT_UTILS_HPP
#define ERSAP_CONCURRENT_UTILS_HPP

#include <algorithm>
#include <atomic>
#include <memory>
#include <mutex>
#include <vector>

#include <blockingconcurrentqueue.h>
#include <thread_pool.hpp>

namespace ersap {
namespace util {

template <typename Task>
class BlockingQueue
{
public:
    explicit BlockingQueue(size_t /*size*/)
    {
        // TODO: use size parameter
    }

    template <typename U>
    bool push(U&& data)
    {
        return queue_.try_enqueue(std::forward<U>(data));
    }

    bool pop(Task& data)
    {
        return queue_.try_dequeue(data);
    }

    bool pop(Task& data, std::int64_t timeout_usecs)
    {
        return queue_.wait_dequeue_timed(data, timeout_usecs);
    }

private:
    moodycamel::BlockingConcurrentQueue<Task> queue_;
};


using ThreadPool = tp::ThreadPoolImpl<tp::FixedFunction<void(), 128>,
                                      BlockingQueue>;


template <typename T>
class ConcurrentRange
{
public:
    using const_iterator = typename T::const_iterator;

public:
    ConcurrentRange(std::shared_ptr<T> cont)
      : cont_{std::move(cont)}
    {
        // nop
    }

    const_iterator begin() const { return cont_->begin(); };

    const_iterator end() const { return cont_->end(); };

private:
    std::shared_ptr<T> cont_;
};


template <typename T>
class CopyOnWriteVector
{
public:
    using pointer_type = std::shared_ptr<T>;
    using container_type = std::vector<pointer_type>;
    using range_type = util::ConcurrentRange<container_type>;

    CopyOnWriteVector()
      : cont_{std::make_shared<container_type>()}
    {
        // nop
    }

    void add(const pointer_type& elem)
    {
        std::lock_guard<std::mutex> lock{m_};
        auto copy = std::make_shared<container_type>(*cont_);
        copy->push_back(elem);
#if !defined(__APPLE__) && defined(__GNUC__) && __GNUC__ < 5
        cont_ = copy;
#else
        std::atomic_exchange(&cont_, copy);
#endif
    }

    void remove(const pointer_type& elem)
    {
        std::lock_guard<std::mutex> lock{m_};
        auto copy = std::make_shared<container_type>(*cont_);
        copy->erase(std::remove(copy->begin(), copy->end(), elem),
                    copy->end());
#if !defined(__APPLE__) && defined(__GNUC__) && __GNUC__ < 5
        cont_ = copy;
#else
        std::atomic_exchange(&cont_, copy);
#endif
    }

    range_type view() const
    {
#if !defined(__APPLE__) && defined(__GNUC__) && __GNUC__ < 5
        std::lock_guard<std::mutex> lock{m_};
        return {cont_};
#else
        return {std::atomic_load(&cont_)};
#endif
    }

private:
    mutable std::mutex m_;
    std::shared_ptr<container_type> cont_;
};

} // end namespace util
} // end namespace ersap

#endif // end of include guard: ERSAP_CONCURRENT_UTILS_HPP
