#ifndef _KINEMATIC_H_
#define _KINEMATIC_H_

#include <mutex>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

class Kinematic : public boost::enable_shared_from_this<Kinematic> {
    public:
        enum SteeringMode {
            STEERING_NONE,  // No servo steering
            STEERING_FRONT, // Front wheel steering
            STEERING_REAR,  // Rear wheel steering
            STEERING_4WD,   // 4 wheel steering
            STEERING_SKID,  // Skid steering
            STEERING_BYPASS // Direct input from remote
        };

        enum DriveMode {
            DRIVE_NONE,
            DRIVE_NORMAL,
            DRIVE_SPINNING,
            DRIVE_BALANCING,
        };

        enum Orientation {
            ORIENTATION_NORTH,
            ORIENTATION_SOUTH,
            ORIENTATION_EAST,
            ORIENTATION_WEST
        };

        void init();
        void cleanup();

        [[nodiscard]] static boost::shared_ptr<Kinematic> create(boost::shared_ptr<class RobotContext> context, boost::shared_ptr<class MotorControl> motor_control, boost::shared_ptr<class Telemetry> telemetry);

        virtual ~Kinematic();

        void setSteeringMode(SteeringMode mode);
        SteeringMode getSteeringMode() const { return m_steering_mode; }

        void setDriveMode(DriveMode mode);
        DriveMode getDriveMode() const { return m_drive_mode; }


        boost::shared_ptr<class RobotContext> context() { return m_context; }
        boost::shared_ptr<class MotorControl> motorControl() { return m_motor_control; }
        boost::shared_ptr<class Telemetry> telemetry() { return m_telemetry; }

    private:
        std::recursive_mutex m_mutex;
        bool m_initialized;

        boost::shared_ptr<class RobotContext> m_context;
        boost::shared_ptr<class MotorControl> m_motor_control;
        boost::shared_ptr<class Telemetry> m_telemetry;
        boost::shared_ptr<class AbstractControlScheme> m_control_scheme;

        SteeringMode m_steering_mode;
        DriveMode    m_drive_mode;

        Kinematic(boost::shared_ptr<class RobotContext> context, boost::shared_ptr<class MotorControl> motor_control, boost::shared_ptr<class Telemetry> telemetry);
};

#endif
