#ifndef _MOTOR_CONTROL_H_
#define _MOTOR_CONTROL_H_

#include <boost/asio.hpp>
#include <robotcontrol-ext.h>

#include "component.h"

class MotorControl : public Component {
    public:
        MotorControl(boost::asio::io_context &io);

        void init() override;
        void cleanup() override;

        void start();
        void stop();

    private:
        bool m_started;
        boost::asio::steady_timer m_timer;
        volatile shm_fbus_t *m_fbus;

        void timer();
};

#endif
