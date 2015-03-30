//------------------------------------------------------------------------------
//  PointToPointjoint.cc
//  (C) 2012 Johannes Hirche, LTU Skelleftea
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physics/joints/pointtopointjoint.h"

namespace Physics
{
#if (__USE_BULLET__)
	__ImplementClass(Physics::PointToPointjoint, 'PPPJ', Bullet::BulletPointToPoint);
#elif(__USE_PHYSX__)
	__ImplementClass(Physics::PointToPointjoint, 'PPPJ', PhysX::PhysXPointToPoint);
#elif(__USE_HAVOK__)
	__ImplementClass(Physics::PointToPointjoint, 'PPPJ', Havok::HavokPointToPoint);
#else
#error "Physics::PointToPointjoint not implemented"
#endif
}
