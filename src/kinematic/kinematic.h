#ifndef _KINEMATIC_H_
#define _KINEMATIC_H_

#include <memory>
#include <mutex>

#include "kinematictypes.h"
#include "../common/withmutex.h"
#include "../robottypes.h"
#include "../motor/motorcontrol.h"
#include "../telemetry/telemetry.h"
#include "../rc/rcreceiver.h"

namespace Robot::Kinematic {

    class Kinematic : public std::enable_shared_from_this<Kinematic>, public WithMutex<std::recursive_mutex> {
        public:

            Kinematic(const std::shared_ptr<::Robot::Context> &context);
            Kinematic(const Kinematic&) = delete; // No copy constructor
            Kinematic(Kinematic&&) = delete; // No move constructor
            virtual ~Kinematic();

            void init(
                const std::shared_ptr<::Robot::Motor::Control> &motor_control, 
                const std::shared_ptr<::Robot::Telemetry::Telemetry> &telemetry, 
                const std::shared_ptr<::Robot::Input::Control> &input_control);
            void cleanup();

            void setSteeringMode(SteeringMode mode);
            SteeringMode getSteeringMode() const { return m_steering_mode; }

            void setDriveMode(DriveMode mode);
            DriveMode getDriveMode() const { return m_drive_mode; }

            void setOrientation(Orientation orientation);
            Orientation getOrientation() const { return m_orientation; }

            const std::shared_ptr<::Robot::Context> &context() const { return m_context; }
            const std::weak_ptr<::Robot::Motor::Control> &motorControl() { return m_motor_control; }

        private:
            bool m_initialized;

            std::shared_ptr<::Robot::Context> m_context;
            std::weak_ptr<::Robot::Motor::Control> m_motor_control;
            std::shared_ptr<class ControlScheme> m_control_scheme;

            boost::signals2::connection m_axis_connection;

            SteeringMode m_steering_mode;
            DriveMode    m_drive_mode;
            Orientation  m_orientation;

            void onSteer(double steering, double throttle, double aux_x, double aux_y);
    };


};

#endif
