#ifndef _ROBOT_INPUT_SOFTWARESOURCE_H_
#define _ROBOT_INPUT_SOFTWARESOURCE_H_

#include <memory>
#include <mutex>
#include <boost/asio.hpp>

#include "abstractinputsource.h"
#include "../softwareinterface.h"

#include <common/withnotify.h>

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

            void setAxis(float steer, float throttle, float aux_x=0.0f, float aux_y=0.0f) override;

            float getSteering() const override { return m_steer; }
            void setSteering(float value) override;
            float getThrottle() const override { return m_throttle; }
            void setThrottle(float value) override;
            float getAuxX() const override { return m_aux_x; }
            void setAuxX(float value) override;
            float getAuxY() const override { return m_aux_y; }
            void setAuxY(float value) override;

        private:
            bool m_initialized;
            bool m_enabled;
            std::string m_name;

            float m_steer;
            float m_throttle;
            float m_aux_x;
            float m_aux_y;

            friend std::ostream &operator<<(std::ostream &os, const SoftwareSource &self)
            {
                return os << "Input::" << self.m_name;
            }

    };

}

#endif


