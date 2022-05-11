#version 420

uniform MyGC {
  mat4 pm;      /* macierz rzutowania */
  vec4 fg, bk;  /* kolory znakow i tla */
} gc;

layout(location=0) in vec4 in_Position;

void main ( void )
{
  gl_Position = gc.pm * in_Position;
} /*main*/

