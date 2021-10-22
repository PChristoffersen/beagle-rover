#ifndef _MOTORGIMBAL_H_
#define _MOTORGIMBAL_H_

#include <stdint.h>
#include <chrono>
#include <mutex>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>


class MotorGimbal : public boost::enable_shared_from_this<MotorGimbal> {
    public:
        [[nodiscard]] static boost::shared_ptr<MotorGimbal> create(uint8_t index, boost::shared_ptr<std::recursive_mutex> mutex);

        virtual ~MotorGimbal();

        void setEnabled(bool enable);
        bool getEnabled() const { return m_enabled; }

        void setPulseUS(uint32_t us);
        uint32_t getPulseUS() const { return m_pulse_us; }

        uint8_t getIndex() const { return m_index; }

        void setAngle(double angle);
        double getAngle() const;

    protected:
        void init();
        void cleanup();

        void update();

        friend class Motor;
        friend class MotorControl;
    private:
        uint8_t m_index;
        boost::shared_ptr<std::recursive_mutex> m_mutex;
        bool m_enabled;
        uint32_t m_pulse_us;
        std::chrono::high_resolution_clock::time_point m_last_pulse;

        MotorGimbal(uint8_t index, boost::shared_ptr<std::recursive_mutex> mutex);
};

#endif
