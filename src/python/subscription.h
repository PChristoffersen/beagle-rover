#ifndef _ROBOT_PYTHON_SUBSCRIPTION_H_
#define _ROBOT_PYTHON_SUBSCRIPTION_H_

#include <memory>
#include <vector>
#include <unordered_set>
#include <boost/log/trivial.hpp> 
#include <boost/signals2.hpp>
#include <boost/lockfree/queue.hpp>
#include <sys/eventfd.h>

#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python.hpp>
#undef BOOST_ALLOW_DEPRECATED_HEADERS

#include <common/withnotify.h>
#include "util.h"

namespace Robot::Python {

    template<typename T>
    class NotifySubscription : public std::enable_shared_from_this<NotifySubscription<T>> {
        public:
            using result_set_t = std::unordered_set<T>;

            static constexpr size_t QUEUE_SIZE { 128 };

            NotifySubscription(const std::string &name, bool nonblocking=true) :
                m_name { name },
                m_queue { QUEUE_SIZE }
            {
                BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
                
                int flags = EFD_CLOEXEC;
                if (nonblocking) {
                    flags |= EFD_NONBLOCK;
                }

                m_fd = eventfd(0, flags);
                if (m_fd==-1) {
                    BOOST_THROW_EXCEPTION(std::system_error(errno, std::generic_category(), "Error creating eventfd"));
                }
            }


            ~NotifySubscription()
            {
                BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
                unsubscribe();
                if (m_fd>=0) {
                    close(m_fd);
                    m_fd = -1;
                }
            }

            /**
             * @brief Disconnect all added connections
             */
            void unsubscribe()
            {
                BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
                for (auto &connection : m_connections) {
                    connection.disconnect();
                }
                m_connections.clear();
            }


            /**
             * @brief Clear the queue from any events
             */
            void clear() 
            {
                read();                
            }

            /**
             * @brief Addd a connection to this subscription
             * 
             * @param connection the connection to add
             */
            void add_connection(boost::signals2::connection &&connection) 
            {
                m_connections.push_back(connection);
            }


            /**
             * @brief The name of the subscription
             * 
             * @return const std::string& 
             */
            const std::string &get_name() const { return m_name; }


            /**
             * @brief Get the subscription filedescriptor for polling do not read directly from it.
             * 
             * @return int filedescriptor
             */
            int get_fd() const { return m_fd; }


            /**
             * @brief Read the set of events written since last read.
             * 
             * @return result_set_t A set of events returned from this event
             */
            result_set_t read() {
                result_set_t result;
                uint64_t cnt { 0ULL };
                auto res = ::read(m_fd, &cnt, sizeof(cnt));
                if (res==-1) {
                    if (errno==EAGAIN) {
                        return result;
                    }
                    BOOST_THROW_EXCEPTION(std::system_error(errno, std::generic_category(), "Error reading from eventfd"));
                }
                //BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__ << " queue=" << cnt;

                int value;
                for (auto i=0u; i<cnt; i++) {
                    if (m_queue.pop(value)) {
                        BOOST_LOG_TRIVIAL(trace) << m_name << " READ ()=" << value;
                        result.insert(value);
                    }
                    else {
                        BOOST_THROW_EXCEPTION(std::runtime_error("Error reading event from queue"));
                    }
                }
                return result;
            }


            /**
             * @brief Write an event to the subscription
             * 
             * @param value Event
             * @return true Success
             * @return false Event queue is full
             */
            bool write(T value) 
            {
                static constexpr uint64_t COUNTER { 1ULL };
                BOOST_LOG_TRIVIAL(trace) << m_name << " WRITE (" << value << ")";

                if (m_queue.push(value)) {
                    auto res = ::write(m_fd, &COUNTER, sizeof(COUNTER));
                    if (res!=sizeof(COUNTER)) {
                        BOOST_THROW_EXCEPTION(std::system_error(errno, std::generic_category(), "Error writing to eventfd"));
                    }
                    return true;
                }
                else {
                    BOOST_LOG_TRIVIAL(warning) << m_name << " PUSH (" << value << ") queue full";
                }
                return false;
            }

        private:
            std::string m_name;
            int m_fd;
            std::vector<boost::signals2::connection> m_connections;
            boost::lockfree::queue<T, boost::lockfree::fixed_sized<true>> m_queue;
    };

    using NotifySubscriptionDefault = NotifySubscription<WithNotifyDefault::NotifyType>;


    /**
     * @brief Create a subscription on object that listens for notify signals
     * 
     * @tparam T Object type
     * @param obj Object to subscribe 
     * @return std::shared_ptr<NotifySubscription<typename T::NotifyType>> New subscription
     */
    template<typename T>
    std::shared_ptr<NotifySubscription<typename T::NotifyType>> notify_subscribe(T &obj)
    {
        std::stringstream sstream;
        sstream << obj;
        auto sub { std::make_shared<NotifySubscription<typename T::NotifyType>>(sstream.str()) };
        sub->add_connection(obj.subscribe(
            [sub_ptr=sub->weak_from_this()] (typename T::NotifyType arg) {
                if (auto sub = sub_ptr.lock()) { 
                    sub->write(arg);
                }
            }
        ));
        return sub;
    }


    /**
     * @brief Adds a subscription 
     * 
     * @tparam T Object type 
     * @param sub Add to this subscription
     * @param obj Object to subscribe
     * @param offset Offset added to notify argument before adding to queue
     */
    template<typename T>
    void notify_attach(NotifySubscription<typename T::NotifyType> &sub, T &obj, typename T::NotifyType offset)
    {
        sub.add_connection(obj.subscribe(
            [sub_ptr=sub.weak_from_this(),offset] (typename T::NotifyType arg) {
                if (auto sub = sub_ptr.lock()) { 
                    sub->write(arg+offset);
                }
            }
        ));
    }

}

#endif

