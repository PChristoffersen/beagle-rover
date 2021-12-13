#include <string>
#include <vector>
#include <boost/log/trivial.hpp>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "util.h"

namespace py = boost::python;

using FloatVector = std::vector<float>;
using IntVector = std::vector<int>;
using StringVector = std::vector<std::string>;


void python_export_std() 
{
    iterable_converter()
        .from_python<FloatVector>();
    py::class_<FloatVector>("FloatVector")
        .def(py::vector_indexing_suite<FloatVector, true>() )
        ;
    iterable_converter()
        .from_python<IntVector>();
    py::class_<IntVector>("IntVector")
        .def(py::vector_indexing_suite<IntVector, true>() )
        ;
    iterable_converter()
        .from_python<StringVector>();
    py::class_<StringVector>("StringVector")
        .def(py::vector_indexing_suite<StringVector, true>() )
        ;

}