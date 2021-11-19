#ifndef _LEDCOLORLAYER_H_
#define _LEDCOLORLAYER_H_

#include <memory>

#include "ledcolor.h"
#include <robotcontrolext.h>

namespace Robot::LED {

    constexpr unsigned int PIXEL_COUNT { RC_EXT_NEOPIXEL_COUNT };

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
            int m_depth;
            bool m_visible;

            std::weak_ptr<class Control> m_control;
    };


    ColorArray &operator+=(ColorArray &dst, const ColorLayer &layer);
    RawColorArray &operator+=(RawColorArray &dst, const ColorLayer &layer);
};


#endif
