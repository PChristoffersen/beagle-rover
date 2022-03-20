/**
 * The balancing algorithm is adapted from the rc_balance.c example in the BeagleBone robotcontrol library
 */
#include "balancing.h"

#include <cmath>
#include <array>
#include <boost/log/trivial.hpp>

#include <robotconfig.h>
#include <robotcontext.h>
#include <common/properties.h>
#include <motor/motor.h>
#include <motor/servo.h>
#include <motor/control.h>
#include "../types.h"

#if ROBOT_HAVE_BALANCE

using namespace std::literals;
using namespace Robot::Config;

namespace Robot::Kinematic {

static constexpr auto PROPERTY_GROUP { "balancing" };
static constexpr auto PROPERTY_PID_P { "pid_p" };
static constexpr auto PROPERTY_PID_I { "pid_i" };
static constexpr auto PROPERTY_PID_D { "pid_d" };
static constexpr auto PROPERTY_ANGLE { "angle" };

static constexpr auto INIT_DELAY { 1000ms };

static constexpr auto BALANCE_P { 1.0f } ;
static constexpr auto BALANCE_I { 10.0f } ;
static constexpr auto BALANCE_D { 0.0f } ;
static constexpr auto BASE_ANGLE { static_cast<float>(M_PI_2) };
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


void ControlSchemeBalancing::registerProperties(const std::shared_ptr<Context> &context)
{
    PropertyMap values;
    values.put(PROPERTY_PID_P, BALANCE_P);
    values.put(PROPERTY_PID_I, BALANCE_I);
    values.put(PROPERTY_PID_D, BALANCE_D);
    values.put(PROPERTY_ANGLE, BASE_ANGLE);
    context->registerProperties(PROPERTY_GROUP, values);
    BOOST_LOG_TRIVIAL(info) << "Register";
}


void ControlSchemeBalancing::init() 
{
    const auto &properties = m_context->properties(PROPERTY_GROUP);
    m_pid.set(properties.get(PROPERTY_PID_P, BALANCE_P), properties.get(PROPERTY_PID_I, BALANCE_I), properties.get(PROPERTY_PID_D, BALANCE_D));
    m_pid.setLimits(-1.0f, 1.0f);
    m_base_angle = properties.get(PROPERTY_ANGLE, BASE_ANGLE);
    m_pid.setSetpoint(m_base_angle);

    initMotors();

    if (auto telemetry = m_telemetry.lock()) {
        m_imu_connection = telemetry->sig_imu.connect(::Robot::Telemetry::IMUSignal::slot_type(&ControlSchemeBalancing::onIMUData, this, boost::placeholders::_1).track_foreign(shared_from_this()));
    }

    if (auto led_control = m_led_control.lock()) {
        led_control->setLED(RC_LED_GREEN, true);
        led_control->setLED(RC_LED_RED, false);
    }

    #if 0
    m_init_timer.expires_after(INIT_DELAY);
    m_init_timer.async_wait(
        [self_ptr=weak_from_this()] (boost::system::error_code error) {
            if (error!=boost::system::errc::success) {
                return;
            }
            if (auto self = self_ptr.lock()) { 
                auto &motors = self->m_motor_control->getMotors();
                motors[MotorPosition::FRONT_LEFT]->servo()->setEnabled(false);
                motors[MotorPosition::FRONT_RIGHT]->servo()->setEnabled(false);
            }
        }
    );
    #endif

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
    dispatch([this,imu_data]{
        if (!m_initialized) 
            return;
        #if 0
        auto angle = imu_data.dmp_TaitBryan[TB_PITCH_X];

        auto duty = m_pid.update(angle);
        #endif
    });
}


}

#endif
