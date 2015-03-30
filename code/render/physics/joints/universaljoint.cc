//------------------------------------------------------------------------------
//  UniversalJoint.cc
//  (C) 2012 Johannes Hirche, LTU Skelleftea
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physics/joints/universaljoint.h"

namespace Physics
{
#if (__USE_BULLET__)
	__ImplementClass(Physics::UniversalJoint, 'PUNJ', Bullet::BulletUniversalJoint);
#elif(__USE_PHYSX__)
	__ImplementClass(Physics::UniversalJoint, 'PUNJ', PhysX::PhysXUniversalJoint);
#elif(__USE_HAVOK__)
	__ImplementClass(Physics::UniversalJoint, 'PUNJ', Havok::HavokUniversalJoint);
#else
#error "Physics::UniversalJoint not implemented"
#endif
}

