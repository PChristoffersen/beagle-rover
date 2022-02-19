#ifndef _ROBOT_NOTIFYSUBSCRIPTION_H_
#define _ROBOT_NOTIFYSUBSCRIPTION_H_

#include <memory>
#include <vector>
#include <chrono>
#include <sstream>
#include <unordered_set>
#include <poll.h>
#include <sys/eventfd.h>
#include <boost/signals2.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/log/trivial.hpp> 

#include "withnotify.h"

namespace Robot {

    template<typename T>
    class NotifySubscription : public std::enable_shared_from_this<NotifySubscription<T>> {
        public:
            using result_type = std::unordered_set<T>;

            static constexpr size_t QUEUE_SIZE { 128 };

            NotifySubscription(bool nonblocking=true) :
                m_queue { QUEUE_SIZE }
            {
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
             * @brief Get the subscription filedescriptor for polling do not read directly from it.
             * 
             * @return int filedescriptor
             */
            int get_fd() const { return m_fd; }


            /**
             * @brief Read the set of events written since last read.
             * 
             * @return result_type A set of events returned from this event
             */
            result_type read() {
                result_type result;
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
                        result.insert(value);
                    }
                    else {
                        BOOST_THROW_EXCEPTION(std::runtime_error("Error reading event from queue"));
                    }
                }
                return result;
            }


            /**
             * @brief Wait for event with timeout
             * 
             * @param timeout Timeout
             * @return result_type A set of events returned from this event
             */
            result_type read(std::chrono::milliseconds timeout) 
            {
                struct pollfd pfd;
                pfd.fd = m_fd;
                pfd.events = POLLIN;
                pfd.revents = 0;
                if (poll(&pfd, 1, timeout.count())<1) {
                    return result_type();
                }
                return read();
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

                if (m_queue.push(value)) {
                    auto res = ::write(m_fd, &COUNTER, sizeof(COUNTER));
                    if (res!=sizeof(COUNTER)) {
                        BOOST_THROW_EXCEPTION(std::system_error(errno, std::generic_category(), "Error writing to eventfd"));
                    }
                    return true;
                }
                else {
                    BOOST_LOG_TRIVIAL(warning) << *this << " PUSH (" << value << ") queue full";
                }
                return false;
            }

        private:
            int m_fd;
            std::vector<boost::signals2::connection> m_connections;
            boost::lockfree::queue<T, boost::lockfree::fixed_sized<true>> m_queue;

            friend std::ostream &operator<<(std::ostream &os, const NotifySubscription<T> &self)
            {
                return os << "Subscription<" << static_cast<const void*>(&self) << ">";
            }

    };

    using NotifySubscriptionDefault = NotifySubscription<WithNotifyDefault::notify_type>;


    /**
     * @brief Create a subscription on object that listens for notify signals
     * 
     * @tparam T Object type
     * @param obj Object to subscribe 
     * @return std::shared_ptr<NotifySubscription<typename T::notify_type>> New subscription
     */
    template<typename T>
    std::shared_ptr<NotifySubscription<typename T::notify_type>> notify_subscribe(T &obj)
    {
        auto sub { std::make_shared<NotifySubscription<typename T::notify_type>>() };
        sub->add_connection(obj.subscribe(
            [sub_ptr=sub->weak_from_this()] (typename T::notify_type arg) {
                if (auto sub = sub_ptr.lock()) { 
                    sub->write(arg);
                }
            }
        ));
        return sub;
    }


    /**
     * @brief Create a filtered subscription on object that listens for notify signals
     * 
     * @tparam T Object type
     * @param obj Object to subscribe 
     * @param events Set of wanted events
     * @return std::shared_ptr<NotifySubscription<typename T::notify_type>> New subscription
     */
    template<typename T>
    std::shared_ptr<NotifySubscription<typename T::notify_type>> notify_subscribe(T &obj, const std::unordered_set<typename T::notify_type> &events)
    {
        auto sub { std::make_shared<NotifySubscription<typename T::notify_type>>() };
        if (events.empty()) {
            return sub;
        }
        sub->add_connection(obj.subscribe(
            [sub_ptr=sub->weak_from_this(), events] (typename T::notify_type arg) {
                if (events.find(arg)==events.end())
                    return;
                if (auto sub = sub_ptr.lock()) { 
                    sub->write(arg);
                }
            }
        ));
        return sub;
    }


    /**
     * @brief Adds a subscription to an existing subscription object
     * 
     * @tparam T Object type 
     * @param sub Add to this subscription
     * @param obj Object to subscribe
     * @param offset Offset added to notify argument before adding to queue
     */
    template<typename T>
    void notify_attach(const std::shared_ptr<NotifySubscription<typename T::notify_type>> &sub, T &obj, typename T::notify_type offset)
    {
        sub->add_connection(obj.subscribe(
            [sub_ptr=sub->weak_from_this(),offset] (typename T::notify_type arg) {
                if (auto sub = sub_ptr.lock()) { 
                    sub->write(arg+offset);
                }
            }
        ));
    }


    /**
     * @brief Adds a filtered subscription to an existing subscription object
     * 
     * @tparam T Object type 
     * @param sub Add to this subscription
     * @param obj Object to subscribe
     * @param events Set of wanted events
     * @param offset Offset added to notify argument before adding to queue
     */
    template<typename T>
    void notify_attach(const std::shared_ptr<NotifySubscription<typename T::notify_type>> &sub, T &obj, typename T::notify_type offset, const std::unordered_set<typename T::notify_type> &events)
    {
        sub->add_connection(obj.subscribe(
            [sub_ptr=sub->weak_from_this(),offset, events] (typename T::notify_type arg) {
                if (events.find(arg)==events.end())
                    return;
                if (auto sub = sub_ptr.lock()) { 
                    sub->write(arg+offset);
                }
            }
        ));
    }


}

#endif

