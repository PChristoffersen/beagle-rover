#ifndef _ABSTRACTINPUTSOURCE_H_
#define _ABSTRACTINPUTSOURCE_H_

#include <memory>
#include "../inputsource.h"
#include "../inputvalue.h"
#include "../../robotcontext.h"
#include "../../common/withmutex.h"

namespace Robot::Input {

    template<typename T>
    class AbstractSource : public Source, public WithMutex<std::recursive_mutex> {
        public:
            AbstractSource(const Signals &signals) :
                Source(signals)
            {
            }

    };

};

#endif
