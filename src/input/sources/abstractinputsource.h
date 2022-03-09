#ifndef _ROBOT_INPUT_ABSTRACTINPUTSOURCE_H_
#define _ROBOT_INPUT_ABSTRACTINPUTSOURCE_H_

#include <memory>
#include <robotcontext.h>
#include <common/withmutex.h>
#include "../types.h"
#include "../inputsource.h"

namespace Robot::Input {

    class AbstractSource : public Source, public WithMutex<std::recursive_mutex> {
        public:
            AbstractSource(const Signals &signals) :
                Source(signals),
                m_enabled { false },
                m_enabled_kinematic { false },
                m_enabled_led { false }
            {
            }

            void setEnabled(bool enabled) override 
            {
                const guard lock(m_mutex);
                if (enabled==m_enabled)
                    return;
                m_enabled = enabled;
            }
            void setEnabledKinematic(bool enabled) override 
            {
                const guard lock(m_mutex);
                if (enabled==m_enabled_kinematic)
                    return;
                m_enabled_kinematic = enabled;
            }
            void setEnabledLED(bool enabled) override 
            {
                const guard lock(m_mutex);
                if (enabled==m_enabled_led)
                    return;
                m_enabled_led = enabled;
            }


        protected:
            bool m_enabled;
            bool m_enabled_kinematic;
            bool m_enabled_led;

            inline void _steer(float direction, float throttle, float aux_x, float aux_y) {
                if (m_enabled) {
                    m_signals.steer(direction, throttle, aux_x, aux_y);
                }
            }
            inline void _setDriveMode(Kinematic::DriveMode drive_mode) {
                if (m_enabled_kinematic) {
                    m_signals.drive_mode(drive_mode);
                }
            }
            inline void _setOrientation(Kinematic::Orientation orientation) {
                if (m_enabled_kinematic) {
                    m_signals.orientation(orientation);
                }
            }
            inline void _setAnimationMode(LED::AnimationMode animation_mode) {
                if (m_enabled_led) {
                    m_signals.animation_mode(animation_mode);
                }
            }
            inline void _setIndicatorMode(LED::IndicatorMode indicator_mode) {
                if (m_enabled_led) {
                    m_signals.indicator_mode(indicator_mode);
                }
            }
            inline void _setBrightness(float brightness) {
                if (m_enabled_led) {
                    m_signals.brightness(brightness);
                }
            }

    };

}

#endif
