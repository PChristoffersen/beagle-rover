#ifndef _INPUTSOURCE_H_
#define _INPUTSOURCE_H_

#include "inputtypes.h"

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
        
        protected:
            const Signals &m_signals;
    };

};

#endif
