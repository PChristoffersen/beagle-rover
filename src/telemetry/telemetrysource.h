#ifndef _TELEMETRYSOURCE_H_
#define _TELEMETRYSOURCE_H_

#include <memory>

#include "telemetrytypes.h"
#include "telemetry.h"

namespace Robot::Telemetry {

    class Source {
        public:
            virtual ~Source() {}

            virtual void init(const std::shared_ptr<Telemetry> &telemetry) 
            {
                m_telemetry = telemetry;
            }
            virtual void cleanup() {
                m_telemetry.reset();
            }

        protected:
            friend class Telemetry;
            std::weak_ptr<Telemetry> m_telemetry;

            void send(const Event &event)
            {
                if (auto telemetry = m_telemetry.lock()) {
                    telemetry->process(event);
                }
            }

    };

};

#endif