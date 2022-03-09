#ifndef _ROBOT_INPUT_SOFTWAREINTERFACE_H_
#define _ROBOT_INPUT_SOFTWAREINTERFACE_H_

#include <common/withnotify.h>

namespace Robot::Input {

    class SoftwareInterface : public WithNotifyInt {
        public:
            virtual ~SoftwareInterface() = default;

            virtual void setAxis(float direction, float throttle, float aux_x=0.0f, float aux_y=0.0f) = 0;

            virtual float getDirection() const = 0;
            virtual void setDirection(float value) = 0;
            virtual float getThrottle() const = 0;
            virtual void setThrottle(float value) = 0;
            virtual float getAuxX() const = 0;
            virtual void setAuxX(float value) = 0;
            virtual float getAuxY() const = 0;
            virtual void setAuxY(float value) = 0;

            virtual void setDriveMode(Kinematic::DriveMode drive_mode) = 0;
            virtual void setOrientation(Kinematic::Orientation orientation) = 0;
            virtual void setAnimationMode(LED::AnimationMode animation_mode) = 0;
            virtual void setIndicatorMode(LED::IndicatorMode indicator_mode) = 0;
            virtual void setBrightness(float brightness) = 0;
    };

}


#endif
