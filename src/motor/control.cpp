#include "control.h"

#include <iostream>
#include <algorithm>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

#include <robotcontext.h>
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
    m_motor_timer { context->io() },
    m_servo_timer { context->io() }
{
    for (uint i=0; i<MOTOR_COUNT; i++) {
        m_motors[i] = std::make_unique<Motor>(i, m_mutex, context);
    }
}


Control::~Control() 
{
    cleanup();
}


void Control::init(const std::shared_ptr<::Robot::Telemetry::Telemetry> &telemetry) 
{
    const guard lock(m_mutex);
    BOOST_ASSERT_MSG(!m_initialized, "Already initialized");
    m_initialized = true;

    for (auto &motor : m_motors) {
       motor->init(telemetry);
    }

    if (m_context->motorPower()) {
        onMotorPower(true);
    }
    m_motor_power_con = m_context->sig_motor_power.connect([&](bool enabled){ onMotorPower(enabled); });
    if (m_context->servoPower()) {
        onServoPower(true);
    }
    m_servo_power_con = m_context->sig_servo_power.connect([&](bool enabled){ onServoPower(enabled); });
}


void Control::cleanup() 
{
    const guard lock(m_mutex);
    if (!m_initialized) 
        return;
    m_initialized = false;
    
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
    const guard lock(m_mutex);
    Motor::odometer_type sum { 0 };
    for (auto &motor : m_motors) {
        sum += motor->getOdometer();
    }
    return sum/m_motors.size();
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
        BOOST_LOG_TRIVIAL(info) << *this << " Stopping motor timer";
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
        BOOST_LOG_TRIVIAL(info) << *this << " Stopping servo timer";
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

    if ( true || (now-last_print) > 200ms ) {
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
#elif 0
        BOOST_LOG_TRIVIAL(info) << 
            boost::format("  Motor[] | %4d | %3.2f | %3.2f | %3.2f |")
            % m_motors[0]->getEncoderValue()
            % m_motors[0]->getRPM()
            % m_motors[0]->getTargetRPM()
            % m_motors[0]->getDuty()
            ;
        resetOdometer();
#endif

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

}