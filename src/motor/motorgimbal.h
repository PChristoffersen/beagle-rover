#ifndef _MOTORGIMBAL_H_
#define _MOTORGIMBAL_H_

#include <cstdint>
#include <memory>
#include <chrono>
#include <mutex>


class MotorGimbal {
    public:
        MotorGimbal(int index, std::recursive_mutex &mutex);
        MotorGimbal(const MotorGimbal&) = delete; // No copy constructor
        MotorGimbal(MotorGimbal&&) = delete; // No move constructor
        virtual ~MotorGimbal();

        int getIndex() const { return m_index; }

        void setEnabled(bool enable);
        bool getEnabled() const { return m_enabled; }

        bool getPassthrough() const { return m_passthrough; }

        void setPulseUS(std::uint32_t us);
        std::uint32_t getPulseUS() const { return m_pulse_us; }

        void setAngle(double angle);
        double getAngle() const;

        void setAngleDegrees(double angle);
        double getAngleDegrees() const;

        void setTrimUS(std::int32_t trim);
        std::int32_t getTrimUS() const { return m_trim_us; }

        void setLimits(std::uint32_t lmin, std::uint32_t lmax);
        void setLimitMin(std::uint32_t limit);
        std::uint32_t getLimitMin() const { return m_limit_min; };
        void setLimitMax(std::uint32_t limit);
        std::uint32_t getLimitMax() const { return m_limit_max; }


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
        int m_index;
        std::recursive_mutex &m_mutex;
        bool m_enabled;
        bool m_passthrough;
        std::uint32_t m_pulse_us;
        std::chrono::high_resolution_clock::time_point m_last_pulse;

        std::int32_t m_trim_us;
        std::uint32_t m_limit_min;
        std::uint32_t m_limit_max;

        int servoChannel() const {
            return m_index+1;
        }
};

#endif
