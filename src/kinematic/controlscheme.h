#ifndef _CONTROLSCHEME_H_
#define _CONTROLSCHEME_H_

class ControlScheme {
    public:
        virtual ~ControlScheme() {}

        virtual void init() = 0;
        virtual void cleanup() = 0;
};

#endif