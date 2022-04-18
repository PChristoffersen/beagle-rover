#ifndef _ABSTRACTTELEMETRYSOURCE_H_
#define _ABSTRACTTELEMETRYSOURCE_H_

#include <memory>
#include "../telemetrysource.h"
#include "../types.h"
#include <robotcontext.h>

namespace Robot::Telemetry {

    template<typename T>
    class AbstractSource : public Source, public std::enable_shared_from_this<T> {
        public:
            AbstractSource() { }
            virtual ~AbstractSource() = default;

    };

}

#endif
