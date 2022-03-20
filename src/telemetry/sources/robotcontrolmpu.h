#ifndef _ROBOT_TELEMETRY_ROBOTCONTROLMPU_H_
#define _ROBOT_TELEMETRY_ROBOTCONTROLMPU_H_

#include <memory>
#include <mutex>
#include <chrono>
#include <boost/asio.hpp>

#include <robotconfig.h>

#if ROBOT_HAVE_ROBOTCONTROL_MPU

#include <common/withstrand.h>
#include <common/withmutex.h>
#include "../telemetry.h"
#include "abstracttelemetrysource.h"

namespace Robot::Telemetry {

    class RobotControlMPU : public AbstractSource<RobotControlMPU>, public WithStrand {
        public:
            using clock_type = std::chrono::high_resolution_clock;

            explicit RobotControlMPU(const std::shared_ptr<Robot::Context> &context);
            RobotControlMPU(const RobotControlMPU&) = delete; // No copy constructor
            RobotControlMPU(RobotControlMPU&&) = delete; // No move constructor
            virtual ~RobotControlMPU();

            void init(const std::shared_ptr<Telemetry> &telemetry);
            void cleanup();

        protected:
            friend class Telemetry;

            Signal sig_event;

        private:
            static RobotControlMPU *instance;
            bool m_initialized;

            clock_type::time_point m_last_telemetry;

            // Interrupt data
            rc_mpu_data_t m_data;

            // Context thread data
            rc_mpu_data_t m_saved_data;

            EventIMU m_event;

            inline void onData(const rc_mpu_data_t &data);
            inline void data_callback();
    };

        
}

#endif
#endif
