#include "rcmpu.h"

#include <iostream>
#include <boost/log/trivial.hpp>

#include "telemetry.h"
#include "telemetrytypes.h"
#include "../robotcontext.h"

using namespace std;

// bus for Robotics Cape and BeagleboneBlue is 2, interrupt pin is on gpio3.21
// change these for your platform
static constexpr auto I2C_BUS { 2 };
static constexpr auto GPIO_INT_PIN_CHIP { 3 };
static constexpr auto GPIO_INT_PIN_PIN { 21 };


static constexpr auto TIMER_INTERVAL { 1000ms };


RCMPU::RCMPU(shared_ptr<RobotContext> context):
    m_initialized { false },
    m_timer { context->io() }
{

}

RCMPU::~RCMPU() 
{
    cleanup();
}


void RCMPU::init() 
{
	rc_mpu_config_t conf = rc_mpu_default_config();
	conf.i2c_bus = I2C_BUS;
	conf.gpio_interrupt_pin_chip = GPIO_INT_PIN_CHIP;
	conf.gpio_interrupt_pin = GPIO_INT_PIN_PIN;

    #if 0
	if(rc_mpu_initialize_dmp(&m_data, conf)){
        BOOST_THROW_EXCEPTION(runtime_error("Error initializing RCMPU"));
	}
    #endif
	if(rc_mpu_initialize(&m_data, conf)){
        BOOST_THROW_EXCEPTION(runtime_error("Error initializing RCMPU"));
	}

    m_timer.expires_after(TIMER_INTERVAL);
    timer_setup();

    m_initialized = true;
}


void RCMPU::cleanup() 
{
    if (!m_initialized)
        return;
    m_initialized = true;

    m_timer.cancel();

	rc_mpu_power_off();
}


void RCMPU::timer_setup() 
{
    m_timer.expires_at(m_timer.expiry() + TIMER_INTERVAL);
    m_timer.async_wait(
        [self_ptr=weak_from_this()] (auto &error) {
            if (auto self = self_ptr.lock()) { 
                self->timer(error); 
            }
        }
    );
}

void RCMPU::timer(boost::system::error_code error) 
{
    if (error!=boost::system::errc::success || !m_initialized) {
        return;
    }

    timer_setup();
}

