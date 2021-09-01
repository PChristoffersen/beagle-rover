#ifndef _COMPONENT_H_
#define _COMPONENT_H_

class Component {
    public:
        Component();
        virtual ~Component();

        virtual void init();
        virtual void cleanup();

    protected:
        bool m_initialized;
};

#endif
