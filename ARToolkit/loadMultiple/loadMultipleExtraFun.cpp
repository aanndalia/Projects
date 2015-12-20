#ifdef _WIN32
#  include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <new>

#include <AR/gsub.h>
#include <AR/param.h>
#include <AR/ar.h>
#include <AR/video.h>
#include <vector>
#include <AR/arMulti.h>
//#include "texture.h"

#ifndef __APPLE__
#  include <GL/glut.h>
#else
#  include <GLUT/glut.h>
#endif


extern "C"{
#include "object.h"
}
#include "HavokUtilities.hpp"
#include <Physics\Internal\Collide\Agent3\Machine\Nn\hkpAgentNnTrack.h>
#include <Physics\Internal\Collide\Agent3\Machine\Nn\hkpAgentNnMachine.h>
#include <Physics\Dynamics\Collide\hkpDynamicsContactMgr.h>

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
#include <iostream>
#include <fstream>
#include <string>
//#include <tchar.h>



#define COLLIDE_DIST 30000.0
#define BALL 0
#define CUBE 1
#define RECT 2
#define PIG  3
#define IGNORE 4
#define CRUSH 30.0
#define FIXEDSURF 5
#define SLINGOFFX 300
#define SLINGOFFY 200
#define ROTRECT 6

/* Object Data */
char            *model_name = "Data/object_data3";
//char			*config_name = "Data/ourpatts/marker.dat";
char                *config_name = "Data/multi/marker.dat";
struct ObjectData_T    *object; 
ARMultiMarkerInfoT  *config;
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
int objectAdded=1;
float numBalls=0;

char level;

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
std::vector<hkpRigidBody *> *objVector;
std::vector<int> *shapeVector;
hkpRigidBody *ourCube;
hkpRigidBody* g_ball;
hkpRigidBody* vel_ball;
hkStopwatch stopWatch;
hkReal lastTime = 0;
hkReal diff = 0;
hkReal timestep = 1.f / 6.f;
int ourCubeAdded=0;
int kUsed = 0;
hkReal slingPosX = 0.0;
hkReal slingPosY = 0.0;
hkReal slingPosZ = 0.0;
hkReal slingVelX = 0.0;
hkReal slingVelY = 0.0;
hkReal slingVelZ = 0.0;
int numPigs = 0;
int mainloopcount = 0;
bool forceDet = false;

int textID = 11;
int textID2 = 12;
int textID3 = 13;
int num_BMtexture=10;
int texCheck = 0;

int multX;
int multY;
int multZ;


struct object{
	hkpRigidBody *body;
	int type;
	int radius;
	int xExt;
	int yExt;
	int zExt;
};


struct object objectArray[1024];
int shapeArray[1024];
hkpRigidBody *fixedSurf;
int numObjects = 0;
hkpRigidBody* pigArray[100];

FILE *pFile;


//3ds Loading
double flo[1000000];
double textflo[670000];
int ver_qty;
double flo2[1000000];
double textflo2[670000];
int ver_qty2;
int l_index;
char temp[10];
//int textID = 0;
int firstCheck = 0;
float scale = 400;
float scale2 = .03;
FILE *fp;
FILE *tpFile;
	

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
static void mouseEvent(int button, int state, int x, int y);
static void   mainLoop(void);
static int draw(struct ObjectData_T *object, int objectnum );
static int  draw_object( int obj_id, double gl_para[16] );
void addHavokSphere(hkReal xPos, hkReal yPos, hkReal zPos, hkReal radius, hkReal sphereMass);
void addFixedSurface(const hkVector4& position, const hkVector4& dimensions);
void addHavokCube(hkReal dimX,hkReal dimY,hkReal dimZ, hkReal posX, hkReal posY, hkReal posZ, hkReal cubeMass);
void addOurCube();
void addVelocityBall(hkReal radius, hkReal posX, hkReal posY, hkReal posZ, hkReal velX, hkReal velY, hkReal velZ);
void drawRecPris(double xLen, double yLen, double zLen);
hkReal computeImpulse(const hkpRigidBody* rbA, const hkpRigidBody* rbB, const hkContactPoint& cp);
bool detectCollisions(hkpRigidBody* pig, int pigID);
void addHavokPig(hkReal xPos, hkReal yPos, hkReal zPos, hkReal radius, hkReal sphereMass);
static void multidraw( double trans1[3][4], double trans2[3][4], int mode );
int LoadBitm(char *filename);
void checkSphube();
void addRotatedHavokCube(hkReal dimX,hkReal dimY,hkReal dimZ, hkReal posX, hkReal posY, hkReal posZ, hkReal cubeMass, hkReal xRot, hkReal yRot, hkReal zRot);
void draw3ds();
void draw3dsSlingshot();
void addHavokCubePig(hkReal dimX,hkReal dimY,hkReal dimZ, hkReal posX, hkReal posY, hkReal posZ, hkReal cubeMass);

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
	FILE *objectfile;
	FILE *object2file;
	FILE *outfile;
	char buffer [2048];
	int index = 0;


	//dragon
	
	objectfile = fopen("example.txt","r");
	fgets(buffer,100,objectfile);
	ver_qty=atoi(buffer);


	fgets(buffer,100,objectfile);
	while(!(feof(objectfile))){
		fgets(buffer,100,objectfile);
		flo[index] = atof(buffer);
		index++;
	}	


	object2file = fopen("textexample.txt","r");

	index = 0;
	fgets(buffer,100,object2file);
	while(!(feof(object2file))){
		fgets(buffer,100,object2file);
		textflo[index] = atof(buffer);
		index++;
	}


	//slingshot

	index = 0;
	objectfile = fopen("nktest.txt","r");
	//outfile = fopen("example2.txt","w");
	fgets(buffer,100,objectfile);
	ver_qty2=atoi(buffer);


	fgets(buffer,100,objectfile);
	while(!(feof(objectfile))){
		fgets(buffer,100,objectfile);
		flo2[index] = atof(buffer);
		index++;
	}	

	//object2file = fopen("textexample.txt","r");
	object2file = fopen("textnktest.txt","r");
	//fgets(buffer,100,objectfile);
	//ver_qty=atoi(buffer);

	index = 0;
	fgets(buffer,100,object2file);
	while(!(feof(object2file))){
		fgets(buffer,100,object2file);
		textflo2[index] = atof(buffer);
		index++;
	}



	std::cout << "Enter level you would like to play" <<std::endl;
	std::cin >> level;

	HavokUtilities* havokUtil = new HavokUtilities();
	havokUtilities = havokUtil;
	keyFrameUtil = new hkpKeyFrameUtility();
	glutInit(&argc, argv);
    init();
	pFile = fopen ("myfile.txt","w");

	arVideoCapStart();
	havokUtilities->registerVisualDebugger();
	objVector = new std::vector<hkpRigidBody *>();
	shapeVector = new std::vector<int>();
	
	/* read level file*/
	FILE* levelFile;
	//char buffer[2048];
	std::string bufferString;
	int numObjectsToAdd;
	int objectType;
	float hkRealAr[256];
	char levelString[2048];
	char tempString[10];
	strcpy(levelString,"level");
	strncat(levelString,&level,1);
	strcat(levelString,".txt\0");


	if(level){
		if(levelFile=fopen(levelString,"r")){
			fgets(buffer,2048,levelFile);
			numObjectsToAdd=atoi(buffer);
			//printf("Objects to add: %d\n",numObjectsToAdd);
			for(int i=0;i<numObjectsToAdd;i++){
				fgets(buffer,2048,levelFile);
				objectType=atoi(buffer);
				//printf("object type: %d\n, i: %d\n",objectType,i);
				if(objectType==BALL){
					for(int j=0;j<5;j++){
						fgets(buffer,2048,levelFile);
						hkRealAr[j]=atof(buffer);
					}
					addHavokSphere(hkRealAr[0],hkRealAr[1],hkRealAr[2],hkRealAr[3],hkRealAr[4]);
				}
				else if(objectType==CUBE){
					for(int j=0;j<7;j++){
						fgets(buffer,2048,levelFile);
						hkRealAr[j]=atof(buffer);
					}
					addHavokCube(hkRealAr[0],hkRealAr[1],hkRealAr[2],hkRealAr[3],hkRealAr[4],hkRealAr[5],hkRealAr[6]);
				}
				else if(objectType==PIG){
					for(int j=0;j<5;j++){
						fgets(buffer,2048,levelFile);
						hkRealAr[j]=atof(buffer);
					}
					addHavokPig(hkRealAr[0],hkRealAr[1],hkRealAr[2],hkRealAr[3],hkRealAr[4]);
					//addHavokCubePig(hkRealAr[3],hkRealAr[3],hkRealAr[3],hkRealAr[0],hkRealAr[1],hkRealAr[2],hkRealAr[4]);
				}
				else if(objectType==RECT){
					for(int j=0;j<7;j++){
						fgets(buffer,2048,levelFile);
						hkRealAr[j]=atof(buffer);
					}
					addHavokCube(hkRealAr[0],hkRealAr[1],hkRealAr[2],hkRealAr[3],hkRealAr[4],hkRealAr[5],hkRealAr[6]);
				}
				else if(objectType==FIXEDSURF){
					for(int j=0;j<6;j++){
						fgets(buffer,2048,levelFile);
						hkRealAr[j]=atof(buffer);
					}
					hkVector4 fixedSurfPos(hkRealAr[0],hkRealAr[1],hkRealAr[2]);
					hkVector4 fixedSurfSize(hkRealAr[3],hkRealAr[4],hkRealAr[5]);
					addFixedSurface(fixedSurfPos,fixedSurfSize);
				}
				else if(objectType==ROTRECT){
					for(int j=0;j<10;j++){
						fgets(buffer,2048,levelFile);
						hkRealAr[j]=atof(buffer);
					}
					addRotatedHavokCube(hkRealAr[0],hkRealAr[1],hkRealAr[2],hkRealAr[3],hkRealAr[4],hkRealAr[5],hkRealAr[6],hkRealAr[7],hkRealAr[8],hkRealAr[9]);
				}
			}
		}
		else{
			std::cout<< "Error opening level file" << std::endl;
			exit(0);
		}
	}
	else{
		std::cout << "That level does not exist"<<std::endl;
		exit(0);
	}

	stopWatch.start();
	//start the main event loop
    argMainLoop( mouseEvent, keyEvent, mainLoop );
	delete havokUtilities;
	return 0;
}

static void mouseEvent(int button, int state, int x, int y)
{
	if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		if((-1*slingVelX)+SLINGOFFX<0)
			multX = -1;
		else
			multX = 1;

		if(slingVelY<0)
			multY = -1;
		else
			multY = 1;

		if((-1*slingVelZ)+SLINGOFFY<0)
			multZ = -1;
		else
			multZ = 1;

		//addVelocityBall(20,slingPosX,slingPosZ,-slingPosY,-1*slingVelX*(slingVelX*multX-SLINGOFF)*multX,-1*slingVelZ*(slingVelZ-100)*multZ,slingVelY*slingVelY*multY); //switched and multiplied according to opengl to havok translation
		//addVelocityBall(20,slingPosX,slingPosZ,-slingPosY,(-1*slingVelX*slingVelX*multX)+SLINGOFF*slingVelX*multX,(-1*slingVelZ*slingVelZ*multZ)+100*slingVelZ*multZ,slingVelY*slingVelY*multY);
		//addVelocityBall(20,0.0,0.0,300.0,-1*slingX,-1*slingY,-1*slingZ);

		addVelocityBall(20,slingPosX,slingPosZ,-slingPosY,((-1*slingVelX)+SLINGOFFX)*((-1*slingVelX)+SLINGOFFX)*multX,((-1*slingVelZ)+SLINGOFFY)*((-1*slingVelZ)+SLINGOFFY)*multZ,slingVelY*slingVelY*multY);

		//addVelocityBall(20,slingPosX,slingPosZ,-slingPosY,(-1*slingVelX),(-1*slingVelZ),slingVelY);
		//addVelocityBall(20,slingPosX,slingPosZ,-slingPosY,(-1*slingVelX)+SLINGOFF,(-1*slingVelZ)+100,slingVelY);
		//printf("\n Vel pos: %lf %lf %lf\n",slingVelX,slingVelY,slingVelZ);
	}
}



static void   keyEvent( unsigned char key, int x, int y)   
{
    /* quit if the ESC key is pressed */
    if( key == 0x1b ) {
        printf("*** %f (frame/sec)\n", (double)count/arUtilTimer());
        cleanup();
        exit(0);
    }
	if(key=='r'){
		/*
		objVector->clear();
		shapeVector->clear();
		numBalls=0;
		ourCubeAdded=0;
		objectAdded=0;
		delete havokUtilities;
		HavokUtilities* havokUtil = new HavokUtilities();
		havokUtilities = havokUtil;
		havokUtilities->registerVisualDebugger();
		addFixedSurface(hkVector4(0,0,0), hkVector4(200.0f,1.0f,200.0f));
		*/
	char argv[] = "loadMultiple.exe";
	STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    // Start the child process. 
    if( !CreateProcess( NULL,   // No module name (use command line)
        argv,        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
    ) 
    {
        printf( "CreateProcess failed (%d).\n", GetLastError() );
        return;
    }

   
	cleanup();
    exit(0);  //goodbye daddy

    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );	
	}
	if(key=='h'){
		/*
		objectAdded=1;
		hkReal offset=-1.0*numBalls;
		//hkReal offset=-1*pow(2,numBalls);
		addHavokSphere(offset,200.0,0.0,5.0,20.0);
		numBalls=numBalls+1;
		printf("check2\n");
		*/
	}
	if(key=='f'){
		
		if((-1*slingVelX)+SLINGOFFX<0)
			multX = -1;
		else
			multX = 1;

		if(slingVelY<0)
			multY = -1;
		else
			multY = 1;

		if((-1*slingVelZ)+SLINGOFFY<0)
			multZ = -1;
		else
			multZ = 1;

		//addVelocityBall(20,slingPosX,slingPosZ,-slingPosY,-1*slingVelX*(slingVelX*multX-SLINGOFF)*multX,-1*slingVelZ*(slingVelZ-100)*multZ,slingVelY*slingVelY*multY); //switched and multiplied according to opengl to havok translation
		//addVelocityBall(20,slingPosX,slingPosZ,-slingPosY,(-1*slingVelX*slingVelX*multX)+SLINGOFF*slingVelX*multX,(-1*slingVelZ*slingVelZ*multZ)+100*slingVelZ*multZ,slingVelY*slingVelY*multY);
		//addVelocityBall(20,0.0,0.0,300.0,-1*slingX,-1*slingY,-1*slingZ);


		//squared
		//addVelocityBall(20,slingPosX,slingPosZ,-slingPosY,((-1*slingVelX)+SLINGOFFX)*((-1*slingVelX)+SLINGOFFX)*multX,((-1*slingVelZ)+SLINGOFFY)*((-1*slingVelZ)+SLINGOFFY)*multZ,slingVelY*slingVelY*multY);

		//linear
		addVelocityBall(20,slingPosX,slingPosZ,-slingPosY,((-1*slingVelX)+SLINGOFFX),((-1*slingVelZ)+SLINGOFFY),slingVelY);


		//addVelocityBall(20,slingPosX,slingPosZ,-slingPosY,(-1*slingVelX),(-1*slingVelZ),slingVelY);
		//addVelocityBall(20,slingPosX,slingPosZ,-slingPosY,(-1*slingVelX)+SLINGOFF,(-1*slingVelZ)+100,slingVelY);
		//printf("\n Vel pos: %lf %lf %lf\n",slingVelX,slingVelY,slingVelZ);
	}



}

/* main loop */
static void mainLoop(void)
{
    ARUint8         *dataPtr;
    ARMarkerInfo    *marker_info;
    int             marker_num;
    int             i,j,k;
	double          err;


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
		//visual confirmation
		argDrawSquare(marker_info[i].vertex,0,0);
	}
	glColor3f( 1.0, 1.0, 1.0 ); //reset after drawing

	
	/* check for known patterns */
    for( i = 0; i < objectnum; i++ ) {
		k = -1;
		for( j = 0; j < marker_num; j++ ) {
	        if( object[i].id == marker_info[j].id) {

				/* you've found a pattern */
				//printf("Found pattern: %d ",patt_id);
				glColor3f( 0.0, 1.0, 0.0 );
				//visual confirmation
				argDrawSquare(marker_info[j].vertex,0,0);

				if( k == -1 ) k = j;
		        else /* make sure you have the best pattern (highest confidence factor) */
					if( marker_info[k].cf < marker_info[j].cf ) k = j;
			}
		}
		glColor3f( 1.0, 1.0, 1.0 ); //reset after drawing
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
	//argSwapBuffers(); MULTI REMOVE

	

	//step the simulation and VDB
		hkpWorldCinfo testInfo;

		havokUtilities->stepSimulation(timestep);
		havokUtilities->stepVisualDebugger(timestep);
		havokUtilities->getWorld()->getCinfo(testInfo);
		hkVector4 testVect = testInfo.m_gravity;
		hkVector4 gravs = havokUtilities->getWorld()->getGravity();

	mainloopcount++;
	//detect collisions with pigs
	for(int i=0; i<1024;i++)
	{
		if(objectArray[i].type == PIG)
		{
			forceDet = detectCollisions(objectArray[i].body, i);
			if(mainloopcount > 200 && forceDet)
			{
				//objectArray[i].body->deactivate();
				havokUtilities->getWorld()->removeEntity(objectArray[i].body);
				objectArray[i].type = IGNORE;
			}
		}
	}

	if( (err=arMultiGetTransMat(marker_info, marker_num, config)) < 0 ) {
		//printf("\nNeg error!\n");
        argSwapBuffers();
        return;
    }

	if(err > 100.0 ) {
		//printf("\nBig error!\n");
        argSwapBuffers();
        return;
    }

	/* draw the multimarker pattern */
    for( i = 0; i < config->marker_num; i++ ) {
        if( config->marker[i].visible >= 0 ) 
				multidraw( config->trans, config->marker[i].trans, 0 );
        else                                 
				multidraw( config->trans, config->marker[i].trans, 1 );
    }

	argSwapBuffers();
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
	

	if( (config = arMultiReadConfigFile(config_name)) == NULL ) {
        printf("config data load error !!\n");
        exit(0);
    }


    /* open the graphics window */
    argInit( &cparam, 2.0, 0, 0, 0, 0 );

	//textID = LoadBitm("spaceshiptextureInvert.bmp"); //Not even sure this is the right place
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

	
	double wmat1[3][4], wmat2[3][4], wmat3[3][4], wmat4[3][4]; 
	int i, j;
 
    argDrawMode3D();
    argDraw3dCamera( 0, 0 );
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd( gl_para );

 	/* set the material */
	
	/*
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambi);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);

    glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);
	*/

	/*DAVID STUFF
	//Add ambient light
	GLfloat ambientColor[] = {0.2f, 0.2f, 0.2f, 1.0f}; //Color (0.2, 0.2, 0.2) how much light is always present
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

	//Add positioned light
	GLfloat lightColor0[] = {0.2f, 1.2f, 1.6f, 1.0f}; //Color (0.5, 0.5, 0.5) 0.1f, 0.6f, 0.8f higher than one is glowy
	GLfloat lightPos0[] = {10.0f, 0.0f, 0.0f, 1.0f}; //Positioned at (4, 0, 8) relative to current transformation, 1=positioned
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

	//Add directed light
	GLfloat lightColor1[] = {0.8f, 0.5f, 0.2f, 1.0f}; //Color (0.5, 0.2, 0.2) across whole scene
	//Coming from the direction (-1, 0.5, 0.5)
	GLfloat lightPos1[] = {-10.0f, 0.0f, 0.0f, 0.0f}; //0=directed
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
	*/

	glEnable(GL_COLOR_MATERIAL); //YAY
	glEnable(GL_TEXTURE_2D);


	//Multi Marker relative
	if( object[0].visible >= 0 && object[1].visible >= 0 ) {
		double tempQuat[3][4];
		for(i=0;i<3;i++){
			for(j=0;j<4;j++){
				tempQuat[i][j]=config->trans[i][j];
			}
		}
		//printf("Base Posit: %lf, %lf, %lf\n",config->trans[0][3],config->trans[1][3],config->trans[2][3]);
	
		
		arUtilMatInv(config->trans, wmat1);
		arUtilMatInv(tempQuat, wmat3);
		
		arUtilMatMul(wmat1, object[0].trans, wmat2);
		arUtilMatMul(wmat3, object[0].trans, wmat4);
		//arUtilMatMul(wmat1, object[2].trans, wmat3);
	}

	/*Original house based relative position!!
	if( object[0].visible >= 0 && object[1].visible >= 0 ) {
		arUtilMatInv(object[1].trans, wmat1);
		arUtilMatMul(wmat1, object[0].trans, wmat2);
		//arUtilMatMul(wmat1, object[2].trans, wmat3);
	}
	*/

	//printf("\nwmat2: %8.4f %8.4f %8.4f\n",wmat2[0][3],wmat2[1][3],wmat2[2][3]);
	slingPosX=wmat2[0][3];
	slingPosY=wmat2[1][3];
	slingPosZ=wmat2[2][3];
	slingVelX=wmat4[0][3];
	slingVelY=wmat4[1][3];
	slingVelZ=wmat4[2][3];

	if(obj_id == 0){
		/*
		glBegin(GL_LINE_STRIP);
		glVertex3f(-wmat2[0][3]+SLINGOFFX,-wmat2[1][3]+SLINGOFFY,-wmat2[2][3]);
		glVertex3f(0,0,0);
		glVertex3f(-wmat2[0][3]+SLINGOFFX,-wmat2[1][3]+SLINGOFFY,-wmat2[2][3]);
		glEnd();
		*/
	}
	else if(obj_id == 1){


		if(objectAdded){
		
			for(int i=0;i<numObjects;i++){
				if(objectArray[i].type != IGNORE)
				{

				glLoadMatrixd( gl_para );	
				objectArray[i].body->getTransform().get4x4ColumnMajor(*quat);
				hkReal temp;
				for(int i=0;i<4;i++){
					for(int j=i+1;j<4;j++){
						temp = quat[j][i];
						quat[j][i]=quat[i][j];
						quat[i][j]=temp;
					}
				}
			
				for(int i=0;i<3;i++){
					for(int j=0;j<4;j++){ 
						ARQuat[i][j] = quat[i][j];
					}
				}

				for(int i=1;i<2;i++){
					for(int j=0;j<4;j++){
						temp=ARQuat[i+1][j];
						ARQuat[i+1][j]=ARQuat[i][j];
						ARQuat[i][j]=-1*temp;
					}
				}

				argConvGlpara(ARQuat,glutQuat);
				glMultMatrixd(glutQuat);
				if(objectArray[i].type==BALL){
					glColor3f(0.0f,1.0f,0.0f);
					//glBindTexture(GL_TEXTURE_2D, textID);
					glutSolidSphere(objectArray[i].radius,10,10);
				}
				else if(objectArray[i].type==CUBE)
				{
					glColor3f(1.0f,1.0f,0.0f);
					glutSolidCube(2*objectArray[i].xExt);
				}
				else if(objectArray[i].type==RECT){
					drawRecPris(objectArray[i].xExt,objectArray[i].yExt,objectArray[i].zExt);
				}
				else if(objectArray[i].type==PIG){
					glColor3f(0.0f,0.0f,1.0f);
					//glBindTexture(GL_TEXTURE_2D, textID);
					glutSolidSphere(objectArray[i].radius,10,10);
				}
				glLoadMatrixd( gl_para );

				}
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

				
				/*
				for( int i=0;i<4;i++)
				{
					for(int j=0;j<4;j++)
					{
						printf("%lf ",ourQuat[i][j]);
					}
					printf("\n");
				}
				*/

			    //hkpKeyFrameUtility
				hkVector4 newPosit(ourQuat[3][0], ourQuat[3][1], ourQuat[3][2], 0);
				hkVector4 newRot1(ourQuat[0][0], ourQuat[0][1], ourQuat[0][2], 0);
				hkVector4 newRot2(ourQuat[1][0], ourQuat[1][1], ourQuat[1][2], 0);
				hkVector4 newRot3(ourQuat[2][0], ourQuat[2][1], ourQuat[2][2], 0);
				hkRotation *newRot = new hkRotation();
				newRot->setRows(newRot1,newRot2,newRot3);
				hkQuaternion newRotQuat(*newRot);
				keyFrameUtil->applyHardKeyFrame(newPosit,newRotQuat,1/timestep,ourCube);
			}
		}
	}
	else if(obj_id == 2){
	}

	



	if(ourCubeAdded && (object[0].visible==0 || object[1].visible==0))
	{
		hkVector4 vel(0.0f, 0.0f, 0.0f);
		ourCube->setLinearVelocity(vel);
		ourCube->setAngularVelocity(vel);
	}

	glColor3f(1.0f,1.0f,1.0f);

	/*
	if(texCheck == 0)
	{
		textID = LoadBitm("brick1.bmp");
		texCheck = 1;
	}*/

	return 0;

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

void addHavokSphere(hkReal xPos, hkReal yPos, hkReal zPos, hkReal radius, hkReal sphereMass)
{
	//HavokUtilities* havokUtilities = new HavokUtilities();
	//havokUtilities->registerVisualDebugger();
	//addFixedSurface(havokUtilities->getWorld(), hkVector4(0,0,0), 
	//			    hkVector4(50.0f,1.0f,50.0f));

	//Add ball
	//hkpRigidBody* g_ball;

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

	try {
	 info.m_shape = new hkpSphereShape( radius );	
	}
	catch (...) {
		exit(1);
	}
	
	
	info.m_position.setAdd4(posy, relPos );
	info.m_motionType  = hkpMotion::MOTION_BOX_INERTIA;
	//info.m_gravity.set(0,-9.8f,0);

	info.m_qualityType = HK_COLLIDABLE_QUALITY_BULLET;
	//info.m_qualityType = HK_COLLIDABLE_QUALITY_MOVING;

	 hkpRigidBody* sphereRigidBody = 0;
	try {
	 sphereRigidBody = new hkpRigidBody( info );
	}
	catch (...) {
		exit(1);
	}
	

	//hk
	//sphereRigidBody-
	//g_ball = sphereRigidBody;
	//objVector->push_back(sphereRigidBody);
	//shapeVector->push_back(0);
	objectArray[numObjects].body=sphereRigidBody;
	objectArray[numObjects].type=BALL;
	objectArray[numObjects].radius=radius;

	shapeArray[numObjects]=0;
	numObjects++;
	havokUtilities->getWorld()->addEntity( sphereRigidBody );
	sphereRigidBody->setAngularDamping(1.0f);
	sphereRigidBody->removeReference();
	info.m_shape->removeReference();

	//hkVector4 vel(  0.0f,4.9f, -100.0f );
	//sphereRigidBody->setLinearVelocity( vel );
	//sphereRigidBody->setGravityFactor(1.8f);
	
	//sphereRigidBody->setLinearDamping(0.0f);
	

	//lastTime = stopWatch.getElapsedSeconds();
	//lastTime = stopWatch.getElapsedSeconds();


}

void addHavokCube(hkReal dimX,hkReal dimY,hkReal dimZ, hkReal posX, hkReal posY, hkReal posZ, hkReal cubeMass){
	//create box shape using given dimensions
	hkVector4 cube_dim(dimX,dimY,dimZ);

	hkpConvexShape* shape;
	try {
	 shape = new hkpBoxShape(cube_dim,0);
	}
	catch (...) {
		exit(1);
	}
	

	//create rigid body information structure 
	hkpRigidBodyCinfo rigidBodyInfo;
	
	//MOTION_FIXED means static element in game scene
	//rigidBodyInfo.m_motionType = hkpMotion::MOTION_FIXED;
	rigidBodyInfo.m_motionType  = hkpMotion::MOTION_BOX_INERTIA;
	rigidBodyInfo.m_shape = shape;
	//hkVector4 position( 0.0f, 300.0f, -30.2f );
	hkVector4 position(posX,posY,posZ);
	rigidBodyInfo.m_position = position;
	//rigidBodyInfo.m_qualityType = HK_COLLIDABLE_QUALITY_BULLET;
	rigidBodyInfo.m_qualityType = HK_COLLIDABLE_QUALITY_MOVING;
	hkpMassProperties massProperties;
	hkpInertiaTensorComputer::computeBoxVolumeMassProperties(cube_dim,cubeMass,massProperties);
	rigidBodyInfo.m_mass=massProperties.m_mass;
	rigidBodyInfo.m_centerOfMass  = massProperties.m_centerOfMass;
	rigidBodyInfo.m_inertiaTensor = massProperties.m_inertiaTensor;
	// this cahnges elasticity
	//rigidBodyInfo.m_restitution = 0.6;
	//printf("Elasticity: %lf\n",rigidBodyInfo.m_restitution);

	//rigidBodyInfo.m_friction = 2000.0;

	//create new rigid body with supplied info
	hkpRigidBody* rigidBody = 0;
	try {
	 rigidBody = new hkpRigidBody(rigidBodyInfo);
	}
	catch (...) {
		exit(1);
	}
	//objVector->push_back(rigidBody);
	//shapeVector->push_back(1);
	objectArray[numObjects].body=rigidBody;
	objectArray[numObjects].type=RECT;
	objectArray[numObjects].xExt=dimX;
	objectArray[numObjects].yExt=dimY;
	objectArray[numObjects].zExt=dimZ;
	numObjects++;
	havokUtilities->getWorld()->addEntity( rigidBody );
	rigidBody->removeReference();
	rigidBodyInfo.m_shape->removeReference();
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
	objectArray[numObjects].body=rigidBody;
	objectArray[numObjects].type=RECT;
	objectArray[numObjects].xExt=dimensions.getQuad().x;
	objectArray[numObjects].yExt=dimensions.getQuad().y;
	objectArray[numObjects].zExt=dimensions.getQuad().z;
	numObjects++;
}

void addVelocityBall(hkReal rad, hkReal posX, hkReal posY, hkReal posZ, hkReal velX, hkReal velY, hkReal velZ)
{
	const hkReal radius = rad;
	const hkReal sphereMass = 200.0f;
	//printf("hello");
	//hkVector4 relPos( 0.0f,radius + 0.0f, 50.0f );
	hkVector4 relPos( 0.0f,0.0f, 0.0f );
	//hkVector4 groundPos( 0.0f, 200.0f, -30.0f );
	hkVector4 groundPos( posX, posY, posZ );
	hkVector4 posy = groundPos;
	hkVector4 velocity(velX,velY,velZ);
	hkpRigidBodyCinfo info;
	hkpMassProperties massProperties;
	hkpInertiaTensorComputer::computeSphereVolumeMassProperties(radius, sphereMass, massProperties);

	info.m_mass = massProperties.m_mass;
	info.m_centerOfMass  = massProperties.m_centerOfMass;
	info.m_inertiaTensor = massProperties.m_inertiaTensor;
	info.m_shape = new hkpSphereShape( radius );
	info.m_position.setAdd4(posy, relPos );
	info.m_motionType  = hkpMotion::MOTION_BOX_INERTIA;
	//info.m_motionType  = hkpMotion::MOTION_KEYFRAMED;
	//info.m_gravity.set(0,-9.8f,0);

	info.m_qualityType = HK_COLLIDABLE_QUALITY_BULLET;
	//info.m_qualityType = HK_COLLIDABLE_QUALITY_KEYFRAMED;
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
	objectArray[numObjects].body=vel_ball;
	objectArray[numObjects].type=BALL;
	objectArray[numObjects].radius=rad;
	numObjects++;
	vel_ball->setLinearVelocity(velocity);
	sphereRigidBody->removeReference();
	info.m_shape->removeReference();

	checkSphube();
}



void drawRecPris(double xLen, double yLen, double zLen){
	
	glColor3f(4.0f,4.0f,4.0f);
	glBindTexture(GL_TEXTURE_2D, textID);
	glBegin(GL_TRIANGLE_STRIP);
	//glColor3f(0.0f,1.0f,.6f);
	glTexCoord2f(0,0);
	glVertex3f(-1*xLen,yLen,-1*zLen);
	glTexCoord2f(0,1);
	glVertex3f(xLen,yLen,-1*zLen);
	glTexCoord2f(1,0);
	glVertex3f(-1*xLen,yLen,zLen);
	//glColor3f(0.0f,.5f,.7f);
	glTexCoord2f(0,0);
	glVertex3f(xLen,yLen,zLen);
	glTexCoord2f(1,0);
	glVertex3f(-1*xLen,-1*yLen,zLen);
	glTexCoord2f(0,1);
	glVertex3f(xLen,-1*yLen,zLen);
	glTexCoord2f(0,0);
	glVertex3f(xLen,yLen,-1*zLen);
	//glColor3f(0.0f,.6f,1.0f);
	glTexCoord2f(1,0);
	glVertex3f(xLen,-1*yLen,-1*zLen);
	glTexCoord2f(0,1);
	glVertex3f(-1*xLen,yLen,-1*zLen);
	glTexCoord2f(0,0);
	glVertex3f(-1*xLen,-1*yLen,-1*zLen);
	//glColor3f(.7f,.5f,0.0f);
	glTexCoord2f(1,0);
	glVertex3f(-1*xLen,yLen,zLen);
	glTexCoord2f(0,1);
	glVertex3f(-1*xLen,-1*yLen,zLen);
	glTexCoord2f(0,0);
	glVertex3f(xLen,-1*yLen,-1*zLen);
	glTexCoord2f(1,0);
	glVertex3f(xLen,-1*yLen,zLen);
	glEnd();
	return;
}

hkReal computeImpulse(const hkpRigidBody* rbA, const hkpRigidBody* rbB, const hkContactPoint& cp)
{
	hkVector4 ra; ra.setSub4(cp.getPosition(), rbA->getCenterOfMassInWorld());
	hkVector4 rb; rb.setSub4(cp.getPosition(), rbB->getCenterOfMassInWorld());
	hkVector4 wcrossr; wcrossr.setCross(rbA->getAngularVelocity(), ra);
	
	hkVector4 padot; padot.setAdd4(rbA->getLinearVelocity(), wcrossr);
	wcrossr.setCross(rbB->getAngularVelocity(), rb);
	
	hkVector4 pbdot; pbdot.setAdd4(rbB->getLinearVelocity(), wcrossr);
	hkVector4 minus; minus.setSub4(padot, pbdot);
	hkReal vrel = cp.getNormal().dot3(minus);

	// Compute coeff of restitution from material parameters
	hkReal e;
	hkReal ea = rbA->getMaterial().getRestitution();
	hkReal eb = rbB->getMaterial().getRestitution();

	// NOTE THAT YOU CAN FUDGE THIS COMBINATION HOWEVER YOU LIKE :)
	// It may be easiest to use eg. the geometric mean (which is what Havok does
	// internally) ie.  sqrt(ea*eb) like this:
	e = hkMath::sqrt( ea * eb );

	hkReal numer = -( 1.0f + e ) * vrel;
	hkReal invMassA = 0.0f;
	hkReal invMassB = 0.0f;
	hkMatrix3 invIA; rbA->getInertiaInvWorld( invIA );
	hkMatrix3 invIB; rbB->getInertiaInvWorld( invIB );
	
	hkVector4 rCrossn, imul;
	hkVector4 denomA; denomA.setZero4();
	hkVector4 denomB; denomB.setZero4();

	if(rbA->getMotionType() != hkpMotion::MOTION_FIXED)
	{
		rCrossn.setCross( ra, cp.getNormal() );
		imul.setMul3( invIA, rCrossn );
		imul.setCross( imul, ra );
		denomA = imul;
		invMassA = rbA->getMassInv();
	}

	if(rbB->getMotionType() != hkpMotion::MOTION_FIXED)
	{
		rCrossn.setCross( rb, cp.getNormal() );
		imul.setMul3( invIB, rCrossn );
		imul.setCross( imul, rb );
		denomB = imul;
		invMassB = rbB->getMassInv();
	}

	hkVector4 sumAB;
	sumAB.setAdd4(denomA,denomB);
	hkReal denom = invMassA + invMassB + (hkReal) cp.getNormal().dot3(sumAB);
	hkReal impulseScale = numer/denom;

	return impulseScale;
}

bool detectCollisions(hkpRigidBody* pig, int pigID)
{
	hkpRigidBody* bodyA;
	bodyA = pig;
	hkReal computedImpulse;
	int numCollisionEntries = bodyA->getLinkedCollidable()->m_collisionEntries.getSize();
	// Find all of the other collidables that overlap it
	//fprintf(pFile, "numCollisionEntries = %d\n", numCollisionEntries);
	for (int j = 0; j < numCollisionEntries; j++)
	{
		hkpCollidable* collisionPartner = bodyA->getLinkedCollidable()->m_collisionEntries[j].m_partner;
		hkpRigidBody* bodyB = hkGetRigidBody( collisionPartner );
		// Check whether or not the overlapping collidable is also an hkpCharacterRigidBody
		//printf("hereA\n");
		//if (bodyB->getMotionType() == hkpMotion::MOTION_CHARACTER)
		//{
			// Grab the contact manager for body A
			//hkpAgentNnEntry *nnEnt = bodyA->getLinkedCollidable()->m_collisionEntries[j].m_agentEntry;
			//nnEnt
			//printf("hereA\n");
			hkpDynamicsContactMgr* contactMgr = (hkpDynamicsContactMgr*)bodyA->getLinkedCollidable()->m_collisionEntries[j].m_agentEntry->m_contactMgr;
			//hkpContactMgr* contactMgr = (hkpContactMgr*)bodyA->getLinkedCollidable()->m_collisionEntries[j].m_agentEntry->m_contactMgr;
			hkArray<hkContactPointId> contactPointIds;

			// Grab all of the contact point IDs
			//contactMgr->
			contactMgr->getAllContactPointIds( contactPointIds );
			//fprintf(stdout, "contactPointIds Size: %d\n", contactPointIds.getSize());
			for (int k = 0; k < contactPointIds.getSize(); k++)
			{
				hkContactPoint* contactPoint = contactMgr->getContactPoint( contactPointIds[k] );
				computedImpulse = computeImpulse(bodyA, bodyB, *contactPoint);
				//fprintf(pFile, "pigID: %d, computed impulse: %lf\n", pigID, computedImpulse);
				//fprintf(stdout, "pigID: %d, computed impulse: %lf\n", pigID, computedImpulse);
				if(computedImpulse > CRUSH)
					return true;
			}
		//}
	}
	return false;
}

void addHavokPig(hkReal xPos, hkReal yPos, hkReal zPos, hkReal radius, hkReal sphereMass)
{
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

	try {
	 info.m_shape = new hkpSphereShape( radius );	
	}
	catch (...) {
		exit(1);
	}
	
	
	info.m_position.setAdd4(posy, relPos );
	info.m_motionType  = hkpMotion::MOTION_BOX_INERTIA;
	//info.m_gravity.set(0,-9.8f,0);

	info.m_qualityType = HK_COLLIDABLE_QUALITY_BULLET;
	//info.m_qualityType = HK_COLLIDABLE_QUALITY_MOVING;

	 hkpRigidBody* sphereRigidBody = 0;
	try {
	 sphereRigidBody = new hkpRigidBody( info );
	}
	catch (...) {
		exit(1);
	}
	
	//objectArray[numObjects].body=sphereRigidBody;
	//objectArray[numObjects].type=PIG;
	//objectArray[numObjects].radius=radius;

	//shapeArray[numObjects]=PIG;
	//numObjects++;

	//pigArray[numPigs] = sphereRigidBody;
	objectArray[numObjects].body=sphereRigidBody;
	objectArray[numObjects].type=PIG;
	objectArray[numObjects].radius=radius;

	shapeArray[numObjects]=PIG;

	numPigs++;
	numObjects++;
	havokUtilities->getWorld()->addEntity( sphereRigidBody );
	sphereRigidBody->setAngularDamping(1.0f);
	sphereRigidBody->removeReference();
	info.m_shape->removeReference();
}

void addHavokCubePig(hkReal dimX,hkReal dimY,hkReal dimZ, hkReal posX, hkReal posY, hkReal posZ, hkReal cubeMass)
{
	/*
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

	try {
	 info.m_shape = new hkpSphereShape( radius );	
	}
	catch (...) {
		exit(1);
	}
	
	
	info.m_position.setAdd4(posy, relPos );
	info.m_motionType  = hkpMotion::MOTION_BOX_INERTIA;
	//info.m_gravity.set(0,-9.8f,0);

	info.m_qualityType = HK_COLLIDABLE_QUALITY_BULLET;
	//info.m_qualityType = HK_COLLIDABLE_QUALITY_MOVING;

	 hkpRigidBody* sphereRigidBody = 0;
	try {
	 sphereRigidBody = new hkpRigidBody( info );
	}
	catch (...) {
		exit(1);
	}
	
	//objectArray[numObjects].body=sphereRigidBody;
	//objectArray[numObjects].type=PIG;
	//objectArray[numObjects].radius=radius;

	//shapeArray[numObjects]=PIG;
	*/

	hkVector4 cube_dim(dimX,dimY,dimZ);

	hkpConvexShape* shape;
	try {
	 shape = new hkpBoxShape(cube_dim,0);
	}
	catch (...) {
		exit(1);
	}
	

	//create rigid body information structure 
	hkpRigidBodyCinfo rigidBodyInfo;
	
	//MOTION_FIXED means static element in game scene
	//rigidBodyInfo.m_motionType = hkpMotion::MOTION_FIXED;
	rigidBodyInfo.m_motionType  = hkpMotion::MOTION_BOX_INERTIA;
	rigidBodyInfo.m_shape = shape;
	//hkVector4 position( 0.0f, 300.0f, -30.2f );
	hkVector4 position(posX,posY,posZ);
	rigidBodyInfo.m_position = position;
	//rigidBodyInfo.m_qualityType = HK_COLLIDABLE_QUALITY_BULLET;
	rigidBodyInfo.m_qualityType = HK_COLLIDABLE_QUALITY_MOVING;
	hkpMassProperties massProperties;
	hkpInertiaTensorComputer::computeBoxVolumeMassProperties(cube_dim,cubeMass,massProperties);
	rigidBodyInfo.m_mass=massProperties.m_mass;
	rigidBodyInfo.m_centerOfMass  = massProperties.m_centerOfMass;
	rigidBodyInfo.m_inertiaTensor = massProperties.m_inertiaTensor;
	// this cahnges elasticity
	//rigidBodyInfo.m_restitution = 0.6;
	//printf("Elasticity: %lf\n",rigidBodyInfo.m_restitution);

	//rigidBodyInfo.m_friction = 2000.0;

	//create new rigid body with supplied info
	hkpRigidBody* rigidBody = 0;
	try {
	 rigidBody = new hkpRigidBody(rigidBodyInfo);
	}
	catch (...) {
		exit(1);
	}
	//objVector->push_back(rigidBody);
	//shapeVector->push_back(1);
	objectArray[numObjects].body=rigidBody;
	objectArray[numObjects].type=PIG;
	objectArray[numObjects].xExt=dimX;
	objectArray[numObjects].yExt=dimY;
	objectArray[numObjects].zExt=dimZ;

	//numObjects++;

	//pigArray[numPigs] = sphereRigidBody;
	/*
	objectArray[numObjects].body=sphereRigidBody;
	objectArray[numObjects].type=PIG;
	objectArray[numObjects].radius=radius;
	*/
	shapeArray[numObjects]=PIG;

	numPigs++;
	numObjects++;
	havokUtilities->getWorld()->addEntity( rigidBody );
	//rigidBody->setAngularDamping(1.0f);
	rigidBody->removeReference();
	rigidBodyInfo.m_shape->removeReference();
}

static void multidraw( double trans1[3][4], double trans2[3][4], int mode )
{
	GLfloat   mat_ambient[]				= {0.0, 0.0, 1.0, 1.0};
	GLfloat   mat_ambient_collide[]     = {1.0, 0.0, 0.0, 1.0};
    GLfloat   mat_flash[]				= {0.0, 0.0, 1.0, 1.0};
	GLfloat   mat_flash_collide[]       = {1.0, 0.0, 0.0, 1.0};
    GLfloat   mat_flash_shiny[] = {50.0};
    GLfloat   light_position[]  = {100.0,-200.0,200.0,0.0};
    GLfloat   ambi[]            = {0.1, 0.1, 0.1, 0.1};
    GLfloat   lightZeroColor[]  = {0.9, 0.9, 0.9, 0.1};
	double    gl_para[16];

	
	double wmat1[3][4], wmat2[3][4], wmat3[3][4]; 
	int i, j;

	glClearDepth( 1.0 );
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_LIGHTING);
 
    argDrawMode3D();
    argDraw3dCamera( 0, 0 );

	/* load the camera transformation matrix */
    glMatrixMode(GL_MODELVIEW);
    argConvGlpara(trans1, gl_para);
    glLoadMatrixd( gl_para );

	//Don't want individual to marker?
    //argConvGlpara(trans2, gl_para);
    //glMultMatrixd( gl_para );


	glEnable(GL_COLOR_MATERIAL); //YAY

	if(objectAdded){
			
		if( object[0].visible == 1 )
		{
			glBegin(GL_LINE_STRIP);
			glVertex3f(SLINGOFFX,-80,SLINGOFFY);
			glVertex3f(slingPosX,slingPosY,slingPosZ);
			glVertex3f(SLINGOFFX,80,SLINGOFFY);
			glEnd();

			glTranslatef(slingPosX,slingPosY,slingPosZ);
			glutSolidSphere(20,10,10);
			glTranslatef(-slingPosX,-slingPosY,-slingPosZ);
		}
		
			for(int i=0;i<numObjects;i++){
				if(objectArray[i].type != IGNORE)
				{

				glLoadMatrixd( gl_para );	
				objectArray[i].body->getTransform().get4x4ColumnMajor(*quat);
				hkReal temp;
				for(int i=0;i<4;i++){
					for(int j=i+1;j<4;j++){
						temp = quat[j][i];
						quat[j][i]=quat[i][j];
						quat[i][j]=temp;
					}
				}
			
				for(int i=0;i<3;i++){
					for(int j=0;j<4;j++){ 
						ARQuat[i][j] = quat[i][j];
					}
				}

				for(int i=1;i<2;i++){
					for(int j=0;j<4;j++){
						temp=ARQuat[i+1][j];
						ARQuat[i+1][j]=ARQuat[i][j];
						ARQuat[i][j]=-1*temp;
					}
				}

				argConvGlpara(ARQuat,glutQuat);
				glMultMatrixd(glutQuat);
				if(objectArray[i].type==BALL){
					glColor3f(0.0f,1.0f,0.0f);
					glutSolidSphere(objectArray[i].radius,10,10);
				}
				else if(objectArray[i].type==CUBE)
				{
					glColor3f(1.0f,1.0f,0.0f);
					glutSolidCube(2*objectArray[i].xExt);
				}
				else if(objectArray[i].type==RECT){
					if(texCheck == 1)
						drawRecPris(objectArray[i].xExt,objectArray[i].yExt,objectArray[i].zExt);
				}
				else if(objectArray[i].type==PIG){
					//glColor3f(1.0f,0.0f,0.0f);
					//glutSolidSphere(objectArray[i].radius,10,10);
					//glColor3f(4.0f,4.0f,4.0f);
					if(texCheck == 1)
					{
						glRotatef(-90, 1.0f, 0.0f, 0.0f);
						draw3ds();
						glRotatef(90, 1.0f, 0.0f, 0.0f);
					}
				}
					glLoadMatrixd( gl_para );
				
				}
			}
		
		}
	glTranslatef(SLINGOFFX,0,0);
	glRotatef(90, 0.0f, 1.0f, 0.0f);
	glRotatef(90, 0.0f, 0.0f, 1.0f);
	draw3dsSlingshot();
	glRotatef(-90, 0.0f, 1.0f, 0.0f);
	glRotatef(-90, 0.0f, 0.0f, 1.0f);
	glTranslatef(-SLINGOFFX,0,0);
	glColor3f(1.0f,1.0f,1.0f);


	glEnable(GL_TEXTURE_2D);

	if(texCheck == 0)
	{
		textID = LoadBitm("brick1.bmp");
		textID2 = LoadBitm("purpscales.bmp");
		textID3 = LoadBitm("wood.bmp");
		texCheck = 1;
	}

	glDisable( GL_LIGHTING );
    glDisable( GL_DEPTH_TEST );
}

/**********************************************************
 *
 * FUNCTION LoadBitmap(char *)
 *
 * This function loads a bitmap file and return the OpenGL reference ID to use that texture
 *
 *********************************************************/

int LoadBitm(char *filename) 
{
    int i, j=0; //Index variables
    FILE *l_file; //File pointer
    unsigned char *l_texture; //The pointer to the memory zone in which we will load the texture
     
    // windows.h gives us these types to work with the Bitmap files
    BITMAPFILEHEADER fileheader; 
    BITMAPINFOHEADER infoheader;
    RGBTRIPLE rgb;

    num_BMtexture++; // The counter of the current texture is increased

    if( (l_file = fopen(filename, "rb"))==NULL) return (-1); // Open the file for reading
    
    fread(&fileheader, sizeof(fileheader), 1, l_file); // Read the fileheader
    
    fseek(l_file, sizeof(fileheader), SEEK_SET); // Jump the fileheader
    fread(&infoheader, sizeof(infoheader), 1, l_file); // and read the infoheader

    // Now we need to allocate the memory for our image (width * height * color deep)
    l_texture = (byte *) malloc(infoheader.biWidth * infoheader.biHeight * 4);
    // And fill it with zeros
    memset(l_texture, 0, infoheader.biWidth * infoheader.biHeight * 4);
 
    // At this point we can read every pixel of the image
    for (i=0; i < infoheader.biWidth*infoheader.biHeight; i++)
    {            
            // We load an RGB value from the file
            fread(&rgb, sizeof(rgb), 1, l_file); 

            // And store it
            l_texture[j+0] = rgb.rgbtRed; // Red component
            l_texture[j+1] = rgb.rgbtGreen; // Green component
            l_texture[j+2] = rgb.rgbtBlue; // Blue component
            l_texture[j+3] = 255; // Alpha value
            j += 4; // Go to the next position
    }

    fclose(l_file); // Closes the file stream
     
    glBindTexture(GL_TEXTURE_2D, num_BMtexture); // Bind the ID texture specified by the 2nd parameter

    // The next commands sets the texture parameters
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // If the u,v coordinates overflow the range 0,1 the image is repeated
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // The magnification function ("linear" produces better results)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); //The minifying function

    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // We don't combine the color with the original surface color, use only the texture map.

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); //mix with textures TINT IS HERE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    // Finally we define the 2d texture
    glTexImage2D(GL_TEXTURE_2D, 0, 4, infoheader.biWidth, infoheader.biHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, l_texture);

    // And create 2d mipmaps for the minifying function
    gluBuild2DMipmaps(GL_TEXTURE_2D, 4, infoheader.biWidth, infoheader.biHeight, GL_RGBA, GL_UNSIGNED_BYTE, l_texture);

    free(l_texture); // Free the memory we used to load the texture

    return (num_BMtexture); // Returns the current texture OpenGL ID
}

void checkSphube()
{
	for(int i=0;i<numObjects-1;i++)
		if(objectArray[numObjects-1].body == objectArray[i].body)
			objectArray[i].type = IGNORE;
}

void addRotatedHavokCube(hkReal dimX,hkReal dimY,hkReal dimZ, hkReal posX, hkReal posY, hkReal posZ, hkReal cubeMass, hkReal xRot, hkReal yRot,hkReal zRot){
	//create box shape using given dimensions
	hkVector4 cube_dim(dimX,dimY,dimZ);

	hkpConvexShape* shape;
	try {
	 shape = new hkpBoxShape(cube_dim,0);
	}
	catch (...) {
		exit(1);
	}
	

	//create rigid body information structure 
	hkpRigidBodyCinfo rigidBodyInfo;
	
	//MOTION_FIXED means static element in game scene
	//rigidBodyInfo.m_motionType = hkpMotion::MOTION_FIXED;
	rigidBodyInfo.m_motionType  = hkpMotion::MOTION_BOX_INERTIA;
	rigidBodyInfo.m_shape = shape;
	//hkVector4 position( 0.0f, 300.0f, -30.2f );
	hkVector4 position(posX,posY,posZ);
	rigidBodyInfo.m_position = position;
	//rigidBodyInfo.m_qualityType = HK_COLLIDABLE_QUALITY_BULLET;
	rigidBodyInfo.m_qualityType = HK_COLLIDABLE_QUALITY_MOVING;
	hkpMassProperties massProperties;
	hkpInertiaTensorComputer::computeBoxVolumeMassProperties(cube_dim,cubeMass,massProperties);
	rigidBodyInfo.m_mass=massProperties.m_mass;
	rigidBodyInfo.m_centerOfMass  = massProperties.m_centerOfMass;
	rigidBodyInfo.m_inertiaTensor = massProperties.m_inertiaTensor;
	// this cahnges elasticity
	//rigidBodyInfo.m_restitution = 0.6;
	//printf("Elasticity: %lf\n",rigidBodyInfo.m_restitution);

	//rigidBodyInfo.m_friction = 2000.0;

	//create new rigid body with supplied info
	hkpRigidBody* rigidBody = 0;
	try {
	 rigidBody = new hkpRigidBody(rigidBodyInfo);
	}
	catch (...) {
		exit(1);
	}
	hkReal max=0;
	char maxRot;
	if(xRot>max)
		max=xRot;
	if(yRot>max)
		max=yRot;
	if(zRot>max)
		max=zRot;
	xRot/=max;
	yRot/=max;
	zRot/=max;
	hkVector4 rotaxis(xRot, yRot, zRot);
	hkQuaternion rotQuat(rotaxis, max);
	rigidBody->setRotation(rotQuat);
	//objVector->push_back(rigidBody);
	//shapeVector->push_back(1);
	objectArray[numObjects].body=rigidBody;
	objectArray[numObjects].type=RECT;
	objectArray[numObjects].xExt=dimX;
	objectArray[numObjects].yExt=dimY;
	objectArray[numObjects].zExt=dimZ;
	numObjects++;
	havokUtilities->getWorld()->addEntity( rigidBody );
	rigidBody->removeReference();
	rigidBodyInfo.m_shape->removeReference();
}

void draw3ds()
{
	int index = 0;
	int index2 = 0;
	//glColor3f(4.0f,4.0f,4.0f);
	
	//fprintf(tpFile, "%d\n",textID2);
	glColor3f(4.0f,4.0f,4.0f);
	glBindTexture(GL_TEXTURE_2D, textID2);
	//glBindTexture(GL_TEXTURE_2D, 1);
	glBegin(GL_TRIANGLES);
	while(index<ver_qty*9)
	{
		glTexCoord2f(textflo[index2],textflo[index2+1]);
		//glColor3f( 0.0, 1.0, 0.0 );
		glVertex3f(flo[index]/scale,flo[index+1]/scale,flo[index+2]/scale);
		glTexCoord2f(textflo[index2+2],textflo[index2+3]);
		//glColor3f( 1.0, 0.0, 0.0 );
		glVertex3f(flo[index+3]/scale,flo[index+4]/scale,flo[index+5]/scale);
		glTexCoord2f(textflo[index2+4],textflo[index2+5]);
		//glColor3f( 0.0, 0.0, 1.0 );
		glVertex3f(flo[index+6]/scale,flo[index+7]/scale,flo[index+8]/scale);
		index = index+9;
		index2 = index2+6;
	}
	glEnd();
}


void draw3dsSlingshot()
{
	int index = 0;
	int index2 = 0;
	//glColor3f(4.0f,4.0f,4.0f);
	
	//fprintf(tpFile, "%d\n",textID2);
	glColor3f(4.0f,4.0f,4.0f);
	glBindTexture(GL_TEXTURE_2D, textID3);
	//glBindTexture(GL_TEXTURE_2D, 1);
	glBegin(GL_TRIANGLES);
	while(index<ver_qty2*9)
	{
		glTexCoord2f(textflo2[index2],textflo2[index2+1]);
		//glColor3f( 0.0, 1.0, 0.0 );
		glVertex3f(flo2[index]/scale2,flo2[index+1]/scale2,flo2[index+2]/scale2);
		glTexCoord2f(textflo2[index2+2],textflo2[index2+3]);
		//glColor3f( 1.0, 0.0, 0.0 );
		glVertex3f(flo2[index+3]/scale2,flo2[index+4]/scale2,flo2[index+5]/scale2);
		glTexCoord2f(textflo2[index2+4],textflo2[index2+5]);
		//glColor3f( 0.0, 0.0, 1.0 );
		glVertex3f(flo2[index+6]/scale2,flo2[index+7]/scale2,flo2[index+8]/scale2);
		index = index+9;
		index2 = index2+6;
	}
	glEnd();
}
