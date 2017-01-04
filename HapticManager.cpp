// UPDATED: 2012-12-05  (Y. Hu: Microsoft Visual Studio 2010; System configuration - Windows 7 + 16:9 screen ratio) 
// UPDATED: 2013-07-30  (Y. Hu: Microsoft Visual Studio 2012 + Platform toolset in Visual Studio 2010 due to OpenHatics-V3.10.5; System configuration - Windows 7 + 16:9 screen ratio) 
// UPDATED: 2014-05-30  (Y. Hu: Microsoft Visual Studio 2012 + Platform toolset in Visual Studio 2010 due to OpenHatics-V3.2.2; System configuration - Windows 7 + 16:9 screen ratio) 
// UPDATED: 2015-07-21  (Y. Hu: Microsoft Visual Studio 2013 + Platform toolset in Visual Studio 2013; OpenHatics-V3.2.2; System configuration - Windows 7 + 16:9 screen ratio)
// UPDATED: 2016-08-25  (Y. Hu: Microsoft Visual Studio 2015 + Platform toolset in Visual Studio 2015; OpenHatics-V3.2.2; System configuration - Windows 7 + 16:9 screen ratio)

#include "caveMain.h"
#include "FaceManager.h"
#include "HapticManager.h"

#include <HDU/hduVector.h>
#include <HDU/hduQuaternion.h>
#include <HDU/hduMatrix.h>
#include <HD/hd.h>
#include <HL/hl.h>
#include <HDU/hduMatrix.h>
#include <HDU/hduError.h>
#include <iostream>
#include <stdarg.h>
#include <string>
#include <HLU/hlu.h>
#include <GL/glut.h>
#include <vector>
#include <map>
#include <assert.h>

using namespace std;

/* Position and orientation of proxy at start of drag. */
hduVector3Dd gStartDragProxyPos;
hduQuaternion gStartDragProxyRot;

/* Position and orientation of drag object at start of drag. */
hduMatrix gStartDragObjTransform;

/* Piece that is currently being dragged */
PuzzlePiece* g_activePiece = nullptr;

class HapticManager : public IHapticManager
{
public:

    HapticManager();
    virtual ~HapticManager();

    virtual void setup(IFaceManager *pFaceManager);
    virtual void cleanup();
    virtual void drawCursor();
    virtual void feelPoints();
	virtual void DisplayInfo(void);
	virtual void updateDragObjectTransform();

protected:
	void UpdateHapticMapping(void);

    static void HLCALLBACK hlTouchCB(HLenum event, HLuint object, HLenum thread,
                                     HLcache *cache, void *userdata);
	static void HLCALLBACK hlUntouchCB(HLenum event, HLuint object, HLenum thread,
									HLcache *cache, void *userdata);
    static void HLCALLBACK hlButtonDownCB(HLenum event, HLuint object, HLenum thread,
                                          HLcache *cache, void *userdata);
    static void HLCALLBACK hlButtonUpCB(HLenum event, HLuint object, HLenum thread,
                                        HLcache *cache, void *userdata);

private:

	void checkMatches();

    HHD m_hHD;
    HHLRC m_hHLRC;

    IFaceManager *m_pFaceManager;
	map < HLuint, PuzzlePiece*> m_HapticPieces;

    double m_cursorScale;
    GLuint m_nCursorDisplayList;
	GLuint gCursorDisplayList;
    HLuint m_effectName;       
	double gCursorScale;
	
	void DrawBitmapString(GLfloat x, GLfloat y, void *font, char *format,...);
};


/*******************************************************************************
 Creates a HapticManager.                                                                                
*******************************************************************************/
IHapticManager *IHapticManager::create()
{
    return new HapticManager;
}


/*******************************************************************************
 Destroys the HapticManager.
*******************************************************************************/
void IHapticManager::destroy(IHapticManager *&pInterface)
{
    HapticManager *pImp = static_cast<HapticManager *>(pInterface);
    if (pImp)
    {
        delete pImp;
        pInterface = 0;
    }
}


/*******************************************************************************
 Constructor.
*******************************************************************************/
HapticManager::HapticManager() : 
    m_pFaceManager(0),
    m_effectName(0),
    m_cursorScale(0),
    m_nCursorDisplayList(0),
	gCursorDisplayList(0),   //initialization updated on 2015-07-21
	gCursorScale(0),         //initialization updated on 2015-07-21
	m_hHD(HD_INVALID_HANDLE),
    m_hHLRC(NULL)
{

}


/*******************************************************************************
 Destructor.
*******************************************************************************/
HapticManager::~HapticManager()
{
}


/*******************************************************************************
 Sets up the haptics.
*******************************************************************************/
void HapticManager::setup( IFaceManager *pFaceManager )
{
	HDErrorInfo error;
	m_hHD = hdInitDevice(HD_DEFAULT_DEVICE);
	if (HD_DEVICE_ERROR(error = hdGetError()))
	{
		hduPrintError(stderr, &error, "Failed to initialize haptic device");
		fprintf(stderr, "Press any key to exit");
		getchar();
		exit(1);
	}

	m_pFaceManager = pFaceManager;
    
    // Create a haptic context for the device.  The haptic context maintains 
    // the state that persists between frame intervals and is used for
    // haptic rendering.
    m_hHLRC = hlCreateContext(m_hHD);
    hlMakeCurrent(m_hHLRC); 

	//Generate unique IDs and register callbacks
	auto pieces = m_pFaceManager->getPieces();
	for (auto piece : pieces)
	{
		HLuint puzzle_id = hlGenShapes(1);
		m_HapticPieces.emplace(puzzle_id, piece.second);

		hlAddEventCallback(HL_EVENT_TOUCH, puzzle_id, HL_CLIENT_THREAD, hlTouchCB, piece.second);
		hlAddEventCallback(HL_EVENT_UNTOUCH, puzzle_id, HL_CLIENT_THREAD, hlUntouchCB, piece.second );
		hlAddEventCallback(HL_EVENT_1BUTTONDOWN, puzzle_id, HL_CLIENT_THREAD, hlButtonDownCB, piece.second );
	}

	// Add an event callback on button to clear the drag object and end dragging.
	hlAddEventCallback(HL_EVENT_1BUTTONUP, HL_OBJECT_ANY, HL_CLIENT_THREAD, hlButtonUpCB, NULL);

}


/*******************************************************************************
 Cleans up the haptics.
*******************************************************************************/
void HapticManager::cleanup()
{   
    if (m_hHLRC != NULL)
    {

        hlBeginFrame();
        hlStopEffect(m_effectName);
        hlEndFrame();    

        hlDeleteEffects(m_effectName, 1);

		for (auto piece : m_HapticPieces)
		{
			hlRemoveEventCallback(HL_EVENT_TOUCH, piece.first, HL_CLIENT_THREAD, hlTouchCB);
			hlRemoveEventCallback(HL_EVENT_UNTOUCH, piece.first, HL_CLIENT_THREAD, hlUntouchCB);
			hlRemoveEventCallback(HL_EVENT_1BUTTONDOWN, piece.first, HL_CLIENT_THREAD, hlButtonDownCB);
			hlRemoveEventCallback(HL_EVENT_1BUTTONUP, piece.first, HL_CLIENT_THREAD, hlButtonUpCB);
		}                      

        hlMakeCurrent(NULL);
        hlDeleteContext(m_hHLRC);
    }

    if (m_hHD != HD_INVALID_HANDLE)
        hdDisableDevice(m_hHD);
}




/*******************************************************************************
 draw the cursor
*******************************************************************************/
void HapticManager::drawCursor(void)
	{
		//cout<<"redraw"<<endl;
		static const double kCursorRadius = 0.5;
		static const int kCursorTess = 15;
		HLdouble proxytransform[16];

		GLUquadricObj *qobj = 0;

		glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT);
		glPushMatrix();

		if (!gCursorDisplayList)
		{
			
			gCursorDisplayList = glGenLists(1);
			glNewList(gCursorDisplayList, GL_COMPILE);
			qobj = gluNewQuadric();
			glPushMatrix();
			glColor3f(1.,0,0); 
			gluSphere(qobj, kCursorRadius*2 * 4, kCursorTess, kCursorTess);
			glColor3f(0,0,1.);
			gluCylinder(qobj,kCursorRadius * 4,kCursorRadius * 4,40,100,5);
			glPopMatrix();
			gluDeleteQuadric(qobj);
			glEndList();
		}  

		// Apply the local position/rotation transform of the haptic device proxy.
		hlGetDoublev(HL_PROXY_TRANSFORM, proxytransform);
		glMultMatrixd(proxytransform);
        
		// Apply the local cursor scale factor.
		glScaled(gCursorScale, gCursorScale, gCursorScale);
	
		glEnable(GL_NORMALIZE);
		glEnable(GL_COLOR_MATERIAL);
		glColor3f(0.0, 0.5, 1.0);

		glCallList(gCursorDisplayList);

		glPopMatrix(); 
		glPopAttrib();
}



/*******************************************************************************
 Draw what the haptic device must render
*******************************************************************************/
void HapticManager::feelPoints()
{      
    hlBeginFrame();

	for (auto hapticPiece : m_HapticPieces)
	{
		// Don't touch the active piece.
		if ( hapticPiece.second == g_activePiece )
			continue;

		PuzzlePiece* puzzlePiece = hapticPiece.second;
		if ( !puzzlePiece )
			continue;
		
		// Don't allow interaction with matched pieces.
		if ( puzzlePiece->GetIsMatched() )
			continue;

		hlBeginShape(HL_SHAPE_FEEDBACK_BUFFER, hapticPiece.first);
			hlTouchableFace(HL_FRONT);
			m_pFaceManager->drawPuzzlePiece(puzzlePiece->GetID());
		hlEndShape();
	}

    // Call any event callbacks that have been triggered.
    hlCheckEvents();
    hlEndFrame();

	updateDragObjectTransform();
}

/******************************************************************************
Calculates updated object transform for drag object based on changes to
proxy transform.
******************************************************************************/
void HapticManager::updateDragObjectTransform()
{
	if ( g_activePiece == nullptr )
		return;

	// Calculated delta between current proxy pos and proxy pos at start 
	// of drag.
	hduVector3Dd proxyPos;
	hlGetDoublev(HL_PROXY_POSITION, proxyPos);
	hduVector3Dd dragDeltaTransl = proxyPos - gStartDragProxyPos;

	// Same for rotation.
	hduMatrix deltaRotMat;
	hduQuaternion proxyRotq;
	hlGetDoublev(HL_PROXY_ROTATION, proxyRotq);
	hduQuaternion dragDeltaRot = gStartDragProxyRot.inverse() * proxyRotq;
	dragDeltaRot.normalize();
	dragDeltaRot.toRotationMatrix(deltaRotMat);

	// Want to rotate about the proxy position, not the origin,
	// so need to translate to/from proxy pos.
	hduMatrix toProxy = hduMatrix::createTranslation(-gStartDragProxyPos);
	hduMatrix fromProxy = hduMatrix::createTranslation(gStartDragProxyPos);
	deltaRotMat = toProxy * deltaRotMat * fromProxy;

	// Compose rotation and translation deltas.
	hduMatrix deltaMat = deltaRotMat * hduMatrix::createTranslation(dragDeltaTransl);

	// Force the player to drop the active piece if it matches.
	if ( !g_activePiece->GetIsMatched() )
		*( g_activePiece->GetTransform()) = gStartDragObjTransform * deltaMat; // Apply deltas to the drag object transform.

	checkMatches();
}

void HapticManager::checkMatches()
{
	string partnerId = g_activePiece->GetPartnerId();

	PuzzlePiece* partner = nullptr;
	for ( auto piece : m_HapticPieces )
	{
		if ( piece.second->GetID() == partnerId )	// find the partner to our active piece
		{
			partner = piece.second;
			break;
		}
	}

	if ( !partner )
		return;

	hduVector3Dd myCenter = g_activePiece->GetCenter();
	hduVector3Dd partnerCenter = partner->GetCenter();

	if ( (fabs(myCenter[0] - partnerCenter[0]) < 0.3) &&	// bounds are 'close enough'
		 (fabs(myCenter[1] - partnerCenter[1]) <= 1.0) &&
		 (fabs(myCenter[2] - partnerCenter[2]) < 0.3) )
	{
		g_activePiece->SetIsMatched( true );
		g_activePiece->SetColor( { 0.5, 0.5, 0.5, 1.0 } );

		partner->SetIsMatched( true );
		partner->SetColor( { 0.5, 0.5, 0.5, 1.0 } );
	}
}

/*******************************************************************************
 Highlights the touched object.
*******************************************************************************/
void HLCALLBACK HapticManager::hlTouchCB(HLenum event, HLuint object, HLenum thread, HLcache *cache, void *userdata)
{
	//PuzzlePiece* piece = (PuzzlePiece*)userdata;
	//if (!piece)
	//	return;

	//piece->SetColor( { (GLfloat)1.0, (GLfloat)0.0, (GLfloat)0.0, (GLfloat)0.7 } );
}

void HLCALLBACK HapticManager::hlUntouchCB(HLenum event, HLuint object,HLenum thread, HLcache *cache, void *userdata)
{
	//PuzzlePiece* piece = (PuzzlePiece*)userdata;
	//if ( !piece )
	//	return;

	//if( piece != g_activePiece )
	//	piece->SetColor({ (GLfloat)0.0, (GLfloat)0.0, (GLfloat)1.0, (GLfloat)0.7 });
}

/*******************************************************************************
 Starts manipulation upon button press.
*******************************************************************************/
void HLCALLBACK HapticManager::hlButtonDownCB(HLenum event, HLuint object,
                                              HLenum thread, HLcache *cache,
                                              void *userdata)
{
	PuzzlePiece* piece = (PuzzlePiece*)userdata;
	if ( !piece )
		return;
	
	g_activePiece = piece;

	// Store off proxy position so we can compute how much it moves each
	// frame (which is how much the drag object should also move).
	hlGetDoublev(HL_PROXY_POSITION, gStartDragProxyPos);
	hlGetDoublev(HL_PROXY_ROTATION, gStartDragProxyRot);

	// Store off initial position and orientation of drag object.
	gStartDragObjTransform = *(piece->GetTransform());

	piece->SetColor({ (GLfloat)1.0, (GLfloat)0.0, (GLfloat)0.0, piece->GetColor().alpha });
}


/*******************************************************************************
 Stops manipulation upon button release.
*******************************************************************************/
void HLCALLBACK HapticManager::hlButtonUpCB(HLenum event, HLuint object,
                                            HLenum thread, HLcache *cache, 
                                            void *userdata)
{
	if( g_activePiece != nullptr && (!g_activePiece->GetIsMatched()))
		g_activePiece->SetColor({ (GLfloat)0.0, (GLfloat)0.0, (GLfloat)1.0, g_activePiece->GetColor().alpha });

	// Button up, done dragging, clear current drag object.
	g_activePiece = nullptr;
}

/*******************************************************************************
 OpenGL function for Displaying string on the screen.
*******************************************************************************/
void HapticManager::DrawBitmapString(GLfloat x, GLfloat y, void *font, char *format,...)
	{
		int len, i;
		va_list args;
		char string[256];

		// special C stuff to interpret a dynamic set of arguments specified by "..."
		va_start(args, format);
		vsprintf_s(string, format, args);
		va_end(args);

		glRasterPos2f(x, y);
		len = (int) strlen(string);
	
		for (i = 0; i < len; i++)
		{
			glutBitmapCharacter(font, string[i]);
		}
	}

/*******************************************************************************
 Information that will be displayed on the screen
*******************************************************************************/
void HapticManager::DisplayInfo(void)
{
	int mWindW=600;
	int mWindH=600;
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

    glPushMatrix();
    glLoadIdentity();

	// switch to 2d orthographic mMode for drawing text
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, mWindW, mWindH, 0);
	glMatrixMode(GL_MODELVIEW);
	
	glColor3f(1.0, 1.0, 1.0);

	int textRowDown = 0; // lines of text already drawn downwards from the top
	int textRowUp = 0; // lines of text already drawn upwards from the bottom
	HLdouble proxyRot[3];
	hdGetDoublev(HD_CURRENT_POSITION, proxyRot);	
	DrawBitmapString(5, 20 + textRowDown * 15, GLUT_BITMAP_HELVETICA_10, "position:");
	textRowDown++;
	DrawBitmapString(5, 20 + textRowDown * 15, GLUT_BITMAP_HELVETICA_10 /*GLUT_BITMAP_9_BY_15*/, "%4.1f,%4.1f,%4.1f", proxyRot[0],proxyRot[1],proxyRot[2]);
	textRowDown++;
	HLdouble proxyRot1[3];
	hlGetDoublev(HL_PROXY_POSITION, proxyRot1); 
	DrawBitmapString(5, 20 + textRowDown * 15, GLUT_BITMAP_HELVETICA_10 /*GLUT_BITMAP_9_BY_15*/, "%4.1f,%4.1f,%4.1f", proxyRot1[0],proxyRot1[1],proxyRot1[2]);
	textRowDown++;
	HLdouble proxyRot2[3];
	hlGetDoublev(HL_DEVICE_POSITION, proxyRot2);	
	DrawBitmapString(5, 20 + textRowDown * 15, GLUT_BITMAP_HELVETICA_10 /*GLUT_BITMAP_9_BY_15*/, "%4.1f,%4.1f,%4.1f", proxyRot2[0],proxyRot2[1],proxyRot2[2]);
	textRowDown++;
	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
    glPopMatrix();	
	// turn depth and lighting back on for 3D rendering
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}

/*******************************************************************************
 function for updating the mapping between opengl anf the haptic device (DO NOT TOUCH)
*******************************************************************************/
void HapticManager::UpdateHapticMapping(void)
{
	
	GLdouble modelview[16];
    GLdouble projection[16];
    GLint viewport[4];

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);
  
    hlMatrixMode(HL_TOUCHWORKSPACE);
    hlLoadIdentity();
	
	HLdouble minPoint[3], maxPoint[3];
	
	minPoint[0]=-10; minPoint[1]=-10; minPoint[2]=1;
	maxPoint[0]=20; maxPoint[1]=20; maxPoint[2]=-20;
	hluFitWorkspaceBox(modelview, minPoint, maxPoint); 

    // Compute cursor scale.
    gCursorScale = hluScreenToModelScale(modelview, projection, viewport);
    gCursorScale *= 20;

}
