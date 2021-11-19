#ifndef _TELEMETRYSOURCE_H_
#define _TELEMETRYSOURCE_H_

#include "telemetrytypes.h"

namespace Robot::Telemetry {

    class Source {
        public:
            virtual ~Source() {}

            virtual void init() = 0;
            virtual void cleanup() = 0;

        protected:
            friend class Telemetry;

            Signal sig_event;
    };

};

#endif