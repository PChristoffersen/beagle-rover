#ifndef _ROBOT_INPUT_SOURCE_H_
#define _ROBOT_INPUT_SOURCE_H_

#include "types.h"

namespace Robot::Input {

    class Source {
        public:
            Source(const Signals &signals) :
                m_signals { signals }
            {
            }
            virtual ~Source() = default;

            virtual void cleanup() = 0;

            virtual void setEnabled(bool enabled) = 0;
            virtual void setEnabledKinematic(bool enabled) = 0;
            virtual void setEnabledLED(bool enabled) = 0;
        
        protected:
            const Signals &m_signals;
    };

}

#endif
