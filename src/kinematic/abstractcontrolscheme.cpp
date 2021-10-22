
#include "abstractcontrolscheme.h"
#include "../robotcontext.h"
#include "kinematic.h"

AbstractControlScheme::AbstractControlScheme(boost::shared_ptr<class Kinematic> kinematic) :
    m_kinematic(kinematic),
    m_context(kinematic->context()),
    m_motor_control(kinematic->motorControl()),
    m_telemetry(kinematic->telemetry())
{

}

        
AbstractControlScheme::~AbstractControlScheme() {

}
    
