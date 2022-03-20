#ifndef _ROBOT_INPUT_CONTROL_H_
#define _ROBOT_INPUT_CONTROL_H_

#include <memory>
#include <mutex>
#include <iostream>

#include <robottypes.h>
#include <common/withstrand.h>
#include <common/withmutex.h>
#include <common/withnotify.h>
#include <rc/types.h>
#include "types.h"
#include "softwareinterface.h"

namespace Robot::Input {

    enum class InputSource {
        MANUAL,
        RC,
        WEB,
        CONTROLLER
    };
    std::ostream &operator<<(std::ostream &os, const InputSource &input_source);

    class Control : public std::enable_shared_from_this<Control>, public WithMutexStd, public WithNotifyInt {
        public:
            Control(const std::shared_ptr<::Robot::Context> &context);
            Control(const Control&) = delete; // No copy constructor
            Control(Control&&) = delete; // No move constructor
            virtual ~Control();
        
            void init(const std::shared_ptr<::Robot::RC::Receiver> &receiver);
            void cleanup();

            void setAxisSource(InputSource input);
            InputSource getAxisSource() const { return m_axis_source; }
            void setKinematicSource(InputSource input);
            InputSource getKinematicSource() const { return m_kinematic_source; }
            void setLedSource(InputSource input);
            InputSource getLedSource() const { return m_led_source; }


            std::shared_ptr<SoftwareInterface> manual() const;
            std::shared_ptr<SoftwareInterface> web() const;

            Signals signals;

        private:
            bool m_initialized;
            
            InputSource m_axis_source;
            InputSource m_kinematic_source;
            InputSource m_led_source;

            std::shared_ptr<class SoftwareSource> m_manual_source;
            std::shared_ptr<class SoftwareSource> m_web_source;
            #if ROBOT_HAVE_RC
            std::unique_ptr<class ::Robot::RC::InputSource> m_rc_source;
            #endif
            std::unique_ptr<class GamepadSource> m_gamepad_source;

            class Source *findSource(InputSource input);

            friend std::ostream &operator<<(std::ostream &os, const Control &self)
            {
                return os << "Robot::Input::Control";
            }
    };

}

#endif
