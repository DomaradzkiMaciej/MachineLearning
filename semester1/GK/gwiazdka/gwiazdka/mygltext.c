
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "openglheader.h"

#include "utilities.h"
#include "mygltext.h"

static GLuint text_program_id = 0, text_shader_id[2];
static GLuint gcbp, fontbp, textbp;

static GLuint gcbi, fontbi, textbi;
static GLint  gcofs[3], fontofs[5], textofs[4];
static GLuint gcbuf;

char LoadTextShaders ( void )
{
  static const char *filename[] =
    { "font.glsl.vert", "font.glsl.frag" };
  static const GLchar *UGCNames[] =
    { "MyGC", "MyGC.pm", "MyGC.fg", "MyGC.bk" };
  static const GLchar *UFontNames[] =
    { "MyFont", "MyFont.chw", "MyFont.chh",
      "MyFont.chf", "MyFont.chl", "MyFont.glyphs" };
  static const GLchar *UTextNames[] =
    { "MyText", "MyText.x", "MyText.y", "MyText.l", "MyText.text" };
  GLint gcbsize, fontsize, textsize;

  text_shader_id[0] = CompileShaderFiles ( GL_VERTEX_SHADER, 1, &filename[0] );
  text_shader_id[1] = CompileShaderFiles ( GL_FRAGMENT_SHADER, 1, &filename[1] );
  text_program_id = LinkShaderProgram ( 2, text_shader_id );
  GetAccessToUniformBlock ( text_program_id, 3, UGCNames,
                            &gcbi, &gcbsize, gcofs, &gcbp );
  GetAccessToUniformBlock ( text_program_id, 5, UFontNames,
                            &fontbi, &fontsize, fontofs, &fontbp );
  GetAccessToUniformBlock ( text_program_id, 4, UTextNames,
                            &textbi, &textsize, textofs, &textbp );
        /* utworz UBO bloku MyGC */
  glGenBuffers ( 1, &gcbuf );
  glBindBufferBase ( GL_UNIFORM_BUFFER, gcbp, gcbuf );
  glBufferData ( GL_UNIFORM_BUFFER, gcbsize, NULL, GL_DYNAMIC_DRAW );
  ExitIfGLError ( "LoadTextShaders" );
  return 1;
} /*LoadTextShaders*/

void SetupTextFrame ( GLint width, GLint height )
{
  GLfloat pm[16];

  M4x4Orthof ( pm, NULL, -0.5, (float)width-0.5,
               (float)height-0.5, -0.5, -1.0, 1.0 );
  glBindBuffer ( GL_UNIFORM_BUFFER, gcbuf );
  glBufferSubData ( GL_UNIFORM_BUFFER, gcofs[0], 16*sizeof(GLfloat), pm );
  ExitIfGLError ( "SetupTextFrame" );
} /*SetupTextFrame*/

void SetTextForeground ( GLfloat fg[4] )
{
  glBindBuffer ( GL_UNIFORM_BUFFER, gcbuf );
  glBufferSubData ( GL_UNIFORM_BUFFER, gcofs[1], 4*sizeof(GLfloat), fg );
  ExitIfGLError ( "SetForeground" );
} /*SetTextForeground*/

void SetTextBackground ( GLfloat bk[4] )
{
  glBindBuffer ( GL_UNIFORM_BUFFER, gcbuf );
  glBufferSubData ( GL_UNIFORM_BUFFER, gcofs[2], 4*sizeof(GLfloat), bk );
  ExitIfGLError ( "SetBackground" );
} /*SetTextBackground*/

myFont *NewFontObject ( GLint chw, GLint chh, GLint chf, GLint chl,
                        int size, GLvoid *glyphs )
{
  myFont *font;

  if ( (font = malloc ( sizeof(myFont) )) ) {
    font->chw = chw;  font->chh = chh;
    glGenBuffers ( 1, &font->ubo );
    glBindBuffer ( GL_UNIFORM_BUFFER, font->ubo );
    glBufferData ( GL_UNIFORM_BUFFER, fontofs[4]+size, NULL, GL_STATIC_DRAW );
    glBufferSubData ( GL_UNIFORM_BUFFER, fontofs[0], sizeof(GLint), &chw );
    glBufferSubData ( GL_UNIFORM_BUFFER, fontofs[1], sizeof(GLint), &chh );
    glBufferSubData ( GL_UNIFORM_BUFFER, fontofs[2], sizeof(GLint), &chf );
    glBufferSubData ( GL_UNIFORM_BUFFER, fontofs[3], sizeof(GLint), &chl );
    glBufferSubData ( GL_UNIFORM_BUFFER, fontofs[4], size, glyphs );
    ExitIfGLError ( "NewFontObject" );
  }
  return font;
} /*NewFontObject*/

void DeleteFontObject ( myFont *font )
{
  glDeleteBuffers ( 1, &font->ubo );
  free ( font );
  ExitIfGLError ( "DeleteFontObject" );
} /*DeleteFontObject*/

myTextObject *NewTextObject ( int maxlength )
{
  myTextObject *to;

  if ( (to = malloc ( sizeof(myTextObject) )) ) {
    memset ( to, 0, sizeof(myTextObject) );
    to->maxlength = maxlength;
    glGenVertexArrays ( 1, &to->vao );
    glBindVertexArray ( to->vao );
    glGenBuffers ( 2, to->buf );
    glBindBuffer ( GL_ARRAY_BUFFER, to->buf[0] );
    glBufferData ( GL_ARRAY_BUFFER, 8*sizeof(GLint), NULL, GL_DYNAMIC_DRAW );
    glEnableVertexAttribArray ( 0 );
    glVertexAttribPointer ( 0, 2, GL_INT, GL_FALSE, 2*sizeof(GLint), (GLvoid*)0 );
    glBindBuffer ( GL_UNIFORM_BUFFER, to->buf[1] );
    glBufferData ( GL_UNIFORM_BUFFER, textofs[3]+maxlength, NULL, GL_DYNAMIC_DRAW );
    ExitIfGLError ( "NewTextObject" );
  }
  return to;
} /*NewTextObject*/

void SetTextObjectContents ( myTextObject *to,
                             GLchar *text, GLint x, GLint y, myFont *font )
{
  GLint ta[8];
  int   lgt;

  lgt = strlen ( text );
  if ( lgt > to->maxlength )
    lgt = to->maxlength;
  y -= font->chh;
  glBindBuffer ( GL_UNIFORM_BUFFER, to->buf[1] );
  glBufferSubData ( GL_UNIFORM_BUFFER, textofs[0], sizeof(GLint), &x );
  glBufferSubData ( GL_UNIFORM_BUFFER, textofs[1], sizeof(GLint), &y );
  glBufferSubData ( GL_UNIFORM_BUFFER, textofs[2], sizeof(GLint), &lgt );
  glBufferSubData ( GL_UNIFORM_BUFFER, textofs[3], lgt*sizeof(GLchar), text );
  to->font = font;
  ta[0] = ta[6] = x;                ta[1] = ta[3] = y;
  ta[2] = ta[4] = x+lgt*font->chw;  ta[5] = ta[7] = y+font->chh;
  glBindBuffer ( GL_ARRAY_BUFFER, to->buf[0] );
  glBufferSubData ( GL_ARRAY_BUFFER, 0, 8*sizeof(GLint), ta );
  ExitIfGLError ( "SetTextObjectContents" );
} /*SetTextObjectContents*/

void DisplayTextObject ( myTextObject *to )
{
  glDisable ( GL_CULL_FACE );
  glDisable ( GL_DEPTH_TEST );
  glUseProgram ( text_program_id );
  glBindBufferBase ( GL_UNIFORM_BUFFER, fontbp, to->font->ubo );
  glBindVertexArray ( to->vao );
  glBindBufferBase ( GL_UNIFORM_BUFFER, textbp, to->buf[1] );
  glDrawArrays ( GL_TRIANGLE_FAN, 0, 4 );
  ExitIfGLError ( "DisplayTextObject" );
} /*DisplayTextObject*/

void DeleteTextObject ( myTextObject *to )
{
  glDeleteVertexArrays ( 1, &to->vao );
  glDeleteBuffers ( 2, to->buf );
  free ( to );
  ExitIfGLError ( "DeleteTextObject" );
} /*DeleteTextObject*/

void TextCleanup ( void )
{
  int i;

  for ( i = 0; i < 2; i++ ) {
    glDetachShader ( text_program_id, text_shader_id[i] );
    glDeleteShader ( text_shader_id[i] );
  }
  glDeleteProgram ( text_program_id );
  glDeleteBuffers ( 1, &gcbuf );
  ExitIfGLError ( "TextCleanup" );
} /*TextCleanup*/

