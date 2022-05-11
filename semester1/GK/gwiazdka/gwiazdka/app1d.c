#include <stdlib.h>
#include <stdio.h>
#include "openglheader.h"   /* najpierw ten */
#include <GL/freeglut.h>  /* potem ten */

#include "utilities.h"
#include "lights.h"
#include "mygltext.h"
#include "app1d.h"

int     WindowHandle;
int     win_width, win_height;
int     last_xi, last_eta;
float   left, right, bottom, top, near, far;

int     app_state = STATE_NOTHING;
float   viewer_rvec[3] = {1.0,0.0,0.0};
float   viewer_rangle = 0.0;

int     option = 2;
char    animate = false;
char    enlight = true;
char    tesselate = 0;

void ReshapeFunc ( int width, int height )
{
  float lr;

  glViewport ( 0, 0, width, height );      /* klatka jest calym oknem */
  SetupTextFrame ( width, height );
  lr = 0.5533*(float)width/(float)height;  /* przyjmujemy aspekt rowny 1 */
  M4x4Frustumf ( trans.pm, NULL, -lr, lr, -0.5533, 0.5533, 5.0, 15.0 );
  glBindBuffer ( GL_UNIFORM_BUFFER, trbuf );
  glBufferSubData ( GL_UNIFORM_BUFFER, trbofs[3], 16*sizeof(GLfloat), trans.pm );
  win_width = width,  win_height = height;
  left = -(right = lr);  bottom = -(top = 0.5533);  near = 5.0;  far = 15.0;
  ExitIfGLError ( "ReshapeFunc" );
  SetupMVPMatrix ();
} /*ReshapeFunc*/

void DisplayFunc ( void )
{
  static GLfloat fg[4] = { 0.0, 0.0, 1.0, 1.0 };
  static GLfloat bk[4] = { 0.0, 0.0, 0.0, 0.0 };

  glClearColor ( 1.0, 1.0, 1.0, 1.0 );
  glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  if ( app_state == STATE_TURNING ) {
    SetTextForeground ( fg );
    SetTextBackground ( bk );
    DisplayTextObject ( vptext );
  }
  glEnable ( GL_DEPTH_TEST );
  if ( tesselate )
    DrawTessIcos ( option, enlight );
  else
    DrawIcosahedron ( option, enlight );
  glUseProgram ( 0 );
  glFlush ();
  glutSwapBuffers ();
} /*DisplayFunc*/

void ToggleAnimation ( void )
{
  if ( (animate = !animate) ) {
    TimerTic ();
    glutIdleFunc ( IdleFunc );
  }
  else {
    glutIdleFunc ( NULL );
    model_rot_angle0 = model_rot_angle;
  }
} /*ToggleAnimation*/

void ToggleLight ( void )
{
  enlight = !enlight;
  glutPostWindowRedisplay ( WindowHandle );
} /*ToggleLight*/

void ToggleTesselation ( void )
{
  tesselate = !tesselate;
  glutPostWindowRedisplay ( WindowHandle );
} /*ToggleTesselation*/

void KeyboardFunc ( unsigned char key, int x, int y )
{
  switch ( key ) {
case 0x1B:            /* klawisz Esc - zatrzymanie programu */
    Cleanup ();
    glutLeaveMainLoop ();
    break;
case 'W':  case 'w':  /* przelaczamy na wierzcholki */
    option = 0;
    glutPostWindowRedisplay ( WindowHandle );
    break;
case 'K':  case 'k':  /* przelaczamy na krawedzie */
    option = 1;
    glutPostWindowRedisplay ( WindowHandle );
    break;
case 'S':  case 's':  /* przelaczamy na sciany */
    option = 2;
    glutPostWindowRedisplay ( WindowHandle );
    break;
case 'L':  case 'l':
    ToggleLight ();
    break;
case 'T':  case 't':
    ToggleTesselation ();
    break;
case ' ':             /* wlaczamy albo wylaczamy animacje */
    ToggleAnimation ();
    break;
default:              /* ignorujemy wszystkie inne klawisze */ 
    break;
  }
} /*KeyboardFunc*/

void MouseFunc ( int button, int state, int x, int y )
{
  switch ( app_state ) {
case STATE_NOTHING:
    if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN ) {
      last_xi = x,  last_eta = y;
      app_state = STATE_TURNING;
      glutPostWindowRedisplay ( WindowHandle );
    }
    break;
case STATE_TURNING:
    if ( button == GLUT_LEFT_BUTTON && state != GLUT_DOWN ) {
      app_state = STATE_NOTHING;
      glutPostWindowRedisplay ( WindowHandle );
    }
    break;
default:
    break;
  }
} /*MouseFunc*/

void MotionFunc ( int x, int y )
{
  switch ( app_state ) {
case STATE_TURNING:
    if ( x != last_xi || y != last_eta ) {
      RotateViewer ( x-last_xi, y-last_eta );
      NotifyViewerPos ();
      last_xi = x,  last_eta = y;
      glutPostWindowRedisplay ( WindowHandle );
    }
    break;
default:
    break;
  }
} /*MotionFunc*/

void JoystickFunc ( unsigned int buttonmask, int x, int y, int z ) { }

void IdleFunc ( void )
{
  model_rot_angle = model_rot_angle0 + 0.78539816 * TimerToc ();
  SetupModelMatrix ( model_rot_axis, model_rot_angle );
  glutPostWindowRedisplay ( WindowHandle );
} /*IdleFunc*/

void TimerFunc ( int value ) {  }

void Initialise ( int argc, char *argv[] )
{
  glutInit ( &argc, argv );
  glutInitContextVersion ( 2, 1 );
  glutInitContextFlags ( GLUT_FORWARD_COMPATIBLE );
  glutInitContextProfile ( GLUT_CORE_PROFILE );
  glutSetOption ( GLUT_ACTION_ON_WINDOW_CLOSE,
                  GLUT_ACTION_GLUTMAINLOOP_RETURNS );
  glutInitWindowSize ( 480, 360 );
  glutInitDisplayMode ( GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA );
  WindowHandle = glutCreateWindow ( "Gwiazdka" );
  if ( WindowHandle < 1 ) {
    fprintf ( stderr, "Error: Could not create a window\n" );
    exit ( 1 );
  }
  glutReshapeFunc ( ReshapeFunc );
  glutDisplayFunc ( DisplayFunc );
  glutKeyboardFunc ( KeyboardFunc );
  glutMouseFunc ( MouseFunc );
  glutMotionFunc ( MotionFunc );
  /*glutJoystickFunc ( JoystickFunc, 16 );*/
  /*glutTimerFunc ( 0, TimerFunc, 0 );*/
  GetGLProcAddresses ();
#ifdef USE_GL3W
  if ( !gl3wIsSupported ( 4, 2 ) )
    ExitOnError ( "Initialise: OpenGL version 4.2 not supported\n" );
#endif
  LoadMyShaders ();
  LoadTextShaders ();
  InitMyObject ();
} /*Initialise*/

int main ( int argc, char *argv[] )
{
  Initialise ( argc, argv );
  glutMainLoop ();
  exit ( 0 );
} /*main*/
