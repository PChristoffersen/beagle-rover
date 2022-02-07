#ifndef _PYTHON_UTIL_H_
#define _PYTHON_UTIL_H_

#include <string>
#include <memory>
#include <boost/log/trivial.hpp> 
#include <boost/signals2.hpp>

#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python.hpp>
#undef BOOST_ALLOW_DEPRECATED_HEADERS

namespace Robot::Python {

    std::string parse_python_exception();


    class NotifySubscription : public std::enable_shared_from_this<NotifySubscription> {
        public:
            NotifySubscription(boost::signals2::connection &&connection) :
                m_connection { connection }
            {
                BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
            }

            ~NotifySubscription()
            {
                BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
                m_connection.disconnect();
            }

            void unsubscribe()
            {
                BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
                m_connection.disconnect();
            }

            bool subscribed() 
            {
                return m_connection.connected();
            }

        private:
            boost::signals2::connection m_connection;
    };


    template<typename T>
    std::shared_ptr<NotifySubscription> notify_subscribe(T &obj, boost::python::object &func)
    {
        return std::make_shared<NotifySubscription>(obj.subscribe([func](typename T::NotifyType arg) {
            namespace py = boost::python;

            PyGILState_STATE gstate = PyGILState_Ensure();
            try {
                func(arg);
            }
            catch (const py::error_already_set &err) {
                BOOST_LOG_TRIVIAL(warning) << "Notify error: " << parse_python_exception();
            }
            PyGILState_Release(gstate);
        }));
    }


    void set_python_thread_name(const char *name);

};


#endif
