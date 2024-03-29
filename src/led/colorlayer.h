#ifndef _ROBOT_LED_COLORLAYER_H_
#define _ROBOT_LED_COLORLAYER_H_

#include <memory>
#include <iostream>
#include <shared_mutex>
#include <boost/signals2.hpp>

#include <robotconfig.h>
#include "types.h"
#include "color.h"
#include "colorarray.h"

namespace Robot::LED {

    constexpr auto LAYER_DEPTH_ANIMATION  { 10 };
    constexpr auto LAYER_DEPTH_INDICATORS { 20 };
    constexpr auto LAYER_BALANCE_INDICATORS { 90 };
    constexpr auto LAYER_RC_INDICATORS { 100 };

    class ColorLayer : public ColorArray<PIXEL_COUNT>, public std::enable_shared_from_this<ColorLayer> {
        public:
            using array_type = ColorArray<PIXEL_COUNT>;

            ColorLayer(const std::string &name, uint depth) : ColorLayer(name, depth, false) {}
            ColorLayer(const std::string &name, uint depth, bool internal);
            virtual ~ColorLayer();

            void setVisible(bool visible);

            void detach();
            void update();

            const std::string &name() const { return m_name; }
            bool internal() const { return m_internal; }
            uint depth() const { return m_depth; }
            bool visible() const { return m_visible; }
            
        protected:
            friend class Control;

            using update_signal = typename boost::signals2::signal<void()>;

            void connect(const std::shared_ptr<class Control> &control, update_signal::slot_type func);
            void disconnect();

        private:
            const std::string m_name;
            const uint m_depth;
            const bool m_internal;
            bool m_visible;

            // Layer connection
            std::weak_ptr<class Control> m_control;
            update_signal m_update_signal;
            boost::signals2::connection m_control_connection;


            friend array_type &operator<<(array_type &dst, ColorLayer &layer);

            friend std::ostream &operator<<(std::ostream &os, const ColorLayer &self)
            {
                return os << "LED::ColorLayer<" << self.m_depth << "," << self.m_name << ">";
            }
    };


}


#endif
