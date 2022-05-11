#version 420

#define MY_LEVEL0 10
#define MY_LEVEL1 10

layout(vertices=3) out;

in Vertex {
    vec4 Colour;
  } In[];

out TCVertex {
    vec4 Colour;
  } Out[];

void main ( void )
{
  if ( gl_InvocationID == 0 ) {
    gl_TessLevelOuter[0] = MY_LEVEL0;
    gl_TessLevelOuter[1] = MY_LEVEL0;
    gl_TessLevelOuter[2] = MY_LEVEL0;
    gl_TessLevelInner[0] = MY_LEVEL1;
  }
  gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
  Out[gl_InvocationID].Colour = In[gl_InvocationID].Colour;
} /*main*/
