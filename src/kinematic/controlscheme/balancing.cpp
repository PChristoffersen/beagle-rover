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


// Structural properties of rover
static constexpr auto BOARD_MOUNT_ANGLE { 0.49 }; // increase if mip tends to roll forward
static constexpr auto GEARBOX { ::Robot::Motor::GEARING };
static constexpr auto ENCODER_RES { ::Robot::Motor::ENCODER_CPR };
static constexpr auto WHEEL_RADIUS_M { WHEEL_DIAMETER_MM/2000.0 };
static constexpr auto TRACK_WIDTH_M	{ WHEEL_BASE_MM / 1000.0 };
//#define V_NOMINAL		7.4


static constexpr auto SAMPLE_RATE_HZ { 100 }; // main filter and control loop speed
static constexpr auto DT             { 1.0 / SAMPLE_RATE_HZ }; // 1/sample_rate

// inner loop controller 100hz
static constexpr auto D1_GAIN    { 1.05 };
static constexpr auto D1_ORDER   { 2 };
static constexpr std::array<double, 3> D1_NUM { -4.945, 8.862, -3.967 };
static constexpr std::array<double, 3> D1_DEN { 1.000, -1.481, 0.4812 };
static constexpr auto D1_SATURATION_TIMEOUT	{ 0.4 };

// outer loop controller 100hz
static constexpr auto D2_GAIN    { 0.9 };
static constexpr auto D2_ORDER   { 2 };
static constexpr std::array<double, 3> D2_NUM { 0.18856,  -0.37209,  0.18354 };
static constexpr std::array<double, 3> D2_DEN { 1.00000,  -1.86046,   0.86046 };
static constexpr auto THETA_REF_MAX	{ 0.33 };

// steering controller
static constexpr auto D3_KP { 1.0 };
static constexpr auto D3_KI { 0.3 };
static constexpr auto D3_KD	{ 0.05 };
static constexpr auto STEERING_INPUT_MAX { 0.5 };

// Thread Loop Rates
//#define BATTERY_CHECK_HZ	5
//#define SETPOINT_MANAGER_HZ	100
//#define PRINTF_HZ		50

// electrical hookups
//#define MOTOR_CHANNEL_L		3
//#define MOTOR_CHANNEL_R		2
//#define MOTOR_POLARITY_L	1
//#define MOTOR_POLARITY_R	-1
//#define ENCODER_CHANNEL_L	3
//#define ENCODER_CHANNEL_R	2
static constexpr auto ENCODER_POLARITY_L {  1 };
static constexpr auto ENCODER_POLARITY_R { -1 };

// other
static constexpr auto TIP_ANGLE { 0.85 };
//#define START_ANGLE		0.3
//#define START_DELAY		0.4
//#define PICKUP_DETECTION_TIME	0.6
static constexpr auto ENABLE_POSITION_HOLD { true };
static constexpr auto SOFT_START_SEC { 0.7 };



ControlSchemeBalancing::ControlSchemeBalancing(std::shared_ptr<Kinematic> kinematic) :
    AbstractControlScheme { kinematic },
    m_telemetry { kinematic->telemetry() },
    m_led_control { kinematic->ledControl() },
    m_init_timer { m_context->io() },
    m_D1 { rc_filter_empty() },
    m_D2 { rc_filter_empty() },
    m_D3 { rc_filter_empty() },
    m_armed { false },
    m_inner_saturation_counter { 0 }
{
}


ControlSchemeBalancing::~ControlSchemeBalancing() 
{
    cleanup();
}


void ControlSchemeBalancing::init() 
{
    const guard lock(m_mutex);

    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;

    initMotors();
    initFilters();

    if (auto telemetry = m_telemetry.lock()) {
        m_mpu_connection = telemetry->sig_mpu.connect(::Robot::Telemetry::MPUSignal::slot_type(&ControlSchemeBalancing::onMPUData, this, boost::placeholders::_1).track_foreign(shared_from_this()));
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

void ControlSchemeBalancing::initFilters()
{
	// set up D1 Theta controller
  	auto D1_num { D1_NUM };
	auto D1_den { D1_DEN };
	if(rc_filter_alloc_from_arrays(&m_D1, DT, D1_num.data(), D1_num.size(), D1_den.data(), D1_den.size())){
        BOOST_THROW_EXCEPTION(std::runtime_error("Error in balancing: failed to make filter D1"));
	}
	m_D1.gain = D1_GAIN;
	rc_filter_enable_saturation(&m_D1, -1.0, 1.0);
	rc_filter_enable_soft_start(&m_D1, SOFT_START_SEC);

	// set up D2 Phi controller
  	auto D2_num { D2_NUM };
	auto D2_den { D2_DEN };
	if(rc_filter_alloc_from_arrays(&m_D2, DT, D2_num.data(), D2_num.size(), D2_den.data(), D2_den.size())){
        BOOST_THROW_EXCEPTION(std::runtime_error("Error in balancing: failed to make filter D2"));
	}
	m_D2.gain = D2_GAIN;
	rc_filter_enable_saturation(&m_D2, -THETA_REF_MAX, THETA_REF_MAX);
	rc_filter_enable_soft_start(&m_D2, SOFT_START_SEC);

	printf("Inner Loop controller D1:\n");
	rc_filter_print(m_D1);
	printf("\nOuter Loop controller D2:\n");
	rc_filter_print(m_D2);

	// set up D3 gamma (steering) controller
	if(rc_filter_pid(&m_D3, D3_KP, D3_KI, D3_KD, 4*DT, DT)){
        BOOST_THROW_EXCEPTION(std::runtime_error("Error in balancing: failed to make steering controller"));
	}
	rc_filter_enable_saturation(&m_D3, -STEERING_INPUT_MAX, STEERING_INPUT_MAX);
}



void ControlSchemeBalancing::cleanup() 
{
    m_mpu_connection.disconnect();

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

    rc_filter_free(&m_D1);
    rc_filter_free(&m_D2);
    rc_filter_free(&m_D3);

    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
}



void ControlSchemeBalancing::arm() 
{
    auto &motors = m_motor_control->getMotors();
    auto &left_motor = motors[MotorPosition::REAR_LEFT];
    auto &right_motor = motors[MotorPosition::REAR_RIGHT];

	rc_filter_reset(&m_D1);
	rc_filter_reset(&m_D2);
	rc_filter_reset(&m_D3);
	m_setpoint.theta = 0.0;
	m_setpoint.phi   = 0.0;
	m_setpoint.gamma = 0.0;

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


void ControlSchemeBalancing::onMPUData(const Robot::Telemetry::MPUData &mpu_data)
{
    const guard lock(m_mutex);
    if (!m_initialized) 
        return;

#if 0
    auto &motors = m_motor_control->getMotors();
    auto &left_motor = motors[MotorPosition::REAR_LEFT];
    auto &right_motor = motors[MotorPosition::REAR_RIGHT];

	float dutyL, dutyR;
	/******************************************************************
	* STATE_ESTIMATION
	* read sensors and compute the state when either ARMED or DISARMED
	******************************************************************/
	// angle theta is positive in the direction of forward tip around X axis
	m_cstate.theta = mpu_data.dmp_TaitBryan[TB_PITCH_X] + BOARD_MOUNT_ANGLE;

	// collect encoder positions, right wheel is reversed
	m_cstate.wheelAngleR = (right_motor->getEncoderValue() * 2.0 * M_PI) / (ENCODER_POLARITY_R * GEARBOX * ENCODER_RES);
	m_cstate.wheelAngleL = (left_motor->getEncoderValue() * 2.0 * M_PI) / (ENCODER_POLARITY_L * GEARBOX * ENCODER_RES);

	// Phi is average wheel rotation also add theta body angle to get absolute
	// wheel position in global frame since encoders are attached to the body
	m_cstate.phi = ((m_cstate.wheelAngleL+m_cstate.wheelAngleR)/2) + m_cstate.theta;

	// steering angle gamma estimate
	m_cstate.gamma = (m_cstate.wheelAngleR-m_cstate.wheelAngleL) * (WHEEL_RADIUS_M/TRACK_WIDTH_M);

	/*************************************************************
	* check for various exit conditions AFTER state estimate
	***************************************************************/
/*
	if(rc_get_state()==EXITING){
		rc_motor_set(0,0.0);
		return;
	}
	// if controller is still ARMED while state is PAUSED, disarm it
	if(rc_get_state()!=RUNNING && setpoint.arm_state==ARMED){
		__disarm_controller();
		return;
	}
*/
	// exit if the controller is disarmed
	if(!m_armed){
		return;
	}

	// check for a tipover
	if(fabs(m_cstate.theta) > TIP_ANGLE){
		disarm();
        BOOST_LOG_TRIVIAL(info) << "tip detected";
		return;
	}

	/************************************************************
	* OUTER LOOP PHI controller D2
	* Move the position setpoint based on phi_dot.
	* Input to the controller is phi error (setpoint-state).
	*************************************************************/
	if (ENABLE_POSITION_HOLD) {
		if (fabs(m_setpoint.phi_dot) > 0.001) {
            m_setpoint.phi += m_setpoint.phi_dot*DT;
        }
		m_cstate.d2_u = rc_filter_march(&m_D2, m_setpoint.phi-m_cstate.phi);
		m_setpoint.theta = m_cstate.d2_u;
	}
	else {
        m_setpoint.theta = 0.0;
    }

	/************************************************************
	* INNER LOOP ANGLE Theta controller D1
	* Input to D1 is theta error (setpoint-state). Then scale the
	* output u to compensate for changing battery voltage.
	*************************************************************/
	//m_D1.gain = D1_GAIN * V_NOMINAL/m_cstate.vBatt;
	m_cstate.d1_u = rc_filter_march(&m_D1, m_setpoint.theta-m_cstate.theta);

	/*************************************************************
	* Check if the inner loop saturated. If it saturates for over
	* a second disarm the controller to prevent stalling motors.
	*************************************************************/
	if (fabs(m_cstate.d1_u)>0.95) {
        m_inner_saturation_counter++;
    }
	else {
        m_inner_saturation_counter = 0;
    }
	// if saturate for a second, disarm for safety
	if (m_inner_saturation_counter > (SAMPLE_RATE_HZ*D1_SATURATION_TIMEOUT)) {
		BOOST_LOG_TRIVIAL(warning) << "Inner loop controller saturated";
		disarm();
		m_inner_saturation_counter = 0;
		return;
	}

	/**********************************************************
	* gama (steering) controller D3
	* move the setpoint gamma based on user input like phi
	***********************************************************/
	if (fabs(m_setpoint.gamma_dot)>0.0001) {
        m_setpoint.gamma += m_setpoint.gamma_dot * DT;
    }
	m_cstate.d3_u = rc_filter_march(&m_D3, m_setpoint.gamma - m_cstate.gamma);

	/**********************************************************
	* Send signal to motors
	* add D1 balance control u and D3 steering control also
	* multiply by polarity to make sure direction is correct.
	***********************************************************/
	dutyL = m_cstate.d1_u - m_cstate.d3_u;
	dutyR = m_cstate.d1_u + m_cstate.d3_u;
	
    //rc_motor_set(MOTOR_CHANNEL_L, MOTOR_POLARITY_L * dutyL);
	//rc_motor_set(MOTOR_CHANNEL_R, MOTOR_POLARITY_R * dutyR);
    #endif
}


}

#endif
