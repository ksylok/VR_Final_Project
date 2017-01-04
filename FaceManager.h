
#ifndef FaceManager_H_
#define FaceManager_H_

#include "PuzzlePiece.h"

#include <map>

// UPDATED: 2012-12-05  (Y. Hu: Microsoft Visual Studio 2010; System configuration - Windows 7 + 16:9 screen ratio) 
// UPDATED: 2013-07-30  (Y. Hu: Microsoft Visual Studio 2012 + Platform toolset in Visual Studio 2010 due to OpenHatics-V3.10.5; System configuration - Windows 7 + 16:9 screen ratio) 
// UPDATED: 2014-05-30  (Y. Hu: Microsoft Visual Studio 2012 + Platform toolset in Visual Studio 2010 due to OpenHatics-V3.2.2; System configuration - Windows 7 + 16:9 screen ratio)
// UPDATED: 2015-07-21  (Y. Hu: Microsoft Visual Studio 2013 + Platform toolset in Visual Studio 2013; OpenHatics-V3.2.2; System configuration - Windows 7 + 16:9 screen ratio)
// UPDATED: 2016-08-25  (Y. Hu: Microsoft Visual Studio 2015 + Platform toolset in Visual Studio 2015; OpenHatics-V3.2.2; System configuration - Windows 7 + 16:9 screen ratio)

/******************************************************************************
 IPointManager Interface
******************************************************************************/
enum drawMode { solidWireframe, translucent };

class IFaceManager
{
public:
    static IFaceManager *create(void);
    static void destroy(IFaceManager *&fInterface);
    
    virtual void setup() = 0;
    virtual void cleanup() = 0;

	virtual void setDrawMode( drawMode newMode ) = 0;
	virtual void drawPuzzlePiece( string id ) = 0;
	virtual void drawAllPuzzlePieces() = 0;

	virtual void drawCenters() = 0;

	virtual map<string, PuzzlePiece*> getPieces() = 0;

protected:
    IFaceManager() {}
    virtual ~IFaceManager() {}

}; 

#endif // FaceManager_H_
