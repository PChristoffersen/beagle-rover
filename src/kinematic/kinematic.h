#ifndef _ROBOT_KINEMATIC_KINEMATIC_H_
#define _ROBOT_KINEMATIC_KINEMATIC_H_

#include <memory>
#include <mutex>

#include <robottypes.h>
#include <common/withmutex.h>
#include <common/withnotify.h>
#include <motor/control.h>
#include <telemetry/telemetry.h>
#include <led/control.h>
#include <rc/receiver.h>
#include "types.h"

namespace Robot::Kinematic {

    class Kinematic : public std::enable_shared_from_this<Kinematic>, public WithMutex<std::recursive_mutex>, public WithNotifyDefault {
        public:

            Kinematic(const std::shared_ptr<::Robot::Context> &context);
            Kinematic(const Kinematic&) = delete; // No copy constructor
            Kinematic(Kinematic&&) = delete; // No move constructor
            virtual ~Kinematic();

            void init(
                const std::shared_ptr<::Robot::Motor::Control> &motor_control, 
                const std::shared_ptr<::Robot::LED::Control> &led_control, 
                const std::shared_ptr<::Robot::Telemetry::Telemetry> &telemetry, 
                const std::shared_ptr<::Robot::Input::Control> &input_control);
            void cleanup();

            void setDriveMode(DriveMode mode);
            DriveMode getDriveMode() const { return m_drive_mode; }

            void setOrientation(Orientation orientation);
            Orientation getOrientation() const { return m_orientation; }

            const std::shared_ptr<::Robot::Context> &context() const { return m_context; }
            const std::weak_ptr<::Robot::Motor::Control> &motorControl() { return m_motor_control; }
            const std::weak_ptr<::Robot::LED::Control> &ledControl() { return m_led_control; }
            const std::weak_ptr<::Robot::Telemetry::Telemetry> &telemetry() { return m_telemetry; }

        private:
            bool m_initialized;

            std::shared_ptr<::Robot::Context> m_context;
            std::weak_ptr<::Robot::Motor::Control> m_motor_control;
            std::weak_ptr<::Robot::LED::Control> m_led_control;
            std::weak_ptr<::Robot::Telemetry::Telemetry> m_telemetry;
            std::shared_ptr<class ControlScheme> m_control_scheme;

            boost::signals2::connection m_axis_connection;

            DriveMode    m_drive_mode;
            Orientation  m_orientation;

            void onSteer(float steering, float throttle, float aux_x, float aux_y);

            friend std::ostream &operator<<(std::ostream &os, const Kinematic &self)
            {
                return os << "Robot::Kinematic::Kinematic";
            }
    };


}

#endif
