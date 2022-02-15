#ifndef _ROBOT_ROBOT_LOGGING_H_
#define _ROBOT_ROBOT_LOGGING_H_

#include <boost/log/trivial.hpp>

namespace Robot::Logging {

    void initLogging(boost::log::trivial::severity_level level = boost::log::trivial::debug);

    void skipLogging();

    class LogInit {
        public:
            LogInit(boost::log::trivial::severity_level level) 
            {
                initLogging(level);
            }
            LogInit() 
            {
                skipLogging();
            }
    };

}

#endif
