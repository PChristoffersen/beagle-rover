#ifndef _ROBOT_MOTOR_CONTROL_H_
#define _ROBOT_MOTOR_CONTROL_H_

#include <memory>
#include <array>
#include <mutex>
#include <chrono>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>

#include <robotconfig.h>
#include <robottypes.h>
#include <common/withnotify.h>
#include <common/withmutex.h>
#include <telemetry/types.h>
#include "types.h"

namespace Robot::Motor {

    class Control : public std::enable_shared_from_this<Control>, public WithMutexStd, public WithNotifyInt {
        public:
            using timer_type = boost::asio::high_resolution_timer;
            using motor_mutex_type = std::mutex;
            using servo_mutex_type = std::mutex;
            using motor_update_signal = typename boost::signals2::signal<void(MotorList&)>;
            using servo_update_signal = typename boost::signals2::signal<void(ServoList&)>;

            explicit Control(const std::shared_ptr<::Robot::Context> &context);
            Control(const Control&) = delete; // No copy constructor
            Control(Control&&) = delete; // No move constructor
            virtual ~Control();

            void init(const std::shared_ptr<::Robot::Telemetry::Telemetry> &telemetry);
            void cleanup();

            void start();
            void stop();

            const MotorList &getMotors() const { return m_motors; }
            const ServoList &getServos() const { return m_servos; }

            motor_mutex_type &motorMutex() const { return m_motor_mutex; }
            motor_mutex_type &servoMutex() const { return m_motor_mutex; }

            motor_update_signal sig_motor;
            servo_update_signal sig_servo;

        private:
            std::shared_ptr<::Robot::Context> m_context;
            bool m_initialized;
            bool m_enabled;
            boost::asio::io_context::strand m_motor_strand;
            boost::asio::io_context::strand m_servo_strand;
            timer_type m_motor_timer;
            timer_type m_servo_timer;
            boost::signals2::connection m_motor_power_con;
            boost::signals2::connection m_servo_power_con;

            mutable motor_mutex_type m_motor_mutex;
            mutable servo_mutex_type m_servo_mutex;
            MotorList m_motors;
            ServoList m_servos;

            void onMotorPower(bool enabled);
            void onServoPower(bool enabled);

            void motorTimerSetup();
            inline void motorTimer();

            void servoTimerSetup();
            inline void servoTimer();

            friend std::ostream &operator<<(std::ostream &os, const Control &control)
            {
                return os << "Robot::Motor::Control";
            }

    };

}

#endif
