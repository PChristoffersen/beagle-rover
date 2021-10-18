#ifndef _TELEMETRYTYPES_H_
#define _TELEMETRYTYPES_H_

#include <vector>
#include <boost/signals2.hpp>

typedef boost::signals2::signal<void(const class TelemetryEvent &)> TelemetrySignal_t;


class TelemetryEvent {
    public:
        virtual ~TelemetryEvent() = default;
};


class TelemetryEventBattery : public TelemetryEvent {
    public:
        TelemetryEventBattery() {}
        uint8_t battery_id;
        std::vector<float> cell_voltage;
};


#endif