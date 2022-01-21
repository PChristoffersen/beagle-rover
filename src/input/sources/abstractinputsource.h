#ifndef _ROBOT_INPUT_ABSTRACTINPUTSOURCE_H_
#define _ROBOT_INPUT_ABSTRACTINPUTSOURCE_H_

#include <memory>
#include <robotcontext.h>
#include <common/withmutex.h>
#include "../types.h"
#include "../inputsource.h"

namespace Robot::Input {

    class AbstractSource : public Source, public WithMutex<std::recursive_mutex> {
        public:
            AbstractSource(const Signals &signals) :
                Source(signals)
            {
            }

    };

};

#endif
