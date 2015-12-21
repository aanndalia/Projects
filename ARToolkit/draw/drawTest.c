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

#include "object.h"


#define COLLIDE_DIST 30000.0

/* Object Data */
char            *model_name = "Data/object_data3";
ObjectData_T    *object;
int             objectnum;

int             xsize, ysize;
int				thresh = 50;
int             count = 0;
int				delta_x = 0; //variable for tracking x-axis movement
int				delta_y = 40; //variable for tracking y-acis movement
int				xdir = 0;
int				ydir = 1;
int				dirChanged = 0;
int				north = 0;
int				east = 0;


int numelem=0;
int numalloc=1000;
double (*wmatstor)[3];

double sdraw[4];
int drawMode = 0;
int seen = 0;
int startTemp[3]; 
int endTemp[3];
int set = 1;
int invis = 0;
int sketchType = 0;
int sphereResolution = 10;



int occluded = 1;


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
static int draw( ObjectData_T *object, int objectnum );
static int  draw_object( int obj_id, double gl_para[16] );

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
	wmatstor = (double(*)[3])malloc((1000*sizeof(double)*3));

	//initialize applications
	glutInit(&argc, argv);
    init();
	
	arVideoCapStart();

	//added cause visible >= is stupid
	printf("%d %d\n",object[0].visible,object[1].visible);

	//start the main event loop
    argMainLoop( NULL, keyEvent, mainLoop );

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

	if(key=='s')
	{
		if(drawMode == 0)
			drawMode = 1;
		else
			drawMode = 0;
	}

	if(key=='i')
	{
		if(invis == 0)
			invis = 1;
		else
			invis = 0;
	}

	if(key=='e')
	{
		free(wmatstor);
		wmatstor = (double(*)[3])malloc((1000*sizeof(double)*3));
		numelem = 0;
		numalloc = 500;

	}

	if(key=='t')
	{
		if(sketchType == 0)
			sketchType = 1;
		else
			sketchType = 0;
	}

	if(key=='r')
	{
		printf("Pick a resolution for sketch spheres.\n");
		scanf("%d",&sphereResolution);
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
}

static void init( void )
{
	ARParam  wparam;

    /* open the video path */
    if( arVideoOpen( vconf ) < 0 ) exit(0);
    /* find the size of the window */
    if( arVideoInqSize(&xsize, &ysize) < 0 ) exit(0);
    printf("Image size (x,y) = (%d,%d)\n", xsize, ysize);

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
static int draw( ObjectData_T *object, int objectnum )
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
	int x;

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

	//added
	glEnable(GL_LINE_SMOOTH);
	


	printf("%d %d\n",object[0].visible,object[1].visible);
	if( object[0].visible >= 0 && object[1].visible >= 0 ) {
    arUtilMatInv(object[0].trans, wmat1);
    arUtilMatMul(wmat1, object[1].trans, wmat2);

    for( j = 0; j < 3; j++ ) {

        for( i = 0; i < 4; i++ ) printf("%8.4f ", wmat2[j][i]);

        printf("\n");
    }

	  printf("\n\n");
	}
	else
		printf("Blargblargblaaaarg\n");




	if(obj_id == 0)
	{
		if(drawMode == 0)
		{
			if(seen == 0 && object[1].visible == 1)
			{
				seen = 1;
				startTemp[0] = wmat2[0][3];
				startTemp[1] = wmat2[1][3];
				startTemp[2] = wmat2[2][3];
			}
			else if(seen == 1 && object[1].visible == 0)
			{
				seen = 0;
				set = 0;
				endTemp[0] = wmat2[0][3];
				endTemp[1] = wmat2[1][3];
				endTemp[2] = wmat2[2][3];
			}

			if(set == 0)
			{
				sdraw[0] = startTemp[0];
				sdraw[1] = startTemp[1];
				sdraw[2] = startTemp[2];
				sdraw[3] = sqrt(pow((double)(startTemp[0] - endTemp[0]),2) + pow((double)(startTemp[1] - endTemp[1]),2) + pow((double)(startTemp[2] - endTemp[2]),2));
				set = 1;
			}
			else if(set == 1)
			{
				glTranslatef(sdraw[0],sdraw[1],sdraw[2]);
				glutSolidSphere(sdraw[3],100,100);
			}
		}
		else
		{
			if(object[1].visible == 1 && (numelem == 0 || (wmat2[0][3]-wmatstor[numelem-1][0])*(wmat2[0][3]-wmatstor[numelem-1][0])+
				(wmat2[1][3]-wmatstor[numelem-1][1])*(wmat2[1][3]-wmatstor[numelem-1][1])+
				(wmat2[2][3]-wmatstor[numelem-1][2])*(wmat2[2][3]-wmatstor[numelem-1][2])>50))
			{	
				numelem=AddToArray(wmat2);
			}
			
			if(invis == 0)
			{
				glLineWidth(100000); //way past max
				if(!sketchType)
					glBegin(GL_LINE_STRIP);
				x = 0;
				while(x<numelem)
				{
					if(sketchType)
					{
						glTranslatef( wmatstor[x][0], wmatstor[x][1], wmatstor[x][2] );
						glutSolidSphere(10,sphereResolution,sphereResolution);
						glTranslatef( -wmatstor[x][0], -wmatstor[x][1], -wmatstor[x][2] );
					}
					else
						glVertex3f(wmatstor[x][0], wmatstor[x][1], wmatstor[x][2]); 

					x++;
				}
				if(!sketchType)
					glEnd();
			}
			printf("There are %d spheres\n",numelem);
		}
		
	}
	else if(obj_id == 1){
		
	}
	else if(obj_id == 2){
		//if(occluded == 1){
		//	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash);
		//	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
		//	/* draw a BIGGER! cube */
		//	glTranslatef( 0.0, 0.0, 30.0 );
		//	glutSolidCube(120);
		//	printf("bigger");
		//}
		//else{
		//	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash);
		//	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
		//	/* draw a cube */
		//	glTranslatef( east*4, north*4, 30.0 );
		//	glutSolidCube(60);
		//}
	}
	

    argDrawMode2D();

    return 0;
}


