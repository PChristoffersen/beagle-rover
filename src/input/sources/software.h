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

            void setAxis(float direction, float throttle, float aux_x=0.0f, float aux_y=0.0f) override;

            float getDirection() const override { return m_direction; }
            void setDirection(float value) override;
            float getThrottle() const override { return m_throttle; }
            void setThrottle(float value) override;
            float getAuxX() const override { return m_aux_x; }
            void setAuxX(float value) override;
            float getAuxY() const override { return m_aux_y; }
            void setAuxY(float value) override;

            void setDriveMode(Kinematic::DriveMode drive_mode) { _setDriveMode(drive_mode); }
            void setOrientation(Kinematic::Orientation orientation) { _setOrientation(orientation); }
            void setAnimationMode(LED::AnimationMode animation_mode) { _setAnimationMode(animation_mode); }
            void setIndicatorMode(LED::IndicatorMode indicator_mode) { _setIndicatorMode(indicator_mode); }
            void setBrightness(float brightness) { _setBrightness(brightness); }

        private:
            bool m_initialized;
            std::string m_name;

            float m_direction;
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


