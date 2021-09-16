#ifndef _MOTOR_CONTROL_H_
#define _MOTOR_CONTROL_H_

#include <vector>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <robotcontrol-ext.h>

#include "component.h"

#define MOTOR_COUNT 1

class MotorControl : public Component, public boost::enable_shared_from_this<MotorControl> {
    public:
        MotorControl(boost::asio::io_context &io);

        void init() override;
        void cleanup() override;

        void start();
        void stop();

    private:
        bool m_started;
        std::vector< boost::shared_ptr<class Motor> > m_motors;

        boost::asio::steady_timer m_timer;
        uint16_t m_last_counter;
        volatile shm_fbus_t *m_fbus;

        void timer();
};

#endif
