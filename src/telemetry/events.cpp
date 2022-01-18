#include "events.h"

namespace Robot::Telemetry {


void EventBattery::update(ValueMap &map) const 
{
    map[name+".id"] = (uint32_t)battery_id;
    map[name+".voltage"] = voltage;
    map[name+".cells"] = (uint32_t)cell_voltage.size();
    /*
    auto cell_key = name+".cell.";
    for (int i=0; i<cell_voltage.size(); i++) {
        map[cell_key+std::to_string(i)] = cell_voltage[i];
    }
    */
}

void EventTemperature::update(ValueMap &map) const 
{
    map[name+".temp"] = temperature;
}

void EventMPU::update(ValueMap &map) const 
{
    //map[name+".pitch"] = temperature;
}


};