#ifndef _ROBOT_INPUT_SOFTWAREINTERFACE_H_
#define _ROBOT_INPUT_SOFTWAREINTERFACE_H_

#include <common/withnotify.h>

namespace Robot::Input {

    class SoftwareInterface : public WithNotifyDefault {
        public:
            virtual ~SoftwareInterface() = default;

            virtual void setAxis(float steer, float throttle, float aux_x=0.0f, float aux_y=0.0f) = 0;

            virtual float getSteering() const = 0;
            virtual void setSteering(float value) = 0;
            virtual float getThrottle() const = 0;
            virtual void setThrottle(float value) = 0;
            virtual float getAuxX() const = 0;
            virtual void setAuxX(float value) = 0;
            virtual float getAuxY() const = 0;
            virtual void setAuxY(float value) = 0;

    };

}


#endif
