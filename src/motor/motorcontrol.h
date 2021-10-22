#ifndef _MOTOR_CONTROL_H_
#define _MOTOR_CONTROL_H_

#include <string>
#include <vector>
#include <mutex>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/signals2.hpp>
#include <robotcontrol-ext.h>

#define MOTOR_COUNT 4

class MotorControl : public boost::enable_shared_from_this<MotorControl> {
    public:
        typedef std::vector< boost::shared_ptr<class Motor> > MotorList;

        [[nodiscard]] static boost::shared_ptr<MotorControl> create(boost::shared_ptr<class RobotContext> context);

        enum MotorPosition {
            FRONT_LEFT = 0,
            FRONT_RIGHT = 1,
            REAR_LEFT = 2,
            REAR_RIGHT = 3,
        };

        virtual ~MotorControl();

        void init();
        void cleanup();

        void start();
        void stop();

        void brake();
        void freeSpin();

        void setEnabled(bool enabled);
        bool getEnabled() const { return m_enabled; }

        void resetOdometer();

        const MotorList getMotors() const { return m_motors; }
        const boost::shared_ptr<class Motor> getMotor(MotorPosition position) { return m_motors[position]; }

    private:
        bool m_initialized;
        bool m_enabled;
        boost::shared_ptr<std::recursive_mutex> m_mutex;
        MotorList m_motors;

        boost::asio::steady_timer m_timer;
        uint16_t m_last_counter;
        volatile shm_fbus_t *m_fbus;

        MotorControl(boost::shared_ptr<class RobotContext> context);

        void timer();
};

#endif
