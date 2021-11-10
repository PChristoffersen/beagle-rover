#ifndef _TELEMETRYTYPES_H_
#define _TELEMETRYTYPES_H_

#include <cstdint>
#include <vector>
#include <boost/signals2.hpp>

using TelemetrySignal = boost::signals2::signal<void(const class TelemetryEvent &)>;


class TelemetryEvent {
    public:
        virtual ~TelemetryEvent() = default;
};


class TelemetryEventBattery : public TelemetryEvent {
    public:
        TelemetryEventBattery() {}
        std::uint8_t battery_id;
        std::vector<float> cell_voltage;
};


#endif