#ifndef _ROBOT_HARDWARE_ABSTRACTPOWER_H_
#define _ROBOT_HARDWARE_ABSTRACTPOWER_H_

#include <memory>
#include <mutex>
#include <common/withmutex.h>
#include <boost/log/trivial.hpp>

namespace Robot::Hardware {

    class AbstractPower : public WithMutexStd, public std::enable_shared_from_this<AbstractPower> {
        public:
            AbstractPower();
            virtual ~AbstractPower() = default;

            uint setEnabled(bool enabled);
            
            bool enabled() const
            {
                return m_active_count>0;
            }

        protected:
            virtual void enable() = 0;
            virtual void disable() = 0;

        private:
            uint m_active_count;

    };

}

#endif
