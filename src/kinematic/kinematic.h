#ifndef _KINEMATIC_H_
#define _KINEMATIC_H_

#include <memory>
#include <mutex>

class Kinematic : public std::enable_shared_from_this<Kinematic> {
    public:
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

        void init();
        void cleanup();

        Kinematic(std::shared_ptr<class RobotContext> context, std::shared_ptr<class MotorControl> motor_control, std::shared_ptr<class Telemetry> telemetry, std::shared_ptr<class RCReceiver> receiver);
        Kinematic(const Kinematic&) = delete; // No copy constructor
        Kinematic(Kinematic&&) = delete; // No move constructor
        virtual ~Kinematic();

        void setSteeringMode(SteeringMode mode);
        SteeringMode getSteeringMode() const { return m_steering_mode; }

        void setDriveMode(DriveMode mode);
        DriveMode getDriveMode() const { return m_drive_mode; }


        std::shared_ptr<class RobotContext> context() { return m_context; }
        std::shared_ptr<class MotorControl> motorControl() { return m_motor_control; }
        std::shared_ptr<class Telemetry> telemetry() { return m_telemetry; }
        std::shared_ptr<class RCReceiver> rc_receiver() { return m_rc_receiver; }

    private:
        std::recursive_mutex m_mutex;
        bool m_initialized;

        std::shared_ptr<class RobotContext> m_context;
        std::shared_ptr<class MotorControl> m_motor_control;
        std::shared_ptr<class Telemetry> m_telemetry;
        std::shared_ptr<class RCReceiver> m_rc_receiver;
        std::shared_ptr<class ControlScheme> m_control_scheme;

        SteeringMode m_steering_mode;
        DriveMode    m_drive_mode;

};

#endif
