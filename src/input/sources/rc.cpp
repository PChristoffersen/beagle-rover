#include "rc.h"

#include <boost/log/trivial.hpp>

#include "../../rc/rcreceiver.h"


using namespace std;

namespace Robot::Input {


static constexpr size_t CHANNEL_THROTTLE { 0 };
static constexpr size_t CHANNEL_RUDDER { 1 };
static constexpr size_t CHANNEL_ELEVATOR { 2 };
static constexpr size_t CHANNEL_AILERON { 3 };


RCSource::RCSource(const Signals &signals) :
    AbstractSource { signals },
    m_initialized { false },
    m_enabled { false }
{

}


RCSource::~RCSource()
{
    cleanup();
}



void RCSource::init(const std::shared_ptr<::Robot::RC::Receiver> &receiver)
{
    const guard lock(m_mutex);
    m_initialized = true;
    m_receiver = receiver;

}


void RCSource::cleanup()
{
    const guard lock(m_mutex);
    if (!m_initialized) 
        return;
    m_initialized = false;
    m_enabled = false;
    m_connection.disconnect();
}


void RCSource::setEnabled(bool enabled)
{
    const guard lock(m_mutex);
    if (enabled==m_enabled) 
        return;
    m_enabled = enabled;

    if (m_enabled) {
        if (auto receiver = m_receiver.lock()) {
            m_connection = receiver->sigData.connect(::Robot::RC::SignalData::slot_type(&RCSource::onRCData, this, _1, _2, _3));
        }
    }
    else {
        m_connection.disconnect();
    }
}



void RCSource::onRCData(::Robot::RC::Flags flags, ::Robot::RC::RSSI rssi, const ::Robot::RC::ChannelList &channels)
{
    const guard lock(m_mutex);
    if (!m_initialized) 
        return;

    double throttle = 0.0;
    double steering = 0.0;

    if (flags.frameLost()) {

    }
    else {
        const auto &vthrotle = channels[CHANNEL_THROTTLE];
        const auto &vrudder = channels[CHANNEL_RUDDER];
        const auto &velevator = channels[CHANNEL_ELEVATOR];
        const auto &vaileron = channels[CHANNEL_AILERON];

        throttle = vthrotle.asPercent();
        steering = vaileron.asPercent();
    }

    #if 1
    static chrono::high_resolution_clock::time_point last_update;
    auto time { chrono::high_resolution_clock::now() };
    if ((time-last_update) > 100ms) {
        BOOST_LOG_TRIVIAL(info) << "Throttle: " << throttle << "  Steer: " << steering;
        m_signals.steer(steering, throttle, 0.0, 0.0);
        last_update = time;
    }
    #else
    m_signals.steer(steering, throttle, 0.0, 0.0);
    #endif
}


};