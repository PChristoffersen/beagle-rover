#ifndef _ROBOT_LED_ABSTRACTANIMATION_H_
#define _ROBOT_LED_ABSTRACTANIMATION_H_

#include <memory>
#include <chrono>
#include <boost/asio.hpp>

#include <common/withstrand.h>
#include <robotcontext.h>
#include "../colorlayer.h"
#include "../animation.h"
#include "../control.h"

namespace Robot::LED {

    template<typename T>
    class AbstractAnimation : public Animation, public std::enable_shared_from_this<T>, public WithStrand {
        public:
            virtual ~AbstractAnimation() {
                m_timer.cancel();
            }

            virtual void init(const std::shared_ptr<Control> &control) override 
            {
                m_control = control;
                if (m_layer) {
                    control->attachLayer(m_layer);
                }
            }

            virtual void cleanup() override 
            {
                m_timer.cancel();
                if (m_layer) {
                    if (auto control = m_control.lock()) {
                        control->detachLayer(m_layer);
                    }
                    m_layer->setVisible(false);
                    m_layer->fill(Color::TRANSPARENT);
                }
                m_control.reset();
            }

        protected:
            using timer_type = boost::asio::steady_timer;
            timer_type m_timer;
            timer_type::duration m_timer_interval;

            std::weak_ptr<Control> m_control;
            std::shared_ptr<ColorLayer> m_layer;

            AbstractAnimation(const strand_type &strand) :
                WithStrand { strand },
                m_timer { strand.context() }
            {
            }
            AbstractAnimation(const strand_type &strand, timer_type::duration timer_interval) :
                WithStrand { strand },
                m_timer { strand.context() },
                m_timer_interval { timer_interval }
            {
            }


            void timerSetup() 
            {
                m_timer.expires_at(m_timer.expiry() + m_timer_interval);
                m_timer.async_wait(boost::asio::bind_executor(m_strand, 
                    [this] (boost::system::error_code error) {
                        if (error!=boost::system::errc::success) {
                            return;
                        }
                        timer(); 
                    }
                ));
            }


            virtual void update() {}

        private:

            void timer() 
            {
                update();
                timerSetup();
            }

    };

}


#endif
