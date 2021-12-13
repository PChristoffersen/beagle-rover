#ifndef _ROBOT_MOTOR_CONTROL_H_
#define _ROBOT_MOTOR_CONTROL_H_

#include <memory>
#include <array>
#include <mutex>
#include <chrono>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <robotcontrolext.h>

#include <robottypes.h>
#include <common/withmutex.h>
#include <rc/types.h>
#include "types.h"

namespace Robot::Motor {

    class Control : public std::enable_shared_from_this<Control>, public WithMutex<std::recursive_mutex> {
        public:
            using timer_type = boost::asio::high_resolution_timer;

            explicit Control(const std::shared_ptr<::Robot::Context> &context);
            Control(const Control&) = delete; // No copy constructor
            Control(Control&&) = delete; // No move constructor
            virtual ~Control();

            void init(const std::shared_ptr<::Robot::RC::Receiver> &receiver);
            void cleanup();

            void start();
            void stop();

            void brake();
            void freeSpin();

            void setEnabled(bool enabled);
            bool getEnabled() const { return m_enabled; }

            void setPassthrough(bool passthrough);
            bool getPassthrough() const { return m_passthrough; }

            void resetOdometer();
            double getOdometer() const;

            const MotorList &getMotors() const { return m_motors; }

        private:
            std::shared_ptr<::Robot::Context> m_context;
            std::weak_ptr<::Robot::RC::Receiver> m_rc_receiver;
            bool m_initialized;
            bool m_enabled;
            bool m_passthrough;
            timer_type m_motor_timer;
            timer_type m_servo_timer;
            boost::signals2::connection m_motor_power_con;
            boost::signals2::connection m_servo_power_con;

   
            MotorList m_motors;

            void onMotorPower(bool enabled);
            void onServoPower(bool enabled);

            void motorTimerSetup();
            inline void motorTimer();

            void servoTimerSetup();
            inline void servoTimer();

            boost::signals2::connection m_rc_connection;
            void onRCData(::Robot::RC::Flags flags, ::Robot::RC::RSSI rssi, const ::Robot::RC::ChannelList &channels);

            friend std::ostream &operator<<(std::ostream &os, const Control &control)
            {
                return os << "Motor::Control";
            }

    };

};

#endif
