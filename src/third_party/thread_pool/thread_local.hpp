// Workaround for older compilers not having thread_local storage.
//
// Based on <http://stackoverflow.com/q/16147621> by
// <http://stackoverflow.com/users/1412605/pontomedon>.
//
// Licensed under CC-BY-SA <https://creativecommons.org/licenses/by-sa/3.0/>.

#ifndef DETAIL_THREAD_LOCAL_H
#define DETAIL_THREAD_LOCAL_H

#include <utility>

#include <stdio.h>
#include <pthread.h>

namespace tp {
namespace detail {

template <class T>
class ThreadLocal
{
private:
    static pthread_key_t key;
    static pthread_once_t once_control;

    static void make_key()
    {
        (void) pthread_key_create(&ThreadLocal::key, ThreadLocal::destructor);
    }

    static void destructor(void* obj)
    {
        delete ((T*) obj);
    }

public:
    template <typename ...Args>
    static T* getThreadInstance(Args&&... args)
    {
        pthread_once(&once_control, ThreadLocal::make_key);
        T* instance = (T*) pthread_getspecific(key);
        if (!instance) {
            try {
                instance = new T(std::forward<Args>(args)...);
                pthread_setspecific(key, instance);
            } catch (const char* ex) {
                printf("Exception during thread local singleton init: %s\n", ex);
            }
        }
        return instance;
    }
};

template <class T>
pthread_key_t ThreadLocal<T>::key;

template <class T>
pthread_once_t ThreadLocal<T>::once_control = PTHREAD_ONCE_INIT;

} // end namespace detail
} // end namespace tp

#endif  // DETAIL_THREAD_LOCAL_H
