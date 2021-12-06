

#ifndef ERSAP_CONCURRENT_MAP_HPP
#define ERSAP_CONCURRENT_MAP_HPP

#include <memory>
#include <mutex>
#include <unordered_map>

namespace ersap {
namespace util {

template <typename K, typename T>
class ConcurrentMap
{
public:
    using key_type = K;
    using element_type = T;
    using mapped_type = std::shared_ptr<element_type>;

public:
    ConcurrentMap() = default;

    ConcurrentMap(const ConcurrentMap&) = delete;
    ConcurrentMap(ConcurrentMap&&) = delete;

    ~ConcurrentMap() = default;

    template <typename... Args>
    mapped_type insert(const key_type& name, Args&&... args)
    {
        std::unique_lock<std::mutex> lock{mutex_};

        auto data = std::make_shared<T>(std::forward<Args>(args)...);
        auto rv = cont_.insert({name, data});
        if (rv.second) {
            return rv.first->second;
        }
        return nullptr;
    }

    mapped_type find(const key_type& name)
    {
        std::unique_lock<std::mutex> lock{mutex_};

        auto it = cont_.find(name);
        if (it != cont_.end()) {
            return it->second;
        }
        return nullptr;
    }

    mapped_type remove(const key_type& name)
    {
        std::unique_lock<std::mutex> lock{mutex_};

        auto it = cont_.find(name);
        if (it == cont_.end()) {
            return nullptr;
        }
        auto c = std::move(it->second);
        cont_.erase(it);
        return c;
    }

    template <typename F>
    void for_each(F f)
    {
        for (auto& x : cont_) {
            f(x.second.get());
        }
    }

    void clear()
    {
        std::unique_lock<std::mutex> lock{mutex_};
        cont_.clear();
    }

private:
    std::mutex mutex_;
    std::unordered_map<key_type, mapped_type> cont_;
};

} // end namespace util
} // end namespace ersap

#endif // end of include guard: ERSAP_CONCURRENT_MAP_HPP
