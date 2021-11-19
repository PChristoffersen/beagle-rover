#ifndef _LED_CONTROL_H_
#define _LED_CONTROL_H_

#include <cstdint>
#include <memory>
#include <array>
#include <mutex>
#include <list>
#include <boost/asio.hpp>

#include <robotcontrolext.h>

#include "../common/withmutex.h"
#include "../robotcontext.h"
#include "ledcolor.h"
#include "ledcolorlayer.h"

namespace Robot::LED {

    class Control : public std::enable_shared_from_this<Control>, public WithMutex<std::recursive_mutex> {
        public:
            using LayerList = std::list<std::shared_ptr<ColorLayer>>;

            explicit Control(std::shared_ptr<Robot::Context> context);
            Control(const Control&) = delete; // No copy constructor
            Control(Control&&) = delete; // No move constructor
            virtual ~Control();

            void init();
            void cleanup();

            Color getBackground() const { return m_background; }
            void setBackground(const Color &color);

            void show();

            void attachLayer(std::shared_ptr<ColorLayer> &layer);
            void detachLayer(std::shared_ptr<ColorLayer> &layer);
            const LayerList &layers() const { return m_layers; }

        protected:
            friend class ColorLayer;
            void removeLayer(std::shared_ptr<ColorLayer> &layer);

        private:
            bool m_initialized;

            Color m_background;
            LayerList m_layers;

            std::chrono::high_resolution_clock::time_point m_last_show;

            void clear(const Color &color);
    };

};


#endif
