#include "telemetry.h"

#include <typeinfo>
#include <boost/log/trivial.hpp>

#include <robotconfig.h>
#include <robotcontext.h>
#include "types.h"
#include "telemetrysource.h"


using namespace std::literals;

namespace Robot::Telemetry {

static constexpr auto TELEMETRY_HISTORY_INTERVAL { 50ms };


Telemetry::Telemetry(const std::shared_ptr<Robot::Context> &context) :
    WithStrand { context->io() },
    m_initialized { false },
    m_timer { context->io() }
{

}


Telemetry::~Telemetry() 
{
    cleanup();
}


void Telemetry::init() 
{
    const guard lock(m_mutex);

    for (auto &source : m_sources) {
        source->init(shared_from_this());
    }
    m_initialized = true;

    // Init IMU map
    m_imu_event.update(m_imu_values);

    {
        EventOdometer ev;
        ev.update(m_odometer_values);
    }

    resetHistory();

    m_timer.expires_after(TELEMETRY_HISTORY_INTERVAL);
    m_base_history = m_timer.expiry();
    timerSetup();
}


void Telemetry::cleanup() 
{
    const guard lock(m_mutex);

    if (!m_initialized)
        return;
    m_initialized = false;
    
    m_timer.cancel();

    for (auto &source : m_sources) {
        source->cleanup();
    }
}


void Telemetry::resetHistory()
{
    m_history_imu.values[0].fill(0.0f);
    m_history_imu.values.fill(m_history_imu.values[0]);
    m_history_imu.head = 0;

    m_history_motor_duty.values[0].fill(0.0f);
    m_history_motor_duty.values.fill(m_history_motor_duty.values[0]);
    m_history_motor_duty.head = 0;

    m_history_motor_rpm.values[0].fill(0.0f);
    m_history_motor_rpm.values.fill(m_history_motor_rpm.values[0]);
    m_history_motor_rpm.head = 0;
}


void Telemetry::addSource(std::shared_ptr<class Source> source)
{
    const guard lock(m_mutex);
    m_sources.push_back(source);
}


void Telemetry::process(const Event &event) 
{
    if (const auto ev = dynamic_cast<const Robot::Telemetry::EventIMU*>(&event)) {
        dispatch([this, evt=*ev]{
            evt.update(m_imu_values);
            m_imu_event = evt;
            notify(NOTIFY_IMU);
        });
    }
    else if (const auto ev = dynamic_cast<const Robot::Telemetry::EventOdometer*>(&event)) {
        dispatch([this, evt=*ev]{
            evt.update(m_odometer_values);
            notify(NOTIFY_ODOMETER);
        });
    }
    else if (const auto ev = dynamic_cast<const Robot::Telemetry::EventMotors*>(&event)) {
        dispatch([this, evt=*ev]{
            m_motors_event = evt;
        });
    }

    sig_event(event);
}

#if ROBOT_HAVE_IMU
void Telemetry::process(const IMUData &data)
{
    sig_imu(data);
}
#endif


std::int64_t Telemetry::historyLastMS() const
{ 
    //BOOST_LOG_TRIVIAL(info) << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(m_last_history-m_base_history).count();
    return std::chrono::duration_cast<std::chrono::milliseconds>(m_last_history-m_base_history).count(); 
}

std::int64_t Telemetry::historyIntervalMS() const
{
    //BOOST_LOG_TRIVIAL(info) << "Interval: " << std::chrono::duration_cast<std::chrono::milliseconds>(TELEMETRY_HISTORY_INTERVAL).count();
    return std::chrono::duration_cast<std::chrono::milliseconds>(TELEMETRY_HISTORY_INTERVAL).count();
}


void Telemetry::timerSetup() 
{
    m_timer.expires_at(m_timer.expiry() + TELEMETRY_HISTORY_INTERVAL);
    m_timer.async_wait(boost::asio::bind_executor(m_strand, 
        [this] (boost::system::error_code error) {
            if (error!=boost::system::errc::success) {
                return;
            }

            // TODO Find a better way of handling telemetry history

            #if ROBOT_HAVE_IMU
            { // IMU
                auto &current = m_history_imu.next();
                current[0] = m_imu_event.pitch;
                current[1] = m_imu_event.roll;
                current[2] = m_imu_event.yaw;
            }
            #else
            { // Dummy IMU
                static int cnt = 0;
                cnt++;
                auto &current = m_history_imu.next();
                current[0] = (cnt%180-90)*M_PI/180.0;
                current[1] = (((cnt/2)%180)-90) *M_PI/180.0;
                current[2] = (((cnt/3)%180) -90  )*M_PI/180.0;
            }
            #endif

            { // Motors
                m_history_motor_duty.next() = m_motors_event.duty;
                
                auto &rpm { m_history_motor_rpm.next() };
                for (auto i=0u; i<m_motors_event.rpm.size(); i++) {
                    rpm[i*2] = m_motors_event.rpm[i];
                    rpm[i*2+1] = m_motors_event.rpm_target[i];
                }
            }

            m_last_history = m_timer.expiry();
            timerSetup();
        }
    ));
}


}
