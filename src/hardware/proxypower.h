#ifndef _ROBOT_HARDWARE_PROXYPOWER_H_
#define _ROBOT_HARDWARE_PROXYPOWER_H_

#include <boost/log/trivial.hpp>
#include "abstractpower.h"

namespace Robot::Hardware {

    class ProxyPower : public AbstractPower {
        public:
            ProxyPower(const std::shared_ptr<::Robot::Hardware::AbstractPower> &delegate) :
                m_delegate { delegate }
            {
            }

        protected:
            virtual void enable() override
            {
                m_delegate->setEnabled(true);
            }
            virtual void disable() override
            {
                m_delegate->setEnabled(false);
            }
        private:
            std::shared_ptr<::Robot::Hardware::AbstractPower> m_delegate;
    };

};

#endif
