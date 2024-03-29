#ifndef _ROBOT_DEBUG_H_
#define _ROBOT_DEBUG_H_

#ifdef ROBOT_DEBUG

#include <memory>
#include <mutex>


#include <common/notifysubscription.h>
#include <common/withnotify.h>
#include <common/withmutex.h>

namespace Robot::Debug {

    /**
     * @brief Test class for running Python tests
     * 
     */
    class TestComponent : public std::enable_shared_from_this<TestComponent>, public WithMutexStd, public WithNotifyInt {
        public:
            TestComponent();
            virtual ~TestComponent();

            void ping(notify_type what) const;

        private:
            friend std::ostream &operator<<(std::ostream &os, const TestComponent &self) 
            {
                return os << "Robot::Debug::TestComponent";
            }

    };

}

#endif

#endif
