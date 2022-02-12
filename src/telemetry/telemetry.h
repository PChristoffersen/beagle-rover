#ifndef _ROBOT_TELEMETRY_TELEMETRY_H_
#define _ROBOT_TELEMETRY_TELEMETRY_H_

#include <cstdint>
#include <memory>
#include <mutex>
#include <map>
#include <variant>
#include <boost/asio.hpp>

#include <robottypes.h>
#include <rc/receiver.h>

#include "../common/withmutex.h"
#include "types.h"
#include "events.h"


namespace Robot::Telemetry {

    class Telemetry : public std::enable_shared_from_this<Telemetry>, public WithMutex<std::mutex> {
        public:
            explicit Telemetry(const std::shared_ptr<::Robot::Context> &context);
            Telemetry(const Telemetry&) = delete; // No copy constructor
            Telemetry(Telemetry&&) = delete; // No move constructor
            virtual ~Telemetry();

            void init();
            void cleanup();

            std::uint32_t valuesVersion() const { return m_values_version; }
            const ValueMap &valuesUnlocked() const { return m_values; }

            Signal sig_event;
            #if ROBOT_HAVE_MPU
            MPUSignal sig_mpu;
            #endif

        protected:
            friend class Source;
            
            void process(const Event &event);

            #if ROBOT_HAVE_MPU
            void process(const MPUData &data);
            #endif

        private:
            bool m_initialized;
            std::vector<std::shared_ptr<class Source>> m_sources;

            std::uint32_t m_values_version;
            ValueMap m_values;
    };
        
}

#endif
