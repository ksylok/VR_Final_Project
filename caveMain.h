
#ifndef PointManipulationAfx_H_
#define PointManipulationAfx_H_

// UPDATED: 2012-12-05  (Y. Hu: Microsoft Visual Studio 2010; System configuration - Windows 7 + 16:9 screen ratio) 
// UPDATED: 2013-07-30  (Y. Hu: Microsoft Visual Studio 2012 + Platform toolset in Visual Studio 2010 due to OpenHatics-V3.10.5; System configuration - Windows 7 + 16:9 screen ratio) 
// UPDATED: 2014-05-30  (Y. Hu: Microsoft Visual Studio 2012 + Platform toolset in Visual Studio 2010 due to OpenHatics-V3.2.2; System configuration - Windows 7 + 16:9 screen ratio) 
// UPDATED: 2015-07-21  (Y. Hu: Microsoft Visual Studio 2013 + Platform toolset in Visual Studio 2013; OpenHatics-V3.2.2; System configuration - Windows 7 + 16:9 screen ratio) 
// UPDATED: 2016-08-25  (Y. Hu: Microsoft Visual Studio 2015 + Platform toolset in Visual Studio 2015; OpenHatics-V3.2.2; System configuration - Windows 7 + 16:9 screen ratio)

#if defined(WIN32)
// identifier was truncated to '255' characters in the debug information.
# pragma warning( disable: 4786 )
# include <windows.h>
#endif // WIN32

#if defined(WIN32) || defined(linux)
#include <cave_ogl.h>
#elif defined(__APPLE__)
# include <GLUT/glut.h>
#endif

#include <HD/hd.h>
#include <HDU/hduVector.h>

#endif // PointManipulationAfx_H_
