#version 420

layout(origin_upper_left) in vec4 gl_FragCoord;

uniform MyGC {
  mat4 pm;      /* macierz rzutowania */
  vec4 fg, bk;  /* kolory znakow i tla */
} gc;

uniform MyFont {
  int  chw, chh;   /* szerokosc i wysokosc znaku */
  int  chf, chl;   /* kody pierwszego i ostatniego znaku */
  uint glyphs[240];
} font;

uniform MyText {
  int  x, y;      /* pozycja pierwszego znaku */
  int  l;         /* dlugosc napisu */
  uint text[64];  /* upakowane znaki napisu */
} text;

out vec4 out_Color;

#define SETFRAG(C) \
 { if ( gc.C.a > 0.0 ) {\
     out_Color = gc.C;\
     return;\
   }\
   else discard; }

#define EXTRACTBYTE(x,b)\
  switch ( b ) {\
default: break;\
case  1: x >>=  8;  break;\
case  2: x >>= 16;  break;\
case  3: x >>= 24;  break;\
  }

void main ( void )
{
  int  x0, y0;
  uint c, r, mask, chrow;

  x0 = int(gl_FragCoord.x) - text.x;
  if ( x0 < 0 || x0 >= font.chw*text.l )
    SETFRAG ( bk )
  y0 = int(gl_FragCoord.y) - text.y - 1;
  if ( y0 < 0 || y0 >= font.chh )
    SETFRAG ( bk )
  c = text.text[x0/(font.chw*4)];
  EXTRACTBYTE ( c, (x0/font.chw) % 4 )
  c &= 0xFF;
  if ( c < font.chf || c > font.chl )
    SETFRAG ( bk )
  c -= font.chf;
  r = c*font.chh + y0;
  if ( font.chw <= 8 ) {
    chrow = font.glyphs[r/4];
    EXTRACTBYTE ( chrow, r % 4 )
  }
  else if ( font.chw <= 16 ) {
    chrow = font.glyphs[r/2];
    if ( r % 2 != 0 )
      chrow >>= 16;
  }
  else
    chrow = font.glyphs[r];
  x0 %= font.chw;
  mask = 0x01 << x0;
  if ( (chrow & mask) != 0 )
    SETFRAG ( fg )
  else
    SETFRAG ( bk )
} /*main*/

