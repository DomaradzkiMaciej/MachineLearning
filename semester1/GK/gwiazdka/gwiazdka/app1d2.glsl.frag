#version 420

in TEVertex {
  vec4 Colour;
} In;

out vec4 out_Colour;

void main ( void )
{
  out_Colour = In.Colour;
} /*main*/
