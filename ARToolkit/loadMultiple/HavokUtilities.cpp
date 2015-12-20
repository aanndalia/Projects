#include "HavokUtilities.hpp"

HavokUtilities::HavokUtilities(void)
{
	m_visualDebuggerActive = false;

	initHavok();
}

HavokUtilities::~HavokUtilities(void)
{
	deinitHavok();
}

void HavokUtilities::initHavok()
{	
	//initialize memory system
	hkPoolMemory* memoryManager = new hkPoolMemory();
	m_threadMemory = new hkThreadMemory(memoryManager);
	hkBaseSystem::init(memoryManager, m_threadMemory, errorReportFunction);
	memoryManager->removeReference();
	
	//stack area: 200k (fast temporary memory to be used by the engine)
	int stackSize = 0x200000;
	m_stackBuffer = hkAllocate<char>(stackSize, HK_MEMORY_CLASS_BASE);
	hkThreadMemory::getInstance().setStackArea(m_stackBuffer, stackSize);

	//get hardware info and initialize total number of threads used
	int totalNumThreadsUsed;
	hkHardwareInfo hwInfo;
	hkGetHardwareInfo(hwInfo);
	totalNumThreadsUsed = hwInfo.m_numThreads;

	//initialize thread pool (totalNumThreadsUsed- 1, one thread used for physics world simulation)
	hkCpuJobThreadPoolCinfo threadPoolCinfo;
	threadPoolCinfo.m_numThreads = totalNumThreadsUsed - 1;

	//enable timers collection, allocating 200 Kb per thread
	threadPoolCinfo.m_timerBufferPerThreadAllocation = 200000;
	m_threadPool = new hkCpuJobThreadPool( threadPoolCinfo );

	//create a job queue. This job queue will be used by all Havok modules to run multithreaded work.
	//In following tutorial it is used only for physics simulation
	hkJobQueueCinfo jobQueueCinfo;
	jobQueueCinfo.m_jobQueueHwSetup.m_numCpuThreads = totalNumThreadsUsed;
	m_jobQueue = new hkJobQueue(jobQueueCinfo);

	//Enable monitors for the thread.
	//Monitors have been enabled above for thread pool threads 
	hkMonitorStream::getInstance().resize(200000);



	//initialize physics world - create world info
	hkpWorldCinfo info;

	//set simualation type to multithreaded
	info.m_simulationType = hkpWorldCinfo::SIMULATION_TYPE_MULTITHREADED;

	//objects that fall "out of the world" will be automatically removed
	info.m_broadPhaseBorderBehaviour = hkpWorldCinfo::BROADPHASE_BORDER_REMOVE_ENTITY;
	
	//standard gravity settings, collision tolerance and world size 
	info.m_gravity.set(0,-9.8f,0);
	info.m_collisionTolerance = 0.1f;; 
	info.setBroadPhaseWorldSize(5000.0f);

	//initialize world with created info
	m_world = new hkpWorld(info);

	//When the simulation type is SIMULATION_TYPE_MULTITHREADED, in the debug build, the sdk performs checks
	//to make sure only one thread is modifying the world at once to prevent multithreaded bugs. Each thread
	//must call markForRead / markForWrite before it modifies the world to enable these checks.
	m_world->markForWrite();

	//Register all collision agents
	//It's important to register collision agents before adding any entities to the world.
	hkpAgentRegisterUtil::registerAllAgents(m_world->getCollisionDispatcher());
	
	//register physics world with job queue
	m_world->registerWithJobQueue(m_jobQueue);

	//world no loger modified - unmark it
	m_world->unmarkForWrite();
}

void HavokUtilities::deinitHavok()
{
	//clean up physics world
	m_world->markForWrite();
	m_world->removeReference();

	//clean up visual debugger(if used)
	if(m_visualDebuggerActive == true)
	{
		m_vdb->removeReference();
		m_context->removeReference();
	}

	//delete job queue, thread poll, deallocate memory
	delete m_jobQueue;
	m_threadPool->removeReference();
	m_threadMemory->setStackArea(0, 0);
	hkDeallocate(m_stackBuffer);
	m_threadMemory->removeReference();
	m_threadMemory = HK_NULL;

	//quit base system
	hkBaseSystem::quit();
}

void HavokUtilities::registerVisualDebugger()
{
	if(!m_visualDebuggerActive)
	{
		// Create context for Visual Debugger
		m_context = new hkpPhysicsContext();

		//Reigster all the physics viewers
		hkpPhysicsContext::registerAllPhysicsProcesses();

		//Add physics world to context, so viewers can see it
		m_world->markForWrite();
		m_context->addWorld(m_world);
		m_world->unmarkForWrite();

		hkArray<hkProcessContext*> contexts;
		contexts.pushBack(m_context);

		//Create VDB instance
		m_vdb = new hkVisualDebugger(contexts);
		m_vdb->serve();

		m_visualDebuggerActive = true;
	}
}

void HavokUtilities::stepSimulation(float dt)
{
	//step multithreaded simulation using this thread and all threads in the thread pool
	m_world->stepMultithreaded(m_jobQueue, m_threadPool, dt);

	hkMonitorStream::getInstance().reset();
	m_threadPool->clearTimerData();		
}

void HavokUtilities::stepVisualDebugger(float dt)
{
	if(m_visualDebuggerActive)
	{
		//synchronize the timer data and step Visual Debugger
		m_context->syncTimers( m_threadPool );
		m_vdb->step();
	}	
}

hkpWorld* HavokUtilities::getWorld()
{
	return m_world;
}
hkVisualDebugger* HavokUtilities::getVisualDebugger()
{
	return m_vdb;
}

	