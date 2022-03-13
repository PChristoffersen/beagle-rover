#ifndef _ROBOT_RC_INPUTSOURCE_H_
#define _ROBOT_RC_INPUTSOURCE_H_

#include <memory>
#include <mutex>
#include <boost/asio.hpp>

#include <robotconfig.h>
#include <input/sources/abstractinputsource.h>
#include "types.h"


#if ROBOT_HAVE_RC

namespace Robot::RC {

    class InputSource : public Robot::Input::AbstractSource {
        public:
            explicit InputSource(const Robot::Input::Signals &signals);
            InputSource(const InputSource&) = delete; // No copy constructor
            InputSource(InputSource&&) = delete; // No move constructor
            virtual ~InputSource();

            void init(const std::shared_ptr<Receiver> &receiver);
            void cleanup();

            void setEnabled(bool enabled) override;
            void setEnabledKinematic(bool enabled) override;
            void setEnabledLED(bool enabled) override;

        private:
            bool m_initialized;
            uint m_enable_cnt;

            bool m_armed;
            bool m_can_arm;
            Kinematic::DriveMode   m_drive_mode;
            Kinematic::Orientation m_orientation;
            LED::AnimationMode m_animation_mode;
            LED::IndicatorMode m_inidicator_mode;
            float m_brightness;

            std::weak_ptr<Receiver> m_receiver;
            boost::signals2::connection m_connection;
            void onRCData(Flags flags, RSSI rssi, const ChannelList &channels);

            void checkReceiverEnable();

            inline Kinematic::DriveMode calculateDriveMode(uint8_t sa, uint8_t se);
            inline Kinematic::Orientation calculateOrientation(uint8_t sb);
            inline LED::AnimationMode calculateAnimationMode(uint8_t sc, uint8_t sd);
            inline LED::IndicatorMode calculateIndicatorMode(uint8_t sg, bool si);
    };

}

#endif
#endif

