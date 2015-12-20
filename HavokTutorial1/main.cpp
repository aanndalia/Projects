#include "HavokUtilities.hpp"

// Keycode
#include <Common/Base/keycode.cxx>

// Classlists
#define INCLUDE_HAVOK_PHYSICS_CLASSES
#define HK_CLASSES_FILE <Common/Serialize/Classlist/hkClasses.h>
#include <Common/Serialize/Util/hkBuiltinTypeRegistry.cxx>

// Generate a custom list to trim memory requirements
#define HK_COMPAT_FILE <Common/Compat/hkCompatVersions.h>
#include <Common/Compat/hkCompat_None.cxx>

void addFixedSurface(hkpWorld* world, const hkVector4& position, const hkVector4& dimensions);

/*
Havok Physics Tutorial 1 console application.
Author: Piotr Pluta
http://piotrpluta.opol.pl
June 2009
*/

int main(int argc, const char** argv)
{
	HavokUtilities* havokUtilities = new HavokUtilities();
	havokUtilities->registerVisualDebugger();
	addFixedSurface(havokUtilities->getWorld(), hkVector4(0,0,0), 
				    hkVector4(50.0f,1.0f,50.0f));

	//Add ball
	hkpRigidBody* g_ball;
	const hkReal radius = 1.5f;
	const hkReal sphereMass = 150.0f;

	hkVector4 relPos( 0.0f,radius + 0.0f, 50.0f );

	hkVector4 groundPos( 0.0f, 200.0f, -30.0f );
	hkVector4 posy = groundPos;

	hkpRigidBodyCinfo info;
	hkpMassProperties massProperties;
	hkpInertiaTensorComputer::computeSphereVolumeMassProperties(radius, sphereMass, massProperties);

	info.m_mass = massProperties.m_mass;
	info.m_centerOfMass  = massProperties.m_centerOfMass;
	info.m_inertiaTensor = massProperties.m_inertiaTensor;
	info.m_shape = new hkpSphereShape( radius );
	info.m_position.setAdd4(posy, relPos );
	info.m_motionType  = hkpMotion::MOTION_BOX_INERTIA;
	//info.m_gravity.set(0,-9.8f,0);

	info.m_qualityType = HK_COLLIDABLE_QUALITY_BULLET;
	//info.m_qualityType = HK_COLLIDABLE_QUALITY_MOVING;

	hkpRigidBody* sphereRigidBody = new hkpRigidBody( info );
	g_ball = sphereRigidBody;

	havokUtilities->getWorld()->addEntity( sphereRigidBody );
	sphereRigidBody->removeReference();
	info.m_shape->removeReference();

	hkVector4 vel(  0.0f,4.9f, -100.0f );
	//sphereRigidBody->setLinearVelocity( vel );
	//sphereRigidBody->setGravityFactor(1.8f);
	sphereRigidBody->setAngularDamping(1.0f);
	//sphereRigidBody->setLinearDamping(0.0f);


	//Add ball2
	hkpRigidBody* g_ball2;
	const hkReal radius2 = 1.5f;
	const hkReal sphereMass2 = 150.0f;

	hkVector4 relPos2( 0.0f,radius2 + 0.0f, 50.0f );

	hkVector4 groundPos2( 0.0f, 100.0f, -30.1f );
	hkVector4 posy2 = groundPos2;

	hkpRigidBodyCinfo info2;
	hkpMassProperties massProperties2;
	hkpInertiaTensorComputer::computeSphereVolumeMassProperties(radius2, sphereMass2, massProperties2);

	info2.m_mass = massProperties2.m_mass;
	info2.m_centerOfMass  = massProperties2.m_centerOfMass;
	info2.m_inertiaTensor = massProperties2.m_inertiaTensor;
	info2.m_shape = new hkpSphereShape( radius2 );
	info2.m_position.setAdd4(posy2, relPos2 );
	info2.m_motionType  = hkpMotion::MOTION_BOX_INERTIA;
	//info.m_gravity.set(0,-9.8f,0);

	//info2.m_qualityType = HK_COLLIDABLE_QUALITY_BULLET;
	info2.m_qualityType = HK_COLLIDABLE_QUALITY_BULLET;

	hkpRigidBody* sphereRigidBody2 = new hkpRigidBody( info2 );
	g_ball2 = sphereRigidBody2;

	havokUtilities->getWorld()->addEntity( sphereRigidBody2 );
	sphereRigidBody2->removeReference();
	info2.m_shape->removeReference();

	//hkVector4 vel2(  0.0f,4.9f, -100.0f );
	//sphereRigidBody->setLinearVelocity( vel2 );
	//sphereRigidBody2->setGravityFactor(1.8f);
	//sphereRigidBody2->setFriction(1.0f,-200.0f);
	sphereRigidBody2->setAngularDamping(1.0f);
	//sphereRigidBody2->setLinearDamping(0.0f);
	

	//add Cube

	//create box shape using given dimensions
	hkVector4 cube_dim(2.0f,2.0f,2.0f);
	hkpConvexShape* shape = new hkpBoxShape(cube_dim,0);

	//create rigid body information structure 
	hkpRigidBodyCinfo rigidBodyInfo;
	
	//MOTION_FIXED means static element in game scene
	//rigidBodyInfo.m_motionType = hkpMotion::MOTION_FIXED;
	rigidBodyInfo.m_motionType  = hkpMotion::MOTION_BOX_INERTIA;
	rigidBodyInfo.m_shape = shape;
	hkVector4 position( 0.0f, 300.0f, -30.2f );
	rigidBodyInfo.m_position = position;
	rigidBodyInfo.m_qualityType = HK_COLLIDABLE_QUALITY_BULLET;

	//rigidBodyInfo.m_friction = 2000.0;

	//create new rigid body with supplied info
	hkpRigidBody* rigidBody = new hkpRigidBody(rigidBodyInfo);

	havokUtilities->getWorld()->addEntity( rigidBody );
	rigidBody->removeReference();
	rigidBodyInfo.m_shape->removeReference();

	//rigidBody->setGravityFactor(1.8f);
	//hkVector4 vel2(  0.0f,4.9f, -100.0f );
	//sphereRigidBody->setLinearVelocity( vel2 );
	//sphereRigidBody2->setGravityFactor(1.8f);
	//sphereRigidBody2->setFriction(1.0f,-200.0f);
	//sphereRigidBody2->setAngularDamping(1.0f);
	//sphereRigidBody2->setLinearDamping(0.0f);

	//create watch object - we will simulate for 30 seconds of real time
	hkStopwatch stopWatch;
	stopWatch.start();
	hkReal lastTime = stopWatch.getElapsedSeconds();

	//initialize fixed time step - one step every 1/60 of a second
	hkReal timestep = 1.f / 60.f;
	//calculate number of steps for entire loop 
	//(30 seconds divided by single time step)
	
	int numSteps = int(30.f / timestep);
	hkVector4 axis;
	hkReal quat[4][4];
	hkReal quat2[4][4];
	hkReal quat3[4][4];
	//application loop, breaks after 15 real time seconds
	for ( int i = 0; i < numSteps; ++i )
	{
		//step the simulation and VDB
		havokUtilities->stepSimulation(timestep);
		havokUtilities->stepVisualDebugger(timestep);

		//pause until the actual time has passed
		while (stopWatch.getElapsedSeconds() < lastTime + timestep);
		{	
			lastTime += timestep;
		}

		//sphereRigidBody->getRotation().m_vec.getMajorAxis()
		//printf("Rotation - ball1: (%lf, %lf, %lf) ball2: (%lf, %lf, %lf)\n", sphereRigidBody->getRotation().m_vec.getQuad().x, sphereRigidBody->getRotation().m_vec.getQuad().y, sphereRigidBody->getRotation().m_vec.getQuad().z, sphereRigidBody2->getRotation().m_vec.getQuad().x, sphereRigidBody2->getRotation().m_vec.getQuad().y, sphereRigidBody2->getRotation().m_vec.getQuad().z);
		sphereRigidBody->getRotation().getAxis(axis);
		sphereRigidBody2->getRotation().getAxis(axis);
		sphereRigidBody->getTransform().get4x4ColumnMajor(*quat);
		sphereRigidBody2->getTransform().get4x4ColumnMajor(*quat2);
		rigidBody->getTransform().get4x4ColumnMajor(*quat3); //Added by BRIAN!!
		//printf("%lf %lf %lf %lf\n%lf %lf %lf %lf\n%lf %lf %lf %lf\n%lf %lf %lf %lf %lf\n\n", quat[0][0], quat[0][1], quat[0][2], quat[0][3], quat[1][0], quat[1][1], quat[1][2], quat[1][3], quat[2][0], quat[2][1], quat[2][2], quat[2][3], quat[3][0], quat[3][1], quat[3][2], quat[3][3], quat[4][0]);
		//printf("%lf %lf %lf %lf\n%lf %lf %lf %lf\n%lf %lf %lf %lf\n%lf %lf %lf %lf\n\n", quat2[0][0], quat2[0][1], quat2[0][2], quat2[0][3], quat2[1][0], quat2[1][1], quat2[1][2], quat2[1][3], quat2[2][0], quat2[2][1], quat2[2][2], quat2[2][3], quat2[3][0], quat2[3][1], quat2[3][2], quat2[3][3]);
		printf("%lf %lf %lf %lf\n%lf %lf %lf %lf\n%lf %lf %lf %lf\n%lf %lf %lf %lf\n\n", quat3[0][0], quat3[0][1], quat3[0][2], quat3[0][3], quat3[1][0], quat3[1][1], quat3[1][2], quat3[1][3], quat3[2][0], quat3[2][1], quat3[2][2], quat3[2][3], quat3[3][0], quat3[3][1], quat3[3][2], quat3[3][3]);
		//printf("%lf\n", quat[0][0]);
		//printf("axis = %lf %lf %lf\n", axis.getQuad().x, axis.getQuad().y, axis.getQuad().z);
		//printf("angle = %lf\n", (sphereRigidBody->getRotation().getAngle())*180/3.141592654);
		//printf("m_vec = (%lf, %lf, %lf)\n", (sphereRigidBody->getRotation()(0), sphereRigidBody->getRotation()(1), sphereRigidBody->getRotation()(2)));
		//printf("major axis = %d\n", sphereRigidBody->getPosition().getMajorAxis());
		//printf("ball1: (%lf, %lf, %lf)\tball2: (%lf, %lf, %lf)\n", sphereRigidBody->getPosition().getQuad().x, sphereRigidBody->getPosition().getQuad().y, sphereRigidBody->getPosition().getQuad().z, sphereRigidBody2->getPosition().getQuad().x, sphereRigidBody2->getPosition().getQuad().y, sphereRigidBody2->getPosition().getQuad().z);
	}
	//printf("ball1: (%lf, %lf, %lf)\tball2: (%lf, %lf, %lf)\n", info.m_position.getQuad().x, info.m_position.getQuad().y, info.m_position.getQuad().z, info2.m_position.getQuad().x, info2.m_position.getQuad().y, info2.m_position.getQuad().z); 
	//sphereRigidBody->removeReference();
	//info.m_shape->removeReference();
	//sphereRigidBody2->removeReference();
	//info2.m_shape->removeReference();
	getchar();
	delete havokUtilities;
}

void addFixedSurface(hkpWorld* world, const hkVector4& position, 
					 const hkVector4& dimensions)
{
	//addFixedSurface function
	//creates fixed surface with specified position and dimensions

	//create box shape using given dimensions
	hkpConvexShape* shape = new hkpBoxShape(dimensions,0);

	//create rigid body information structure 
	hkpRigidBodyCinfo rigidBodyInfo;
	
	//MOTION_FIXED means static element in game scene
	rigidBodyInfo.m_motionType = hkpMotion::MOTION_FIXED;
	rigidBodyInfo.m_shape = shape;
	rigidBodyInfo.m_position = position;
	//rigidBodyInfo.m_friction = 2000.0;

	//create new rigid body with supplied info
	hkpRigidBody* rigidBody = new hkpRigidBody(rigidBodyInfo);

	//rigidBody->setFriction(1000.0f);

	//add rigid body to physics world
	world->lock();
	world->addEntity(rigidBody);

	//decerase reference counter for rigid body and shape
	rigidBody->removeReference();
	shape->removeReference();

	world->unlock();
}