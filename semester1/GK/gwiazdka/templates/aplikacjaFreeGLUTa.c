#include <stdlib.h>
#include <stdio.h>
#include <GL/gl3w.h>      /* najpierw ten */
#include <GL/freeglut.h>  /* potem ten */

#include "utilities.h"
#include "myheader.h"

int WindowHandle;
 ... /* inne zmienne globalne potrzebne w programie */

void Cleanup ( void ) { ... }

void ReshapeFunc ( int width, int height ) { ... }
void DisplayFunc ( void ) { ... }
void KeyboardFunc ( unsigned char key, int x, int y ) { ... }
void MouseFunc ( int button, int state, int x, int y ) { ... }
void MotionFunc ( int x, int y ) { ... }
void JoystickFunc ( unsigned int buttonmask, int x, int y, int z ) { ... }
void TimerFunc ( int value ) { ... }
void IdleFunc ( void ) { ... }

void LoadMyShaders ( void ) { ... }
void InitMyObject ( void ) { ... }

void Initialise ( int argc, char *argv[] )
{
  glutInit ( &argc, argv );
  glutInitContextVersion ( 4, 2 );
  glutInitContextFlags ( GLUT_FORWARD_COMPATIBLE );
  glutInitContextProfile ( GLUT_CORE_PROFILE );
  glutSetOption ( GLUT_ACTION_ON_WINDOW_CLOSE,
                  GLUT_ACTION_GLUTMAINLOOP_RETURNS );
  glutInitWindowSize ( 480, 360 );
  glutInitDisplayMode ( GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA );
  WindowHandle = glutCreateWindow ( "Aplikacja FreeGLUTa" );
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
  /*glutIdleFunc ( IdleFunc );*/
  if ( gl3wInit () ) {
    fprintf ( stderr, "Error: gl3wInit failes\n" );
    exit ( 1 );
  }
  LoadMyShaders ();
  InitMyObject ();
} /*Initialise*/

int main ( int argc, char *argv[] )
{
  Initialise ( argc, argv );
  glutMainLoop ();
  exit ( 0 );
} /*main*/
