#include <stdlib.h>   
#include <string.h>
#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "openglheader.h"
#include <GL/glx.h>

#include "xvariables.h"
#include "initglxctx.h"
#include "utilities.h"

typedef GLXContext (*PFNGLXCREATECONTEXTATTRIBSARBPROC)
                     ( Display *dpy, GLXFBConfig config,
                       GLXContext share_context, Bool direct,
                       const int *attrib_list );

Display     *xdisplay;
int         xscreen;
Window      xrootwin;
Visual      *xvisual;
Colormap    xcolormap;
GLXFBConfig *glxfbc;
GLXContext  glxcontext;

void InitGLXContext ( int major, int minor, int *visattr )
{
  PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB;
  int nelements;
  int vattr[] =
    { GLX_RGBA,
      GLX_DOUBLEBUFFER,
      GLX_RED_SIZE,    8,
      GLX_GREEN_SIZE,  8,
      GLX_BLUE_SIZE,   8,
      GLX_DEPTH_SIZE, 24,
      None };
  int ctxattr[] =
    { GLX_CONTEXT_MAJOR_VERSION_ARB, 0,
      GLX_CONTEXT_MINOR_VERSION_ARB, 0,
      GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
      GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
      None };
  XVisualInfo *vii;

  if ( !visattr )
    visattr = vattr;
  if ( !(vii = glXChooseVisual( xdisplay, 0, visattr )) )
    ExitOnError ( "InitGLContext 0" );
  xvisual = vii->visual;
  ctxattr[1] = major;  ctxattr[3] = minor;
  glxfbc = glXChooseFBConfig ( xdisplay, xscreen, 0, &nelements );
  if ( (glXCreateContextAttribsARB =
        (PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddress
            ( (const GLubyte*)"glXCreateContextAttribsARB" )) ) {
    if ( !(glxcontext = glXCreateContextAttribsARB ( xdisplay,
                                           *glxfbc, 0, 1, ctxattr )) )
      ExitOnError ( "InitGLContext 1" );
  }
  else
    ExitOnError ( "InitGLContext 2" );
  glxfbc = glXChooseFBConfig ( xdisplay, xscreen, 0, &nelements );
  glXCreateContextAttribsARB =
     (PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddress
         ( (const GLubyte*)"glXCreateContextAttribsARB" );
  glxcontext = glXCreateContextAttribsARB ( xdisplay, *glxfbc, 0, 1, ctxattr );
  XFree ( vii );
} /*InitGLXContext*/

