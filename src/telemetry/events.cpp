#include "events.h"

namespace Robot::Telemetry {

void EventMotors::update(ValueMap &map) const
{
    
}

void EventBattery::update(ValueMap &map) const 
{
    map["id"] = (uint32_t)battery_id;
    map["voltage"] = voltage;
    map["cells"] = (uint32_t)cell_voltage.size();
}

void EventTemperature::update(ValueMap &map) const 
{
    map["temp"] = temperature;
}

void EventOdometer::update(ValueMap &map) const
{
    map["value"] = value;
}

void EventIMU::update(ValueMap &map) const 
{
    map["pitch"] = pitch;
    map["roll"] = roll;
    map["yaw"] = yaw;
}


}