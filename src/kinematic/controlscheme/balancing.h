#ifndef _ROBOT_KINEMATIC_CONTROLSCHEMEBALANCING_H_
#define _ROBOT_KINEMATIC_CONTROLSCHEMEBALANCING_H_

#include <memory>
#include <chrono>
#include <boost/asio.hpp>

#include <robotconfig.h>

#if ROBOT_HAVE_BALANCE

#include <telemetry/telemetry.h>
#include <led/types.h>
#include <math/pid.h>
#include "abstractcontrolscheme.h"

namespace Robot::Kinematic {

    class ControlSchemeBalancing : public AbstractControlScheme, public std::enable_shared_from_this<ControlSchemeBalancing> {
        public: 
            using clock_type = std::chrono::high_resolution_clock;

            explicit ControlSchemeBalancing(std::shared_ptr<Kinematic> kinematic);
            ControlSchemeBalancing(const ControlSchemeBalancing&) = delete; // No copy constructor
            ControlSchemeBalancing(ControlSchemeBalancing&&) = delete; // No move constructor
            virtual ~ControlSchemeBalancing();

            virtual void init() override;
            virtual void cleanup() override;


            static void registerProperties(const std::shared_ptr<Context> &context);

        private:
            enum class State {
                IDLE,
                CRITICAL,
                WARNING,
                GRACE,
                OK,
            };

            std::weak_ptr<Robot::Telemetry::Telemetry> m_telemetry;
            std::weak_ptr<Robot::LED::Control> m_led_control;
            
            std::shared_ptr<Robot::LED::ColorLayer> m_layer;

            boost::asio::steady_timer m_init_timer;
            boost::signals2::connection m_imu_connection;

            bool m_armed_grace;
            clock_type::time_point m_armed_grace_start;
            bool m_armed;
            State m_state;
            float m_base_angle;

            Robot::Math::PID m_pid;

            void initMotors();

            void disarm();
            void arm();

            inline void updateState(State state);

            void onIMUData(const Robot::Telemetry::IMUData &data);
    };

}

#endif
#endif
