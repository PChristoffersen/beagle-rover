#ifndef _ROBOT_SYSTEM_NETWORK_H_
#define _ROBOT_SYSTEM_NETWORK_H_

#include <memory>
#include <mutex>
#include <vector>
#include <string>
#include <set>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <boost/asio.hpp>


#include <robotconfig.h>
#include <robottypes.h>
#include <common/withstrand.h>
#include <common/withmutex.h>
#include <common/withnotify.h>


struct nl_sock;
struct nl_object;
struct nl_msg;
struct nl_cb;
struct nl_cache_mngr;
struct rtnl_link;
struct rtnl_addr;
struct genlmsghdr;

namespace Robot::System {

    class Network : public std::enable_shared_from_this<Network>, public WithMutexStd, public WithNotifyInt, public WithStrand {
        public:
            static constexpr notify_type NOTIFY_SIGNAL { 1 };

            using timer_type = boost::asio::high_resolution_timer;

            struct Interface : public WithMutexStd {
                enum class Type {
                    ETHERNET,
                    WIRELESS,
                    WIRELESS_AP,
                    USB_GADGET,
                };

                struct WifiInfo {
                    std::string ssid;
                    uint frequency;
                    uint channel;
                    const char *channel_width;
                    int8_t signal_dbm;
                    uint8_t signal_rssi;
                };

                using index_type = std::uint32_t;
                using address_list_type = std::set<std::string>;
                using wifiinfo_type = std::unique_ptr<WifiInfo>;


                index_type index;
                std::string name;
                std::string mac;
                Type type;
                bool active;
                address_list_type addresses;
                wifiinfo_type wifi;

                friend std::ostream &operator<<(std::ostream &os, const Interface &self)
                {
                    return os 
                        << "<Interface "
                        << self.name
                        << ">"
                    ;
                }
            };

            using interface_type = std::shared_ptr<Interface>;
            using interface_map_type = std::unordered_map<Interface::index_type, interface_type>;
            using interface_list_type = std::vector<interface_type>;

            explicit Network(const std::shared_ptr<::Robot::Context> &context);
            Network(const Network&) = delete; // No copy constructor
            Network(Network&&) = delete; // No move constructor
            virtual ~Network();

            void init();
            void cleanup();
        
            void dump() const;

            interface_list_type interfaces() const;
            interface_list_type interfaces(Interface::Type type) const;


        private:
            static void delete_sock(struct nl_sock *sock) noexcept;
            static void delete_cache_mngr(struct nl_cache_mngr *cache_mngr) noexcept;
            static void delete_msg(struct nl_msg *msg) noexcept;
            static void delete_cb(struct nl_cb *cb) noexcept;
            using sock_type = std::unique_ptr<struct nl_sock, decltype(&delete_sock)>;
            using cache_mngr_type = std::unique_ptr<struct nl_cache_mngr, decltype(&delete_cache_mngr)>;
            using msg_type = std::unique_ptr<struct nl_msg, decltype(&delete_msg)>;
            using cb_type = std::unique_ptr<struct nl_cb, decltype(&delete_cb)>;

            bool m_initialized;

            int m_nl80211_id;
            sock_type m_socket;
            boost::asio::posix::stream_descriptor m_socket_descriptor;
            cache_mngr_type m_cache_mngr;
            boost::asio::posix::stream_descriptor m_cache_descriptor;

            interface_map_type m_interfaces;

            timer_type m_timer;

            void initSocket();
            void initInterfaces();

            void interface_cb(struct rtnl_link *link, int action);
            void interface_addr_cb(struct rtnl_addr *addr, int action);

            void nl80211_get_interface(Interface::index_type index);
            void nl80211_get_station(Interface::index_type index);

            void nl80211_get_interface_cb(struct genlmsghdr *gnlh);
            void nl80211_get_station_cb(struct genlmsghdr *gnlh);

            int socket_cb(struct nl_msg *msg);

            void async_read(boost::asio::posix::stream_descriptor &desc, struct nl_cache_mngr *mngr);
            void async_read(boost::asio::posix::stream_descriptor &desc, sock_type &socket);

            void startTimer();

            friend std::ostream &operator<<(std::ostream &os, const Network &self)
            {
                return os << "Robot::System::Network";
            }

    };

}

#endif
