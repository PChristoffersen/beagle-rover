#ifndef _TELEMETRYSOURCE_H_
#define _TELEMETRYSOURCE_H_

#include "telemetrytypes.h"

class TelemetrySource {
    public:
        virtual ~TelemetrySource() {}

        virtual void init() = 0;
        virtual void cleanup() = 0;

    protected:
        friend class Telemetry;

        TelemetrySignal sig_event;
};

#endif