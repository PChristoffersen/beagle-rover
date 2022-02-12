#ifndef _ROBOT_SYSTEM_NETWORK_H_
#define _ROBOT_SYSTEM_NETWORK_H_

#include <memory>
#include <mutex>

#include <robotconfig.h>
#include <robottypes.h>
#include <common/withmutex.h>

namespace Robot::System {

    class Network : public std::enable_shared_from_this<Network>, public WithMutex<std::mutex> {
        public:
            explicit Network(const std::shared_ptr<::Robot::Context> &context);
            Network(const Network&) = delete; // No copy constructor
            Network(Network&&) = delete; // No move constructor
            virtual ~Network();

            void init();
            void cleanup();
        
        private:
            bool m_initialized;
            int m_sockfd;

            std::string m_mac;

            void update();

            std::string calculateMAC() const;
    };

}

#endif
