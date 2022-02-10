#ifndef _ROBOT_PYTHON_UTIL_H_
#define _ROBOT_PYTHON_UTIL_H_

#include <string>
#include <unordered_set>
#include <boost/log/trivial.hpp> 

#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python.hpp>
#undef BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python/tuple.hpp>


namespace Robot::Python {

    std::string parse_python_exception();

    class DummyComponent { 
    };

    template<typename T>
    boost::python::tuple container_to_tuple(const T &container) {
        boost::python::tuple obj { boost::python::handle<>(PyTuple_New(container.size())) };
        int idx = 0;
        for (auto &v : container) {
            PyTuple_SET_ITEM(obj.ptr(), idx, boost::python::incref(boost::python::object(v).ptr()));
            idx++;
        }
        return obj;
    }

};


#endif
