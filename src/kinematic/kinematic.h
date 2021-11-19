#ifndef _KINEMATIC_H_
#define _KINEMATIC_H_

#include <memory>
#include <mutex>

#include "../common/withmutex.h"
#include "../robotcontext.h"
#include "../motor/motorcontrol.h"
#include "../telemetry/telemetry.h"
#include "../rc/rcreceiver.h"
#include "controlscheme.h"

namespace Robot::Kinematic {

    enum class SteeringMode {
        NONE,  // No servo steering
        FRONT, // Front wheel steering
        REAR,  // Rear wheel steering
        ALL,   // 4 wheel steering
        SKID   // Skid steering
    };

    enum class DriveMode {
        NONE,
        NORMAL,
        SPINNING,
        BALANCING,
        PASSTHROUGH // Direct input from remote
    };

    enum class Orientation {
        NORTH,
        SOUTH,
        EAST,
        WEST
    };

    class Kinematic : public std::enable_shared_from_this<Kinematic>, public WithMutex<std::recursive_mutex> {
        public:

            void init();
            void cleanup();

            Kinematic(std::shared_ptr<Robot::Context> context, std::shared_ptr<Robot::Motor::Control> motor_control, std::shared_ptr<Robot::Telemetry::Telemetry> telemetry, std::shared_ptr<Robot::RC::Receiver> receiver);
            Kinematic(const Kinematic&) = delete; // No copy constructor
            Kinematic(Kinematic&&) = delete; // No move constructor
            virtual ~Kinematic();

            void setSteeringMode(SteeringMode mode);
            SteeringMode getSteeringMode() const { return m_steering_mode; }

            void setDriveMode(DriveMode mode);
            DriveMode getDriveMode() const { return m_drive_mode; }


            std::shared_ptr<Robot::Context> context() { return m_context; }
            std::shared_ptr<Robot::Motor::Control> motorControl() { return m_motor_control; }
            std::shared_ptr<Robot::Telemetry::Telemetry> telemetry() { return m_telemetry; }
            std::shared_ptr<Robot::RC::Receiver> rc_receiver() { return m_rc_receiver; }

        private:
            bool m_initialized;

            std::shared_ptr<Robot::Context> m_context;
            std::shared_ptr<Robot::Motor::Control> m_motor_control;
            std::shared_ptr<Robot::Telemetry::Telemetry> m_telemetry;
            std::shared_ptr<Robot::RC::Receiver> m_rc_receiver;
            std::shared_ptr<ControlScheme> m_control_scheme;

            SteeringMode m_steering_mode;
            DriveMode    m_drive_mode;

    };


};

#endif
