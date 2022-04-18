#ifndef _ROBOT_TELEMETRY_TELEMETRY_H_
#define _ROBOT_TELEMETRY_TELEMETRY_H_

#include <cstdint>
#include <memory>
#include <mutex>
#include <map>
#include <variant>
#include <string>
#include <unordered_map>
#include <boost/asio.hpp>

#include <robottypes.h>
#include <rc/receiver.h>

#include <common/withmutex.h>
#include <common/withnotify.h>
#include <common/withstrand.h>
#include "types.h"
#include "events.h"
#include "history.h"

namespace Robot::Telemetry {

    class Telemetry : public std::enable_shared_from_this<Telemetry>, public WithMutexStd, public WithNotifyInt, public WithStrand {
        public:
            using history_timer_type = boost::asio::high_resolution_timer;
            using history_clock_type = history_timer_type::clock_type;

            static constexpr notify_type NOTIFY_IMU { 1 };
            static constexpr notify_type NOTIFY_ODOMETER { 2 };

            explicit Telemetry(const std::shared_ptr<::Robot::Context> &context);
            Telemetry(const Telemetry&) = delete; // No copy constructor
            Telemetry(Telemetry&&) = delete; // No move constructor
            virtual ~Telemetry();

            void init();
            void cleanup();

            Signal sig_event;
            #if ROBOT_HAVE_IMU
            IMUSignal sig_imu;
            #endif

            const ValueMap &imuValues() const { return m_imu_values; }
            const ValueMap &odometerValues() const { return m_odometer_values; }

            const HistoryIMU &historyIMU() const { return m_history_imu; }
            const HistoryMotorDuty &historyMotorDuty() const { return m_history_motor_duty; }
            const HistoryMotorRPM &historyMotorRPM() const { return m_history_motor_rpm; }

            std::int64_t historyLastMS() const;
            std::int64_t historyIntervalMS() const;

            void addSource(std::shared_ptr<class Source> source);

        protected:
            friend class Source;
            
            void process(const Event &event);

            #if ROBOT_HAVE_IMU
            void process(const IMUData &data);
            #endif

        private:
            bool m_initialized;
            std::vector<std::shared_ptr<class Source>> m_sources;

            ValueMap m_imu_values;
            ValueMap m_odometer_values;

            EventIMU m_imu_event;
            HistoryIMU m_history_imu;

            EventMotors m_motors_event;
            HistoryMotorDuty m_history_motor_duty;
            HistoryMotorRPM m_history_motor_rpm;

            history_clock_type::time_point m_base_history;
            history_clock_type::time_point m_last_history;
            history_timer_type m_timer;

            void resetHistory();

            void timerSetup();

    };
        
}

#endif
