#ifndef _WITHMUTEX_H_
#define _WITHMUTEX_H_

#include <mutex>

namespace Robot {

    template<typename MUTEX>
    class WithMutex {
        public:
            void lock() { m_mutex.lock(); }
            void unlock() { m_mutex.unlock(); }
            void trylock() { m_mutex.trylock(); }

            MUTEX &mutex() { return m_mutex; }
        protected:
            MUTEX m_mutex;
    };
    
};

#endif
