#include "wifi.h"

#include <sys/ioctl.h>
#include <algorithm>
#include <linux/wireless.h>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp> 

using namespace std;
namespace Robot::System {

static constexpr auto IFNAME { "wlan0" };


WiFi::WiFi(const std::shared_ptr<Robot::Context> &context) :
    m_initialized { false },
    m_sockfd { -1 }
{
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
}


WiFi::~WiFi() 
{
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
    cleanup();
}


void WiFi::init()
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
    m_initialized = true;

    m_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_sockfd<0) {
        BOOST_THROW_EXCEPTION(runtime_error("Error initializing WiFi"));
    }

    m_mac = calculateMAC();
    update();
}

void WiFi::cleanup()
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
    if (!m_initialized)
        return;
    m_initialized = false;

    close(m_sockfd);
    m_sockfd = -1;
}


void WiFi::update()
{
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
        string ssid { buffer, req.u.essid.length };
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


}

string WiFi::calculateMAC() const {
    //SIOCGIFHWADDR for mac addr
    ifreq req2;
    strcpy(req2.ifr_name, IFNAME);
    //this will get the mac address of the interface
    if(ioctl(m_sockfd, SIOCGIFHWADDR, &req2) == -1){
        BOOST_LOG_TRIVIAL(warning) << "mac error";
        return "";
    }
    else{
        string mac = (boost::format("%+02x:%+02x:%+02x:%+02x:%+02x:%+02x")
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
}


};
