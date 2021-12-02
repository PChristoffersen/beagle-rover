#ifndef _ROBOT_LED_ABSTRACTANIMATION_H_
#define _ROBOT_LED_ABSTRACTANIMATION_H_

#include <memory>
#include <chrono>
#include <boost/asio.hpp>
#include <robotcontext.h>
#include "../animation.h"

namespace Robot::LED {

    template<typename T>
    class AbstractAnimation : public Animation, public std::enable_shared_from_this<T> {
        public:
            virtual ~AbstractAnimation() {
                m_timer.cancel();
            }

        protected:
            using timer_type = boost::asio::steady_timer;
            timer_type m_timer;
            timer_type::duration m_timer_interval;

            AbstractAnimation(const std::shared_ptr<Robot::Context> &context) :
                m_timer { context->io() }
            {
            }
            AbstractAnimation(const std::shared_ptr<Robot::Context> &context, timer_type::duration timer_interval) :
                m_timer { context->io() },
                m_timer_interval { timer_interval }
            {
            }
            AbstractAnimation(const std::shared_ptr<Robot::Context> &context, int depth, timer_type::duration timer_interval) :
                Animation { depth },
                m_timer { context->io() },
                m_timer_interval { timer_interval }
            {
            }


            void timerSetup() 
            {
                m_timer.expires_at(m_timer.expiry() + m_timer_interval);
                m_timer.async_wait(
                    [self_ptr=this->weak_from_this()] (boost::system::error_code error) {
                        if (auto self = self_ptr.lock()) { 
                            self->timer(error); 
                        }
                    }
                );
            }


            virtual void update(ColorLayer &layer) {}

        private:

            void timer(boost::system::error_code error) 
            {
                const ColorLayerLock lock { m_layer };
                if (error!=boost::system::errc::success) {
                    return;
                }
                update(*m_layer);
                timerSetup();
            }

    };

};


#endif
