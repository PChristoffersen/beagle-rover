#include "network.h"

#include <sys/ioctl.h>
#include <sys/types.h>
#include <net/if.h>
#include <linux/wireless.h>
#include <unistd.h>

#include <algorithm>
#include <bitset>
#include <sstream>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp> 
#include <boost/algorithm/string/join.hpp>

namespace Robot::System {

static constexpr auto IFNAME { "wlan0" };


Network::Network(const std::shared_ptr<Robot::Context> &context) :
    m_initialized { false },
    m_sockfd { -1 }
{
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
}


Network::~Network() 
{
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
    cleanup();
}


int pushif(std::vector<std::string> &vals, std::string name, int flags, int arg)
{
    if (flags & arg) {
        vals.push_back(name);
        flags &= ~arg;
    }
    return flags;
}

std::string flags2str(short flags) 
{
    std::vector<std::string> vals;
    flags = pushif(vals, "UP", flags, IFF_UP);
    flags = pushif(vals, "BROADCAST", flags, IFF_BROADCAST);
    flags = pushif(vals, "DEBUG", flags, IFF_DEBUG);
    flags = pushif(vals, "LOOPBACK", flags, IFF_LOOPBACK);
    flags = pushif(vals, "POINTTOPOINT", flags, IFF_POINTOPOINT);
    flags = pushif(vals, "RUNNING", flags, IFF_RUNNING);
    flags = pushif(vals, "NOARP", flags, IFF_NOARP);
    flags = pushif(vals, "PROMISC", flags, IFF_PROMISC);
    flags = pushif(vals, "NOTRAILERS", flags, IFF_NOTRAILERS);
    flags = pushif(vals, "ALLMULTI", flags, IFF_ALLMULTI);
    flags = pushif(vals, "MASTER", flags, IFF_MASTER);
    flags = pushif(vals, "SLAVE", flags, IFF_SLAVE);
    flags = pushif(vals, "MULTICAST", flags, IFF_MULTICAST);
    flags = pushif(vals, "AUTOMEDIA", flags, IFF_AUTOMEDIA);
    flags = pushif(vals, "DYNAMIC", flags, IFF_DYNAMIC);
    flags = pushif(vals, "LOWER_UP", flags, IFF_LOWER_UP);
    flags = pushif(vals, "DORMANT", flags, IFF_DORMANT);
    flags = pushif(vals, "ECHO", flags, IFF_ECHO);


    std::stringstream s;
    s << boost::algorithm::join(vals, ", ");
    if (flags) {
        s << ", " << std::bitset<32>(flags);
    }
    return s.str();
}

void Network::init()
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
    m_initialized = true;

    m_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_sockfd<0) {
        BOOST_THROW_EXCEPTION(std::runtime_error("Error initializing Network"));
    }

    #if 0
    m_mac = calculateMAC();
    update();
    #endif

    detectInterface();

}


void Network::detectInterface()
{
    for (auto &ifname : listInterfaces()) {
        struct iwreq wreq;
        memset(&wreq, 0x00, sizeof(wreq));
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wstringop-truncation"
        strncpy(wreq.ifr_name, ifname.c_str(), IFNAMSIZ);
        #pragma GCC diagnostic pop
        BOOST_LOG_TRIVIAL(info) << "Interface " << ifname;

        // Check for wireless interface
        if (ioctl(m_sockfd, SIOCGIWNAME, &wreq) != -1) {
            BOOST_LOG_TRIVIAL(info) << "    type: wireless (" << wreq.u.name << ")";

            if (ioctl(m_sockfd, SIOCGIWMODE, &wreq)==-1) continue;
            if (wreq.u.mode==IW_MODE_MASTER) {
            }
            else if (wreq.u.mode==IW_MODE_AUTO || wreq.u.mode==IW_MODE_ADHOC || wreq.u.mode==IW_MODE_INFRA || wreq.u.mode==IW_MODE_MESH) {
                BOOST_LOG_TRIVIAL(info) << "    mode: Other";

            }

            //SIOCGIWESSID for ssid
            char buffer[IW_ESSID_MAX_SIZE];
            std::fill(std::begin(buffer), std::end(buffer), 0x00);
            wreq.u.essid.pointer = buffer;
            wreq.u.essid.length = sizeof(buffer);
            //this will gather the SSID of the connected network
            if(ioctl(m_sockfd, SIOCGIWESSID, &wreq) == -1){
                //die with error, invalid interface
                BOOST_LOG_TRIVIAL(warning) << "Invalid interface (2)";
            }
            BOOST_LOG_TRIVIAL(info) << "    ESSID " << buffer;

            continue;
        }
    }
}


std::vector<std::string> Network::listInterfaces()
{
    std::vector<std::string> res;
    struct if_nameindex *if_ni = if_nameindex();
    if (if_ni != nullptr) {
        struct ifreq req;
        for (auto i = if_ni; !(i->if_index==0 && i->if_name==nullptr); i++) {
            memset(&req, 0x00, sizeof(req));
            #pragma GCC diagnostic push
            #pragma GCC diagnostic ignored "-Wstringop-truncation"
            strncpy(req.ifr_name, i->if_name, IFNAMSIZ);
            #pragma GCC diagnostic pop
            if (ioctl(m_sockfd, SIOCGIFFLAGS, &req)==-1) {
                continue;
            }
            // Ignore loopback interfaces and interfaces with no arp
            if (req.ifr_flags & (IFF_LOOPBACK|IFF_NOARP) ) {
                continue;
            }
            res.push_back(i->if_name);
        }
        if_freenameindex(if_ni);
    }
    return res;
}


void Network::cleanup()
{
    const guard lock(m_mutex);
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
    if (!m_initialized)
        return;
    m_initialized = false;

    close(m_sockfd);
    m_sockfd = -1;
}


void Network::update()
{
#if 0
    iwreq req;
    strcpy(req.ifr_name, IFNAME);

    iw_statistics stats;
    req.u.data.pointer = &stats;
    req.u.data.length = sizeof(iw_statistics);

    //this will gather the signal strength
    if(ioctl(m_sockfd, SIOCGIWSTATS, &req) == -1){
        //die with error, invalid interface
        BOOST_LOG_TRIVIAL(warning) << "Invalid interface";
    }
    else if(stats.qual.updated & IW_QUAL_DBM) {
        //signal is measured in dBm and is valid for us to use
        BOOST_LOG_TRIVIAL(info) << "Signal level: " << (int)(stats.qual.level - 256) << " dBm";
    }

    //SIOCGIWESSID for ssid
    char buffer[32];
    std::fill(std::begin(buffer), std::end(buffer), 0x00);
    req.u.essid.pointer = buffer;
    req.u.essid.length = sizeof(buffer);
    //this will gather the SSID of the connected network
    if(ioctl(m_sockfd, SIOCGIWESSID, &req) == -1){
        //die with error, invalid interface
        BOOST_LOG_TRIVIAL(warning) << "Invalid interface (2)";
    }
    else{
        std::string ssid { buffer, req.u.essid.length };
        BOOST_LOG_TRIVIAL(info) << "SSID: " << ssid;
    }

    //SIOCGIWRATE for bits/sec (convert to mbit)
    //this will get the bitrate of the link
    if(ioctl(m_sockfd, SIOCGIWRATE, &req) == -1){
        BOOST_LOG_TRIVIAL(warning) << "Bitrate fail";
    }else{
        //BOOST_LOG_TRIVIAL(info) << "Bitrate " << (int)req.u.bitrate;
        //bitrate=bitrate/1000000;
    }
#endif

}

std::string Network::calculateMAC() const {
#if 0
    //SIOCGIFHWADDR for mac addr
    ifreq req2;
    strcpy(req2.ifr_name, IFNAME);
    //this will get the mac address of the interface
    if(ioctl(m_sockfd, SIOCGIFHWADDR, &req2) == -1){
        BOOST_LOG_TRIVIAL(warning) << "mac error";
        return "";
    }
    else{
        std::string mac = (boost::format("%+02x:%+02x:%+02x:%+02x:%+02x:%+02x")
            % (uint)(uint8_t)req2.ifr_hwaddr.sa_data[0]
            % (uint)(uint8_t)req2.ifr_hwaddr.sa_data[1]
            % (uint)(uint8_t)req2.ifr_hwaddr.sa_data[2]
            % (uint)(uint8_t)req2.ifr_hwaddr.sa_data[3]
            % (uint)(uint8_t)req2.ifr_hwaddr.sa_data[4]
            % (uint)(uint8_t)req2.ifr_hwaddr.sa_data[5]
        ).str()
            ;
        BOOST_LOG_TRIVIAL(info) << "MAC: " << mac;
        return mac;
    }
    #endif
    return "";
}


}

