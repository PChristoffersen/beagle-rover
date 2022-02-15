#ifndef _ROBOT_LED_CONTROL_H_
#define _ROBOT_LED_CONTROL_H_

#include <cstdint>
#include <memory>
#include <array>
#include <mutex>
#include <shared_mutex>
#include <list>
#include <chrono>
#include <boost/asio.hpp>

#include <robotconfig.h>
#include <common/withmutex.h>
#include <common/withnotify.h>
#include <common/asyncsignal.h>
#include <robottypes.h>
#include "color.h"
#include "colorlayer.h"

namespace Robot::LED {

    enum class AnimationMode {
        NONE,
        HEADLIGHTS,
        CONSTRUCTION,
        POLICE,
        AMBULANCE,
        KNIGHT_RIDER,
        RAINBOW,
    };

    enum class IndicatorMode {
        NONE,
        LEFT,
        RIGHT,
        HAZARD
    };


    class Control : public std::enable_shared_from_this<Control>, public WithMutex<std::recursive_mutex>, public WithNotifyDefault {
        public:
            using LayerList = std::list<std::weak_ptr<ColorLayer>>;

            explicit Control(const std::shared_ptr<::Robot::Context> &context);
            Control(const Control&) = delete; // No copy constructor
            Control(Control&&) = delete; // No move constructor
            virtual ~Control();

            void init();
            void cleanup();

            #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
            void setLED(rc_led_t led, bool state);
            #endif

            Color getBackground() const { return m_background; }
            void setBackground(const Color &color);

            void setAnimation(AnimationMode mode);
            AnimationMode getAnimation() const { return m_animation_mode; }

            void setIndicators(IndicatorMode mode);
            IndicatorMode getIndicators() const { return m_indicator_mode; }

            void show();

            void attachLayer(const std::shared_ptr<ColorLayer> &layer);
            void detachLayer(const std::shared_ptr<ColorLayer> &layer);
            const LayerList &layers() const { return m_layers; }

        private:
            std::shared_ptr<::Robot::Context> m_context;
            bool m_initialized;
            std::shared_ptr<::Robot::ASyncSignal> m_show_signal;
            boost::signals2::connection m_show_connection;

            Color m_background;
            LayerList m_layers;

            AnimationMode m_animation_mode;
            std::shared_ptr<class ColorLayer> m_animation_layer;
            std::shared_ptr<class Animation> m_animation;

            IndicatorMode m_indicator_mode;
            std::shared_ptr<class ColorLayer> m_indicator_layer;
            std::shared_ptr<class Indicator> m_indicator;

            void clear(const Color &color);
            void updatePixels();
            void showPixels(const RawColorArray &pixels);

            friend std::ostream &operator<<(std::ostream &os, const Control &self)
            {
                return os << "Robot::LED::Control";
            }

    };

}


#endif
