#ifndef _ROBOT_TELEMETRY_HISTORY_H_
#define _ROBOT_TELEMETRY_HISTORY_H_

#include <array>
#include <cinttypes>
#include <chrono>
#include <motor/types.h>

namespace Robot::Telemetry {

    constexpr auto HISTORY_VALUE_COUNT { 60u };

    template<typename T>
    struct HistoryData {
        using value_type = T;
        using array_type = std::array<T, HISTORY_VALUE_COUNT>;

        typename array_type::size_type head;
        array_type values;

        HistoryData() :
            head { 0u }
        {
        }

        value_type &next() 
        {
            auto prev = head;
            head = (head+1)%values.size();
            return values[prev];
        }
    };

    using HistoryFloat = HistoryData<float>;

    using HistoryIMU = HistoryData<std::array<float,3>>;
    using HistoryMotorDuty = HistoryData<std::array<float,Motor::MOTOR_COUNT>>;
    using HistoryMotorRPM = HistoryData<std::array<float,Motor::MOTOR_COUNT*2>>;

}


#endif