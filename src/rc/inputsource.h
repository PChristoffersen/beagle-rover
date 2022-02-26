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

            void setEnabled(bool enabled);

        private:
            bool m_initialized;
            bool m_enabled;

            std::weak_ptr<Receiver> m_receiver;
            boost::signals2::connection m_connection;
            void onRCData(Flags flags, RSSI rssi, const ChannelList &channels);
    };

}

#endif
#endif

