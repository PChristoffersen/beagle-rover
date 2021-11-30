#ifndef _TELEMETRY_H_
#define _TELEMETRY_H_

#include <cstdint>
#include <memory>
#include <boost/asio.hpp>

#include "telemetrytypes.h"
#include "../robottypes.h"
#include "../rc/rcreceiver.h"


namespace Robot::Telemetry {

    class Telemetry : public std::enable_shared_from_this<Telemetry> {
        public:
            explicit Telemetry(const std::shared_ptr<::Robot::Context> &context);
            Telemetry(const Telemetry&) = delete; // No copy constructor
            Telemetry(Telemetry&&) = delete; // No move constructor
            virtual ~Telemetry();

            void init();
            void cleanup();

            Signal sig_event;

        protected:
            friend class Source;
            
            void process(const Event &event);

        private:
            bool m_initialized;
            std::vector<std::shared_ptr<class Source>> m_sources;
    };
        
};

#endif
