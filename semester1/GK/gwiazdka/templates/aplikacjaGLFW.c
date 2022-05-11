#include <stdlib.h>
#include <stdio.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "utilities.h"
#include "myheader.h"

GLFWwindow   *mywindow;

void myGLFWErrorHandler ( int error, const char *description )
{
  fprintf ( stderr, "GLFW error: %s\n", description );
  exit ( 1 );
} /*myGLFWErrorHandler*/

void ReshapeFunc ( GLFWwindow *win, int width, int height ) {}
void DisplayFunc ( GLFWwindow *win ) {}
void KeyboardFunc ( GLFWwindow *win, int key, int scancode, int action, int mods ) {}
void MouseFunc ( GLFWwindow *win, int button, int action, int mods ) {}
void MotionFunc ( GLFWwindow *win, double x, double y ) {}
void LoadMyShaders ( void ) {}
void InitMyObject ( void ) {}
void Cleanup ( void ) {}

void Initialise ( int argc, char **argv )
{
  glfwSetErrorCallback ( myGLFWErrorHandler );
  if ( !glfwInit () ) {
    fprintf ( stderr, "Error: glfwInit failed\n" );
    exit ( 1 );
  }
  if ( !(mywindow = glfwCreateWindow ( 480, 360,
                                       "Druga aplikacja", NULL, NULL )) ) {
    glfwTerminate ();
    fprintf ( stderr, "Error: glfwCreateWindow failed\n" );
    exit ( 1 );
  }
  glfwMakeContextCurrent ( mywindow );
  if ( ec = gl3wInit () )
    exit ( 1 );
  glfwSetWindowSizeCallback ( mywindow, ReshapeFunc );
  glfwSetWindowRefreshCallback ( mywindow, DisplayFunc );
  glfwSetKeyCallback ( mywindow, KeyboardFunc );
  glfwSetMouseButtonCallback ( mywindow, MouseFunc );
  glfwSetCursorPosCallback ( mywindow, MotionFunc );
  LoadMyShaders ();
  InitMyObject ();
  ReshapeFunc ( mywindow, 480, 360 );
} /*Initialise*/

void MessageLoop ( void )
{
  while ( !glfwWindowShouldClose ( mywindow ) )
    glfwWaitEvents ();
} /*MessageLoop*/

int main ( int argc, char **argv )
{
  Initialise ( argc, argv );
  MessageLoop ();
  Cleanup ();
  glfwTerminate ();
  exit ( 0 );
} /*main*/
