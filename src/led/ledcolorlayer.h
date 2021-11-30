#ifndef _LEDCOLORLAYER_H_
#define _LEDCOLORLAYER_H_

#include <memory>
#include <iostream>

#include "ledcolor.h"
#include <robotcontrolext.h>

namespace Robot::LED {

    constexpr uint PIXEL_COUNT { RC_EXT_NEOPIXEL_COUNT };

    constexpr auto LAYER_DEPTH_ANIMATION  { 10 };
    constexpr auto LAYER_DEPTH_INDICATORS { 20 };
    constexpr auto LAYER_RC_INDICATORS { 100 };

    using ColorArray = std::array<Color, PIXEL_COUNT>;
    using RawColorArray = std::array<Color::raw_type, PIXEL_COUNT>;

    class ColorLayer : public ColorArray, public std::enable_shared_from_this<ColorLayer> {
        public:
            explicit ColorLayer(int depth);
            virtual ~ColorLayer();

            void setVisible(bool visible);

            void show();

            int depth() const { return m_depth; }
            bool visible() const { return m_visible; }
            
            void detach();

            void lock();
            void unlock();

            const std::weak_ptr<class Control> &control() const { return m_control; }

        protected:
            friend class Control;

            void setControl(const std::shared_ptr<class Control> &control);

        private:
            const int m_depth;
            bool m_visible;

            std::weak_ptr<class Control> m_control;

            friend RawColorArray &operator<<(RawColorArray &dst, const ColorLayer &layer);


            friend std::ostream &operator<<(std::ostream &os, const ColorLayer &self)
            {
                return os << "LED::ColorLayer<" << self.m_depth << ">";
            }

    };

    class ColorLayerLock {
        public:
            ColorLayerLock(const std::shared_ptr<ColorLayer> &layer) : 
                m_layer { layer }
            {
                m_layer->lock();
            }
            ~ColorLayerLock() 
            {
                m_layer->unlock();
            }
        private:
            const std::shared_ptr<ColorLayer> &m_layer;
    };

};


#endif
