#ifndef _ROBOT_INPUT_SOFTWARESOURCE_H_
#define _ROBOT_INPUT_SOFTWARESOURCE_H_

#include <memory>
#include <mutex>
#include <boost/asio.hpp>

#include "abstractinputsource.h"
#include "../softwareinterface.h"


namespace Robot::Input {

    class SoftwareSource : public AbstractSource, public SoftwareInterface {
        public:
            explicit SoftwareSource(std::string m_name, const Signals &signals);
            SoftwareSource(const SoftwareSource&) = delete; // No copy constructor
            SoftwareSource(SoftwareSource&&) = delete; // No move constructor
            virtual ~SoftwareSource();

            void init();
            void cleanup();

            void setEnabled(bool enabled);

            virtual void steer(float steer, float throttle, float aux_x=0.0f, float aux_y=0.0f);

        private:
            bool m_initialized;
            bool m_enabled;
            std::string m_name;

            friend std::ostream &operator<<(std::ostream &os, const SoftwareSource &self)
            {
                return os << "Input::" << self.m_name;
            }

    };

};

#endif


