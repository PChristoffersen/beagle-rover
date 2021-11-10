#ifndef _CONTROLSCHEMEPASSTHROUGH_H_
#define _CONTROLSCHEMEPASSTHROUGH_H_

#include <boost/asio.hpp>
#include <boost/signals2.hpp>

#include <robotcontrolext.h>

#include "../rcreceiver/rcreceiver.h"
#include "abstractcontrolscheme.h"

class ControlSchemePassthrough : public AbstractControlScheme<ControlSchemePassthrough> {
    public: 
        explicit ControlSchemePassthrough(std::shared_ptr<class Kinematic> kinematic);
        ControlSchemePassthrough(const ControlSchemePassthrough&) = delete; // No copy constructor
        ControlSchemePassthrough(ControlSchemePassthrough&&) = delete; // No move constructor
        virtual ~ControlSchemePassthrough();

        virtual void init() override;
        virtual void cleanup() override;

    private:
        boost::signals2::connection m_rc_connection;

        void onRCData(RCReceiver::Flags flags, uint8_t rssi, const RCReceiver::ChannelList &channels);
};

#endif
