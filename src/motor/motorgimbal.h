#ifndef _MOTORGIMBAL_H_
#define _MOTORGIMBAL_H_

#include <stdint.h>
#include <chrono>
#include <boost/shared_ptr.hpp>


class MotorGimbal {
    public:
        MotorGimbal(uint8_t index);
        ~MotorGimbal();

        void init();
        void cleanup();

        void setEnabled(bool enable);
        bool getEnabled() const { return m_enabled; }

        void setPulseUS(uint32_t us);
        uint32_t getPulseUS() const { return m_pulse_us; }

        uint8_t getIndex() const { return m_index; }

    protected:
        void update();

        friend class Motor;
        friend class MotorControl;
    private:
        uint8_t m_index;
        bool m_enabled;
        uint32_t m_pulse_us;
        std::chrono::high_resolution_clock::time_point m_last_pulse;

};

#endif
