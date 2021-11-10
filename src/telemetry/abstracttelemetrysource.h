#ifndef _ABSTRACTTELEMETRYSOURCE_H_
#define _ABSTRACTTELEMETRYSOURCE_H_

#include <memory>
#include "telemetrysource.h"
#include "telemetrytypes.h"


template<typename T>
class AbstractTelemetrySource : public TelemetrySource, public std::enable_shared_from_this<T> {
    public:
        virtual ~AbstractTelemetrySource() { }

};

#endif
