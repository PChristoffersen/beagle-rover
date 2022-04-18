#include "network.h"

#include <net/if.h>
#include <netlink/netlink.h>    //lots of netlink functions
#include <netlink/genl/genl.h>  //genl_connect, genlmsg_put
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>  //genl_ctrl_resolve
#include <netlink/route/link.h>
#include <netlink/route/addr.h>
#include <netlink/utils.h>
#include <linux/if_arp.h>
#include <linux/if_addr.h>
#include <linux/if.h>
#include <linux/nl80211.h>      //NL80211 definitions

#include <chrono>
#include <algorithm>
#include <bitset>
#include <sstream>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp> 
#include <boost/algorithm/string/join.hpp>

using namespace std::literals;


namespace Robot::System {


static constexpr auto WIFI_SIGNAL_TIMER_INTERVAL { 10s };

static constexpr auto WIFI_SIGNAL_MIN { -100 };
static constexpr auto WIFI_SIGNAL_MAX { -50 };


void Network::delete_sock(struct nl_sock *sock) noexcept
{
    if (sock) {
        nl_close(sock);
        nl_socket_free(sock);
    }
}

void Network::delete_cache_mngr(struct nl_cache_mngr *cache_mngr) noexcept
{
    if (cache_mngr) {
        nl_cache_mngr_free(cache_mngr);
    }
}

void Network::delete_msg(struct nl_msg *msg) noexcept
{
    if (msg) {
        nlmsg_free(msg);
    }
}

void Network::delete_cb(struct nl_cb *cb) noexcept
{
    if (cb) {
        nl_cb_put(cb);
    }
}




static const char *channel_width_name(enum nl80211_chan_width width)
{
	switch (width) {
	case NL80211_CHAN_WIDTH_20_NOHT:
		return "20 MHz (no HT)";
	case NL80211_CHAN_WIDTH_20:
		return "20 MHz";
	case NL80211_CHAN_WIDTH_40:
		return "40 MHz";
	case NL80211_CHAN_WIDTH_80:
		return "80 MHz";
	case NL80211_CHAN_WIDTH_80P80:
		return "80+80 MHz";
	case NL80211_CHAN_WIDTH_160:
		return "160 MHz";
	case NL80211_CHAN_WIDTH_5:
		return "5 MHz";
	case NL80211_CHAN_WIDTH_10:
		return "10 MHz";
	default:
		return "unknown";
	}
}

uint8_t dbm_to_rssi(int8_t dbm) {
    return 2 * (std::clamp<int8_t>(dbm, WIFI_SIGNAL_MIN, WIFI_SIGNAL_MAX)+100);
}



uint ieee80211_frequency_to_channel(uint freq)
{
	/* see 802.11-2007 17.3.8.3.2 and Annex J */
	if (freq == 2484)
		return 14;
	/* see 802.11ax D6.1 27.3.23.2 and Annex E */
	else if (freq == 5935)
		return 2;
	else if (freq < 2484)
		return (freq - 2407) / 5;
	else if (freq >= 4910 && freq <= 4980)
		return (freq - 4000) / 5;
	else if (freq < 5950)
		return (freq - 5000) / 5;
	else if (freq <= 45000) /* DMG band lower limit */
		/* see 802.11ax D6.1 27.3.23.2 */
		return (freq - 5950) / 5;
	else if (freq >= 58320 && freq <= 70200)
		return (freq - 56160) / 2160;
	else
		return 0;
}




std::ostream &operator<<(std::ostream &os, const Network::Interface::Type &type)
{
    switch (type) {
        case Network::Interface::Type::ETHERNET:
            os << "ETHERNET";
            return os;
        case Network::Interface::Type::WIRELESS:
            os << "WIRELESS";
            return os;
        case Network::Interface::Type::WIRELESS_AP:
            os << "WIRELESS_AP";
            return os;
        case Network::Interface::Type::USB_GADGET:
            os << "USB_GADGET";
            return os;
    }
    os << "UNKNOWN";
    return os;
}




#if 0
static int pushif(std::vector<std::string> &vals, std::string name, int flags, int arg)
{
    if (flags & arg) {
        vals.push_back(name);
        flags &= ~arg;
    }
    return flags;
}
static std::string tostr(std::vector<std::string> &vals, uint flags)
{
    std::stringstream s;
    s << boost::algorithm::join(vals, ", ");
    if (flags) {
        s << ", " << std::bitset<32>(flags);
    }
    return s.str();
}
static std::string flags2str(short flags) 
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

    return tostr(vals, flags);
}
#endif



Network::Network(const std::shared_ptr<Robot::Context> &context) :
    WithStrand { context->io() },
    m_initialized { false },
    m_nl80211_id { -1 },
    m_socket { nullptr, &delete_sock },
    m_socket_descriptor { context->io() },
    m_cache_mngr { nullptr, &delete_cache_mngr },
    m_cache_descriptor { context->io() },
    m_timer { context->io() }
{
}

Network::~Network() 
{
    cleanup();
}



void Network::init()
{
    m_initialized = true;

    initSocket();
    initInterfaces();

    async_read(m_socket_descriptor, m_socket);

    m_timer.expires_after(0s);
    startTimer();
}


void Network::cleanup()
{
    if (!m_initialized)
        return;
    m_initialized = false;

    m_timer.cancel();

    m_cache_descriptor.cancel();
    m_cache_mngr = nullptr;
    m_socket_descriptor.cancel();
    m_socket = nullptr;
    m_nl80211_id = -1;

    m_interfaces.clear();
}



Network::interface_list_type Network::interfaces() const
{
    const guard _lock(m_mutex);
    interface_list_type res;
    for (const auto &entry: m_interfaces) {
        res.push_back(entry.second);
    }
    return res;
}


Network::interface_list_type Network::interfaces(Network::Interface::Type type) const
{
    const guard _lock(m_mutex);
    interface_list_type res;
    for (const auto &entry: m_interfaces) {
        if (entry.second->type==type) {
            res.push_back(entry.second);
        }
    }
    return res;
}


void Network::initSocket()
{
    m_socket.reset(nl_socket_alloc());
    if (!m_socket) {
        BOOST_THROW_EXCEPTION(std::system_error(errno, std::generic_category(), "Failed to allocate netlink genl socket"));
    }
    if (genl_connect(m_socket.get())) { 
        BOOST_THROW_EXCEPTION(std::system_error(errno, std::generic_category(), "Failed to connect netlink genl socket"));
    }
    m_nl80211_id = genl_ctrl_resolve(m_socket.get(), NL80211_GENL_NAME);
    if (m_nl80211_id < 0) {
        BOOST_LOG_TRIVIAL(warning) << "No wireless API found";
    }

    nl_socket_set_nonblocking(m_socket.get());
    m_socket_descriptor.assign(nl_socket_get_fd(m_socket.get()));

    nl_socket_modify_err_cb(m_socket.get(), NL_CB_CUSTOM, +[](struct sockaddr_nl *nla, struct nlmsgerr *err, void *arg) -> int {
        // Ignore nodev errors 
        if (err->error==-ENODEV) {
            return NL_SKIP;
        }
        char buffer[128];
        BOOST_LOG_TRIVIAL(warning) << "Netlink socket error " << err->error << ": " << strerror_r(-err->error, buffer, sizeof(buffer));
        return NL_SKIP;
    }, this);

    nl_socket_modify_cb(m_socket.get(), NL_CB_VALID, NL_CB_CUSTOM, +[](struct nl_msg *msg, void *arg) -> int {
        return static_cast<Network*>(arg)->socket_cb(msg);
    }, this);

}



void Network::initInterfaces() 
{
    int res;
    
    struct nl_cache_mngr *mngr = nullptr;
    res = nl_cache_mngr_alloc(nullptr, NETLINK_ROUTE, NL_AUTO_PROVIDE, &mngr);
    if (res<0 || mngr==nullptr) {
        BOOST_THROW_EXCEPTION(std::system_error(errno, std::generic_category(), "Failed to allocate netlink route cache manager"));
    }
    m_cache_mngr.reset(mngr);

    
    // Setup cache for interfaces
    struct nl_cache *cache = nullptr;
    res = nl_cache_mngr_add(mngr, "route/link", +[](struct nl_cache *cache, struct nl_object *obj, int action, void *arg){
        static_cast<Network*>(arg)->interface_cb(reinterpret_cast<struct rtnl_link*>(obj), action);
    }, this, &cache);
    if (res<0) {
        BOOST_THROW_EXCEPTION(std::system_error(errno, std::generic_category(), "Failed to add to cache mngr"));
    }
    nl_cache_foreach(cache, +[](struct nl_object *obj, void *arg) {
        static_cast<Network*>(arg)->interface_cb(reinterpret_cast<struct rtnl_link*>(obj), NL_ACT_NEW);
    }, this);


    // Setup cache for addresses
    cache = nullptr;
    res = nl_cache_mngr_add(mngr, "route/addr", +[](struct nl_cache *cache, struct nl_object *obj, int action, void *arg){
        static_cast<Network*>(arg)->interface_addr_cb(reinterpret_cast<struct rtnl_addr*>(obj), action);
    }, this, &cache);
    if (res<0) {
        BOOST_THROW_EXCEPTION(std::system_error(errno, std::generic_category(), "Failed to add to cache mngr"));
    }
    nl_cache_foreach(cache, +[](struct nl_object *obj, void *arg) {
        static_cast<Network*>(arg)->interface_addr_cb(reinterpret_cast<struct rtnl_addr*>(obj), NL_ACT_NEW);
    }, this);

    m_cache_descriptor.assign(nl_cache_mngr_get_fd(mngr));
    async_read(m_cache_descriptor, mngr);
}




void Network::interface_cb(struct rtnl_link *link, int action)
{
    // Skip interfaces we are not interested in.
     
     // We only want ethernet
    if (rtnl_link_get_arptype(link)!=ARPHRD_ETHER)
        return;
    // We don't want bridge interfaces
    if (rtnl_link_get_family(link)==PF_BRIDGE) 
        return;
    // If type is set it is probably some special interface
    if (rtnl_link_get_type(link)!=nullptr)
        return;
    // Skip interfaces with flags we don't want
    auto flags = rtnl_link_get_flags(link); 
    if (flags & (IFF_NOARP|IFF_MASTER|IFF_SLAVE|IFF_POINTOPOINT|IFF_LOOPBACK)) 
        return;

    auto index = rtnl_link_get_ifindex(link);

    if (action==NL_ACT_NEW) {
        BOOST_LOG_TRIVIAL(info) << "New interface " << index << "  " << rtnl_link_get_name(link);
        
        char buffer[128];
        auto name = rtnl_link_get_name(link);

        auto iface = std::make_shared<Interface>();
        iface->index = index;
        iface->name = name;
        iface->mac = nl_addr2str(rtnl_link_get_addr(link), buffer, sizeof(buffer));
        iface->active = (flags & IFF_RUNNING) && (flags & IFF_UP);
        iface->type = Interface::Type::ETHERNET;

        if (false && iface->name.rfind("usb",0)==0) {
            // Assume interfaces starting with usb is usb gadget devices.
            iface->type = Interface::Type::USB_GADGET;
        }
        else if (m_nl80211_id>=0) {
            // Check for wireless extension
            defer([this, index]() {
                nl80211_get_interface(index);
            });
        }

        {
            const guard _lock(m_mutex);
            m_interfaces[index] = iface;
        }
        notify(NOTIFY_DEFAULT);
    }
    else if (action==NL_ACT_CHANGE) {
        BOOST_LOG_TRIVIAL(info) << "Change interface " << index << "  " << rtnl_link_get_name(link);
        auto itr = m_interfaces.find(index);
        if (itr==m_interfaces.end()) {
            return;
        }
        auto iface = itr->second;

        bool changed = false;

        const Interface::guard _lock(iface->mutex());
        bool active = (flags & IFF_RUNNING) && (flags & IFF_UP);
        changed |= (iface->active!=active);
        iface->active = active;

        // Update wireless information for nl80211 interfaces
        if (iface->wifi) {
            defer([this, index]() {
                nl80211_get_interface(index);
            });
        }

        if (changed) {
            notify(NOTIFY_DEFAULT);
        }
    }
    else if (action==NL_ACT_DEL) {
        BOOST_LOG_TRIVIAL(info) << "Delete interface " << index << "  " << rtnl_link_get_name(link);
        {
            const guard _lock(m_mutex);
            m_interfaces.erase(index);
        }
        notify(NOTIFY_DEFAULT);
    }


    //BOOST_LOG_TRIVIAL(info) << action_to_str(action) << " IFace " << index << "  " << name;
    //BOOST_LOG_TRIVIAL(info) << "    Flags: " << flags2str(flags);
    //BOOST_LOG_TRIVIAL(info) << "     addr: " << nl_addr2str(hwaddr, buffer, sizeof(buffer));
    //BOOST_LOG_TRIVIAL(info) << " linkmode: " << (uint)rtnl_link_get_linkmode(link);
    //BOOST_LOG_TRIVIAL(info) << "     type: " << (uint32_t)rtnl_link_get_type(link);
    //BOOST_LOG_TRIVIAL(info) << "     Oper: " << oper2str(rtnl_link_get_operstate(link));
    //BOOST_LOG_TRIVIAL(info) << "       HW: " << rtnl_link_get_arptype(link) << "  " << nl_llproto2str(rtnl_link_get_arptype(link), buffer, sizeof(buffer));

}


void Network::interface_addr_cb(struct rtnl_addr *addr, int action)
{
    auto family = rtnl_addr_get_family(addr);
    if (family!=AF_INET)
        return;

    auto index = rtnl_addr_get_ifindex(addr);
    auto itr = m_interfaces.find(index);
    if (itr==m_interfaces.end()) {
        return;
    }
    auto iface = itr->second;

    auto local = rtnl_addr_get_local(addr);
    if (!local) 
        return;

    const Interface::guard _lock(iface->mutex());
    char buffer[128];

    if (action==NL_ACT_NEW) {
        std::string addr = nl_addr2str(local, buffer, sizeof(buffer));
        BOOST_LOG_TRIVIAL(trace) << "New addr " << index << " " << addr;
        iface->addresses.insert(addr);
        notify(NOTIFY_DEFAULT);
    }
    else if (action==NL_ACT_DEL) {
        std::string addr = nl_addr2str(local, buffer, sizeof(buffer));
        BOOST_LOG_TRIVIAL(trace) << "Del addr " << index << " " << addr;
        iface->addresses.erase(addr);
        notify(NOTIFY_DEFAULT);
    }
    else {
        return;
    }
}


void Network::nl80211_get_interface(Interface::index_type index)
{
    msg_type msg = { nlmsg_alloc(), &delete_msg };
    genlmsg_put(msg.get(), NL_AUTO_PORT, NL_AUTO_SEQ, m_nl80211_id, 0, 0, NL80211_CMD_GET_INTERFACE, 0);
    nla_put_u32(msg.get(), NL80211_ATTR_IFINDEX, index);

    int ret = nl_send_auto(m_socket.get(), msg.get());
    if (ret<0) {
        BOOST_LOG_TRIVIAL(warning) << "Netlink error sending get interface message";
    }
}


void Network::nl80211_get_station(Interface::index_type index)
{
    msg_type msg = { nlmsg_alloc(), &delete_msg };
    genlmsg_put(msg.get(), NL_AUTO_PORT, NL_AUTO_SEQ, m_nl80211_id, 0, NLM_F_DUMP, NL80211_CMD_GET_STATION, 0);
    nla_put_u32(msg.get(), NL80211_ATTR_IFINDEX, index);

    int ret = nl_send_auto(m_socket.get(), msg.get());
    if (ret<0) {
        BOOST_LOG_TRIVIAL(warning) << "Netlink error sending get station message";
    }
}


void Network::nl80211_get_interface_cb(struct genlmsghdr *gnlh)
{
    struct nlattr *tb_msg[NL80211_ATTR_MAX + 1];
    nla_parse(tb_msg, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0), genlmsg_attrlen(gnlh, 0), NULL);

    auto itr = m_interfaces.find(nla_get_u32(tb_msg[NL80211_ATTR_IFINDEX]));
    if (itr==m_interfaces.end())
        return;

    auto &iface = itr->second;
    const Interface::guard _lock(iface->mutex());
    //BOOST_LOG_TRIVIAL(info) << "nl8021_get " << iface->name;

    if (tb_msg[NL80211_ATTR_IFTYPE]) {
        switch (nla_get_u32(tb_msg[NL80211_ATTR_IFTYPE])) {
            case NL80211_IFTYPE_AP:
            case NL80211_IFTYPE_AP_VLAN:
                iface->type = Interface::Type::WIRELESS_AP;
                break;
            default:
                iface->type = Interface::Type::WIRELESS;
                break;
        }
        //BOOST_LOG_TRIVIAL(info) << "Type: " << nla_get_u32(tb_msg[NL80211_ATTR_IFTYPE]) << "  " << tb_msg[NL80211_ATTR_IFTYPE]->nla_len;
    }

    if (!iface->wifi) {
        iface->wifi = std::make_unique<Interface::WifiInfo>();
        if (iface->type==Interface::Type::WIRELESS) {
            defer([this, index=iface->index]{
                nl80211_get_station(index);
            });
        }
        else {
            iface->wifi->signal_dbm = WIFI_SIGNAL_MAX;
            iface->wifi->signal_rssi = 100;
        }
    }
    auto &wifi = iface->wifi;
    bool changed = false;

    if (tb_msg[NL80211_ATTR_SSID]) {
        auto ssid = nla_get_string(tb_msg[NL80211_ATTR_SSID]);
        if (wifi->ssid!=ssid) {
            wifi->ssid = ssid;
            changed |= true;
            BOOST_LOG_TRIVIAL(trace) << "     SSID: " << wifi->ssid;
        }
    }
    else {
        if (!wifi->ssid.empty()) {
            wifi->ssid = "";
            changed |= true;
        }
    }

    if (tb_msg[NL80211_ATTR_WIPHY_FREQ]) {
        auto freq = nla_get_u32(tb_msg[NL80211_ATTR_WIPHY_FREQ]);
        nl80211_chan_width width = static_cast<nl80211_chan_width>(nla_get_u32(tb_msg[NL80211_ATTR_CHANNEL_WIDTH]));
        BOOST_LOG_TRIVIAL(trace) << "   Freq: " << freq;
        BOOST_LOG_TRIVIAL(trace) << "  Width: " << width;
        changed |= (freq!=wifi->frequency);
        wifi->channel_width = channel_width_name(width);
        wifi->channel = ieee80211_frequency_to_channel(freq);
        wifi->frequency = freq;
    }
    else {
        changed |= (wifi->frequency!=0);
        wifi->channel_width = "";
        wifi->channel = 0;
        wifi->frequency = 0;
    }

    if (changed) {
        notify(NOTIFY_DEFAULT);
    }
}


void Network::nl80211_get_station_cb(struct genlmsghdr *gnlh)
{
    struct nlattr *tb_msg[NL80211_ATTR_MAX + 1];
    struct nlattr *sinfo[NL80211_STA_INFO_MAX + 1];
    static struct nla_policy stats_policy[NL80211_STA_INFO_MAX + 1] = {
        { NLA_UNSPEC, 0, 0 }, //__NL80211_STA_INFO_INVALID
        { NLA_U32, 0, 0 }, //NL80211_STA_INFO_INACTIVE_TIME,
        { NLA_U32, 0, 0 }, //NL80211_STA_INFO_RX_BYTES,
        { NLA_U32, 0, 0 }, //NL80211_STA_INFO_TX_BYTES,
        { NLA_U16, 0, 0 }, //NL80211_STA_INFO_LLID,
        { NLA_U16, 0, 0 }, //NL80211_STA_INFO_PLID,
        { NLA_U8, 0, 0 }, //NL80211_STA_INFO_PLINK_STATE,
        { NLA_U8, 0, 0 }, //NL80211_STA_INFO_SIGNAL,
        { NLA_NESTED, 0, 0 }, //NL80211_STA_INFO_TX_BITRATE,
        { NLA_U32, 0, 0 }, //NL80211_STA_INFO_RX_PACKETS,
        { NLA_U32, 0, 0 }, //NL80211_STA_INFO_TX_PACKETS,
        { NLA_UNSPEC, 0, 0 }, //NL80211_STA_INFO_TX_RETRIES,
        { NLA_UNSPEC, 0, 0 }, //NL80211_STA_INFO_TX_FAILED,
        { NLA_UNSPEC, 0, 0 }, //NL80211_STA_INFO_SIGNAL_AVG,
        { NLA_NESTED, 0, 0 }, //NL80211_STA_INFO_RX_BITRATE,
        { NLA_UNSPEC, 0, 0 }, //NL80211_STA_INFO_BSS_PARAM,
        { NLA_UNSPEC, 0, 0 }, //NL80211_STA_INFO_CONNECTED_TIME,
        { NLA_UNSPEC, 0, 0 }, //NL80211_STA_INFO_STA_FLAGS,
    };
    nla_parse(tb_msg, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0), genlmsg_attrlen(gnlh, 0), NULL);
    if (!tb_msg[NL80211_ATTR_STA_INFO]) 
        return;

    auto itr = m_interfaces.find(nla_get_u32(tb_msg[NL80211_ATTR_IFINDEX]));
    if (itr==m_interfaces.end())
        return;
    auto &iface = itr->second;
    if (!iface->wifi)
        return;
    auto &wifi = iface->wifi;

    //BOOST_LOG_TRIVIAL(info) << "Station info " << iface->name;
    if (nla_parse_nested(sinfo, NL80211_STA_INFO_MAX, tb_msg[NL80211_ATTR_STA_INFO], stats_policy)) {
        BOOST_LOG_TRIVIAL(info) << "Error parsing station info";
        return;
    }

    const Interface::guard _lock(iface->mutex());
    bool changed = false;

    if (sinfo[NL80211_STA_INFO_SIGNAL]) {
        auto signal = static_cast<int8_t>(nla_get_u8(sinfo[NL80211_STA_INFO_SIGNAL]));
        if (signal!=wifi->signal_dbm) {
            wifi->signal_dbm = signal;
            wifi->signal_rssi = dbm_to_rssi(signal);
            changed = true;
        }
    }
    else {
        if (wifi->signal_dbm!=WIFI_SIGNAL_MIN) {
            wifi->signal_dbm = WIFI_SIGNAL_MIN;
            wifi->signal_rssi = 0;
            changed = true;
        }
    }

    if (changed) {
        notify(NOTIFY_SIGNAL);
    }
}


int Network::socket_cb(struct nl_msg *msg)
{
    auto hdr = nlmsg_hdr(msg);
    if (nlmsg_get_proto(msg)==NETLINK_GENERIC && hdr->nlmsg_type==m_nl80211_id) {
        auto gnlh = genlmsg_hdr(hdr);
        auto cmd = gnlh->cmd;

        if (cmd==NL80211_CMD_NEW_INTERFACE || cmd==NL80211_CMD_GET_INTERFACE) {
            nl80211_get_interface_cb(gnlh);
        }
        else if (cmd==NL80211_CMD_NEW_STATION || cmd==NL80211_CMD_GET_STATION) {
            nl80211_get_station_cb(gnlh);
        }
        else {
            BOOST_LOG_TRIVIAL(info) << "Netlink unknown command " << static_cast<uint32_t>(cmd);
        }
    }
    else {
        BOOST_LOG_TRIVIAL(warning) << "Netlink unknown message " << hdr->nlmsg_type;
    }
    
    return NL_OK;
}



void Network::async_read(boost::asio::posix::stream_descriptor &desc, struct nl_cache_mngr *mngr)
{
    desc.async_wait(boost::asio::posix::descriptor::wait_read, boost::asio::bind_executor(m_strand, 
        [this,&desc,mngr](boost::system::error_code error) {
            if (error!=boost::system::errc::success) {
                return;
            }
            if (nl_cache_mngr_data_ready(mngr)<0) {
                BOOST_LOG_TRIVIAL(error) << "Error updating netlink cache mngr";
                return;
            }
            async_read(desc, mngr);
    }));
}


void Network::async_read(boost::asio::posix::stream_descriptor &desc, sock_type &socket)
{
    desc.async_wait(boost::asio::posix::descriptor::wait_read, boost::asio::bind_executor(m_strand, 
        [this,&desc,&socket](boost::system::error_code error) {
            if (error!=boost::system::errc::success) {
                return;
            }
            if (nl_recvmsgs_default(socket.get())<0) {
                BOOST_LOG_TRIVIAL(error) << "Error receiving messages";
                return;
            }
            async_read(desc, socket);
    }));
}



void Network::startTimer()
{
    m_timer.expires_at(m_timer.expiry() + WIFI_SIGNAL_TIMER_INTERVAL);
    m_timer.async_wait(boost::asio::bind_executor(m_strand, 
        [this] (boost::system::error_code error) {
            if (error!=boost::system::errc::success) {
                return;
            }

            for (auto &entry : m_interfaces) {
                auto &iface = entry.second;
                if (iface->type==Interface::Type::WIRELESS) {
                    //BOOST_LOG_TRIVIAL(info) << "Polling wireless signal status";
                    nl80211_get_station(iface->index);
                }
            }

            startTimer();
        }
    ));
}



void Network::dump() const
{
    for (auto &entry : m_interfaces) {
        auto &iface = entry.second;

        BOOST_LOG_TRIVIAL(info) << "Iface " << iface->index << " " << iface->type << " " << iface->name << "  : " << iface->mac;
        BOOST_LOG_TRIVIAL(info) << " active: " << iface->active;
        for (auto addr : iface->addresses) {
            BOOST_LOG_TRIVIAL(info) << "   addr: " << addr;
        }
        if (iface->wifi) {
            BOOST_LOG_TRIVIAL(info) << "   ssid: " << iface->wifi->ssid;
            BOOST_LOG_TRIVIAL(info) << "  width: " << iface->wifi->channel_width;
            BOOST_LOG_TRIVIAL(info) << "channel: " << iface->wifi->channel << "  " << iface->wifi->frequency;
            BOOST_LOG_TRIVIAL(info) << " signal: " << (int)iface->wifi->signal_dbm << "  " << (int)iface->wifi->signal_rssi;
        }
    }
}


}

