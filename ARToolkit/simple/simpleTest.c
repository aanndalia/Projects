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
#include "texture.h"
//#include <GL\glaux.h>		// Header File For The Glaux Library
//
// Camera configuration.
//
#ifdef _WIN32
char			*vconf = "Data\\WDM_camera_flipV.xml";
#else
char			*vconf = "";
#endif

//#define POLYNUM 4968

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

int				angle		   = 0; //added
float red=0.0, blue=0.0, green=0.0; //added
int scale = 150;


static void   init(void);
static void   cleanup(void);
static void   keyEvent( unsigned char key, int x, int y);
static void   mainLoop(void);
static void   draw( void );

//
//double grab[POLYNUM*15];
double flo[1000000];
double textflo[670000];
int ver_qty;
int l_index;
char temp[10];
int textID = 0;
int firstCheck = 0;
//GLuint	texture[1];	
FILE *fp;



int main(int argc, char **argv)
{
	FILE *objectfile;
	FILE *object2file;
	FILE *outfile;
	char buffer [100];
	int index = 0;

	objectfile = fopen("example.txt","r");
	//outfile = fopen("example2.txt","w");
	fgets(buffer,100,objectfile);
	ver_qty=atoi(buffer);


	fgets(buffer,100,objectfile);
	while(!(feof(objectfile))){
		fgets(buffer,100,objectfile);
		flo[index] = atof(buffer);
		index++;
	}

	

	object2file = fopen("textexample.txt","r");
	//fgets(buffer,100,objectfile);
	//ver_qty=atoi(buffer);

	index = 0;
	fgets(buffer,100,object2file);
	while(!(feof(object2file))){
		fgets(buffer,100,object2file);
		textflo[index] = atof(buffer);
		index++;
	}


	/*
	fp = fopen("C:\\ARToolKit\\bin\\example.txt", "r");
	for(l_index = 0; l_index < POLYNUM*15; l_index++)
	{
		fgets(temp,10,fp);
		printf("%s\n", temp); 
		grab[l_index] = strtod(temp, NULL);
		grab[l_index] = grab[l_index];
		//fscanf (fp,"%F",&grab[l_index]);
		//printf("%g",grab[l_index]);
		//grab[l_index] = grab[l_index];
	}
	printf("%lf %lf %lf %lf %lf %lf %lf %lf %lf\n",grab[2],grab[3],grab[4],grab[7],grab[8],grab[9],grab[12],grab[13],grab[14]);
	*/

	glutInit(&argc, argv);
	init();

	

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
    int index=0;
	int index2=0;
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
	
	glEnable(GL_TEXTURE_2D);
	if(firstCheck == 0)
	{
		textID = LoadBitm("spaceshiptextureInvert.bmp");
		firstCheck = 1;
	}
    /* load the camera transformation matrix */
    argConvGlpara(patt_trans, gl_para);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd( gl_para );
	
	//NO EFFECTS FOR YOU
	/*
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambi);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);	
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	
	*/
	glutSpecialUpFunc(processSpecialKeys);
	glutKeyboardFunc(processNormalKeys);

	glColor3f(red,green,blue);

	
    glTranslatef( 0.0, 0.0, 0.0 );
    //glutSolidCube(50.0);
	
	/*
	glBegin(GL_TRIANGLES);
		glVertex3f(-25,-25,0.0);
		glVertex3f(25,0.0,0.0);
		glVertex3f(0.0,25,0.0);
	//glEnd();

	glTranslatef( 100.0, 100.0, 0.0 );

	//glBegin(GL_TRIANGLES);
		glVertex3f(30,30,0.0);
		glVertex3f(20,0.0,0.0);
		glVertex3f(0.0,20,0.0);
	glEnd();
	*/
	
	/*
	glBegin(GL_TRIANGLES);
		glVertex3f(26.5055,-13.875,-22.2084);
		glVertex3f(15.507,4.16064,-24.4197);
		glVertex3f(26.5055,4.16064,-24.4197);
	glEnd();
	*/
	
	/*
	glBegin(GL_TRIANGLES);
		glVertex3f(grab[2],grab[3],grab[4]);
		glVertex3f(grab[7],grab[8],grab[9]);
		glVertex3f(grab[12],grab[13],grab[14]);
	glEnd();
	
	printf("%lf %lf %lf %lf %lf %lf %lf %lf %lf\n",grab[2],grab[3],grab[4],grab[7],grab[8],grab[9],grab[12],grab[13],grab[14]);
	*/
	//26.5055 -13.875 -22.2084
	//15.507 4.16064 -24.4197
	//26.5055 4.16064 -24.4197
	
	/*
	//3DtastiC~!
	glBindTexture(GL_TEXTURE_2D, textID);
	glBegin(GL_TRIANGLES); // glBegin and glEnd delimit the vertices that define a primitive (in our case triangles)
    for (l_index=0;l_index<POLYNUM*15;l_index+=15)
    {
        //----------------- FIRST VERTEX -----------------
        // Texture coordinates of the first vertex
        glTexCoord2f( grab[l_index],
                      grab[l_index+1]);
        // Coordinates of the first vertex
        glVertex3f( grab[l_index+2],
                    grab[l_index+3],
                    grab[l_index+4]); //Vertex definition
		

        //----------------- SECOND VERTEX -----------------
        // Texture coordinates of the second vertex
        glTexCoord2f( grab[l_index+5],
                      grab[l_index+6]);
        // Coordinates of the second vertex
        glVertex3f( grab[l_index+7],
                    grab[l_index+8],
                    grab[l_index+9]);
        
        //----------------- THIRD VERTEX -----------------
        // Texture coordinates of the third vertex
        glTexCoord2f( grab[l_index+10],
                     grab[l_index+11]);
        // Coordinates of the Third vertex
        glVertex3f( grab[l_index+12],
                    grab[l_index+13],
                    grab[l_index+14]);
		//printf("%d ",l_index);
    }
    glEnd();
	//3DtastiC~ time over!
	*/


	
	// save the previous settings, in this case save 
	// we're refering to the camera settings.
	//glPushMatrix();
	
	// Perform a rotation around the y axis (0,1,0) 
	// by the amount of degrees defined in the variable angle

	
	
	


	/*
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
	*/
	
	// discard the modelling transformations
	// after this the matrix will have only the camera settings.
	//glPopMatrix();
	
	// swapping the buffers causes the rendering above to be 
	// shown
	//glutSwapBuffers();
	
	// finally increase the angle for the next frame
	//angle+=3;

	//glRasterPos2f(70, 40);

	//glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,'H');
	//glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,'I');

	//glutStrokeCharacter(GLUT_STROKE_ROMAN ,'H');
	//glutStrokeCharacter(GLUT_STROKE_ROMAN ,'I');
	glBindTexture(GL_TEXTURE_2D, textID);
	glBegin(GL_TRIANGLES);
	while(index<ver_qty*9){
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

    glDisable( GL_LIGHTING );

    glDisable( GL_DEPTH_TEST );
}


