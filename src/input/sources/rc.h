#ifndef _ROBOT_INPUT_INPUT_RCSOURCE_H_
#define _ROBOT_INPUT_INPUT_RCSOURCE_H_

#include <memory>
#include <mutex>
#include <boost/asio.hpp>

#include <rc/types.h>
#include "abstractinputsource.h"
#include "../value.h"


namespace Robot::Input {

    class RCSource : public AbstractSource {
        public:
            explicit RCSource(const Signals &signals);
            RCSource(const RCSource&) = delete; // No copy constructor
            RCSource(RCSource&&) = delete; // No move constructor
            virtual ~RCSource();

            void init(const std::shared_ptr<::Robot::RC::Receiver> &receiver);
            void cleanup();

            void setEnabled(bool enabled);

        private:
            bool m_initialized;
            bool m_enabled;

            std::weak_ptr<::Robot::RC::Receiver> m_receiver;
            boost::signals2::connection m_connection;
            void onRCData(::Robot::RC::Flags flags, ::Robot::RC::RSSI rssi, const ::Robot::RC::ChannelList &channels);
    };

};

#endif


