#ifndef _TELEMETRY_H_
#define _TELEMETRY_H_

#include <cstdint>
#include <memory>
#include <boost/asio.hpp>

#include "telemetrysource.h"
#include "telemetrytypes.h"
#include "../robotcontext.h"
#include "../rc/rcreceiver.h"


namespace Robot::Telemetry {

    class Telemetry : public std::enable_shared_from_this<Telemetry> {
        public:
            explicit Telemetry(const std::shared_ptr<Robot::Context> &context, const std::shared_ptr<Robot::RC::Receiver> &receiver);
            Telemetry(const Telemetry&) = delete; // No copy constructor
            Telemetry(Telemetry&&) = delete; // No move constructor
            virtual ~Telemetry();

            void init();
            void cleanup();

            Signal sig_event;

        protected:
            void process(const Event &event);
            void processBattery(const EventBattery &event);
            void send(std::uint16_t appId, std::uint32_t data);

        private:
            bool m_initialized;
            std::weak_ptr<Robot::RC::Receiver> m_receiver;
            std::vector<std::shared_ptr<class Source>> m_sources;
            std::vector<boost::signals2::connection> m_source_connections;

    };
        
};

#endif
