#ifndef _ROBOT_TELEMETRY_TELEMETRY_H_
#define _ROBOT_TELEMETRY_TELEMETRY_H_

#include <cstdint>
#include <memory>
#include <mutex>
#include <map>
#include <variant>
#include <string>
#include <unordered_map>
#include <boost/asio.hpp>

#include <robottypes.h>
#include <rc/receiver.h>

#include <common/withmutex.h>
#include <common/withnotify.h>
#include "types.h"
#include "events.h"


namespace Robot::Telemetry {

    class Telemetry : public std::enable_shared_from_this<Telemetry>, public WithMutex<std::mutex>, public WithNotifyInt {
        public:
            static constexpr notify_type NOTIFY_IMU { 1 };

            explicit Telemetry(const std::shared_ptr<::Robot::Context> &context);
            Telemetry(const Telemetry&) = delete; // No copy constructor
            Telemetry(Telemetry&&) = delete; // No move constructor
            virtual ~Telemetry();

            void init();
            void cleanup();

            Signal sig_event;
            #if ROBOT_HAVE_IMU
            IMUSignal sig_imu;
            #endif

            const ValueMap &imuValues() const { return m_imu_values; }

        protected:
            friend class Source;
            
            void process(const Event &event);

            #if ROBOT_HAVE_IMU
            void process(const IMUData &data);
            #endif

        private:
            bool m_initialized;
            std::vector<std::shared_ptr<class Source>> m_sources;

            ValueMap m_imu_values;

    };
        
}

#endif
