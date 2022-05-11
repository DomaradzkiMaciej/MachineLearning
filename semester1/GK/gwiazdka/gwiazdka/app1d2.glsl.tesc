#version 420

#define MY_LEVEL 10

layout(vertices=2) out;

in Vertex {
    vec4 Colour;
  } In[];

out TCVertex {
    vec4 Colour;
  } Out[];

void main ( void )
{
  if ( gl_InvocationID == 0 ) {
    gl_TessLevelOuter[0] = 1;
    gl_TessLevelOuter[1] = MY_LEVEL;
  }
  gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
  Out[gl_InvocationID].Colour = In[gl_InvocationID].Colour;
} /*main*/
