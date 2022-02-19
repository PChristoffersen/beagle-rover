#include "robotdebug.h"

#ifdef ROBOT_DEBUG

namespace Robot::Debug {


TestComponent::TestComponent()
{
}


TestComponent::~TestComponent()
{

}

void TestComponent::ping(notify_type what) const
{
    notify(what);
}


}

#endif
