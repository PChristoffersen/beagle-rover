#include "robotcontrolmpu.h"

#if ROBOT_HAVE_ROBOTCONTROL_MPU

#include <iostream>
#include <boost/log/trivial.hpp>

#include <robotcontext.h>
#include "../types.h"
#include "../telemetry.h"

using namespace std::literals;

namespace Robot::Telemetry {


inline static constexpr double rad2deg(double rad) {
    return rad*180.0/M_PI;
}


static const std::string SOURCE_NAME { "mpu" };

// bus for Robotics Cape and BeagleboneBlue is 2, interrupt pin is on gpio3.21
// change these for your platform
static constexpr auto I2C_BUS { 2 };
static constexpr auto GPIO_INT_PIN_CHIP { 3 };
static constexpr auto GPIO_INT_PIN_PIN { 21 };

static constexpr auto MPU_SAMPLE_RATE_HZ { 100 };
static constexpr auto MPU_ORIENTATION { ORIENTATION_Z_UP };
static constexpr auto MPU_TEMP_INTERVAL { 1000ms };
static constexpr auto MPU_INTERRUPT_SCHED_POLICY { SCHED_FIFO };
static constexpr auto MPU_INTERRUPT_PRIORITY { 10 };

static constexpr auto TELEMETRY_INTERVAL { 200ms };

RobotControlMPU *RobotControlMPU::instance { nullptr };

RobotControlMPU::RobotControlMPU(const std::shared_ptr<Robot::Context> &context):
    WithStrand { context->io() },
    m_initialized { false },
    m_event { SOURCE_NAME }
{

}

RobotControlMPU::~RobotControlMPU() 
{
    cleanup();
}


inline void RobotControlMPU::onData(const rc_mpu_data_t &data)
{
    if (auto telemetry = m_telemetry.lock()) {
        sendData(telemetry, data);


        m_saved_data = m_data;

        auto now = clock_type::now();
        if (now-m_last_telemetry > TELEMETRY_INTERVAL) {
            m_event.pitch = m_saved_data.fused_TaitBryan[TB_PITCH_X];
            m_event.roll  = m_saved_data.fused_TaitBryan[TB_ROLL_Y];
            m_event.yaw   = m_saved_data.fused_TaitBryan[TB_YAW_Z];

            #if 0
            BOOST_LOG_TRIVIAL(info) 
                << boost::format("MPU %04d  ") % m_data_count
                << boost::format("| %6.1f | %6.1f |") % rad2deg(m_saved_data.compass_heading_raw) % rad2deg(m_saved_data.compass_heading)
                << boost::format("| %6.1f | %6.1f | %6.1f |") % rad2deg(m_saved_data.fused_TaitBryan[TB_PITCH_X]) % rad2deg(m_saved_data.fused_TaitBryan[TB_ROLL_Y]) % rad2deg(m_saved_data.fused_TaitBryan[TB_YAW_Z])
                ;
            #endif

            sendEvent(telemetry, m_event);
            m_last_telemetry = now;
        }
    }
}


inline void RobotControlMPU::data_callback() 
{
    auto data = m_data;
    dispatch([this,data]{
        onData(data);
    });
}

void RobotControlMPU::init(const std::shared_ptr<Telemetry> &telemetry) 
{
    AbstractSource::init(telemetry);

    instance = this;

	rc_mpu_config_t conf = rc_mpu_default_config();
	conf.i2c_bus = I2C_BUS;
	conf.gpio_interrupt_pin_chip = GPIO_INT_PIN_CHIP;
	conf.gpio_interrupt_pin = GPIO_INT_PIN_PIN;

	conf.orient = MPU_ORIENTATION;

    conf.enable_magnetometer = 1;
    //conf.mag_sample_rate_div
    //conf.read_mag_after_callback = 0;

	conf.dmp_sample_rate = MPU_SAMPLE_RATE_HZ;
    //conf.dmp_fetch_accel_gyro=1;
    conf.dmp_auto_calibrate_gyro = 1;
    conf.dmp_interrupt_sched_policy = MPU_INTERRUPT_SCHED_POLICY;
    conf.dmp_interrupt_priority = MPU_INTERRUPT_PRIORITY;

	if(rc_mpu_initialize_dmp(&m_data, conf)){
        BOOST_THROW_EXCEPTION(std::runtime_error("Error initializing RobotControlMPU"));
	}

    rc_mpu_set_dmp_callback([]() { RobotControlMPU::instance->data_callback(); });

    m_last_telemetry = clock_type::now();

    m_initialized = true;
}


void RobotControlMPU::cleanup() 
{
    if (!m_initialized)
        return;
    m_initialized = false;

    BOOST_LOG_TRIVIAL(info) << "RobotControlMPU::cleanup()";

	rc_mpu_power_off();
    rc_mpu_set_dmp_callback(nullptr);
    instance = nullptr;

    AbstractSource::cleanup();
}



}

#endif
