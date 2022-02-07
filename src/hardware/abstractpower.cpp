#include "abstractpower.h"

#include <boost/log/trivial.hpp>



namespace Robot::Hardware {

AbstractPower::AbstractPower() :
    m_active_count { 0 }
{
}


uint AbstractPower::setEnabled(bool enabled)
{
    const guard lock(m_mutex);
    if (enabled) {
        m_active_count++;
        enable();
    }
    else {
        assert(m_active_count>0);
        m_active_count--;
        if (m_active_count==0) {
            disable();
        }
    }
    return m_active_count;
}

}
