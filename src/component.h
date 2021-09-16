#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>


class Component {
    public:
        Component(boost::shared_ptr<boost::asio::io_context> io);
        virtual ~Component();

        virtual void init();
        virtual void cleanup();

    protected:
        bool m_initialized;
        boost::shared_ptr<boost::asio::io_context> m_io;
};

#endif
