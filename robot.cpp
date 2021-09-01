#include "robot.h"

#include <robotcontrol.h>
#include <robotcontrol-ext.h>


using namespace boost;
using namespace boost::asio;


Robot::Robot(io_context &io) : 
    m_motor_control(io),
    m_battery(io)
{
}

Robot::~Robot() {
    if (m_initialized) {
        cleanup();
    }
}


void Robot::init() {
    rc_adc_init();
    rc_servo_init();
    rc_motor_init();
    //rc_motor_standby(1);
    rc_ext_pru_init();
    //rc_ext_neopixel_init();
    //rc_ext_encoder_init();
    rc_ext_fbus_init();

    m_motor_control.init();
    m_battery.init();
    Component::init();
}

void Robot::cleanup() {
    m_battery.cleanup();
    m_motor_control.cleanup();

    rc_ext_fbus_cleanup();
    //rc_ext_encoder_cleanup();
    //rc_ext_neopixel_cleanup();
    rc_ext_pru_cleanup();
    rc_motor_cleanup();
    rc_servo_cleanup();
    rc_adc_cleanup();

    Component::cleanup();
}
