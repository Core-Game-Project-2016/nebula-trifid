//------------------------------------------------------------------------------
//  physxscene.cc
//  (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------

#include "stdneb.h"
#include "physics/physx/physxscene.h"
#include "physics/physicsbody.h"
#include "physics/collider.h"
#include "db/filterset.h"
#include "PxScene.h"
#include "PxSceneDesc.h"
#include "extensions/PxDefaultCpuDispatcher.h"
#include "extensions/PxDefaultSimulationFilterShader.h"
#include "physxphysicsserver.h"
#include "physxutils.h"
#include "PxPhysics.h"
#include "PxRigidActor.h"
#include "PxSimulationEventCallback.h"
#include "physics/physicsprobe.h"
#include "characterkinematic/PxControllerManager.h"
#include "PxQueryReport.h"
#include "physics/physicsobject.h"

namespace PhysX
{

using namespace Physics;
using namespace physx;

PxFilterFlags Simulationfilter(PxFilterObjectAttributes	attributes0,
	PxFilterData				filterData0,
	PxFilterObjectAttributes	attributes1,
	PxFilterData				filterData1,
	PxPairFlags&				pairFlags,
	const void*					constantBlock,
	PxU32						constantBlockSize)
{
	PxFilterFlags filterFlags = PxDefaultSimulationFilterShader(attributes0,
		filterData0, attributes1, filterData1, pairFlags, constantBlock, constantBlockSize);
	if (pairFlags & PxPairFlag::eSOLVE_CONTACT)
	{
		if (filterData0.word1 & CollisionFeedbackFull || filterData1.word1 & CollisionFeedbackFull)
		{
			pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_PERSISTS | PxPairFlag::eNOTIFY_TOUCH_LOST | PxPairFlag::eDETECT_DISCRETE_CONTACT | PxPairFlag::eNOTIFY_CONTACT_POINTS;
		}
		else if (filterData0.word1 & CollisionSingle || filterData1.word1 & CollisionSingle)
		{
			pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eDETECT_DISCRETE_CONTACT | PxPairFlag::eNOTIFY_CONTACT_POINTS;
		}
	}
	return filterFlags;
}



__ImplementAbstractClass(PhysX::PhysXScene, 'PXSC', Physics::BaseScene);

//------------------------------------------------------------------------------
/**
*/
PhysXScene::PhysXScene():
	scene(NULL)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
PhysXScene::~PhysXScene()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
PhysXScene::Attach(const Ptr<Physics::PhysicsObject> & obj)
{
	if (obj->IsA(PhysX::PhysXProbe::RTTI))
	{
		this->triggers.InsertSorted(obj.cast<PhysX::PhysXProbe>());
	}
	BaseScene::Attach(obj);
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXScene::Detach(const Ptr<Physics::PhysicsObject> & obj)
{
	if (obj->IsA(PhysX::PhysXProbe::RTTI))
	{
		IndexT idx = this->triggers.BinarySearchIndex(obj.cast<PhysX::PhysXProbe>());
		n_assert(idx != InvalidIndex);
		this->triggers.EraseIndex(idx);
	}
	BaseScene::Detach(obj);
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXScene::RenderDebug()
{
	n_error("not implemented");
}

//------------------------------------------------------------------------------
/**
*/
Util::Array<Ptr<Physics::Contact>>
PhysXScene::RayCheck(const Math::vector& pos, const Math::vector& dir, const FilterSet& excludeSet, RayTestType rayType)
{	
	Math::vector ndir = Math::vector::normalize3(dir);
	PxRaycastBuffer hits;
	Util::Array<Ptr<Physics::Contact>> contacts;
	//FIXME setup filter
	PxQueryFilterData filter;
	filter.flags = PxQueryFlag::eANY_HIT;
	if (this->scene->raycast(Neb2PxVec(pos), Neb2PxVec(ndir), dir.length3(), hits, PxHitFlag::eDEFAULT, filter))
	{
		Ptr<Physics::Contact> contact = Physics::Contact::Create();		
		const PxRaycastHit & hit = hits.touches[0];
		contact->SetPoint(Px2NebPoint(hit.position));
		contact->SetType(Physics::BaseContact::Type::RayCheck);
		contact->SetCollisionObject((Physics::PhysicsObject*)hit.actor->userData);
		contacts.Append(contact);
	}
	return contacts;
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXScene::SetGravity(const Math::vector& v)
{
	this->scene->setGravity(Neb2PxVec(v));
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXScene::AttachStatic(const Ptr<Physics::PhysicsObject> &obj)
{
	n_error("not implemented");
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXScene::OnActivate()
{	
	BaseScene::OnActivate();
	PxSceneDesc sceneDesc(PhysXServer::Instance()->physics->getTolerancesScale());
	sceneDesc.gravity = Neb2PxVec(this->GetGravity());
	this->dispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = this->dispatcher;
	sceneDesc.filterShader = Simulationfilter;
	this->scene = PhysXServer::Instance()->physics->createScene(sceneDesc);	
	this->scene->setSimulationEventCallback(PhysXServer::Instance());
	this->time = -1.0f;
	this->controllerManager= PxCreateControllerManager(*this->scene);	
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXScene::OnDeactivate()
{
	BaseScene::OnDeactivate();	
	this->controllerManager->release();
	this->scene->release();
	this->dispatcher->release();
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXScene::Trigger()
{	
	const float& simulationFrameTime = PhysXServer::Instance()->GetSimulationFrameTime();

	if (this->time < 0.0f)
	{
		// first frame, simulate one step
		this->time = PhysXServer::Instance()->GetTime()+ simulationFrameTime;
		this->scene->simulate(simulationFrameTime);		
		// for now be synchronous
		this->scene->fetchResults(true);
	}
	else
	{		
		Timing::Time delta = this->simulationSpeed * (PhysXServer::Instance()->GetTime() - this->time);
		if (delta > simulationFrameTime)
		{
			// clear triggers
			for (int i = 0;i < this->triggers.Size();i++)
			{
				this->triggers[i]->ClearOverlap();
			}			
		}
		while (delta > simulationFrameTime)
		{
			this->scene->simulate(simulationFrameTime);
			this->scene->fetchResults(true);
			delta -= simulationFrameTime;
			this->time += simulationFrameTime;
		}
	}		
}

//------------------------------------------------------------------------------
/**
*/
void 
PhysXScene::AddIgnoreCollisionPair(const Ptr<Physics::PhysicsBody>& bodyA, const Ptr<Physics::PhysicsBody>& bodyB)
{
	n_error("BaseScene::AddIgnoreCollisionPair: Do not use the base method, implement and use the method from a sub class from this");
}

//------------------------------------------------------------------------------
/**
*/
void
PhysXScene::CollisionEnable(Physics::CollideCategory a, Physics::CollideCategory b, bool enable)
{
	PxSetGroupCollisionFlag(a, b, enable);
}

//------------------------------------------------------------------------------
/**
*/
bool
PhysXScene::IsCollisionEnabled(Physics::CollideCategory a, Physics::CollideCategory b)
{
	return PxGetGroupCollisionFlag(a, b);
}

}