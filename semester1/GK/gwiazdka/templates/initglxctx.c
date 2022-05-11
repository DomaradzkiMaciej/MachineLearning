#include <stdlib.h>   
#include <string.h>
#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl3w.h>
#include <GL/glx.h>

#include "utilities.h"
#include "initglxctx.h"

typedef GLXContext (*PFNGLXCREATECONTEXTATTRIBSARBPROC)
                     ( Display *dpy, GLXFBConfig config,
                       GLXContext share_context, Bool direct,
                       const int *attrib_list );

Display    *xdisplay;
int        xscreen;
Window     xrootwin;
Visual     *xvisual;
Colormap   xcolormap;
GLXContext glxcontext;

void InitGLXContext ( int major, int minor )
{
  PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB;
  int nelements;
  int visattr[] =
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
  GLXFBConfig *fbc;

  if ( !(vii = glXChooseVisual( xdisplay, 0, visattr )) )
    exit ( 1 );
  xvisual = vii->visual;
  ctxattr[1] = major;  ctxattr[3] = minor;
  fbc = glXChooseFBConfig ( xdisplay, xscreen, 0, &nelements );
  if ( (glXCreateContextAttribsARB =
        (PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddress
            ( (const GLubyte*)"glXCreateContextAttribsARB" )) ) {
    if ( !(glxcontext = glXCreateContextAttribsARB ( xdisplay,
                                           *fbc, 0, 1, ctxattr )) )
      exit ( 1 );
  }
  else
    exit ( 1 );
  fbc = glXChooseFBConfig ( xdisplay, xscreen, 0, &nelements );
  glXCreateContextAttribsARB =
     (PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddress
         ( (const GLubyte*)"glXCreateContextAttribsARB" );
  glxcontext = glXCreateContextAttribsARB ( xdisplay, *fbc, 0, 1, ctxattr );
} /*InitGLXContext*/

