#ifndef _INPUT_SOFTWARESOURCE_H_
#define _INPUT_SOFTWARESOURCE_H_

#include <memory>
#include <mutex>
#include <boost/asio.hpp>

#include "abstractinputsource.h"
#include "../inputvalue.h"
#include "../inputinterface.h"


namespace Robot::Input {

    class SoftwareSource : public AbstractSource<SoftwareSource>, public InputInterface {
        public:
            explicit SoftwareSource(const Signals &signals);
            SoftwareSource(const SoftwareSource&) = delete; // No copy constructor
            SoftwareSource(SoftwareSource&&) = delete; // No move constructor
            virtual ~SoftwareSource();

            void init();
            void cleanup();

            void setEnabled(bool enabled);

            virtual void steer(double steer, double throttle, double aux_x=0.0, double aux_y=0.0);

        private:
            bool m_initialized;
            bool m_enabled;

    };

};

#endif


