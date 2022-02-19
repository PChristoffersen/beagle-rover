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

    /**
     * @brief Extracts the text from the current raised python exception
     * 
     * @return std::string Error string
     */
    std::string parse_python_exception();

    
    /**
     * @brief Converts a c++ container to a python tuple
     * 
     * @tparam T Container type
     * @param container Any container that can be iterated through range-based for loop
     * @return boost::python::tuple Python tuple
     */
    template<typename T>
    boost::python::tuple container_to_tuple(const T &container) 
    {
        boost::python::tuple obj { boost::python::handle<>(PyTuple_New(container.size())) };
        int idx = 0;
        for (auto &v : container) {
            PyTuple_SET_ITEM(obj.ptr(), idx, boost::python::incref(boost::python::object(v).ptr()));
            idx++;
        }
        return obj;
    }


    template<typename T>
    T tuple_to_container(const boost::python::tuple &t) 
    {
        T res;      
        auto len = boost::python::len(t);
        for (boost::python::ssize_t i = 0; i<len; i++) {
            boost::python::object item { boost::python::handle(boost::python::borrowed(PyTuple_GetItem(t.ptr(), i))) };
            res.insert(boost::python::extract<typename T::value_type>(item));
        }
        return res;
    }


}


#endif
