#ifndef _ROBOT_SYSTEM_POWER_H_
#define _ROBOT_SYSTEM_POWER_H_

#include <memory>
#include <mutex>

#include <robotconfig.h>
#include <robottypes.h>
#include <common/withmutex.h>
#include <common/withnotify.h>
#include <telemetry/types.h>

namespace Robot::System {

    class Power : public std::enable_shared_from_this<Power>, public WithMutex<std::mutex>, public WithNotifyDefault {
        public:
            explicit Power(const std::shared_ptr<::Robot::Context> &context);
            Power(const Power&) = delete; // No copy constructor
            Power(Power&&) = delete; // No move constructor
            virtual ~Power();

            void init(const std::shared_ptr<::Robot::Telemetry::Telemetry> &telemetry);
            void cleanup();
        
        private:
            bool m_initialized;

            boost::signals2::connection m_telemetry_connection;

            void telemetryEvent(const ::Robot::Telemetry::Event &event);

            friend std::ostream &operator<<(std::ostream &os, const Power &self)
            {
                return os << "Robot::System::Power";
            }
    };

}


#endif
