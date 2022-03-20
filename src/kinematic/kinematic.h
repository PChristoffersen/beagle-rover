#ifndef _ROBOT_KINEMATIC_KINEMATIC_H_
#define _ROBOT_KINEMATIC_KINEMATIC_H_

#include <memory>
#include <mutex>

#include <robottypes.h>
#include <common/withstrand.h>
#include <common/withmutex.h>
#include <common/withnotify.h>
#include <motor/control.h>
#include <telemetry/telemetry.h>
#include <led/control.h>
#include <rc/receiver.h>
#include "types.h"

namespace Robot::Kinematic {

    class Kinematic : public std::enable_shared_from_this<Kinematic>, public WithMutexStd, public WithNotifyInt, public WithStrand {
        public:
            static constexpr notify_type NOTIFY_TELEMETRY { 1 };
            using odometer_type = std::int32_t;

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

            odometer_type getOdometer() const { return m_odometer; }
            void resetOdometer();


            const std::shared_ptr<::Robot::Context> &context() const { return m_context; }
            const std::shared_ptr<::Robot::Motor::Control> &motorControl() { return m_motor_control; }
            const std::weak_ptr<::Robot::LED::Control> &ledControl() { return m_led_control; }
            const std::weak_ptr<::Robot::Telemetry::Telemetry> &telemetry() { return m_telemetry; }

        private:
            bool m_initialized;

            std::shared_ptr<::Robot::Context> m_context;
            std::shared_ptr<::Robot::Motor::Control> m_motor_control;
            std::weak_ptr<::Robot::LED::Control> m_led_control;
            std::weak_ptr<::Robot::Telemetry::Telemetry> m_telemetry;
            std::shared_ptr<class ControlScheme> m_control_scheme;

            boost::signals2::connection m_motor_update_connection;
            boost::signals2::connection m_axis_connection;
            boost::signals2::connection m_drive_mode_connection;
            boost::signals2::connection m_orientation_connection;

            DriveMode    m_drive_mode;
            Orientation  m_orientation;
            odometer_type m_odometer;
            odometer_type m_odometer_base;
            MotorMap m_motor_map;

            void onSteer(float steering, float throttle, float aux_x, float aux_y);

            void onMotorUpdate(const ::Robot::Motor::MotorList &motors);

            friend std::ostream &operator<<(std::ostream &os, const Kinematic &self)
            {
                return os << "Robot::Kinematic::Kinematic";
            }
    };


}

#endif
