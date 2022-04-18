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
#include <led/control.h>
#include <led/colorlayer.h>
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


static constexpr auto BALANCE_P { 1.0f } ;
static constexpr auto BALANCE_I { 10.0f } ;
static constexpr auto BALANCE_D { 0.0f } ;


static constexpr auto CRITICAL_ANGLE_DEGREES { 30.0f };
static constexpr auto WARNING_ANGLE_DEGREES { 10.0f };

static constexpr auto BASE_ANGLE { static_cast<float>(M_PI_2) };
static constexpr auto CRITICAL_ANGLE { static_cast<float>(CRITICAL_ANGLE_DEGREES*M_PI/180.0) };
static constexpr auto WARNING_ANGLE { static_cast<float>(WARNING_ANGLE_DEGREES*M_PI/180.0) };


static constexpr auto ARM_GRACE_PERIOD { 4s };

static constexpr auto INIT_DELAY { 500ms };
static constexpr Robot::Math::PID::sample_time_type BALANCE_INTERVAL { 10ms };



ControlSchemeBalancing::ControlSchemeBalancing(std::shared_ptr<Kinematic> kinematic) :
    AbstractControlScheme { kinematic },
    m_telemetry { kinematic->telemetry() },
    m_led_control { kinematic->ledControl() },
    m_layer { std::make_shared<LED::ColorLayer>("balancing", LED::LAYER_BALANCE_INDICATORS) },
    m_init_timer { m_context->io() },
    m_armed { false },
    m_state { State::IDLE },
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
}


void ControlSchemeBalancing::init() 
{
    m_initialized = true;

    const auto &properties = m_context->properties(PROPERTY_GROUP);
    m_pid.set(properties.get(PROPERTY_PID_P, BALANCE_P), properties.get(PROPERTY_PID_I, BALANCE_I), properties.get(PROPERTY_PID_D, BALANCE_D));
    m_pid.setLimits(-1.0f, 1.0f);
    m_base_angle = properties.get(PROPERTY_ANGLE, BASE_ANGLE);
    m_pid.setSetpoint(m_base_angle);
    m_pid.reset();

    m_layer->fill(LED::Color::TRANSPARENT);
    m_layer->setVisible(true);
    if (auto led_control = m_led_control.lock()) {
        led_control->attachLayer(m_layer);
    }

    m_state = State::IDLE;
    m_armed = false;
    m_armed_grace = false;

    initMotors();

    if (auto telemetry = m_telemetry.lock()) {
        m_imu_connection = telemetry->sig_imu.connect(Telemetry::IMUSignal::slot_type(&ControlSchemeBalancing::onIMUData, this, boost::placeholders::_1));
    }

    m_init_timer.expires_after(INIT_DELAY);
    m_init_timer.async_wait(boost::asio::bind_executor(m_strand,
        [this](boost::system::error_code error) {
            if (error!=boost::system::errc::success || !m_initialized) {
                return;
            }
            auto &motors = m_motor_control->getMotors();
            motors[static_cast<uint>(MotorPosition::FRONT_LEFT)]->servo()->setEnabled(false);
            motors[static_cast<uint>(MotorPosition::FRONT_RIGHT)]->servo()->setEnabled(false);
        }
    ));

}


void ControlSchemeBalancing::initMotors()
{
    auto &motors = m_motor_control->getMotors();

    // Set all motor angles to 0 degrees and throttle to 0
    {
        auto &motor = motors[static_cast<uint>(MotorPosition::FRONT_LEFT)];
        motor->setDuty(0.0);
        motor->setEnabled(false);
        motor->brake();
        motor->servo()->setValue(Value::fromAngle(WHEEL_MIN_ANGLE));
        motor->servo()->setEnabled(true);
    }
    {
        auto &motor = motors[static_cast<uint>(MotorPosition::FRONT_RIGHT)];
        motor->setDuty(0.0);
        motor->setEnabled(false);
        motor->brake();
        motor->servo()->setValue(Value::fromAngle(WHEEL_MIN_ANGLE));
        motor->servo()->setEnabled(true);
    }
    {
        auto &motor = motors[static_cast<uint>(MotorPosition::REAR_LEFT)];
        motor->setDuty(0.0);
        motor->setEnabled(false);
        motor->servo()->setValue(Value::fromAngle(WHEEL_STRAIGHT_ANGLE));
        motor->servo()->setEnabled(true);
    }
    {
        auto &motor = motors[static_cast<uint>(MotorPosition::REAR_RIGHT)];
        motor->setDuty(0.0);
        motor->setEnabled(false);
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

    m_layer->detach();
    m_layer->setVisible(false);
    m_layer->fill(LED::Color::TRANSPARENT);

    m_init_timer.cancel();

    if (auto led_control = m_led_control.lock()) {
        led_control->setLED(RC_LED_GREEN, false);
        led_control->setLED(RC_LED_RED, false);
    }

    const auto &motors = m_motor_control->getMotors();
    for (auto &motor : motors) {
        motor->setDuty(0.0);
    }

    m_state = State::IDLE;
}



void ControlSchemeBalancing::arm() 
{
    auto &left_motor = m_motor_control->getMotor(static_cast<uint>(MotorPosition::REAR_LEFT));
    auto &right_motor = m_motor_control->getMotor(static_cast<uint>(MotorPosition::REAR_RIGHT));

    m_pid.reset();

    left_motor->resetOdometer();
    left_motor->setDuty(0.0);
    left_motor->setEnabled(true);
    right_motor->resetOdometer();
    right_motor->setDuty(0.0);
    right_motor->setEnabled(true);

    m_armed = true;
}

void ControlSchemeBalancing::disarm() 
{
    auto &left_motor = m_motor_control->getMotor(static_cast<uint>(MotorPosition::REAR_LEFT));
    auto &right_motor = m_motor_control->getMotor(static_cast<uint>(MotorPosition::REAR_RIGHT));

    left_motor->setEnabled(false);
    right_motor->setEnabled(false);

    m_armed = false;
}


void ControlSchemeBalancing::updateState(State state)
{
    if (state==m_state) 
        return;
    m_state = state;

    switch (m_state) {
        case State::IDLE:
            {
                LED::ColorLayer::guard _lock(m_layer->mutex());
                auto &front = m_layer->segment(LED::FRONT_SEGMENT);
                front.fill(LED::Color::TRANSPARENT);
            }
            break;
        case State::CRITICAL:
            {
                LED::ColorLayer::guard _lock(m_layer->mutex());
                auto &front = m_layer->segment(LED::FRONT_SEGMENT);
                front.fill(LED::Color::RED);
                m_layer->update();
            }
            break;
        case State::WARNING:
            {
                LED::ColorLayer::guard _lock(m_layer->mutex());
                auto &front = m_layer->segment(LED::FRONT_SEGMENT);
                front.fill(LED::Color::YELLOW);
                if (!m_armed) {
                    front[0] = LED::Color::RED;
                    front[front.size()-1] = LED::Color::RED;
                }
                m_layer->update();
            }
            break;
        case State::GRACE:
            {
                LED::ColorLayer::guard _lock(m_layer->mutex());
                auto &front = m_layer->segment(LED::FRONT_SEGMENT);
                front.fill(LED::Color::GREEN);
                front[0] = LED::Color::MAGENTA;
                front[front.size()-1] = LED::Color::MAGENTA;
                m_layer->update();
            }
            break;
        case State::OK:
            {
                LED::ColorLayer::guard _lock(m_layer->mutex());
                auto &front = m_layer->segment(LED::FRONT_SEGMENT);
                front.fill(LED::Color::GREEN);
                m_layer->update();
            }
            break;
    }
}

void ControlSchemeBalancing::onIMUData(const Telemetry::IMUData &imu_data)
{
    dispatch([this,imu_data]{
        if (!m_initialized) 
            return;

        auto angle = imu_data.dmp_TaitBryan[TB_PITCH_X];
        auto diff = std::abs<float>(angle-m_base_angle);

        if (diff>CRITICAL_ANGLE) {
            updateState(State::CRITICAL);
            if (m_armed) {
                disarm();
            }
            m_armed_grace = false;
        }
        else if (diff>WARNING_ANGLE) {
            updateState(State::WARNING);
            m_armed_grace = false;
        }
        else if (m_armed) {
            updateState(State::OK);
        }
        else {
            if (m_armed_grace) {
                // In arm grace period
                if ( (clock_type::now()-m_armed_grace_start) > ARM_GRACE_PERIOD ) {
                    BOOST_LOG_TRIVIAL(info) << "Robot armed";
                    arm();
                    updateState(State::OK);
                }
                else {
                    updateState(State::GRACE);
                }
            }
            else {
                // Start arm grace period
                BOOST_LOG_TRIVIAL(info) << "Arm grace period begin";
                m_armed_grace = true;
                m_armed_grace_start = clock_type::now();
                updateState(State::GRACE);
            }
        }

        float duty = 0.0f;
        if (m_armed) {
            duty = m_pid.update(angle);
            BOOST_LOG_TRIVIAL(info) << "Duty " << boost::format("%.2f") % duty;
        }
        else {
            duty = 0.0f;
        }
        auto &left_motor = m_motor_control->getMotor(static_cast<uint>(MotorPosition::REAR_LEFT));
        auto &right_motor = m_motor_control->getMotor(static_cast<uint>(MotorPosition::REAR_RIGHT));
        left_motor->setDuty(duty);
        right_motor->setDuty(duty);

        auto now = clock_type::now();
        static std::chrono::high_resolution_clock::time_point last;
        if ((now-last) > 100ms) {
            BOOST_LOG_TRIVIAL(info) 
                << boost::format("angle: %.2f   diff: %.2f") % angle 
                << "  armed: "  << m_armed 
                << "  state: " << static_cast<int>(m_state) 
                << "  duty: " << boost::format("%.2f") % duty;
            last = now;
        }
    });
}


}

#endif
