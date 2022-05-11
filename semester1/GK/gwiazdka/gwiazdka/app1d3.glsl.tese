#version 420

layout(triangles,equal_spacing) in;

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
  float s, t, u;
  vec4  vert;

  s = gl_TessCoord.x;  t = gl_TessCoord.y;  u = gl_TessCoord.z;
  vert = s*gl_in[0].gl_Position + t*gl_in[1].gl_Position + u*gl_in[2].gl_Position;
  vert.xyz = normalize ( vert.xyz );  vert.w = 1.0;
  gl_Position = trb.mvpm * vert;
  Out.Colour = s*In[0].Colour + t*In[1].Colour + u*In[2].Colour;
} /*main*/
