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
#include "ledcolor.h"
#include "ledcolorlayer.h"

class LEDControl : public std::enable_shared_from_this<LEDControl>, public WithMutex<std::recursive_mutex> {
    public:
        using LayerList = std::list<std::shared_ptr<LEDColorLayer>>;

        explicit LEDControl(std::shared_ptr<class RobotContext> context);
        LEDControl(const LEDControl&) = delete; // No copy constructor
        LEDControl(LEDControl&&) = delete; // No move constructor
        virtual ~LEDControl();

        void init();
        void cleanup();

        LEDColor getBackground() const { return m_background; }
        void setBackground(const LEDColor &color);

        void show();

        void addLayer(std::shared_ptr<class LEDColorLayer> &layer);
        void removeLayer(std::shared_ptr<class LEDColorLayer> &layer);
        const LayerList &layers() const { return m_layers; }

    private:
        bool m_initialized;

        LEDColor m_background;
        LayerList m_layers;

        void clear(const LEDColor &color);
};

#endif
