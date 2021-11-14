#ifndef _MOTOR_CONTROL_H_
#define _MOTOR_CONTROL_H_

#include <memory>
#include <array>
#include <mutex>
#include <chrono>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <robotcontrolext.h>

class MotorControl : public std::enable_shared_from_this<MotorControl> {
    public:
        static constexpr auto MOTOR_COUNT { 4 };

        static constexpr std::uint32_t PULSE_MIN { 500 };
        static constexpr std::uint32_t PULSE_MAX { 2500 };
        static constexpr std::int32_t PULSE_CENTER { (PULSE_MAX+PULSE_MIN)/2 };
        static constexpr std::int32_t PULSE_RANGE { (PULSE_MAX-PULSE_MIN) };

        static constexpr auto MOTOR_PASSTHROUGH_OFFSET { 0 };
        static constexpr auto SERVO_PASSTHROUGH_OFFSET { 4 };

        using MotorList = std::array<std::unique_ptr<class Motor>, MOTOR_COUNT>;

        enum MotorPosition {
            FRONT_LEFT = 0,
            FRONT_RIGHT = 1,
            REAR_LEFT = 2,
            REAR_RIGHT = 3,
        };

        explicit MotorControl(std::shared_ptr<class RobotContext> context);
        MotorControl(const MotorControl&) = delete; // No copy constructor
        MotorControl(MotorControl&&) = delete; // No move constructor
        virtual ~MotorControl();

        void init();
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

        const MotorList &getMotors() const { return m_motors; }

        static double pulseToPos(std::uint32_t us) {
            return (double)((std::int32_t)us-PULSE_CENTER)*2.0/PULSE_RANGE;
        }

    private:
        bool m_initialized;
        bool m_enabled;
        bool m_passthrough;
        std::recursive_mutex m_mutex;
        boost::asio::high_resolution_timer m_timer;
        
        std::vector<std::unique_ptr<class Motor>> m_motor_holder;
        MotorList m_motors;

        void timer(boost::system::error_code error);
        inline void timer_setup();

};

#endif
