#ifndef _SYSTEMWIFI_H_
#define _SYSTEMWIFI_H_

#include <memory>
#include <mutex>

#include "../robottypes.h"
#include "../common/withmutex.h"

namespace Robot::System {

    class WiFi : public std::enable_shared_from_this<WiFi>, public WithMutex<std::mutex> {
        public:
            explicit WiFi(const std::shared_ptr<::Robot::Context> &context);
            WiFi(const WiFi&) = delete; // No copy constructor
            WiFi(WiFi&&) = delete; // No move constructor
            virtual ~WiFi();

            void init();
            void cleanup();
        
        private:
            bool m_initialized;
            int m_sockfd;

            std::string m_mac;

            void update();

            std::string calculateMAC() const;
    };

};


#endif