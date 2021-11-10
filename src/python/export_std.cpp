#include <string>
#include <vector>
#include <boost/log/trivial.hpp>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "util.h"

using namespace boost;
using namespace boost::python;

using FloatVector = std::vector<float>;
using IntVector = std::vector<int>;
using StringVector = std::vector<std::string>;


void python_export_std() 
{
    iterable_converter()
        .from_python<FloatVector>();
    class_<FloatVector>("FloatVector")
        .def(vector_indexing_suite<FloatVector, true>() )
        ;
    iterable_converter()
        .from_python<IntVector>();
    class_<IntVector>("IntVector")
        .def(vector_indexing_suite<IntVector, true>() )
        ;
    iterable_converter()
        .from_python<StringVector>();
    class_<StringVector>("StringVector")
        .def(vector_indexing_suite<StringVector, true>() )
        ;

}