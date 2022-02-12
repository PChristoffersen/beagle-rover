#ifndef _ROBOT_TELEMETRY_ROBOTCONTROLMPU_H_
#define _ROBOT_TELEMETRY_ROBOTCONTROLMPU_H_

#include <memory>
#include <mutex>
#include <chrono>
#include <boost/asio.hpp>

#include <robotconfig.h>
#include "../../common/withmutex.h"
#include "../telemetry.h"
#include "abstracttelemetrysource.h"

#if ROBOT_HAVE_ROBOTCONTROL_MPU

namespace Robot::Telemetry {

    class RobotControlMPU : public AbstractSource<RobotControlMPU>, public WithMutex<std::mutex> {
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
            boost::asio::steady_timer m_timer;

            // Interrupt data
            std::uint32_t m_data_count;
            rc_mpu_data_t m_data;
            clock_type::time_point m_last_temp_read;

            // Context thread data
            std::mutex m_data_mutex;
            rc_mpu_data_t m_saved_data;
            std::uint32_t m_saved_data_count;

            EventMPU m_event;



            void timer_setup();
            void timer(boost::system::error_code error);

            void data_callback();

    };

        
}

#endif
#endif
