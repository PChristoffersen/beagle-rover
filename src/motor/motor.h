#ifndef _MOTOR_H_
#define _MOTOR_H_

#include <stdint.h>
#include <chrono>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <boost/shared_ptr.hpp>
#include <PID.h>

#include "../component.h"

class Motor : public Component {
    public:
        Motor(uint8_t index, boost::shared_ptr<boost::asio::io_context> io);
        ~Motor();

        void init() override;
        void cleanup() override;

        void setDuty(double duty);
        double getRPM();
        void setRPM(double rpm);

    protected:
        void update();

        friend class MotorControl;
    private:
        uint8_t m_index;
        int32_t m_last_enc_value;
        std::chrono::high_resolution_clock::time_point m_last_enc_time;

        double m_duty;
        double m_target_rpm;
        double m_rpm;
        PIDController<double> m_pid;

        boost::asio::steady_timer m_timer;

        int32_t value;

        void ticker();
};


#endif 

