#ifndef _ROBOT_RC_RECEIVER_H_
#define _ROBOT_RC_RECEIVER_H_

#include <cstdint>
#include <memory>
#include <mutex>
#include <chrono>
#include <array>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>

#include <robotconfig.h>
#include <robottypes.h>
#include <common/withstrand.h>
#include <common/withmutex.h>
#include <common/withnotify.h>
#include <telemetry/types.h>
#include "types.h"

namespace Robot::RC {

    class Receiver : public std::enable_shared_from_this<Receiver>, public WithMutexStd, public WithNotifyInt, public WithStrand {
        public:
            using timer_type = boost::asio::steady_timer;

            static constexpr notify_type NOTIFY_CHANNELS { 1 };

            SignalFlags sigFlags;
            SignalRSSI sigRSSI;
            SignalData sigData;

            explicit Receiver(const std::shared_ptr<::Robot::Context> &context);
            Receiver(const Receiver&) = delete; // No copy constructor
            Receiver(Receiver&&) = delete; // No move constructor
            virtual ~Receiver();

            void init(const std::shared_ptr<::Robot::Telemetry::Telemetry> &telemetry);
            void cleanup();

            void setEnabled(bool enabled);
            bool getEnabled() const { return m_enabled; }

            bool isConnected() const { return !m_flags.frameLost(); }
            std::uint8_t getRSSI() const { return m_rssi; }
            Flags getFlags() const { return m_flags; }

            const ChannelList &channels() const { return m_channels; }

        private:
            std::shared_ptr<::Robot::Context> m_context;
            bool m_initialized;
            bool m_enabled;
            timer_type m_timer;
            std::uint32_t m_last_counter;
            #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
            volatile shm_fbus_t *m_fbus;
            #endif

            ChannelList m_channels;
            #ifdef HAVE_ROBOTCONTROL
            rc_filter_t m_rssi_filter;
            #endif
            RSSI m_rssi;
            Flags m_flags;

            boost::signals2::connection m_telemetry_connection;

            void timerSetup();
            inline void timer();

            void onTelemetryEvent(const ::Robot::Telemetry::Event &event);
            void sendTelemetry(std::uint16_t appId, std::uint32_t data);

            friend std::ostream &operator<<(std::ostream &os, const Receiver &self)
            {
                return os << "Robot::RC::Receiver";
            }

   };
 
}

#endif 
