#ifndef _ROBOT_LED_COLORLAYER_H_
#define _ROBOT_LED_COLORLAYER_H_

#include <memory>
#include <iostream>
#include <shared_mutex>
#include <boost/signals2.hpp>

#include <robotconfig.h>
#include <common/withmutex.h>
#include <common/asyncsignal.h>
#include "color.h"

namespace Robot::LED {

    constexpr uint PIXEL_COUNT { ROBOT_NEOPIXEL_COUNT };
    constexpr uint SEGMENT_COUNT { 2 };

    constexpr auto LAYER_DEPTH_ANIMATION  { 10 };
    constexpr auto LAYER_DEPTH_INDICATORS { 20 };
    constexpr auto LAYER_RC_INDICATORS { 100 };

    using ColorArray = std::array<Color, PIXEL_COUNT>;
    using RawColorArray = std::array<Color::raw_type, PIXEL_COUNT>;

    class ColorLayer : public ColorArray, public WithMutex<std::recursive_mutex>, public std::enable_shared_from_this<ColorLayer> {
        public:
            class Segment {
                public:
                    Segment(ColorLayer *parent, const std::string &name, size_type offset, size_type size) : m_offset { offset }, m_size { size }, m_parent { parent } { }
                    const std::string &name() const { return m_name; }
                    Color &operator[](size_type pos) {
                        return (*m_parent)[m_offset+pos];
                    }
                    const Color &operator[](size_type pos) const {
                        return (*m_parent)[m_offset+pos];
                    }
                    size_type size() const { return m_size; }
                private:
                    const std::string m_name;
                    const size_type m_offset;
                    const size_type m_size;
                    ColorLayer *m_parent;
            };
            using SegmentArray = std::array<Segment, SEGMENT_COUNT>;

            ColorLayer(const std::string &name, uint depth) : ColorLayer(name, depth, false) {}
            ColorLayer(const std::string &name, uint depth, bool internal);
            virtual ~ColorLayer();

            void setVisible(bool visible);

            void update();

            const std::string &name() const { return m_name; }
            bool internal() const { return m_internal; }
            uint depth() const { return m_depth; }
            bool visible() const { return m_visible; }
            
            SegmentArray &segments() { return m_segments; }

        protected:
            friend class Control;

            void setSignal(const std::shared_ptr<::Robot::ASyncSignal> &sig);
            void clearSignal();

        private:
            const std::string m_name;
            const uint m_depth;
            const bool m_internal;
            bool m_visible;
            SegmentArray m_segments;
            std::shared_ptr<::Robot::ASyncSignal> m_show_sig;

            std::weak_ptr<class Control> m_control;

            friend RawColorArray &operator<<(RawColorArray &dst, ColorLayer &layer);


            friend std::ostream &operator<<(std::ostream &os, const ColorLayer &self)
            {
                return os << "LED::ColorLayer<" << self.m_depth << ">";
            }

    };

}


#endif
