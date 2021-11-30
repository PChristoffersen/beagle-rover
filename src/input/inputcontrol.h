#ifndef _INPUTCONTROL_H_
#define _INPUTCONTROL_H_

#include <memory>
#include <mutex>
#include <iostream>

#include "../common/withmutex.h"
#include "../robottypes.h"
#include "../rc/rcreceiver.h"
#include "inputinterface.h"
#include "inputtypes.h"

namespace Robot::Input {

    enum class InputSource {
        MANUAL,
        RC,
        WEB,
        CONTROLLER
    };

    class Control : public std::enable_shared_from_this<Control>, public WithMutex<std::recursive_mutex> {
        public:
            Control(const std::shared_ptr<::Robot::Context> &context);
            Control(const Control&) = delete; // No copy constructor
            Control(Control&&) = delete; // No move constructor
            virtual ~Control();
        
            void init(const std::shared_ptr<::Robot::RC::Receiver> &receiver);
            void cleanup();

            void setInputSource(InputSource input);
            InputSource getInputSource() const { return m_input_source; }

            InputInterface *manual() const;
            InputInterface *web() const;

            Signals signals;

        private:
            bool m_initialized;
            
            InputSource m_input_source;
            class Source *m_active_source;

            std::unique_ptr<class SoftwareSource> m_manual_source;
            std::unique_ptr<class SoftwareSource> m_web_source;
            std::unique_ptr<class RCSource> m_rc_source;
            std::unique_ptr<class GamepadSource> m_gamepad_source;
    };

};

#endif
