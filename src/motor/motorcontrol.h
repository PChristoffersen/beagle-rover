#ifndef _MOTOR_CONTROL_H_
#define _MOTOR_CONTROL_H_

#include <memory>
#include <array>
#include <mutex>
#include <chrono>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <robotcontrolext.h>

#include "../common/withmutex.h"
#include "../robotcontext.h"

namespace Robot::Motor {

    class Control : public std::enable_shared_from_this<Control>, public WithMutex<std::recursive_mutex> {
        public:
            static constexpr auto MOTOR_COUNT { 4 };

            using MotorList = std::array<std::unique_ptr<class Motor>, MOTOR_COUNT>;

            enum MotorPosition {
                FRONT_LEFT = 0,
                FRONT_RIGHT = 1,
                REAR_LEFT = 2,
                REAR_RIGHT = 3,
            };

            explicit Control(std::shared_ptr<Robot::Context> context);
            Control(const Control&) = delete; // No copy constructor
            Control(Control&&) = delete; // No move constructor
            virtual ~Control();

            void init();
            void cleanup();

            void start();
            void stop();

            void brake();
            void freeSpin();

            void setEnabled(bool enabled);
            bool getEnabled() const { return m_enabled; }

            void setPassthrough(bool passthrough, off_t servo_offset);
            bool getPassthrough() const { return m_passthrough; }

            void resetOdometer();

            const MotorList &getMotors() const { return m_motors; }

        private:
            bool m_initialized;
            bool m_enabled;
            bool m_passthrough;
            boost::asio::high_resolution_timer m_timer;
            
            std::vector<std::unique_ptr<class Motor>> m_motor_holder;
            MotorList m_motors;

            void timer(boost::system::error_code error);
            inline void timer_setup();

    };

};

#endif
