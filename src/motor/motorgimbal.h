#ifndef _MOTORGIMBAL_H_
#define _MOTORGIMBAL_H_

#include <cstdint>
#include <memory>
#include <chrono>
#include <mutex>


class MotorGimbal {
    public:
        MotorGimbal(uint8_t index, std::recursive_mutex &mutex);
        virtual ~MotorGimbal();

        void setEnabled(bool enable);
        bool getEnabled() const { return m_enabled; }

        bool getPassthrough() const { return m_passthrough; }

        void setPulseUS(uint32_t us);
        uint32_t getPulseUS() const { return m_pulse_us; }

        uint8_t getIndex() const { return m_index; }

        void setAngle(double angle);
        double getAngle() const;

        void setTrimUS(int32_t trim);
        int32_t getTrimUS() const { return m_trim_us; }

        void setLimits(uint32_t lmin, uint32_t lmax);
        void setLimitMin(uint32_t limit);
        uint32_t getLimitMin() const { return m_limit_min; };
        void setLimitMax(uint32_t limit);
        uint32_t getLimitMax() const { return m_limit_max; }


    protected:
        void init();
        void cleanup();

        void setPassthrough(bool passthrough);

        void update();

        friend class Motor;
        friend class MotorControl;
    private:
        static constexpr auto PULSE_INTERVAL { std::chrono::milliseconds(20) };

        bool m_initialized;
        uint8_t m_index;
        std::recursive_mutex &m_mutex;
        bool m_enabled;
        bool m_passthrough;
        uint32_t m_pulse_us;
        std::chrono::high_resolution_clock::time_point m_last_pulse;

        int32_t m_trim_us;
        uint32_t m_limit_min;
        uint32_t m_limit_max;
};

#endif
