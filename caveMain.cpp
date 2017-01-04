// UPDATED: 2012-12-05  (Y. Hu: Microsoft Visual Studio 2010; System configuration - Windows 7 + 16:9 screen ratio) 
// UPDATED: 2013-07-30  (Y. Hu: Microsoft Visual Studio 2012 + Platform toolset in Visual Studio 2010 due to OpenHatics-V3.10.5; System configuration - Windows 7 + 16:9 screen ratio) 
// UPDATED: 2014-05-30  (Y. Hu: Microsoft Visual Studio 2012 + Platform toolset in Visual Studio 2010 due to OpenHatics-V3.2.2; System configuration - Windows 7 + 16:9 screen ratio) 
// UPDATED: 2015-07-21  (Y. Hu: Microsoft Visual Studio 2013 + Platform toolset in Visual Studio 2013; OpenHatics-V3.2.2; System configuration - Windows 7 + 16:9 screen ratio)
// UPDATED: 2016-08-25  (Y. Hu: Microsoft Visual Studio 2015 + Platform toolset in Visual Studio 2015; OpenHatics-V3.2.2; System configuration - Windows 7 + 16:9 screen ratio)

#include <stdio.h>

#ifndef WIN32
#include <sys/time.h>
#endif
#include "caveMain.h"
#include "FaceManager.h"
#include "hapticManager.h"

/* Function prototypes */
void init_gl();
void drawScene();
void frame();
void cleanup();
void initScene();
void drawCave(void);
void drawEye(int eye);
void drawInStereo();
void drawOpenGL();
void glutKeyboard(unsigned char key);

#ifndef WIN32
#include <sys/time.h>
#endif

static IFaceManager *gFaceManager = nullptr;
static IHapticManager *gHapticManager = nullptr;
static float rotateAmt;
/* information about the stereoscopy */
static int stereo = 1;


int main(int argc,char **argv)
{
	//for CAVELib compatibility, this function will most likely do nothing...
		CAVEConfigure( &argc, argv, NULL);
		//register your per-context (per-window) initialization function
		CAVEInitApplication( (CAVECALLBACK)init_gl, 0 );
		//register your pre-frame function
		CAVEFrameFunction( (CAVECALLBACK)frame, 0 );
		//register your Display function
		CAVEDisplay( (CAVECALLBACK)drawScene, 0 );

	#ifdef KNAVE_LIB
		//register your cleanup function
		// (Because of the way glut does things under the hood,
		//  we have to register it ahead of time,
		//  to get it called back when we quit. )
		CAVEStopApplication( (CAVECALLBACK)cleanup, 0 );
	#endif
		initScene();
		//get things going!
		CAVEInit();
		// Usually, in a CAVELib App, we wait for the escape key
		// to be hit then call CAVEExit, but KNAVELib is going to
		// handle that for us...

	#ifndef KNAVE_LIB
		while (!CAVEgetbutton(CAVE_ESCKEY))
		{
			// Nap so that this busy loop doesn't waste CPU time 
			// reset timeval struct every time for linux compatibility 
			timeval tval;
			tval.tv_usec = 25000;
			tval.tv_sec = 0;
			select(0,NULL,NULL,NULL,&tval);
			//printf( "App running\n");
		}
    
		//call your cleanup function
		CAVEStopApplication( (CAVECALLBACK)cleanup, 0 );
		CAVEExit();
	
	#endif

	return 1;
}
/*******************************************************************************
Init function for OpenGL
*******************************************************************************/
void init_gl(){
	//glEnable(GL_COLOR_MATERIAL);
}

/*******************************************************************************
CAVE specific function DO NOT TOUCH
*******************************************************************************/
void frame(void)
{
    //printf( "Frame Update.\n" );

	if( CAVEBUTTON1 ) glutKeyboard('w');
	if( CAVEBUTTON2 ) glutKeyboard('d');
    if( CAVEBUTTON3 ) glutKeyboard('s');
	
    //timing for navigation!

    float jx, jy, dt, t;
    
    const float SPEED = 2.0f;

    static float prevtime = 0;

    jx=CAVE_JOYSTICK_X;
    jy=CAVE_JOYSTICK_Y;
    t = CAVEGetTime();
    dt = t - prevtime;

    ////// Frames Per Second Counter //////
    
    if( CAVEMasterDisplay() ){
      
      static int frameCount = 0;
      static float lastFpsTime = t;

      ++frameCount;
      if( t-lastFpsTime > 1.0 ){
	printf( "fps: %d %f %f %f\n", frameCount, dt, t, prevtime );
	frameCount = 0;
	lastFpsTime = t;
      }

    }
    
    prevtime = t;

    if (fabs(jy)>0.1 ){
      float wandFront[3];
      CAVEGetVector(CAVE_WAND_FRONT,wandFront);
      CAVENavTranslate(wandFront[0]*jy*SPEED*dt, wandFront[1]*jy*SPEED*dt,
		       wandFront[2]*jy*SPEED*dt);
    }
    if (fabs(jx)>0.2 )
	CAVENavRot(-jx*25.0f*dt,'y');


	rotateAmt += 180.0 * dt;

}

/*******************************************************************************
Cleanup function to remove everything you are using in this program
*******************************************************************************/
void cleanup(){
	gHapticManager->cleanup();
	gFaceManager->cleanup();
}
/*******************************************************************************
Init function if you have object to initiate
*******************************************************************************/
void initScene()
{
    gFaceManager = IFaceManager::create();
    gFaceManager->setup();

    gHapticManager = IHapticManager::create();
    gHapticManager->setup(gFaceManager);
}

/*******************************************************************************
 Function called for rendering
*******************************************************************************/
void drawScene(){
	if (stereo==1){
		drawEye(1);
		drawEye(2);
	}
	else{
		drawOpenGL();
	}
}
/*******************************************************************************
 Your openGL function add or remove object in your scene
*******************************************************************************/
void drawOpenGL(){
	drawCave();
	gFaceManager->drawAllPuzzlePieces();
//	gFaceManager->drawCenters();
	gHapticManager->feelPoints();
	gHapticManager->drawCursor();
	gHapticManager->UpdateHapticMapping();
	gHapticManager->DisplayInfo();
	
}

/*******************************************************************************
 Function responsible for the stereoscopy (NO NEED TO TOUCH)
*******************************************************************************/
void drawEye(int eye){
	//glMatrixMode( GL_PROJECTION );
	//glLoadIdentity();
	double dx0 = -0.070 / 4.0; 
	double dx1 = 0.40 / 4.0; 

/* information for glFrustum DO NOT CHANGE UNLESS YOU KNOW WHAT YOU ARE DOING*/
//	float frustumTop = 1.2;
//	float frustumRight = 1;
	double frustumTop = 1.0;
	double frustumRight = 1.7777; //Monitor aspect ratio is 16:9 = 1.7777	
	double CAVENear = 1.0;
	double CAVEFar = 10000.0;

	glDrawBuffer(GL_BACK_LEFT);
	if (eye==2){
		glDrawBuffer (GL_BACK_RIGHT);
		dx0=-dx0;
		dx1=-dx1;
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-frustumRight-dx0, frustumRight-dx0, -frustumTop, frustumTop,CAVENear,CAVEFar);
	glTranslatef(0+dx1,0,0);
	glMatrixMode(GL_MODELVIEW);
	glShadeModel(GL_SMOOTH);
	drawOpenGL();
    glFlush();
}

/*******************************************************************************
GLUT callback for key presses.
*******************************************************************************/
void glutKeyboard(unsigned char key)
{
	switch (key) {
	case 'w':
		gFaceManager->setDrawMode(drawMode::solidWireframe);
		break;
	case 's':
		gFaceManager->setDrawMode(drawMode::translucent);
		break;
	}
}

/*******************************************************************************
CAVE specific function DO NOT TOUCH
*******************************************************************************/
void drawCave(void)
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	
	glDisable( GL_LIGHTING );

	CAVENavTransform();
}