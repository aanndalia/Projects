#ifndef _HAVOK_UTILITIES_
#define _HAVOK_UTILITIES_

#include <Common/Base/hkBase.h>
#include <Common/Base/DebugUtil/StatisticsCollector/hkStatisticsCollector.h>
#include <Common/Base/Memory/Memory/hkMemory.h>
#include <Common/Base/Memory/hkThreadMemory.h>
#include <Common/Base/Memory/Memory/Pool/hkPoolMemory.h>
#include <Common/Base/Monitor/hkMonitorStream.h>
#include <Common/Base/System/hkBaseSystem.h>
#include <Common/Base/System/Error/hkDefaultError.h>
#include <Common/Base/System/Stopwatch/hkStopwatch.h>
#include <Common/Base/Thread/CriticalSection/hkCriticalSection.h>
#include <Common/Base/Thread/Job/ThreadPool/Cpu/hkCpuJobThreadPool.h>
#include <Common/Base/Thread/Job/ThreadPool/Spu/hkSpuJobThreadPool.h>
#include <Common/Base/Thread/JobQueue/hkJobQueue.h>

#include <Physics/Collide/hkpCollide.h>		
#include <Physics/Collide/Agent/ConvexAgent/SphereBox/hkpSphereBoxAgent.h>		
#include <Physics/Collide/Filter/Group/hkpGroupFilter.h>
#include <Physics/Collide/Filter/Group/hkpGroupFilterSetup.h>
#include <Physics/Collide/Dispatch/hkpAgentRegisterUtil.h>
#include <Physics/Collide/Dispatch/hkpCollisionDispatcher.h>
#include <Physics/Collide/Query/CastUtil/hkpWorldRayCastInput.h>			
#include <Physics/Collide/Query/CastUtil/hkpWorldRayCastOutput.h>	
#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics/Collide/Shape/Convex/Sphere/hkpSphereShape.h>
#include <Physics/Collide/Shape/Convex/Triangle/hkpTriangleShape.h>
#include <Physics/Collide/Shape/Convex/Capsule/hkpCapsuleShape.h>
#include <Physics/Collide/Shape/Convex/ConvexVertices/hkpConvexVerticesShape.h>
#include <Physics/Collide/Shape/Compound/Tree/Mopp/hkpMoppBvTreeShape.h>
#include <Physics/Collide/Shape/Compound/Tree/Mopp/hkpMoppUtility.h>
#include <Physics/Collide/Shape/Compound/Collection/ExtendedMeshShape/hkpExtendedMeshShape.h>
#include <Physics/Collide/Shape/Compound/Collection/List/hkpListShape.h>
#include <Physics/Collide/Shape/HeightField/TriSampledHeightField/hkpTriSampledHeightFieldBvTreeShape.h>
#include <Physics/Collide/Shape/HeightField/TriSampledHeightField/hkpTriSampledHeightFieldCollection.h>
#include <Physics/Dynamics/World/hkpWorld.h>
#include <Physics/Dynamics/Entity/hkpRigidBody.h>
#include <Physics/Dynamics/Phantom/hkpPhantom.h>
#include <Physics/Dynamics/Phantom/hkpSimpleShapePhantom.h>
#include <Physics/Dynamics/World/Simulation/hkpSimulation.h>
#include <Physics/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h>

#include <Common/Visualize/hkVisualDebugger.h>
#include <Physics/Utilities/VisualDebugger/hkpPhysicsContext.h>

#include <stdio.h>
static void HK_CALL errorReportFunction(const char* msg, void*)
{
	printf("%s", msg);
}

/*
Class HavokUtilities
Author: Piotr Pluta
http://piotrpluta.opol.pl
June 2009

Class used to encapsulate Havok Physics data and initialize Havok engine. 
Only one instance is needed (refactor to singleton if desired)

Interface overview:
    
- HavokUtilities() sets default timestep and calls private initHavok() method (data initialization).
- ~HavokUtilities() calls deinitHavok(). This way, if HavokUtilities object is destroyed, all resources assigned by it will be freed.
- registerVisualDebugger() sets up Visual Debugger. If you want to use VD, you have to call this after creating HavokUtilities object
- stepSimulation(float dt) and stepVisualDebugger(float dt) methods have to be called inside some loop. They tell the simulation and VD to step forward by delta time dt.
- getWorld() returns pointer to hkpWorld (Havok world instance)
- getVisualDebugger() returns pointer to hkVisualDebugger (VD instance)

Usage:

//Create class instance. Doing this initializes Havok automatically.
HavokUtilities* havokUtilities = new HavokUtilities();

//Register Havok Visual Debugger(if needed)
havokUtilities->registerVisualDebugger()

//Add something to simulation here (use getWorld() to access physics world)

//Step simulation and visual debugger in a loop
havokUtilities->stepSimulation(timestep);
havokUtilities->stepVisualDebugger(timestep);

//delete HavokUtilities. All resources aquired during initialization will be released
delete havokUtilities;
*/

class HavokUtilities
{
public:
	HavokUtilities(void);
	virtual ~HavokUtilities(void);
	
	void registerVisualDebugger();

	void stepSimulation(float dt);	
	void stepVisualDebugger(float dt);

	hkpWorld* getWorld();
	hkVisualDebugger* getVisualDebugger();
	
private:
	void initHavok();
	void deinitHavok();

	bool m_visualDebuggerActive;
	
	hkpWorld* m_world;
	hkVisualDebugger* m_vdb;
	hkpPhysicsContext* m_context;

	hkJobThreadPool* m_threadPool;
	hkThreadMemory* m_threadMemory;
	hkJobQueue* m_jobQueue;
	char* m_stackBuffer;
};

#endif