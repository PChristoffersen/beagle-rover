#ifndef _ROBOT_TELEMETRY_RCMPU_H_
#define _ROBOT_TELEMETRY_RCMPU_H_

#include <memory>
#include <mutex>
#include <chrono>
#include <boost/asio.hpp>

#include <robotconfig.h>
#include "../../common/withmutex.h"
#include "../types.h"
#include "abstracttelemetrysource.h"

#if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE

namespace Robot::Telemetry {

    class RCMPU : public AbstractSource<RCMPU>, public WithMutex<std::mutex> {
        public:
            using clock_type = std::chrono::high_resolution_clock;

            explicit RCMPU(const std::shared_ptr<Robot::Context> &context);
            RCMPU(const RCMPU&) = delete; // No copy constructor
            RCMPU(RCMPU&&) = delete; // No move constructor
            virtual ~RCMPU();

            void init(const std::shared_ptr<Telemetry> &telemetry);
            void cleanup();

        protected:
            friend class Telemetry;

            Signal sig_event;

        private:
            static RCMPU *instance;
            bool m_initialized;
            boost::asio::steady_timer m_timer;

            // Interrupt data
            std::uint32_t m_data_count;
            rc_mpu_data_t m_data;
            clock_type::time_point m_last_temp_read;

            // Context thread data
            std::mutex m_data_mutex;
            std::uint32_t m_saved_data_count;
            rc_mpu_data_t m_saved_data;

            EventMPU m_event;



            void timer_setup();
            void timer(boost::system::error_code error);

            void data_callback();
    };

        
};

#endif
#endif
