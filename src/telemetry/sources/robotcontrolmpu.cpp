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

static constexpr auto TIMER_INTERVAL { 250ms };


RobotControlMPU *RobotControlMPU::instance { nullptr };

RobotControlMPU::RobotControlMPU(const std::shared_ptr<Robot::Context> &context):
    AbstractSource { context },
    m_initialized { false },
    m_timer { context->io() },
    m_data_count { 0 },
    m_saved_data_count { 0 },
    m_event { SOURCE_NAME }
{

}

RobotControlMPU::~RobotControlMPU() 
{
    cleanup();
}


void RobotControlMPU::init(const std::shared_ptr<Telemetry> &telemetry) 
{
    const guard lock(m_mutex);
    AbstractSource::init(telemetry);

    m_last_temp_read = clock_type::now() - MPU_TEMP_INTERVAL;
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

    m_timer.expires_after(TIMER_INTERVAL);
    timer_setup();

    m_initialized = true;
}


void RobotControlMPU::cleanup() 
{
    const guard lock(m_mutex);
    if (!m_initialized)
        return;
    m_initialized = false;

    BOOST_LOG_TRIVIAL(info) << "RobotControlMPU::cleanup()";

    m_timer.cancel();

	rc_mpu_power_off();
    instance = nullptr;
    
    AbstractSource::cleanup();
}


void RobotControlMPU::timer_setup() 
{
    m_timer.expires_at(m_timer.expiry() + TIMER_INTERVAL);
    m_timer.async_wait(
        [self_ptr=weak_from_this()] (boost::system::error_code error) {
            if (auto self = self_ptr.lock()) { 
                self->timer(error); 
            }
        }
    );
}

void RobotControlMPU::timer(boost::system::error_code error) 
{
    const guard lock(m_mutex);
    if (error!=boost::system::errc::success || !m_initialized) {
        return;
    }


    bool have_data = false;
    {
        const guard data_lock(m_data_mutex);
        if (m_saved_data_count != m_data_count) {
            have_data = true;
            m_event.pitch = m_saved_data.fused_TaitBryan[TB_PITCH_X];
            m_event.roll  = m_saved_data.fused_TaitBryan[TB_ROLL_Y];
            m_event.yaw   = m_saved_data.fused_TaitBryan[TB_YAW_Z];
            m_event.temp  = m_saved_data.temp;
            m_saved_data_count = m_data_count;

            #if 0
            BOOST_LOG_TRIVIAL(info) 
                << boost::format("MPU %04d  ") % m_data_count
                << boost::format("| %6.1f | %6.1f |") % rad2deg(m_saved_data.compass_heading_raw) % rad2deg(m_saved_data.compass_heading)
                << boost::format("| %6.1f | %6.1f | %6.1f |") % rad2deg(m_saved_data.fused_TaitBryan[TB_PITCH_X]) % rad2deg(m_saved_data.fused_TaitBryan[TB_ROLL_Y]) % rad2deg(m_saved_data.fused_TaitBryan[TB_YAW_Z])
                ;
            #endif
        }
    }

    if (have_data) {
        if (auto telemetry = m_telemetry.lock()) {
            sendEvent(telemetry, m_event);
        }
    }




    /*
    if(show_compass){
                printf("   %6.1f   |", data.compass_heading_raw*RAD_TO_DEG);
                printf("   %6.1f   |", data.compass_heading*RAD_TO_DEG);
        }
        if(show_quat && enable_mag){
                // print fused quaternion
                printf(" %4.1f %4.1f %4.1f %4.1f |",    data.fused_quat[QUAT_W], \
                                                        data.fused_quat[QUAT_X], \
                                                        data.fused_quat[QUAT_Y], \
                                                        data.fused_quat[QUAT_Z]);
        }
        else if(show_quat){
                // print quaternion
                printf(" %4.1f %4.1f %4.1f %4.1f |",    data.dmp_quat[QUAT_W], \
                                                        data.dmp_quat[QUAT_X], \
                                                        data.dmp_quat[QUAT_Y], \
                                                        data.dmp_quat[QUAT_Z]);
        }
        if(show_tb && enable_mag){
                // print fused TaitBryan Angles
                printf("%6.1f %6.1f %6.1f |",   data.fused_TaitBryan[TB_PITCH_X]*RAD_TO_DEG,\
                                                data.fused_TaitBryan[TB_ROLL_Y]*RAD_TO_DEG,\
                                                data.fused_TaitBryan[TB_YAW_Z]*RAD_TO_DEG);
        }
        else if(show_tb){
                // print TaitBryan angles
                printf("%6.1f %6.1f %6.1f |",   data.dmp_TaitBryan[TB_PITCH_X]*RAD_TO_DEG,\
                                                data.dmp_TaitBryan[TB_ROLL_Y]*RAD_TO_DEG,\
                                                data.dmp_TaitBryan[TB_YAW_Z]*RAD_TO_DEG);
        }
        if(show_accel){
                printf(" %5.2f %5.2f %5.2f |",  data.accel[0],\
                                                data.accel[1],\
                                                data.accel[2]);
        }
        if(show_gyro){
                printf(" %5.1f %5.1f %5.1f |",  data.gyro[0],\
                                                data.gyro[1],\
                                                data.gyro[2]);
        }
        if(show_temp){
                rc_mpu_read_temp(&data);
                printf(" %6.2f |", data.temp);
        }
        */


    timer_setup();
}


void RobotControlMPU::data_callback()
{
    const auto now = clock_type::now();
    if (now-m_last_temp_read > MPU_TEMP_INTERVAL) {
        rc_mpu_read_temp(&m_data);
        m_last_temp_read = now;
    }

    {
        const guard lock(m_data_mutex);
        m_data_count++;
        m_saved_data = m_data;
    }


    if (auto telemetry = m_telemetry.lock()) {
        sendData(telemetry, m_data);
    }
}

}

#endif
