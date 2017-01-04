
#ifndef HapticManager_H_
#define HapticManager_H_

// UPDATED: 2012-12-05  (Y. Hu: Microsoft Visual Studio 2010; System configuration - Windows 7 + 16:9 screen ratio) 
// UPDATED: 2013-07-30  (Y. Hu: Microsoft Visual Studio 2012 + Platform toolset in Visual Studio 2010 due to OpenHatics-V3.10.5; System configuration - Windows 7 + 16:9 screen ratio) 
// UPDATED: 2014-05-30  (Y. Hu: Microsoft Visual Studio 2012 + Platform toolset in Visual Studio 2010 due to OpenHatics-V3.2.2; System configuration - Windows 7 + 16:9 screen ratio) 
// UPDATED: 2015-07-21  (Y. Hu: Microsoft Visual Studio 2013 + Platform toolset in Visual Studio 2013; OpenHatics-V3.2.2; System configuration - Windows 7 + 16:9 screen ratio)
// UPDATED: 2016-08-25  (Y. Hu: Microsoft Visual Studio 2015 + Platform toolset in Visual Studio 2015; OpenHatics-V3.2.2; System configuration - Windows 7 + 16:9 screen ratio)

class IFaceManager;

class IHapticManager
{
public:

	static IHapticManager *create();
	static void destroy(IHapticManager *&pInterface);

	virtual void setup(IFaceManager *pFaceManager) = 0;
	virtual void cleanup() = 0;

	virtual void UpdateHapticMapping(void) = 0;
	virtual void DisplayInfo(void) = 0;
	virtual void drawCursor() = 0;
	virtual void feelPoints() = 0;


protected:

	IHapticManager() {}
	virtual ~IHapticManager() {}

};

#endif // HapticManager_H_