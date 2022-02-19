#ifndef _ROBOT_WITHNOTIFY_H_
#define _ROBOT_WITHNOTIFY_H_

#include <boost/signals2.hpp>

namespace Robot {

    template<typename T>
    class WithNotify {
        public:
            using notify_type = T;
            using notify_signal = typename boost::signals2::signal<void(T)>;

            boost::signals2::connection subscribe(const typename notify_signal::slot_type &subscriber) 
            {
                return m_sig.connect(subscriber);
            }

        protected:

            void notify(notify_type arg) const
            {
                m_sig(arg);
            }

        private:
            notify_signal m_sig;
    };
    

    class WithNotifyDefault : public WithNotify<int> {
        public:
            static constexpr notify_type NOTIFY_DEFAULT { 0 };
    };

};

#endif
