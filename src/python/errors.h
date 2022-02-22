#ifndef _ROBOT_PYTHON_ERRORS_H_
#define _ROBOT_PYTHON_ERRORS_H_

#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python.hpp>
#undef BOOST_ALLOW_DEPRECATED_HEADERS


namespace Robot::Python {

    class attribute_error : public std::logic_error {
        public:
            using std::logic_error::logic_error;
    };


}

#endif
