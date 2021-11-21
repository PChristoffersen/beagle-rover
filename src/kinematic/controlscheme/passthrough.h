#ifndef _CONTROLSCHEMEPASSTHROUGH_H_
#define _CONTROLSCHEMEPASSTHROUGH_H_

#include <boost/asio.hpp>
#include <boost/signals2.hpp>

#include <robotcontrolext.h>

#include "../../rc/rcreceiver.h"
#include "abstractcontrolscheme.h"

namespace Robot::Kinematic {

    class ControlSchemePassthrough : public AbstractControlScheme<ControlSchemePassthrough> {
        public: 
            explicit ControlSchemePassthrough(std::shared_ptr<Kinematic> kinematic);
            ControlSchemePassthrough(const ControlSchemePassthrough&) = delete; // No copy constructor
            ControlSchemePassthrough(ControlSchemePassthrough&&) = delete; // No move constructor
            virtual ~ControlSchemePassthrough();

            virtual void init() override;
            virtual void cleanup() override;

        private:
            static constexpr auto MOTOR_PASSTHROUGH_OFFSET { 0u };
            static constexpr auto SERVO_PASSTHROUGH_OFFSET { 4u };

            boost::signals2::connection m_rc_connection;

            void onRCData(Robot::RC::Receiver::Flags flags, uint8_t rssi, const Robot::RC::Receiver::ChannelList &channels);
    };

};

#endif
