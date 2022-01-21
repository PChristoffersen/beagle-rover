#include "rc.h"

#include <boost/log/trivial.hpp>

#include <rc/receiver.h>

#if ROBOT_HAVE_RC

namespace Robot::Input {

enum Channel : size_t {
    THROTTLE,
    AILERON,
    ELEVATOR,
    RUDDER,
};


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
            m_connection = receiver->sigData.connect(::Robot::RC::SignalData::slot_type(&RCSource::onRCData, this, boost::placeholders::_1, boost::placeholders::_2, boost::placeholders::_3));
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

    if (flags.frameLost()) {
        m_signals.steer(0.0, 0.0, 0.0, 0.0);
        return;
    }


    const auto &vthrotle = channels[Channel::THROTTLE];
    const auto &vaileron = channels[Channel::AILERON];
    const auto &vrudder = channels[Channel::RUDDER];
    const auto &velevator = channels[Channel::ELEVATOR];

    auto throttle = vthrotle.asPercent();
    auto steering = vaileron.asPercent();
    auto aux_x = vrudder.asPercent();
    auto aux_y = velevator.asPercent();

    #if 0
    static chrono::high_resolution_clock::time_point last_update;
    auto time { chrono::high_resolution_clock::now() };
    if ((time-last_update) > 100ms) {
        BOOST_LOG_TRIVIAL(info) << "Throttle: " << throttle << "  Steer: " << steering;
        m_signals.steer(steering, throttle, 0.0, 0.0);
        last_update = time;
    }
    #else
    m_signals.steer(steering, throttle, aux_x, aux_y);
    #endif
}


};

#endif
