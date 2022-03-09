/**
 * The balancing algorithm is adapted from the rc_balance.c example in the BeagleBone robotcontrol library
 */
#include "balancing.h"

#include <cmath>
#include <array>
#include <boost/log/trivial.hpp>

#include <robotconfig.h>
#include <robotcontext.h>
#include <motor/motor.h>
#include <motor/servo.h>
#include <motor/control.h>
#include "../types.h"

#if ROBOT_HAVE_BALANCE

using namespace std::literals;
using namespace Robot::Config;

namespace Robot::Kinematic {

static constexpr auto INIT_DELAY { 1000ms };

static constexpr auto BALANCE_P { 1.0f } ;
static constexpr auto BALANCE_I { 10.0f } ;
static constexpr auto BALANCE_D { 0.0f } ;
static constexpr Robot::Math::PID::sample_time_type BALANCE_INTERVAL { 20ms };

ControlSchemeBalancing::ControlSchemeBalancing(std::shared_ptr<Kinematic> kinematic) :
    AbstractControlScheme { kinematic },
    m_telemetry { kinematic->telemetry() },
    m_led_control { kinematic->ledControl() },
    m_init_timer { m_context->io() },
    m_armed { false },
    m_pid { BALANCE_P, BALANCE_I, BALANCE_D, BALANCE_INTERVAL }
{
}


ControlSchemeBalancing::~ControlSchemeBalancing() 
{
    cleanup();
}


void ControlSchemeBalancing::init() 
{
    const guard lock(m_mutex);

    initMotors();

    if (auto telemetry = m_telemetry.lock()) {
        m_imu_connection = telemetry->sig_imu.connect(::Robot::Telemetry::IMUSignal::slot_type(&ControlSchemeBalancing::onIMUData, this, boost::placeholders::_1).track_foreign(shared_from_this()));
    }

    if (auto led_control = m_led_control.lock()) {
        led_control->setLED(RC_LED_GREEN, true);
        led_control->setLED(RC_LED_RED, false);
    }

    
    m_init_timer.expires_after(INIT_DELAY);
    m_init_timer.async_wait(
        [self_ptr=weak_from_this()] (boost::system::error_code error) {
            if (error!=boost::system::errc::success) {
                return;
            }
            if (auto self = self_ptr.lock()) { 
                    const guard lock(self->m_mutex);

                auto &motors = self->m_motor_control->getMotors();
                motors[MotorPosition::FRONT_LEFT]->servo()->setEnabled(false);
                motors[MotorPosition::FRONT_RIGHT]->servo()->setEnabled(false);
            }
        }
    );


    m_initialized = true;
}


void ControlSchemeBalancing::initMotors()
{
    auto &motors = m_motor_control->getMotors();

    // Set all motor angles to 0 degrees and throttle to 0
    {
        auto &motor = motors[MotorPosition::FRONT_LEFT];
        motor->setDuty(0.0);
        motor->brake();
        motor->setEnabled(false);
        motor->servo()->setValue(Value::fromAngle(WHEEL_MIN_ANGLE));
        motor->servo()->setEnabled(true);
    }
    {
        auto &motor = motors[MotorPosition::FRONT_RIGHT];
        motor->setDuty(0.0);
        motor->brake();
        motor->setEnabled(false);
        motor->servo()->setValue(Value::fromAngle(WHEEL_MIN_ANGLE));
        motor->servo()->setEnabled(true);
    }
    {
        auto &motor = motors[MotorPosition::REAR_LEFT];
        motor->setDuty(0.0);
        motor->setEnabled(true);
        motor->servo()->setValue(Value::fromAngle(WHEEL_STRAIGHT_ANGLE));
        motor->servo()->setEnabled(true);
    }
    {
        auto &motor = motors[MotorPosition::REAR_RIGHT];
        motor->setDuty(0.0);
        motor->setEnabled(true);
        motor->servo()->setValue(Value::fromAngle(WHEEL_STRAIGHT_ANGLE));
        motor->servo()->setEnabled(true);
    }
}



void ControlSchemeBalancing::cleanup() 
{
    m_imu_connection.disconnect();

    const guard lock(m_mutex);

    if (!m_initialized) 
        return;
    m_initialized = false;

    m_init_timer.cancel();

    if (auto led_control = m_led_control.lock()) {
        led_control->setLED(RC_LED_GREEN, false);
        led_control->setLED(RC_LED_RED, false);
    }

    const auto &motors = m_motor_control->getMotors();
    for (auto &motor : motors) {
        motor->setDuty(0.0);
    }

}



void ControlSchemeBalancing::arm() 
{
    auto &motors = m_motor_control->getMotors();
    auto &left_motor = motors[MotorPosition::REAR_LEFT];
    auto &right_motor = motors[MotorPosition::REAR_RIGHT];

    m_pid.reset();

    left_motor->resetOdometer();
    left_motor->setDuty(0.0);
    left_motor->setEnabled(true);
    right_motor->resetOdometer();
    right_motor->setDuty(0.0);
    right_motor->setEnabled(true);
}

void ControlSchemeBalancing::disarm() 
{
    auto &motors = m_motor_control->getMotors();
    auto &left_motor = motors[MotorPosition::REAR_LEFT];
    auto &right_motor = motors[MotorPosition::REAR_RIGHT];

    left_motor->setEnabled(false);
    right_motor->setEnabled(false);
}


void ControlSchemeBalancing::onIMUData(const Robot::Telemetry::IMUData &imu_data)
{
    const guard lock(m_mutex);
    if (!m_initialized) 
        return;

}


}

#endif
