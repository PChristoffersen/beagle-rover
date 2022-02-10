#ifndef _ROBOT_WITHNOTIFY_H_
#define _ROBOT_WITHNOTIFY_H_

#include <boost/signals2.hpp>

namespace Robot {

    template<typename T>
    class WithNotify {
        public:
            using NotifyType = T;
            using NotifySignal = typename boost::signals2::signal<void(T)>;

            boost::signals2::connection subscribe(const typename NotifySignal::slot_type &subscriber) 
            {
                return m_sig.connect(subscriber);
            }

            void notify(NotifyType arg) 
            {
                m_sig(arg);
            }

        private:
            NotifySignal m_sig;
    };
    

    class WithNotifyDefault : public WithNotify<int> {
        public:
            static constexpr NotifyType NOTIFY_DEFAULT { 0 };
    };

};

#endif
