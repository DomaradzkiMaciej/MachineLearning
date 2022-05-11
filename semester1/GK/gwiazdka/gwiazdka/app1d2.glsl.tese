#version 420

layout(isolines,equal_spacing) in;

in TCVertex {
    vec4 Colour;
  } In[];

out TEVertex {
    vec4 Colour;
  } Out;

uniform TransBlock {
  mat4 mm, mmti, vm, pm, mvpm;
  vec4 eyepos;
} trb;

void main ( void )
{
  float t, t1;
  vec4  vert;

  t = gl_TessCoord.x;  t1 = 1.0-t;
  vert = t1*gl_in[0].gl_Position + t*gl_in[1].gl_Position;
  vert.xyz = normalize ( vert.xyz );  vert.w = 1.0;
  gl_Position = trb.mvpm * vert;
  Out.Colour = t1*In[0].Colour + t*In[1].Colour;
} /*main*/
