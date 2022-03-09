#include "inputsource.h"

#include <iomanip>
#include <boost/log/trivial.hpp>

#include "receiver.h"

#if ROBOT_HAVE_RC

using namespace std::literals;


namespace Robot::RC {

InputSource::InputSource(const Robot::Input::Signals &signals) :
    AbstractSource { signals },
    m_initialized { false },
    m_armed { false },
    m_can_arm { false },
    m_drive_mode { Kinematic::DriveMode::NONE },
    m_orientation { Kinematic::Orientation::NORTH },
    m_animation_mode { LED::AnimationMode::NONE },
    m_inidicator_mode { LED::IndicatorMode::NONE },
    m_brightness { 1.0f }
{

}


InputSource::~InputSource()
{
    cleanup();
}



void InputSource::init(const std::shared_ptr<Receiver> &receiver)
{
    const guard lock(m_mutex);
    m_initialized = true;
    m_receiver = receiver;
}


void InputSource::cleanup()
{
    const guard lock(m_mutex);
    if (!m_initialized) 
        return;
    m_initialized = false;
    m_enabled = false;
    m_connection.disconnect();
}


void InputSource::setEnabled(bool enabled)
{
    const guard lock(m_mutex);
    if (enabled==m_enabled) 
        return;
    m_enabled = enabled;

    m_armed = false;
    m_can_arm = false;
    if (m_enabled) {
        m_drive_mode = Kinematic::DriveMode::NONE;
        if (auto receiver = m_receiver.lock()) {
            receiver->setEnabled(true);
            m_connection = receiver->sigData.connect(SignalData::slot_type(&InputSource::onRCData, this, boost::placeholders::_1, boost::placeholders::_2, boost::placeholders::_3));
        }
    }
    else {
        if (auto receiver = m_receiver.lock()) {
            receiver->setEnabled(false);
            m_connection.disconnect();
        }
    }
}


void InputSource::setEnabledKinematic(bool enabled) 
{
    const guard lock(m_mutex);
    if (enabled==m_enabled_kinematic) 
        return;
    m_enabled_kinematic = enabled;

    if (enabled) {
        _setDriveMode(m_drive_mode);
        _setOrientation(m_orientation);
    }
}


void InputSource::setEnabledLED(bool enabled) 
{
    const guard lock(m_mutex);
    if (enabled==m_enabled_led) 
        return;
    m_enabled_led = enabled;

    if (enabled) {
        _setAnimationMode(m_animation_mode);
        _setIndicatorMode(m_inidicator_mode);
        _setBrightness(m_brightness);
    }
}




inline Kinematic::DriveMode InputSource::calculateDriveMode(uint8_t sa, uint8_t se)
{
    // SA Sets drive mode
    switch (sa) {
        case 0: // Wheel steer
            // SE Dictates the pos
            switch (se) {
                case 0: 
                    return Kinematic::DriveMode::FRONT_WHEEL;
                default:
                case 1:
                    return Kinematic::DriveMode::ALL_WHEEL;
                case 2: 
                    return Kinematic::DriveMode::REAR_WHEEL;
            }
        case 1: // Spinning
            return Kinematic::DriveMode::SPINNING;
        case 2: // Skid steer
            return Kinematic::DriveMode::SKID;
    }
    return Kinematic::DriveMode::NONE;
}


inline Kinematic::Orientation InputSource::calculateOrientation(uint8_t sb)
{
    switch (sb) {
        case 0:
            return Kinematic::Orientation::EAST;
        case 1:
            return Kinematic::Orientation::NORTH;
        case 2:
            return Kinematic::Orientation::WEST;
    }
    return Kinematic::Orientation::NORTH;
}



inline LED::AnimationMode InputSource::calculateAnimationMode(uint8_t sc, uint8_t sd)
{
    switch (sc) {
        case 0:
            switch (sd) {
                case 0:
                    return LED::AnimationMode::NONE;
                case 1:
                    return LED::AnimationMode::HEADLIGHTS;
                case 2:
                    return LED::AnimationMode::KNIGHT_RIDER;
            }
            break;
        case 1:
            switch (sd) {
                case 0:
                    return LED::AnimationMode::AMBULANCE;
                case 1:
                    return LED::AnimationMode::POLICE;
                case 2:
                    return LED::AnimationMode::CONSTRUCTION;
            }
            break;
        case 2:
            switch (sd) {
                case 0:
                    return LED::AnimationMode::RUNNING_LIGHT;
                case 1:
                    return LED::AnimationMode::RAINBOW;
                case 2:
                    return LED::AnimationMode::RAINBOW_WAVE;
            }
            break;
    }
    return LED::AnimationMode::NONE;
}



inline LED::IndicatorMode InputSource::calculateIndicatorMode(uint8_t sg, bool si)
{
    if (si) {
        return LED::IndicatorMode::HAZARD;
    }
    switch (sg) {
        case 0:
            return LED::IndicatorMode::LEFT;
        case 1:
            return LED::IndicatorMode::NONE;
        case 2:
            return LED::IndicatorMode::RIGHT;
    }
    return LED::IndicatorMode::NONE;
}




void InputSource::onRCData(Flags flags, RSSI rssi, const ChannelList &channels)
{
    const guard lock(m_mutex);
    if (!m_initialized) 
        return;

    // Check flags to determine if we still have connection to the Controller
    if (flags.frameLost()) {
        _steer(0.0, 0.0, 0.0, 0.0);
        if (m_drive_mode!=Kinematic::DriveMode::NONE) {
            m_drive_mode = Kinematic::DriveMode::NONE;
            _setDriveMode(m_drive_mode);
        }
        m_armed = false;
        m_can_arm = false;
        return;
    }

    // Convert channels to values

    // Sticks
    auto stickL_y { channels[0] };
    auto stickR_x { channels[1] };
    auto stickR_y { channels[2] };
    auto stickL_x { channels[3] };

    // Analogue dials
    auto s1 { channels[4] };
    auto s2 { channels[5] };
    auto sliderL { channels[6] };
    auto sliderR { channels[7] };

    // Buttons
    auto sa { channels[8].asButton(3) };
    auto sb { channels[9].asButton(3) };
    auto sc { channels[10].asButton(3) };
    auto sd { channels[11].asButton(3) };
    auto se { channels[12].asButton(3) };
    auto sf { channels[13].asToggle() };
    auto sg { channels[14].asButton(3) };
    auto shi { channels[15].asButton(4) };
    auto sh { static_cast<bool>(shi&2) };
    auto si { static_cast<bool>(shi&1) };


    // We can only arm if SF switch have been flicked to off
    if (!m_armed && !sf) {
        m_can_arm = true;
    }
    m_armed = (m_can_arm && sf);


    #if 0
    static std::chrono::high_resolution_clock::time_point last_update;
    auto time { std::chrono::high_resolution_clock::now() };
        
    if ((time-last_update) > 200ms) {
        BOOST_LOG_TRIVIAL(info) << "RC "
            << std::fixed << std::setprecision(2) 
            << "lstick=(" << stickL_x.asFloat() << "," << stickL_y.asFloat() << ") "
            << "rstick=(" << stickR_x.asFloat() << "," << stickR_y.asFloat() << ") "
            << "s1=" << s1.asFloat() << " "
            << "s2=" << s2.asFloat() << " "
            << "sL=" << sliderL.asFloat() << " "
            << "sR=" << sliderR.asFloat() << " "
            << "sa=" << static_cast<int>(sa) << " "
            << "sb=" << static_cast<int>(sb) << " "
            << "sc=" << static_cast<int>(sc) << " "
            << "sd=" << static_cast<int>(sd) << " "
            << "se=" << static_cast<int>(se) << " "
            << "sf=" << sf << " "
            << "sg=" << static_cast<int>(sg) << " "
            << "sh=" << sh << " "
            << "si=" << si << " "
            ;
        last_update = time;
    }
    #endif

    // Set output based on steering
    if (m_armed) {
        // Robot armed
        auto drive_mode = calculateDriveMode(sa, se);
        auto orientation = calculateOrientation(sb);

        auto throttle  = stickL_y.asPercent();
        auto direction = stickR_x.asFloat();
        auto aux_x     = stickL_x.asFloat();
        auto aux_y     = stickR_y.asFloat();
        
        if (sh) { // SH switch toggles reverse
            throttle = -throttle;
        }

        if (orientation!=m_orientation) {
            m_orientation = orientation;
            _setOrientation(m_orientation);
        }
        if (drive_mode!=m_drive_mode) {
            m_drive_mode = drive_mode;
            _setDriveMode(m_drive_mode);
        }
        _steer(direction, throttle, aux_x, aux_y);
    }
    else {
        // Disarmed set drive mode to NONE
        m_drive_mode = Kinematic::DriveMode::NONE;
        _setDriveMode(m_drive_mode);
    }

    // Determine LED settings
    if (m_enabled_led) {
        auto animation_mode = calculateAnimationMode(sc, sd);
        if (animation_mode!=m_animation_mode) {
            m_animation_mode = animation_mode;
            _setAnimationMode(m_animation_mode);
        }
        auto indicator_mode = calculateIndicatorMode(sg, si);
        if (indicator_mode!=m_inidicator_mode) {
            m_inidicator_mode = indicator_mode;
            _setIndicatorMode(m_inidicator_mode);
        }
        m_brightness = std::round(255.0f * s2.asPercent())/255.0f;
        _setBrightness(m_brightness);
    }

}




}

#endif
