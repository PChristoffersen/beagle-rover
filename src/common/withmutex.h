#ifndef _ROBOT_WITHMUTEX_H_
#define _ROBOT_WITHMUTEX_H_

#include <mutex>
#include <shared_mutex>
#include <boost/log/trivial.hpp>

namespace Robot {

    template<typename MUTEX>
    class WithMutex {
        public:
            using mutex_type = MUTEX;
            using guard = std::lock_guard<MUTEX>;

            void mutex_lock() { m_mutex.lock(); }
            void mutex_unlock() { m_mutex.unlock(); }
            void mutex_trylock() { m_mutex.trylock(); }

            MUTEX &mutex() { return m_mutex; }
        protected:
            MUTEX m_mutex;
    };

    template<typename MUTEX>    
    class shared_guard {
        public:
            shared_guard(MUTEX &mutex) : m_mutex { mutex } 
            { 
                m_mutex.lock_shared();
            }
            ~shared_guard() 
            {
                m_mutex.unlock_shared();
            }
        private:
            MUTEX &m_mutex;
    };

}

#endif
