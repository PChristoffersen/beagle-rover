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
    m_motor_strand { context->io() },
    m_servo_strand { context->io() },
    m_motor_timer { context->io() },
    m_servo_timer { context->io() }
{
    for (uint i=0; i<MOTOR_COUNT; i++) {
        m_servos[i] = std::make_unique<Servo>(i, context, m_servo_strand);
        m_motors[i] = std::make_unique<Motor>(i, context, m_motor_strand, m_servos[i].get());
    }
}


Control::~Control() 
{
    cleanup();
    for (auto &motor : m_motors) {
        motor.reset();
    }
    for (auto &servo : m_servos) {
        servo.reset();
    }
}


void Control::init(const std::shared_ptr<::Robot::Telemetry::Telemetry> &telemetry) 
{
    BOOST_ASSERT_MSG(!m_initialized, "Already initialized");
    m_initialized = true;

    for (auto &servo : m_servos) {
        servo->init();
    }
    for (auto &motor : m_motors) {
        motor->init();
    }

    if (m_context->motorPower()) {
        onMotorPower(true);
    }
    m_motor_power_con = m_context->sig_motor_power.connect([this](bool enabled){ onMotorPower(enabled); });
    if (m_context->servoPower()) {
        onServoPower(true);
    }
    m_servo_power_con = m_context->sig_servo_power.connect([this](bool enabled){ onServoPower(enabled); });
}


void Control::cleanup() 
{
    if (!m_initialized) 
        return;
    m_initialized = false;
    
    m_motor_power_con.disconnect();
    m_servo_power_con.disconnect();
    m_motor_timer.cancel();
    m_servo_timer.cancel();

    for (auto &servo : m_servos) {
        servo->cleanup();
    }
    for (auto &motor : m_motors) {
        motor->cleanup();
    }

}



void Control::onMotorPower(bool enabled) 
{
    const guard lock(m_mutex);
    BOOST_LOG_TRIVIAL(trace) << "MotorControl::onMotorPower " << enabled;
    if (!m_initialized)
        return;

    boost::asio::defer(m_servo_strand, [this, enabled]{
        if (enabled) {
            BOOST_LOG_TRIVIAL(info) << *this << " Starting motor timer";
            m_motor_timer.expires_after(0s);
            motorTimerSetup();
        }
        else {
            BOOST_LOG_TRIVIAL(info) << *this << " Stopping motor timer";
            m_motor_timer.cancel();
        }
    });
}

void Control::onServoPower(bool enabled)
{
    const guard lock(m_mutex);
    BOOST_LOG_TRIVIAL(trace) << "MotorControl::onServoPower " << enabled;
    if (!m_initialized)
        return;

    boost::asio::defer(m_servo_strand, [this, enabled] {
        if (enabled) {
            BOOST_LOG_TRIVIAL(info) << *this << " Starting servo timer";
            m_servo_timer.expires_after(0ms);
            servoTimerSetup();
        }
        else {
            BOOST_LOG_TRIVIAL(info) << *this << " Stopping servo timer";
            m_servo_timer.cancel();
        }
    });
}



void Control::motorTimer() 
{
    const guard lock(m_motor_mutex);
    //BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;

    // Update motors
    for (auto &motor : m_motors) {
        motor->update();
    }
    sig_motor(m_motors);

    motorTimerSetup();
}

void Control::motorTimerSetup() {
    m_motor_timer.expires_at(m_motor_timer.expiry() + MOTOR_TIMER_INTERVAL);
    m_motor_timer.async_wait(boost::asio::bind_executor(m_servo_strand, 
        [this] (boost::system::error_code error) {
            if (error!=boost::system::errc::success) {
                return;
            }
            motorTimer(); 
        }
    ));
}




void Control::servoTimer() 
{
    const guard lock(m_servo_mutex);
    for (auto &servo : m_servos) {
        servo->update();
    }
    sig_servo(m_servos);

    servoTimerSetup();
}

void Control::servoTimerSetup() {
    m_servo_timer.expires_at(m_servo_timer.expiry() + SERVO_TIMER_INTERVAL);
    m_servo_timer.async_wait(boost::asio::bind_executor(m_servo_strand, 
        [this] (boost::system::error_code error) {
            if (error!=boost::system::errc::success) {
                return;
            }
            servoTimer(); 
        }
    ));
}


}