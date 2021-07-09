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
