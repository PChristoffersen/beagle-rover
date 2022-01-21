#ifndef _ROBOT_INPUT_SOFTWAREINTERFACE_H_
#define _ROBOT_INPUT_SOFTWAREINTERFACE_H_


namespace Robot::Input {

    class SoftwareInterface {
        public:
            virtual void steer(float steer, float throttle, float aux_x=0.0f, float aux_y=0.0f) = 0;
    };

};


#endif
