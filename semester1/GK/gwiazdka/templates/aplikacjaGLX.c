#include <stdlib.h>
#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl3w.h>
#include <GL/glx.h>

#include "utilities.h"
#include "initglxctx.h"
#include "myheader.h"

Window xmywin;
XEvent xevent;
char   terminate;

void LoadMyShaders ( void )
{
} /*LoadMyShaders*/

void DestroyMyShaders ( void )
{
} /*DestroyMyShaders*/

void InitMyObject ( int argc, char **argv )
{
} /*InitMyObject*/

void DestroyMyObject ( void )
{
} /*DestroyMyObject*/

void Initialise ( int argc, char **argv )
{
  InitMyGLXWindow ( argc, argv, 4, 2, 480, 360 );
  LoadMyShaders ();
  InitMyObject ( argc, argv );
} /*Initialise*/

void MyWinExpose ( void )
{
  glClearColor ( 0.0, 0.5, 1.0, 1.0 );
  glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glFlush ();
  glXSwapBuffers ( xdisplay, xmywin );
} /*MyWinExpose*/

void MyWinConfigureNotify ( int width, int height )
{
} /*MyWinConfigureNotify*/

void MyWinButtonPress ( int button, int x, int y )
{
} /*MyWinButtonPress*/

void MyWinButtonRelease ( int button, int x, int y )
{
} /*MyWinButtonRelease*/

void MyWinMotionNotify ( int x, int y )
{
} /*MyWinMotionNotify*/

void MyWinKeyPress ( unsigned int state, unsigned int key )
{
} /*MyWinKeyPress*/

void MyWinClientMessage ( void )
{
} /*MyWinClientMessage*/

void MyWinMessageProc ( void )
{
  switch ( xevent.xany.type ) {
case Expose:
    if ( xevent.xexpose.count == 0 )
      MyWinExpose ();
    break;
case ConfigureNotify:
    MyWinConfigureNotify ( xevent.xconfigure.width, xevent.xconfigure.height );
    break;
case ButtonPress:
    MyWinButtonPress ( xevent.xbutton.button,
                       xevent.xbutton.x, xevent.xbutton.y );
    break;
case ButtonRelease:
    MyWinButtonRelease ( xevent.xbutton.button,  
                         xevent.xbutton.x, xevent.xbutton.y );
    break;
case MotionNotify:
    MyWinMotionNotify ( xevent.xmotion.x, xevent.xmotion.y );
    break;
case KeyPress:
    MyWinKeyPress ( xevent.xkey.state, xevent.xkey.keycode );
    break;
case ClientMessage:
    MyWinClientMessage ();
    break;
default:    break;
  }
} /*MyWinMessageProc*/

void MessageLoop ( void )
{
  terminate = 0;
  do {
    XNextEvent ( xdisplay, &xevent );
    if ( xevent.xany.window == xmywin )
      MyWinMessageProc ();
  } while ( !terminate );
} /*MessageLoop*/

void InitMyGLXWindow ( int argc, char **argv,
                       int major, int minor, int width, int height )
{
  XSetWindowAttributes swa;

  if ( !(xdisplay = XOpenDisplay ( "" )) )
    exit ( 1 );
  xscreen = DefaultScreen ( xdisplay );
  xrootwin = RootWindow ( xdisplay, xscreen );
  InitGLXContext ( major, minor );
  if ( !(xcolormap = XCreateColormap ( xdisplay, xrootwin,
                                       xvisual, AllocNone )) )
    exit ( 1 );
  swa.colormap = xcolormap;
  swa.event_mask = ExposureMask | StructureNotifyMask| ButtonPressMask |
                   ButtonReleaseMask | PointerMotionMask | KeyPressMask ;
  xmywin = XCreateWindow ( xdisplay, xrootwin, 0, 0, width, height,
                           0, 24, InputOutput, xvisual,
                           CWColormap | CWEventMask, &swa );
  XMapWindow ( xdisplay, xmywin );
  if ( !glXMakeCurrent ( xdisplay, xmywin, glxcontext ) ) {
    printf ( "Error: glXMakeCurrent failed\n" );
    exit ( 1 );
  }
  if ( gl3wInit () )
    exit ( 1 );
} /*InitMyGLXWindow*/

void DestroyMyGLXWindow ( void )
{
  glXMakeContextCurrent ( xdisplay, None, None, NULL );
  glXDestroyContext ( xdisplay, glxcontext );
  XDestroyWindow ( xdisplay, xmywin );
  XCloseDisplay ( xdisplay );
} /*DestroyMyGLXWindow*/

void Cleanup ( void )
{
  DestroyMyObject ();
  DestroyMyShaders ();
  DestroyMyGLXWindow ();
} /*Cleanup*/

int main ( int argc, char **argv )
{
  Initialise ( argc, argv );
  MessageLoop ();
  Cleanup ();
  exit ( 0 );
} /*main*/
