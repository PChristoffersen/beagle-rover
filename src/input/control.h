#ifndef _ROBOT_INPUT_CONTROL_H_
#define _ROBOT_INPUT_CONTROL_H_

#include <memory>
#include <mutex>
#include <iostream>

#include <robottypes.h>
#include <common/withmutex.h>
#include <rc/receiver.h>
#include "types.h"
#include "softwareinterface.h"

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
        
            #if ROBOT_HAVE_RC
            void init(const std::shared_ptr<::Robot::RC::Receiver> &receiver);
            #else
            void init();
            #endif
            void cleanup();

            void setInputSource(InputSource input);
            InputSource getInputSource() const { return m_input_source; }

            SoftwareInterface *manual() const;
            SoftwareInterface *web() const;

            Signals signals;

        private:
            bool m_initialized;
            
            InputSource m_input_source;
            class Source *m_active_source;

            std::unique_ptr<class SoftwareSource> m_manual_source;
            std::unique_ptr<class SoftwareSource> m_web_source;
            #if ROBOT_HAVE_RC
            std::unique_ptr<class RCSource> m_rc_source;
            #endif
            std::unique_ptr<class GamepadSource> m_gamepad_source;
    };

};

#endif
