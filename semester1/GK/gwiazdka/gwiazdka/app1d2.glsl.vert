#version 420

layout(location=0) in vec4 in_Position;
layout(location=1) in vec4 in_Colour;

out Vertex {
  vec4 Colour;
} Out;

void main ( void )
{
  gl_Position = in_Position;
  Out.Colour = in_Colour;
} /*main*/
