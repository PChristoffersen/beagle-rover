#ifndef _MOTOR_CONTROL_H_
#define _MOTOR_CONTROL_H_

#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <robotcontrolext.h>

class MotorControl : public std::enable_shared_from_this<MotorControl> {
    public:
        typedef std::vector<std::unique_ptr<class Motor>> MotorList;

        enum MotorPosition {
            FRONT_LEFT = 0,
            FRONT_RIGHT = 1,
            REAR_LEFT = 2,
            REAR_RIGHT = 3,
        };

        MotorControl(std::shared_ptr<class RobotContext> context);
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

        const MotorList &getMotors() const { return m_motors; }
        

    private:
        bool m_initialized;
        bool m_enabled;
        std::recursive_mutex m_mutex;
        
        std::vector<std::unique_ptr<class Motor>> m_motor_holder;
        MotorList m_motors;

        boost::asio::high_resolution_timer m_timer;
        uint16_t m_last_counter;
        volatile shm_fbus_t *m_fbus;

        void timer(boost::system::error_code error);
};

#endif
