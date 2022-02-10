#ifndef _ROBOT_WITHMUTEX_H_
#define _ROBOT_WITHMUTEX_H_

#include <mutex>

namespace Robot {

    template<typename MUTEX>
    class WithMutex {
        public:
            using guard = std::lock_guard<MUTEX>;

            void mutex_lock() { m_mutex.lock(); }
            void mutex_unlock() { m_mutex.unlock(); }
            void mytex_trylock() { m_mutex.trylock(); }

            MUTEX &getMutex() { return m_mutex; }
        protected:
            MUTEX m_mutex;
    };
    
};

#endif
