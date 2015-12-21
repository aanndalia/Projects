#ifdef _WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#ifndef __APPLE__
#include <GL/gl.h>
#include <GL/glut.h>
#else
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#endif
#include <AR/gsub.h>
#include <AR/video.h>
#include <AR/param.h>
#include <AR/ar.h>

//
// Camera configuration.
//
#ifdef _WIN32
char			*vconf = "Data\\WDM_camera_flipV.xml";
#else
char			*vconf = "";
#endif

int             xsize, ysize;
int             thresh = 150; 
int             count = 0;

char           *cparam_name    = "Data/camera_para.dat";
ARParam         cparam;

char           *patt_name      = "Data/patt.hiro";
int             patt_id;
double          patt_width     = 80.0;
double          patt_center[2] = {0.0, 0.0};
double          patt_trans[3][4];

//int				angle		   = 0; //added
float red=0.0, blue=0.0, green=0.0; //added
static float angle=0.0,ratio;
static float x=0.0f,y=1.75f,z=5.0f;
static float lx=0.0f,ly=0.0f,lz=-1.0f;
static GLint snowman_display_list;
int i;
int j;



static void   init(void);
static void   cleanup(void);
static void   keyEvent( unsigned char key, int x, int y);
static void   mainLoop(void);
static void   draw( void );



void drawSnowMan() {

	glColor3f(1.0f, 1.0f, 1.0f);

// Draw Body	
	glTranslatef(0.0f ,0.75f, 0.0f);
	glutSolidSphere(75.0f,20,20);


// Draw Head
	glTranslatef(0.0f, 1.0f, 0.0f);
	glutSolidSphere(25.0f,20,20);

// Draw Eyes
	glPushMatrix();
	glColor3f(0.0f,0.0f,0.0f);
	glTranslatef(5.0f, 10.0f, 18.0f);
	glutSolidSphere(5.0f,10,10);
	glTranslatef(-0.1f, 0.0f, 0.0f);
	glutSolidSphere(5.0f,10,10);
	glPopMatrix();

// Draw Nose
	glColor3f(1.0f, 0.5f , 0.5f);
	glRotatef(0.0f,1.0f, 0.0f, 0.0f);
	glutSolidCone(8.0f,5.0f,10,2);
}

GLuint createDL() {
	GLuint snowManDL;

	// Create the id for the list
	snowManDL = glGenLists(1);

	// start list
	glNewList(snowManDL,GL_COMPILE);

	// call the function that contains 
	// the rendering commands
		drawSnowMan();

	// endList
	glEndList();

	return(snowManDL);
}

void initScene() {

	glEnable(GL_DEPTH_TEST);
	snowman_display_list = createDL();
}





int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	init();

	initScene(); //added

    arVideoCapStart();
    argMainLoop( NULL, keyEvent, mainLoop );
	return (0);
}



void processSpecialKeys(int key, int x, int y) {

	switch(key) {
		case GLUT_KEY_F1 : 
				red = x/256; 
				green = y/256; 
				blue = 0.0; break;
		case GLUT_KEY_F2 : 
				red = 0.8; 
				green = 1.0; 
				blue = 0.2; break;
		case GLUT_KEY_F3 : 
				red = 0.0; 
				green = 0.0; 
				blue = 1.0; break;
	}

	printf("X: %d Y: %d\n",x,y);
}

void processNormalKeys(unsigned char key, int x, int y) {

	if (key == 27) 
		exit(0);
	else if (key=='r')
	{
		int mod = glutGetModifiers();
		if (mod == GLUT_ACTIVE_ALT)
			red = 0.0;
		else
			red = 1.0;
	}
	else if (key = '1')
	{
		red = 0.0;
		green = 0.0;
		blue = 0.0;
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
}

/* main loop */
static void mainLoop(void)
{
    ARUint8         *dataPtr;
    ARMarkerInfo    *marker_info;
    int             marker_num;
    int             j, k;

    /* grab a vide frame */
    if( (dataPtr = (ARUint8 *)arVideoGetImage()) == NULL ) {
        arUtilSleep(2);
        return;
    }
    if( count == 0 ) arUtilTimerReset();
    count++;

    argDrawMode2D();
    argDispImage( dataPtr, 0,0 );

    /* detect the markers in the video frame */
    if( arDetectMarker(dataPtr, thresh, &marker_info, &marker_num) < 0 ) {
        cleanup();
        exit(0);
    }

    arVideoCapNext();

    /* check for object visibility */
    k = -1;
    for( j = 0; j < marker_num; j++ ) {
        if( patt_id == marker_info[j].id ) {
            if( k == -1 ) k = j;
            else if( marker_info[k].cf < marker_info[j].cf ) k = j;
        }
    }
    if( k == -1 ) {
        argSwapBuffers();
        return;
    }

    /* get the transformation between the marker and the real camera */
    arGetTransMat(&marker_info[k], patt_center, patt_width, patt_trans);

	printf("%f %f %f\n",patt_trans[0][3],patt_trans[1][3],patt_trans[2][3]); //tutorialtastic

    draw();

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

    if( (patt_id=arLoadPatt(patt_name)) < 0 ) {
        printf("pattern load error !!\n");
        exit(0);
    }

    /* open the graphics window */
    argInit( &cparam, 1.0, 0, 0, 0, 0 );
}

/* cleanup function called when program exits */
static void cleanup(void)
{
    arVideoCapStop();
    arVideoClose();
    argCleanup();
}

static void draw( void )
{
    double    gl_para[16];
    GLfloat   mat_ambient[]     = {0.0, 0.0, 1.0, 1.0};
    GLfloat   mat_flash[]       = {0.0, 0.0, 1.0, 1.0};
    GLfloat   mat_flash_shiny[] = {50.0};
    GLfloat   light_position[]  = {100.0,-200.0,200.0,0.0};
    GLfloat   ambi[]            = {0.1, 0.1, 0.1, 0.1};
    GLfloat   lightZeroColor[]  = {0.9, 0.9, 0.9, 0.1};
    
    argDrawMode3D();
    argDraw3dCamera( 0, 0 );
    glClearDepth( 1.0 );
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    /* load the camera transformation matrix */
    argConvGlpara(patt_trans, gl_para);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd( gl_para );
	
	/*NO EFFECTS FOR YOU
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambi);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);	
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	*/

	glutSpecialFunc(processSpecialKeys);
	glutKeyboardFunc(processNormalKeys);

	glColor3f(red,green,blue);

	
    glMatrixMode(GL_MODELVIEW);
    glTranslatef( 0.0, 0.0, 0.0 );
    //glutSolidCube(50.0);
	
	/*
	glBegin(GL_TRIANGLES);
		glVertex3f(-25,-25,0.0);
		glVertex3f(25,0.0,0.0);
		glVertex3f(0.0,25,0.0);
	glEnd();

	glTranslatef( 100.0, 100.0, 0.0 );

	glBegin(GL_TRIANGLES);
		glVertex3f(30,30,0.0);
		glVertex3f(20,0.0,0.0);
		glVertex3f(0.0,20,0.0);
	glEnd();
	*/

	/*NO MORE ANIMATION, ONLY SNOW
	
	// save the previous settings, in this case save 
	// we're refering to the camera settings.
	glPushMatrix();
	
	// Perform a rotation around the y axis (0,1,0) 
	// by the amount of degrees defined in the variable angle
	glTranslatef( -100.0, -100.0, 0.0 );
	glRotatef(angle,1.0,1.0,1.0);
	glBegin(GL_TRIANGLES);
		glVertex3f(-55,-55,0.0);
		glVertex3f(55,0.0,0.0);
		glVertex3f(0.0,55,0.0);
	glEnd();
	
	glRotatef(-angle,1.0,1.0,1.0);
	glTranslatef( 200.0, 200.0, 0.0 );
	glRotatef(-angle,1.0,-2.0,2.0);
	
	glBegin(GL_TRIANGLES);
		glVertex3f(-55,-55,0.0);
		glVertex3f(55,0.0,0.0);
		glVertex3f(0.0,55,0.0);
	glEnd();
	
	// discard the modelling transformations
	// after this the matrix will have only the camera settings.
	glPopMatrix();
	
	// swapping the buffers causes the rendering above to be 
	// shown
	glutSwapBuffers();
	
	// finally increase the angle for the next frame
	angle+=3;

	*/


	glColor3f(0.9f, 0.9f, 0.9f);
	glBegin(GL_QUADS);
		glVertex3f(-100.0f, 0.0f, -100.0f);
		glVertex3f(-100.0f, 0.0f,  100.0f);
		glVertex3f( 100.0f, 0.0f,  100.0f);
		glVertex3f( 100.0f, 0.0f, -100.0f);
	glEnd();

	// Draw 36 SnowMen



	for(i = -3; i < 3; i++)
		for(j=-3; j < 3; j++) {
			glPushMatrix();
			glTranslatef(i*10.0,0,j * 10.0);
			glCallList(snowman_display_list);;
			glPopMatrix();
		}
	glutSwapBuffers();



    glDisable( GL_LIGHTING );

    glDisable( GL_DEPTH_TEST );
}


