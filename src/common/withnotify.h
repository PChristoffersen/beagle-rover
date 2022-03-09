#ifndef _ROBOT_WITHNOTIFY_H_
#define _ROBOT_WITHNOTIFY_H_

#include <boost/signals2.hpp>

namespace Robot {

    /**
     * @brief Addon class for components that adds notification signals to signal property changes
     * 
     * @tparam T The type signaled. Must be a simple type since it is added to a nonlocking queue in notifysubscription.
     */
    template<typename T>
    class WithNotify {
        public:
            using notify_type = T;
            using notify_signal = typename boost::signals2::signal<void(T)>;

            virtual boost::signals2::connection subscribe(const typename notify_signal::slot_type &subscriber) 
            {
                return m_sig.connect(subscriber);
            }

        protected:

            void notify(const notify_type &arg) const
            {
                m_sig(arg);
            }

        private:
            notify_signal m_sig;
    };
    

    class WithNotifyInt : public WithNotify<int> {
        public:
            static constexpr notify_type NOTIFY_DEFAULT { 0 };
    };

};

#endif
