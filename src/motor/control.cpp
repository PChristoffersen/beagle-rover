#include "control.h"

#include <iostream>
#include <algorithm>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

#include <robotcontrol.h>
#include <robotcontrolext.h>

#include <robotcontext.h>
#include <rc/receiver.h>
#include "types.h"
#include "motor.h"
#include "servo.h"

using namespace std::literals;

namespace Robot::Motor {

static constexpr auto MOTOR_PASSTHROUGH_OFFSET { 0u };
static constexpr auto SERVO_PASSTHROUGH_OFFSET { 4u };




Control::Control(const std::shared_ptr<Robot::Context> &context) : 
    m_context { context },
    m_initialized { false },
    m_enabled { false },
    m_passthrough { false },
    m_motor_timer { context->io() },
    m_servo_timer { context->io() }
{
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
    for (uint i=0; i<MOTOR_COUNT; i++) {
        m_motors[i] = std::make_unique<Motor>(i, m_mutex, context);
    }
}


Control::~Control() 
{
    cleanup();
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}


void Control::init(const std::shared_ptr<::Robot::RC::Receiver> &receiver) 
{
    const guard lock(m_mutex);
    BOOST_ASSERT_MSG(!m_initialized, "Already initialized");
    m_initialized = true;

    for (auto &motor : m_motors) {
       motor->init();
    }

    onMotorPower(m_context->motorPower());
    m_motor_power_con = m_context->sig_motor_power.connect([&](bool enabled){ onMotorPower(enabled); });
    onServoPower(m_context->servoPower());
    m_servo_power_con = m_context->sig_servo_power.connect([&](bool enabled){ onServoPower(enabled); });

    m_rc_receiver = receiver->weak_from_this();
}


void Control::cleanup() 
{
    const guard lock(m_mutex);
    if (!m_initialized) 
        return;
    m_initialized = false;
    
    m_rc_connection.disconnect();
    m_rc_receiver.reset();

    m_motor_power_con.disconnect();
    m_servo_power_con.disconnect();
    m_motor_timer.cancel();
    m_servo_timer.cancel();

    setEnabled(false);

    for (auto &motor : m_motors) {
       motor->cleanup();
    }

}





void Control::brake() 
{
    const guard lock(m_mutex);
    for (auto &motor : m_motors) {
       motor->brake();
    }
}

void Control::freeSpin() 
{
    const guard lock(m_mutex);
    for (auto &motor : m_motors) {
       motor->freeSpin();
    }
}



void Control::setEnabled(bool enabled) 
{
    const guard lock(m_mutex);

    m_enabled = enabled;

    for (auto &motor : m_motors) {
        motor->setEnabled(m_enabled);
        motor->servo()->setEnabled(false);
    }
}


void Control::resetOdometer() 
{
    const guard lock(m_mutex);

    for (auto &motor : m_motors) {
        motor->resetOdometer();
    }
}


double Control::getOdometer() const
{
    double sum = 0.0;
    for (auto &motor : m_motors) {
        sum += motor->getOdometer();
    }
    return sum/m_motors.size();
}


void Control::setPassthrough(bool passthrough) 
{
    const guard lock(m_mutex);
    if (passthrough != m_passthrough) {
        m_passthrough = passthrough;

        if (!m_passthrough) {
            m_rc_connection.disconnect();
        }

        for (auto &motor : m_motors) {
            motor->setPassthrough(passthrough);
            motor->servo()->setPassthrough(passthrough);
        }

        // Set limits
        std::array<fbus_servo_limit_t, RC_SERVO_CH_MAX> limits;
        for (auto &limit : limits) {
            limit.low = Value::PULSE_MIN;
            limit.high = Value::PULSE_MAX;
        }
        for (const auto &motor : m_motors) {
            auto &limit = limits[SERVO_PASSTHROUGH_OFFSET+motor->servo()->getIndex()];
            limit.low = motor->servo()->getLimitMin();
            limit.high = motor->servo()->getLimitMax();

        }
        rc_ext_fbus_set_servo_limit(limits.data());

        // Set servo map
        std::array<uint8_t, FBUS_CHANNELS> map;
        map.fill(FBUS_SERVO_UNMAPPED);
        if (passthrough) {
            for (const auto &motor : m_motors) {
                auto index = motor->servo()->getIndex();
                map[SERVO_PASSTHROUGH_OFFSET+index] = index;
            }
        }
        rc_ext_fbus_set_servo_map(map.data());

        if (!m_passthrough) {
            if (const auto receiver = m_rc_receiver.lock()) {
                m_rc_connection = receiver->sigData.connect([&](auto flags, auto rssi, const auto &channels) { onRCData(flags, rssi, channels); });
            }
        }
    }
}



void Control::onMotorPower(bool enabled) 
{
    const guard lock(m_mutex);
    BOOST_LOG_TRIVIAL(trace) << "MotorControl::onMotorPower " << enabled;
    if (!m_initialized)
        return;

    if (enabled) {
        BOOST_LOG_TRIVIAL(info) << *this << " Starting motor timer";
        m_motor_timer.expires_after(0s);
        motorTimerSetup();
    }
    else {
        m_motor_timer.cancel();
    }
}

void Control::onServoPower(bool enabled)
{
    const guard lock(m_mutex);
    BOOST_LOG_TRIVIAL(trace) << "MotorControl::onServoPower " << enabled;
    if (!m_initialized)
        return;

    if (enabled) {
        BOOST_LOG_TRIVIAL(info) << *this << " Starting servo timer";
        m_servo_timer.expires_after(0ms);
        servoTimerSetup();
    }
    else {
        m_servo_timer.cancel();
    }
}


void Control::motorTimerSetup() {
    m_motor_timer.expires_at(m_motor_timer.expiry() + MOTOR_TIMER_INTERVAL);
    m_motor_timer.async_wait(
        [self_ptr=weak_from_this()] (boost::system::error_code error) {
            if (error!=boost::system::errc::success) {
                return;
            }
            if (auto self = self_ptr.lock()) { 
                self->motorTimer(); 
            }
        }
    );
}


void Control::motorTimer() 
{
    const guard lock(m_mutex);
    if (!m_initialized) 
        return;

    //BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;

    // Update motors
    for (auto &motor : m_motors) {
        motor->update();
    }

    static clock_type::time_point last_print;
    auto now = clock_type::now();

    if ( (now-last_print) > 200ms ) {
#if 0
        BOOST_LOG_TRIVIAL(info) << 
            boost::format("  Motors | %4d | %4d | %4d | %4d || %.2f | %.2f | %.2f | %.2f |")
            % m_motors[0]->getEncoderValue()
            % m_motors[1]->getEncoderValue()
            % m_motors[2]->getEncoderValue()
            % m_motors[3]->getEncoderValue()
            % m_motors[0]->getRPM()
            % m_motors[1]->getRPM()
            % m_motors[2]->getRPM()
            % m_motors[3]->getRPM()
            ;
#else
        BOOST_LOG_TRIVIAL(info) << 
            boost::format("  Motor[] | %4d || %.2f | %.2f | %.2f | %.2f |")
            % m_motors[0]->getEncoderValue()
            % m_motors[0]->getOdometer()
            % m_motors[0]->getRPM()
            % m_motors[0]->getTargetRPM()
            % m_motors[0]->getDuty()
            ;
#endif
        //resetOdometer();

        last_print = now;
    }


    motorTimerSetup();
}


void Control::servoTimerSetup() {
    m_servo_timer.expires_at(m_servo_timer.expiry() + SERVO_TIMER_INTERVAL);
    m_servo_timer.async_wait(
        [self_ptr=weak_from_this()] (boost::system::error_code error) {
            if (error!=boost::system::errc::success) {
                return;
            }
            if (auto self = self_ptr.lock()) { 
                self->servoTimer(); 
            }
        }
    );
}

void Control::servoTimer() 
{
    const guard lock(m_mutex);
    if (!m_initialized) 
        return;

    for (auto &motor : m_motors) {
        motor->servo()->update();
    }

    servoTimerSetup();
}


void Control::onRCData(::Robot::RC::Flags flags, ::Robot::RC::RSSI rssi, const ::Robot::RC::ChannelList &channels)
{
    const guard lock(m_mutex);
}


};