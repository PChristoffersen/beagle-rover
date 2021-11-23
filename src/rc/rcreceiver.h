#ifndef _RCRECEIVER_H_
#define _RCRECEIVER_H_

#include <cstdint>
#include <memory>
#include <mutex>
#include <chrono>
#include <array>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <robotcontrolext.h>

#include "../robotcontext.h"
#include "../common/withmutex.h"
#include "../telemetry/telemetrytypes.h"
#include "rctypes.h"


namespace Robot::RC {

    class Receiver : public std::enable_shared_from_this<Receiver>, public WithMutex<std::recursive_mutex> {
        public:
            SignalFlags sigFlags;
            SignalRSSI sigRSSI;
            SignalData sigData;

            explicit Receiver(const std::shared_ptr<Robot::Context> &context);
            Receiver(const Receiver&) = delete; // No copy constructor
            Receiver(Receiver&&) = delete; // No move constructor
            virtual ~Receiver();

            void init(const std::shared_ptr<Robot::Telemetry::Telemetry> &telemetry);
            void cleanup();

            void setEnabled(bool enabled);
            bool getEnabled() const { return m_enabled; }

            bool isConnected() const { return m_flags.bits.frame_lost; }
            std::uint8_t getRSSI() const { return m_rssi; }
            Flags getFlags() const { return m_flags; }

            const ChannelList &channels() const { return m_channels; }

        private:
            std::shared_ptr<Robot::Context> m_context;
            bool m_initialized;
            bool m_enabled;
            boost::asio::steady_timer m_timer;
            std::uint32_t m_last_counter;
            volatile shm_fbus_t *m_fbus;

            ChannelList m_channels;
            RSSI m_rssi;
            Flags m_flags;

            boost::signals2::connection m_telemetry_connection;

            void timerSetup();
            void timer(boost::system::error_code error);

            void telemetryEvent(const Robot::Telemetry::Event &event);
            void sendTelemetry(std::uint16_t appId, std::uint32_t data);
   };
 
};


#endif 
