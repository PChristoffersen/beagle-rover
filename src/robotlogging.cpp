#include "robotlogging.h"

#include <mutex>
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>

namespace Robot::Logging {


static std::once_flag logging_flag;


void initLogging(boost::log::trivial::severity_level level)
{
    std::call_once(logging_flag, [level]() {
        namespace logging = boost::log;
        namespace keywords = boost::log::keywords;
        namespace expr = boost::log::expressions;
    
        logging::add_common_attributes();

        auto sink = logging::add_console_log();
        auto formatter =
                expr::stream
                << "["
                << expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%H:%M:%S.%f")
                << "] ["
                << expr::attr<boost::log::attributes::current_thread_id::value_type>("ThreadID") 
                << "] ["
                << boost::log::trivial::severity
                << "]  "
                << expr::message;

        sink->set_formatter(formatter);
        sink->locked_backend()->auto_flush(true);

        logging::core::get()->set_filter(
            logging::trivial::severity >= level
        );


        BOOST_LOG_TRIVIAL(info) << "Logging initialized";
    });
}

void skipLogging() 
{
    std::call_once(logging_flag, [](){ });
}


}