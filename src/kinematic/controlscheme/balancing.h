#ifndef _ROBOT_KINEMATIC_CONTROLSCHEMEBALANCING_H_
#define _ROBOT_KINEMATIC_CONTROLSCHEMEBALANCING_H_

#include <memory>
#include <chrono>
#include <boost/asio.hpp>

#include <robotconfig.h>
#include <telemetry/telemetry.h>
#include <led/control.h>
#include "abstractcontrolscheme.h"

#if ROBOT_HAVE_BALANCE

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

        private:
            std::weak_ptr<Robot::Telemetry::Telemetry> m_telemetry;
            std::weak_ptr<Robot::LED::Control> m_led_control;

            boost::asio::steady_timer m_init_timer;

            boost::signals2::connection m_mpu_connection;

            rc_filter_t m_D1;
            rc_filter_t m_D2;
            rc_filter_t m_D3;

            bool m_armed;

            int m_inner_saturation_counter;

            struct {
                //arm_state_t arm_state;	///< see arm_state_t declaration
                //drive_mode_t drive_mode;///< NOVICE or ADVANCED
                double theta;		///< body lean angle (rad)
                double phi;		///< wheel position (rad)
                double phi_dot;		///< rate at which phi reference updates (rad/s)
                double gamma;		///< body turn angle (rad)
                double gamma_dot;	///< rate at which gamma setpoint updates (rad/s)
            } m_setpoint;

            struct {
                double wheelAngleR;	///< wheel rotation relative to body
                double wheelAngleL;
                double theta;		///< body angle radians
                double phi;		///< average wheel angle in global frame
                double gamma;		///< body turn (yaw) angle radians
                double vBatt;		///< battery voltage
                double d1_u;		///< output of balance controller D1 to motors
                double d2_u;		///< output of position controller D2 (theta_ref)
                double d3_u;		///< output of steering controller D3 to motors
                double mot_drive;	///< u compensated for battery voltage
            } m_cstate;



            void initMotors();
            void initFilters();

            void disarm();
            void arm();

            void onMPUData(const Robot::Telemetry::MPUData &data);
    };

};

#endif
#endif
