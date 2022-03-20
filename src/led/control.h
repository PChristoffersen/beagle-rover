#ifndef _ROBOT_LED_CONTROL_H_
#define _ROBOT_LED_CONTROL_H_

#include <cstdint>
#include <memory>
#include <array>
#include <mutex>
#include <atomic>
#include <list>
#include <chrono>
#include <boost/asio.hpp>

#include <robotconfig.h>
#include <common/withstrand.h>
#include <common/withmutex.h>
#include <common/withnotify.h>
#include <robottypes.h>
#include "types.h"

namespace Robot::LED {

    class Control : public std::enable_shared_from_this<Control>, public WithMutexStd, public WithNotifyInt, public WithStrand {
        public:
            static constexpr notify_type NOTIFY_UPDATE { 1 };

            using color_array_type = ColorArray<PIXEL_COUNT>;
            using raw_color_array_type = RawColorArray<PIXEL_COUNT>;
            using LayerList = std::list<std::shared_ptr<ColorLayer>>;

            explicit Control(const std::shared_ptr<::Robot::Context> &context);
            Control(const Control&) = delete; // No copy constructor
            Control(Control&&) = delete; // No move constructor
            virtual ~Control();

            void init(const std::shared_ptr<::Robot::Input::Control> &input_control);
            void cleanup();

            #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
            void setLED(rc_led_t led, bool state);
            #endif

            void setBrightness(Color::brightness_type brightness);
            Color::brightness_type getBrightness() const { return m_brightness; }

            void setColorCorrection(Color::Correction correction);
            Color::Correction getColorCorrection() const { return m_color_correction; }

            Color getBackground() const { return m_background; }
            void setBackground(Color color);

            void setAnimation(AnimationMode mode);
            AnimationMode getAnimation() const { return m_animation_mode; }

            void setIndicators(IndicatorMode mode);
            IndicatorMode getIndicators() const { return m_indicator_mode; }

            void update();

            void attachLayer(std::shared_ptr<ColorLayer> layer);
            void detachLayer(std::shared_ptr<ColorLayer> layer);
            LayerList layers(bool filter_internal=false) const;

            const color_array_type &output() const { return m_pixels; };

        private:
            std::shared_ptr<::Robot::Context> m_context;
            bool m_initialized;
            boost::signals2::connection m_update_connection;

            boost::signals2::connection m_animation_mode_connection;
            boost::signals2::connection m_indicator_mode_connection;
            boost::signals2::connection m_brightness_connection;

            Color::brightness_type m_brightness;
            Color::Correction m_color_correction;
            Color m_background;

            std::atomic_uint m_update_cnt;
            color_array_type m_pixels;
            LayerList m_layers;

            AnimationMode m_animation_mode;
            std::shared_ptr<class Animation> m_animation;

            IndicatorMode m_indicator_mode;
            std::shared_ptr<class Indicator> m_indicator;

            void clear(const Color &color);
            void updatePixels();
            void showPixels();

            friend std::ostream &operator<<(std::ostream &os, const Control &self)
            {
                return os << "Robot::LED::Control";
            }

    };

}


#endif
