#ifndef _MOTOR_CONTROL_H_
#define _MOTOR_CONTROL_H_

#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/signals2.hpp>
#include <robotcontrol-ext.h>

#define MOTOR_COUNT 1

class MotorControl : public boost::enable_shared_from_this<MotorControl> {
    public:
        typedef std::vector< boost::shared_ptr<class Motor> > MotorList;

        MotorControl(boost::shared_ptr<class RobotContext> context);

        void init();
        void cleanup();

        void start();
        void stop();

        void brake();
        void freeSpin();

        const MotorList getMotors() const { return m_motors; }

        void connect(boost::shared_ptr<class RCReceiver> receiver);

        void onRCFlags(uint8_t flags);

    private:
        bool m_started;
        MotorList m_motors;

        boost::asio::steady_timer m_timer;
        uint16_t m_last_counter;
        volatile shm_fbus_t *m_fbus;

        void timer();
};

#endif
