#include "events.h"

namespace Robot::Telemetry {


void EventMotor::update(ValueMap &map) const 
{
    map["duty"] = duty;
    map["rpm"] = rpm;
}

void EventServo::update(ValueMap &map) const 
{
    map["angle"] = angle;
}

void EventBattery::update(ValueMap &map) const 
{
    map["id"] = (uint32_t)battery_id;
    map["voltage"] = voltage;
    map["cells"] = (uint32_t)cell_voltage.size();
    /*
    auto cell_key = name+".cell.";
    for (int i=0; i<cell_voltage.size(); i++) {
        map[cell_key+std::to_string(i)] = cell_voltage[i];
    }
    */
}

void EventTemperature::update(ValueMap &map) const 
{
    map["temp"] = temperature;
}

void EventIMU::update(ValueMap &map) const 
{
    map["pitch"] = pitch;
    map["roll"] = roll;
    map["yaw"] = yaw;
}


}