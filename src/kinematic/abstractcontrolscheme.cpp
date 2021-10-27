
#include "abstractcontrolscheme.h"
#include "../robotcontext.h"
#include "kinematic.h"

using namespace std;


AbstractControlScheme::AbstractControlScheme(shared_ptr<Kinematic> kinematic) :
    m_kinematic { kinematic },
    m_context { kinematic->context() },
    m_motor_control { kinematic->motorControl() },
    m_telemetry { kinematic->telemetry() }
{

}

        
AbstractControlScheme::~AbstractControlScheme() 
{

}
    
