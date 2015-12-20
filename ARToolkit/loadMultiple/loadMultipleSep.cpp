#ifdef _WIN32
#  include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef __APPLE__
#  include <GL/glut.h>
#else
#  include <GLUT/glut.h>
#endif
#include <AR/gsub.h>
#include <AR/param.h>
#include <AR/ar.h>
#include <AR/video.h>
#include <vector>

extern "C"{
#include "object.h"
}
#include "HavokUtilities.hpp"
#include <Physics/Utilities/Constraint/Keyframe/hkpKeyFrameUtility.h> //added, don't know if matters


// Keycode
#include <Common/Base/keycode.cxx>

// Classlists
#define INCLUDE_HAVOK_PHYSICS_CLASSES
#define HK_CLASSES_FILE <Common/Serialize/Classlist/hkClasses.h>
#include <Common/Serialize/Util/hkBuiltinTypeRegistry.cxx>

// Generate a custom list to trim memory requirements
#define HK_COMPAT_FILE <Common/Compat/hkCompatVersions.h>
#include <Common/Compat/hkCompat_None.cxx>

#define COLLIDE_DIST 30000.0

/* Object Data */
char            *model_name = "Data/object_data3";
struct ObjectData_T    *object;
int             objectnum;

int             xsize, ysize;
int				thresh = 100;
int             count = 0;
int				delta_x = 0; //variable for tracking x-axis movement
int				delta_y = 40; //variable for tracking y-acis movement
int				xdir = 0;
int				ydir = 1;
int				dirChanged = 0;
int				north = 0;
int				east = 0;
int objectAdded;
int numBalls=0;
int numCubes=0;
int numelem=0;
int numalloc=0;
double (*wmatstor)[3];

int occluded = 1;

/* for havok*/
hkReal quat[4][4];
hkReal ourQuat[4][4];
hkReal initQuat[16];
double ARQuat[3][4];
double glutQuat[16];

HavokUtilities* havokUtilities;
hkpKeyFrameUtility *keyFrameUtil;
std::vector<hkpRigidBody *> ballVector;
std::vector<hkpRigidBody *> cubeVector;
hkpRigidBody *ourCube;
hkpRigidBody* g_ball;
hkpRigidBody* vel_ball;
hkStopwatch stopWatch;
hkReal lastTime = 0;
hkReal diff = 0;
hkReal timestep = 1.f / 6.f;
int ourCubeAdded=0;
int kUsed = 0;
//FILE *pFile;

/* set up the video format globals */

#ifdef _WIN32
char			*vconf = "Data\\WDM_camera_flipV.xml";
#else
char			*vconf = "";
#endif

char           *cparam_name    = "Data/camera_para.dat";
ARParam         cparam;

static void   init(void);
static void   cleanup(void);
static void   keyEvent( unsigned char key, int x, int y);
static void   mainLoop(void);
static int draw(struct ObjectData_T *object, int objectnum );
static int  draw_object( int obj_id, double gl_para[16] );
void addHavokSphere(hkReal xPos, hkReal yPos, hkReal zPos);
void addFixedSurface(const hkVector4& position, const hkVector4& dimensions);
void addHavokCube(hkReal dimX,hkReal dimY,hkReal dimZ, hkReal posX, hkReal posY, hkReal posZ);
void addOurCube();
void addVelocityBall(hkReal radius, hkReal posX, hkReal posY, hkReal posZ);

int AddToArray(double wmat[3][4]){
	if(numelem==numalloc){

		
		int numallocb = numalloc*2;


		int tmpSize = numallocb * sizeof(double)*3;

		void * _tmp = realloc(wmatstor,tmpSize);
		if (!_tmp){
            fprintf(stderr, "ERROR: Couldn't realloc memory!\n");
            return(-1);
        }
		wmatstor = (double(*)[3])_tmp;
		numalloc=numallocb;
	}
		wmatstor[numelem][0]=wmat[0][3];
		wmatstor[numelem][1]=wmat[1][3];
		wmatstor[numelem][2]=wmat[2][3];
		numelem++;
		
		return numelem;
}


int main(int argc, char **argv)
{
	//initialize applications
	HavokUtilities* havokUtil = new HavokUtilities();
	havokUtilities = havokUtil;
	keyFrameUtil = new hkpKeyFrameUtility();
	glutInit(&argc, argv);
    init();
	//pFile = fopen ("myfile.txt","w");

	
	arVideoCapStart();
	havokUtilities->registerVisualDebugger();
	addFixedSurface(hkVector4(0,0,0), hkVector4(200.0f,1.0f,200.0f));
	//stopWatch.start();
	
	//start the main event loop
    argMainLoop( NULL, keyEvent, mainLoop );
	delete havokUtilities;
	return 0;
}

static void   keyEvent( unsigned char key, int x, int y)   
{
    /* quit if the ESC key is pressed */
    if( key == 0x1b ) {
        printf("*** %f (frame/sec)\n", (double)count/arUtilTimer());
        cleanup();
        exit(0);
    }
	if(key=='c'){
		if(dirChanged == 0)
			dirChanged = 1;
		else
			dirChanged = 0;
	}
	if(key=='w')
		north++;
	if(key=='s')
		north--;
	if(key=='a')
		east--;
	if(key=='d')
		east++;
	if(key=='h'){
		objectAdded=1;
		hkReal offset=-1.0*(numBalls + numCubes);
		//hkReal offset=-1*pow(2,numBalls);
		addHavokSphere(offset,200.0f,0.0f);
		numBalls++;
		printf("Adding sphere.\n");
	}
	if(key=='r'){
		ballVector.clear();
		cubeVector.clear();
		numBalls=0;
		numCubes=0;
		ourCubeAdded=0;
		objectAdded=0;
		delete havokUtilities;
		HavokUtilities* havokUtil = new HavokUtilities();
		havokUtilities = havokUtil;
		havokUtilities->registerVisualDebugger();
		//havokUtilities->getWorld()->
		/*
		havokUtilities->
		addFixedSurface(hkVector4(0,0,0), hkVector4(200.0f,1.0f,200.0f));
		*/
		//delete havokUtilities;
		//HavokUtilities* havokUtil = new HavokUtilities();
		//havokUtilities = havokUtil;
		//havokUtilities->registerVisualDebugger();
		addFixedSurface(hkVector4(0,0,0), hkVector4(200.0f,1.0f,200.0f));
	}
	if(key=='b'){
		objectAdded=1;
		hkReal offset=-1.0*(numCubes * numBalls);
		addHavokCube(10.0f,10.0f,10.0f,offset,200.0f,0.0f);
		numCubes++;
		printf("Adding cube.\n");
	}
	if(key=='t'){
		objectAdded=1;
		addHavokCube(10.0f,10.0f,10.0f,0,200.0f,0.0f);
		addHavokSphere(0,200.0f,0.0f);
		numCubes++;
		printf("Adding cube.\n");
		//hkReal offset=-1*pow(2,numBalls);

		numBalls++;
		printf("Adding sphere.\n");
	}

	if(key=='k'){
		if(ourCubeAdded == 0){
			ourCubeAdded=1;
			addOurCube();
		}
	}
	if(key=='v'){
		addVelocityBall(5.0f,50.0f,50.0f,50.0f);
	}
	if(key=='z'){
		addHavokSphere(50.0f,20.0f,50.0f);
	}

}

/* main loop */
static void mainLoop(void)
{
    ARUint8         *dataPtr;
    ARMarkerInfo    *marker_info;
    int             marker_num;
    int             i,j,k;

    /* grab a video frame */
    if( (dataPtr = (ARUint8 *)arVideoGetImage()) == NULL ) {
        arUtilSleep(2);
        return;
    }
	
    if( count == 0 ) arUtilTimerReset();  
    count++;

	/*draw the video*/
    argDrawMode2D();
    argDispImage( dataPtr, 0,0 );

	glColor3f( 1.0, 0.0, 0.0 );
	glLineWidth(6.0);

	/* detect the markers in the video frame */ 
	if(arDetectMarker(dataPtr, thresh, 
		&marker_info, &marker_num) < 0 ) {
		cleanup(); 
		exit(0);
	}
	for( i = 0; i < marker_num; i++ ) {
		argDrawSquare(marker_info[i].vertex,0,0);
	}

	/* check for known patterns */
    for( i = 0; i < objectnum; i++ ) {
		k = -1;
		for( j = 0; j < marker_num; j++ ) {
	        if( object[i].id == marker_info[j].id) {

				/* you've found a pattern */
				//printf("Found pattern: %d ",patt_id);
				glColor3f( 0.0, 1.0, 0.0 );
				argDrawSquare(marker_info[j].vertex,0,0);

				if( k == -1 ) k = j;
		        else /* make sure you have the best pattern (highest confidence factor) */
					if( marker_info[k].cf < marker_info[j].cf ) k = j;
			}
		}
		if( k == -1 ) {
			object[i].visible = 0;
			continue;
		}
		
		/* calculate the transform for each marker */
		if( object[i].visible == 0 ) {
            arGetTransMat(&marker_info[k],
                          object[i].marker_center, object[i].marker_width,
                          object[i].trans);
        }
        else {
            arGetTransMatCont(&marker_info[k], object[i].trans,
                          object[i].marker_center, object[i].marker_width,
                          object[i].trans);
        }
        object[i].visible = 1;
	}
	
	arVideoCapNext();

	/* draw the AR graphics */
    draw( object, objectnum );

	/*swap the graphics buffers*/
	argSwapBuffers();

	//step the simulation and VDB
	if(objectAdded)
	{
		hkpWorldCinfo testInfo;
		//diff = stopWatch.getElapsedSeconds()-lastTime;
		//lastTime = stopWatch.getElapsedSeconds();
		//havokUtilities->stepSimulation(diff);
		//havokUtilities->stepVisualDebugger(diff);
		havokUtilities->stepSimulation(timestep);
		havokUtilities->stepVisualDebugger(timestep);
		havokUtilities->getWorld()->getCinfo(testInfo);
		hkVector4 testVect = testInfo.m_gravity;
		hkVector4 gravs = havokUtilities->getWorld()->getGravity();
		//printf("gravs: (%lf %lf %lf)\n",gravs.getQuad().x,gravs.getQuad().y,gravs.getQuad().z);
	}
}

static void init( void )
{
	ARParam  wparam;

    /* open the video path */
    if( arVideoOpen( vconf ) < 0 ) exit(0);
    /* find the size of the window */
    if( arVideoInqSize(&xsize, &ysize) < 0 ) exit(0);
    //printf("Image size (x,y) = (%d,%d)\n", xsize, ysize);

    /* set the initial camera parameters */
    if( arParamLoad(cparam_name, 1, &wparam) < 0 ) {
        printf("Camera parameter load error !!\n");
        exit(0);
    }
    arParamChangeSize( &wparam, xsize, ysize, &cparam );
    arInitCparam( &cparam );
    printf("*** Camera Parameter ***\n");
    arParamDisp( &cparam );

	/* load in the object data - trained markers and associated bitmap files */
    if( (object=read_ObjData(model_name, &objectnum)) == NULL ) exit(0);
    printf("Objectfile num = %d\n", objectnum);

    /* open the graphics window */
    argInit( &cparam, 2.0, 0, 0, 0, 0 );
}

/* cleanup function called when program exits */
static void cleanup(void)
{
	arVideoCapStop();
    arVideoClose();
    argCleanup();
}

/* draw the the AR objects */
static int draw(struct ObjectData_T *object, int objectnum )
{
    int     i;
    double  gl_para[16];
       
	glClearDepth( 1.0 );
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_LIGHTING);

    /* calculate the viewing parameters - gl_para */
	
	for( i = 0; i < objectnum; i++ ) {		
		if(i == 1)
			occluded = 1;
        if( object[i].visible == 0 ) continue;
        argConvGlpara(object[i].trans, gl_para);
        draw_object( object[i].id, gl_para);
    }
     
	glDisable( GL_LIGHTING );
    glDisable( GL_DEPTH_TEST );
	
    return(0);
}

/* draw the user object */
static int  draw_object( int obj_id, double gl_para[16])
{
    GLfloat   mat_ambient[]				= {0.0, 0.0, 1.0, 1.0};
	GLfloat   mat_ambient_collide[]     = {1.0, 0.0, 0.0, 1.0};
    GLfloat   mat_flash[]				= {0.0, 0.0, 1.0, 1.0};
	GLfloat   mat_flash_collide[]       = {1.0, 0.0, 0.0, 1.0};
    GLfloat   mat_flash_shiny[] = {50.0};
    GLfloat   light_position[]  = {100.0,-200.0,200.0,0.0};
    GLfloat   ambi[]            = {0.1, 0.1, 0.1, 0.1};
    GLfloat   lightZeroColor[]  = {0.9, 0.9, 0.9, 0.1};

	
	double wmat1[3][4], wmat2[3][4]; 
	int i, j;
 
    argDrawMode3D();
    argDraw3dCamera( 0, 0 );
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd( gl_para );

 	/* set the material */
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambi);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);

    glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);

	
	if( object[0].visible >= 0 && object[1].visible >= 0 ) {
    arUtilMatInv(object[1].trans, wmat1);
    arUtilMatMul(wmat1, object[0].trans, wmat2);
	/*
    for( j = 0; j < 3; j++ ) {

        for( i = 0; i < 4; i++ ) printf("%8.4f ", wmat2[j][i]);

        printf("\n");
    }

    printf("\n\n");*/
	}

	if(obj_id == 0){
		/*if((wmat2[0][3]-wmatstor[numelem-1][0])*(wmat2[0][3]-wmatstor[numelem-1][0])+
			(wmat2[1][3]-wmatstor[numelem-1][1])*(wmat2[1][3]-wmatstor[numelem-1][1])+
			(wmat2[2][3]-wmatstor[numelem-1][2])*(wmat2[2][3]-wmatstor[numelem-1][2])>1)
		{
			numelem=AddToArray(wmat2);
		}*/
		/*
		int numElemB = numelem;
		//numelem = numElemB+1;
	
		int a = 0;
		while(a<numelem){
			glTranslatef( wmatstor[a][0], wmatstor[a][1], wmatstor[a][2] );
			glutSolidSphere(20,100,100);
			a++;
		}
		*/
		/*
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash_collide);
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_collide);
		//draw a sphere 
		glTranslatef( delta_x, delta_y, 30.0 );
		glutSolidSphere(30,120,120);
		glTranslatef( 0.0, 0.0, 50.0);
		glutSolidSphere(20, 120, 120);
		if(delta_x == 40 && delta_y == 40){
			if(dirChanged==0){
				xdir = 1;
				ydir = 2;
			}
			else{
				xdir = 0;
				ydir = 1;
			}
			printf("corner1 xDir:%d yDir:%d dirChanged:%d\n", xdir, ydir, dirChanged);
		}
		if(delta_x == 40 && delta_y == -40){
			if(dirChanged==0){
				xdir = 2;
				ydir = 1;
			}
			else{
				xdir = 1;
				ydir = 2;
			}
			printf("corner2 xDir:%d yDir:%d\n", xdir, ydir);
		}
		if(delta_x == -40 && delta_y == -40){
			if(dirChanged==0){
				xdir = 1;
				ydir = 0;
			}
			else{
				xdir = 2;
				ydir = 1;
			}
			printf("corner3 xDir:%d yDir:%d\n", xdir, ydir);
		}
		if(delta_x == -40 && delta_y == 40){
			if(dirChanged==0){
				xdir = 0;
				ydir = 1;
			}
			else{
				xdir = 1;
				ydir = 0;
			}
			printf("corner4 xDir:%d yDir:%d\n", xdir, ydir);
		}

		if (ydir == 2){
			if (dirChanged==0)
				delta_y-=4;
			else
				delta_y+=4;
		}
		if (ydir == 0){
			if(dirChanged==0)
				delta_y+=4;
			else
				delta_y-=4;
		}
		if (xdir == 2){
			if(dirChanged==0)
				delta_x-=4;
			else
				delta_x+=4;
		}
		if (xdir == 0){
			if(dirChanged==0)
				delta_x+=4;
			else
				delta_x-=4;
		}
		*/
	}
	else if(obj_id == 1){
		//glGetFloatv(GL_MODELVIEW_MATRIX,initQuat);
		
		if(objectAdded){
			

			for(int i=0;i<numBalls;i++){
			/*	for(int k=0;k<16;k++){
					glutQuat[k]=-1*glutQuat[k];
				}*/
				//glMultMatrixd(glutQuat);
				ballVector[i]->getTransform().get4x4ColumnMajor(*quat);
			//printf("%lf %lf %lf %lf\n%lf %lf %lf %lf\n%lf %lf %lf %lf\n%lf %lf %lf %lf %lf\n\n", quat[0][0], quat[0][1], quat[0][2], quat[0][3], quat[1][0], quat[1][1], quat[1][2], quat[1][3], quat[2][0], quat[2][1], quat[2][2], quat[2][3], quat[3][0], quat[3][1], quat[3][2], quat[3][3], quat[4][0]);
				hkReal temp;
				for(int i=0;i<4;i++){
					for(int j=i+1;j<4;j++){
						temp = quat[j][i];
						quat[j][i]=quat[i][j];
						quat[i][j]=temp;
					}
				}

				for(int i=0;i<3;i++)
					for(int j=0;j<4;j++)
						ARQuat[i][j] = quat[i][j];
				


				argConvGlpara(ARQuat,glutQuat);
				glMultMatrixd(glutQuat);
				//printf("Shape Vector: %d \n", shapeVector[i]);
				//if(shapeVector[i]==0)
				//{
				//	glutSolidSphere(15,15,15);
				//	//printf("In sphere space. \n");
				//} 
				//else if(shapeVector[i]==1)
				//{
				//	glutSolidCube(20.0);
				//	//printf("In cube space. \n");
				//}
				glutSolidSphere(15,15,15);
				glLoadMatrixd( gl_para );
			}
			for(int i=0;i<numCubes;i++){
			/*	for(int k=0;k<16;k++){
					glutQuat[k]=-1*glutQuat[k];
				}*/
				//glMultMatrixd(glutQuat);
				cubeVector[i]->getTransform().get4x4ColumnMajor(*quat);
			//printf("%lf %lf %lf %lf\n%lf %lf %lf %lf\n%lf %lf %lf %lf\n%lf %lf %lf %lf %lf\n\n", quat[0][0], quat[0][1], quat[0][2], quat[0][3], quat[1][0], quat[1][1], quat[1][2], quat[1][3], quat[2][0], quat[2][1], quat[2][2], quat[2][3], quat[3][0], quat[3][1], quat[3][2], quat[3][3], quat[4][0]);
				hkReal temp;
				for(int i=0;i<4;i++){
					for(int j=i+1;j<4;j++){
						temp = quat[j][i];
						quat[j][i]=quat[i][j];
						quat[i][j]=temp;
					}
				}

				for(int i=0;i<3;i++)
					for(int j=0;j<4;j++)
						ARQuat[i][j] = quat[i][j];
				


				argConvGlpara(ARQuat,glutQuat);
				glMultMatrixd(glutQuat);
				//printf("Shape Vector: %d \n", shapeVector[i]);
				//if(shapeVector[i]==0)
				//{
				//	glutSolidSphere(15,15,15);
				//	//printf("In sphere space. \n");
				//}
				//else if(shapeVector[i]==1)
				//{
				//	glutSolidCube(20.0);
				//	//printf("In cube space. \n");
				//}
				glutSolidCube(20);
				glLoadMatrixd( gl_para );
			}
		}
		glLoadMatrixd( gl_para );
		if(object[0].visible==1){
			if(ourCubeAdded){
		 		argConvGlpara(wmat2,glutQuat);
				glMultMatrixd(glutQuat);
				glutSolidCube(40.0);
				for(int i=0;i<3;i++){
					for(int j=0;j<4;j++){
						ourQuat[i][j]=wmat2[i][j];
					}
				}
				ourQuat[3][0]=0;
				ourQuat[3][1]=0;
				ourQuat[3][2]=0;
				ourQuat[3][3]=1;
				hkReal temp2;
				for(int i=0;i<4;i++){
					for(int j=i+1;j<4;j++){
						temp2 = ourQuat[j][i];
						ourQuat[j][i]=ourQuat[i][j];
						ourQuat[i][j]=temp2;
					}
				}
				hkTransform ourTrans;
				ourTrans.set4x4ColumnMajor(*ourQuat);
				for( int i=0;i<4;i++)
				{
					for(int j=0;j<4;j++)
					{
						printf("%lf ",ourQuat[i][j]);
					}
					printf("\n");
				}

				//ourCube->setTransform(ourTrans);
				

			    //hkpKeyFrameUtility
				hkVector4 newPosit(ourQuat[3][0], ourQuat[3][1], ourQuat[3][2], 0);
				hkVector4 newRot1(ourQuat[0][0], ourQuat[0][1], ourQuat[0][2], 0);
				hkVector4 newRot2(ourQuat[1][0], ourQuat[1][1], ourQuat[1][2], 0);
				hkVector4 newRot3(ourQuat[2][0], ourQuat[2][1], ourQuat[2][2], 0);
				hkRotation *newRot = new hkRotation();
				newRot->setRows(newRot1,newRot2,newRot3);
				hkQuaternion newRotQuat(*newRot);
				keyFrameUtil->applyHardKeyFrame(newPosit,newRotQuat,100/timestep,ourCube);

				
				
				
				//applyHardKeyFrame
				

			}
		}
		//glutSolidSphere(50,15,15);

		/*
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash);
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
		// draw a cube 
		glTranslatef( east*4, north*4, 30.0 );
		glutSolidCube(60);
		occluded = 0;
		*/
	}
	else if(obj_id == 2){

		/*
		if(occluded == 1){
			glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash);
			glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
			// draw a BIGGER! cube 
			glTranslatef( 0.0, 0.0, 30.0 );
			glutSolidCube(120);
			printf("bigger");
			
		}
		else{
			glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash);
			glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
			// draw a cube 
			glTranslatef( east*4, north*4, 30.0 );
			glutSolidCube(60);
		}*/
	}
	
	if(ourCubeAdded && (object[0].visible==0 || object[1].visible==0))
	{
		hkVector4 vel(0.0f, 0.0f, 0.0f);
		ourCube->setLinearVelocity(vel);
		ourCube->setAngularVelocity(vel);
	}

    argDrawMode2D();

    return 0;
}

void addHavokCube(hkReal dimX,hkReal dimY,hkReal dimZ, hkReal posX, hkReal posY, hkReal posZ){
	//create box shape using given dimensions
	hkVector4 cube_dim(dimX,dimY,dimZ);
	hkpConvexShape* shape = new hkpBoxShape(cube_dim,0);

	//create rigid body information structure 
	hkpRigidBodyCinfo rigidBodyInfo;
	
	//MOTION_FIXED means static element in game scene
	//rigidBodyInfo.m_motionType = hkpMotion::MOTION_FIXED;
	rigidBodyInfo.m_motionType  = hkpMotion::MOTION_BOX_INERTIA;
	rigidBodyInfo.m_shape = shape;
	//hkVector4 position( 0.0f, 300.0f, -30.2f );
	hkVector4 position(posX,posY,posZ);
	rigidBodyInfo.m_position = position;
	rigidBodyInfo.m_qualityType = HK_COLLIDABLE_QUALITY_BULLET;

	//rigidBodyInfo.m_friction = 2000.0;

	//create new rigid body with supplied info
	hkpRigidBody* rigidBody = new hkpRigidBody(rigidBodyInfo);
	cubeVector.push_back(rigidBody);
	havokUtilities->getWorld()->addEntity( rigidBody );
	rigidBody->removeReference();
	rigidBodyInfo.m_shape->removeReference();

}

void addOurCube(){
	//create box shape using given dimensions
	hkVector4 cube_dim(40.0f,40.0f,40.0f);
	hkpConvexShape* shape = new hkpBoxShape(cube_dim,0);

	//create rigid body information structure 
	hkpRigidBodyCinfo rigidBodyInfo;
	
	//MOTION_FIXED means static element in game scene
	//rigidBodyInfo.m_motionType = hkpMotion::MOTION_FIXED;
	rigidBodyInfo.m_motionType  = hkpMotion::MOTION_BOX_INERTIA;
	rigidBodyInfo.m_shape = shape;
	//hkVector4 position( 0.0f, 300.0f, -30.2f );
	hkVector4 position(0.0f,200.0f,0.0f);
	rigidBodyInfo.m_position = position;
	//rigidBodyInfo.m_qualityType = HK_COLLIDABLE_QUALITY_BULLET;
	
	//special stuff
	//rigidBodyInfo.m_motionType = hkpMotion::MOTION_KEYFRAMED;
	rigidBodyInfo.m_qualityType = HK_COLLIDABLE_QUALITY_KEYFRAMED; //really not sure what difference these make
	//rigidBodyInfo.m_qualityType = HK_COLLIDABLE_QUALITY_MOVING;

	//rigidBodyInfo.m_friction = 2000.0;

	//create new rigid body with supplied info
	hkpRigidBody* rigidBody = new hkpRigidBody(rigidBodyInfo);
	ourCube=rigidBody;
	havokUtilities->getWorld()->addEntity( rigidBody );
	rigidBody->removeReference();
	rigidBodyInfo.m_shape->removeReference();

	//added
	ourCube->setMotionType(hkpMotion::MOTION_KEYFRAMED);
}

void addHavokSphere(hkReal xPos, hkReal yPos, hkReal zPos)
{
	//HavokUtilities* havokUtilities = new HavokUtilities();
	//havokUtilities->registerVisualDebugger();
	//addFixedSurface(havokUtilities->getWorld(), hkVector4(0,0,0), 
	//			    hkVector4(50.0f,1.0f,50.0f));

	//Add ball
	//hkpRigidBody* g_ball;
	const hkReal radius = 15.0f;
	const hkReal sphereMass = 150.0f;

	//hkVector4 relPos( 0.0f,radius + 0.0f, 50.0f );
	hkVector4 relPos( 0.0f,0.0f, 0.0f );
	//hkVector4 groundPos( 0.0f, 200.0f, -30.0f );
	hkVector4 groundPos( xPos, yPos, zPos );
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
	//hk
	//sphereRigidBody-
	//g_ball = sphereRigidBody;
	ballVector.push_back(sphereRigidBody);
	havokUtilities->getWorld()->addEntity( sphereRigidBody );
	sphereRigidBody->setAngularDamping(1.0f);
	sphereRigidBody->removeReference();
	info.m_shape->removeReference();

	//hkVector4 vel(  0.0f,4.9f, -100.0f );
	//sphereRigidBody->setLinearVelocity( vel );
	//sphereRigidBody->setGravityFactor(1.8f);
	
	//sphereRigidBody->setLinearDamping(0.0f);
	
	stopWatch.start();
	//lastTime = stopWatch.getElapsedSeconds();
	//lastTime = stopWatch.getElapsedSeconds();


}

void addFixedSurface(const hkVector4& position, 
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
	//rigidBodyInfo.m_qualityType = HK_COLLIDABLE_QUALITY_KEYFRAMED;

	//create new rigid body with supplied info
	hkpRigidBody* rigidBody = new hkpRigidBody(rigidBodyInfo);

	//rigidBody->setFriction(1000.0f);

	//add rigid body to physics world
	//havokUtilities->getWorld()->lock();
	havokUtilities->getWorld()->lock();
	//world->lock();
	havokUtilities->getWorld()->addEntity(rigidBody);
	//world->addEntity(rigidBody);

	//decerase reference counter for rigid body and shape
	rigidBody->removeReference();
	shape->removeReference();

	//world->unlock();
	havokUtilities->getWorld()->unlock();
}

void addVelocityBall(hkReal rad, hkReal posX, hkReal posY, hkReal posZ)
{
	const hkReal radius = rad;
	const hkReal sphereMass = 150.0f;

	//hkVector4 relPos( 0.0f,radius + 0.0f, 50.0f );
	hkVector4 relPos( 0.0f,0.0f, 0.0f );
	//hkVector4 groundPos( 0.0f, 200.0f, -30.0f );
	hkVector4 groundPos( posX, posY, posZ );
	hkVector4 velocity(0.0f, -5.0f, 0.0f);
	hkVector4 posy = groundPos;

	hkpRigidBodyCinfo info;
	hkpMassProperties massProperties;
	hkpInertiaTensorComputer::computeSphereVolumeMassProperties(radius, sphereMass, massProperties);

	info.m_mass = massProperties.m_mass;
	info.m_centerOfMass  = massProperties.m_centerOfMass;
	info.m_inertiaTensor = massProperties.m_inertiaTensor;
	info.m_shape = new hkpSphereShape( radius );
	info.m_position.setAdd4(posy, relPos );
	//info.m_motionType  = hkpMotion::MOTION_BOX_INERTIA;
	info.m_motionType  = hkpMotion::MOTION_KEYFRAMED;
	//info.m_gravity.set(0,-9.8f,0);

	//info.m_qualityType = HK_COLLIDABLE_QUALITY_BULLET;
	info.m_qualityType = HK_COLLIDABLE_QUALITY_KEYFRAMED;
	//info.m_qualityType = HK_COLLIDABLE_QUALITY_MOVING;
	hkpRigidBody* sphereRigidBody = new hkpRigidBody( info );
	
	//hk
	//sphereRigidBody-
	//g_ball = sphereRigidBody;
	//objVector.push_back(sphereRigidBody);

	//shapeVector.push_back(0);
	vel_ball = sphereRigidBody;
	havokUtilities->getWorld()->addEntity( vel_ball );
	//sphereRigidBody->setAngularDamping(1.0f);
	//vel_ball->applyLinearImpulse(velocity);
	//vel_ball->setLin
	vel_ball->setLinearVelocity(velocity);
	sphereRigidBody->removeReference();
	info.m_shape->removeReference();
}


