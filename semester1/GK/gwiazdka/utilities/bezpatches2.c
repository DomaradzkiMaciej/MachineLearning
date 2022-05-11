#include <stdlib.h>
#include <string.h>
#include <stdio.h>    
#include <math.h>   
#include "openglheader.h"

#include "utilities.h"
#include "bezpatches.h"

GLuint bezpbi, bezpbbp,
       cpbi, cpbbp, cpibi, cpibbp,  /* dostep do opisu plata Beziera */
       txcbi, txcbp;
GLint  bezpbsize, bezpbofs[10], cpbofs[1], cpibofs[1], txcofs[1];
GLuint ubeznloc, ubeztloc;

GLuint empty_vao;              /* dziedzina plata */

void ConstructEmptyVAO ( void )
{
  glGenVertexArrays ( 1, &empty_vao );
  glBindVertexArray ( empty_vao );
  glVertexAttrib1f ( 0, 0.0 );
  ExitIfGLError ( "ConstructEmptyVAO" );
} /*ConstructEmptyVAO*/

void DeleteEmptyVAO ( void )
{
  glBindVertexArray ( 0 );
  glDeleteVertexArrays ( 1, &empty_vao );
  ExitIfGLError ( "DeleteEmptyVAO" );
} /*DeleteEmptyVAO*/

BezierPatchObjf* EnterBezierPatches ( GLint udeg, GLint vdeg, GLint dim,
                      GLint np, GLint nq, GLint ncp, const GLfloat *cp,
                      GLint stride_p, GLint stride_q, GLint stride_u, GLint stride_v,
                      const GLfloat *colour )
{
  BezierPatchObjf *bp;
  GLint           size;
  int             zero;

  if ( dim < 2 || dim > 4 || np*nq < 1 ||
       udeg < 1 || udeg > MAX_PATCH_DEGREE ||
       vdeg < 1 || vdeg > MAX_PATCH_DEGREE )
    return NULL;
  bp = malloc ( sizeof(BezierPatchObjf) );
  if ( bp ) {
    memset ( bp, 0, sizeof(BezierPatchObjf) );
    bp->udeg = udeg;  bp->vdeg = vdeg;  bp->dim = dim;
    bp->npatches = np*nq;
    bp->stride_p = stride_p;  bp->stride_q = stride_q;
    bp->stride_u = stride_u;  bp->stride_v = stride_v;

    bp->buf[0] = NewUniformBlockObject ( bezpbsize, bezpbbp );
    glBufferSubData ( GL_UNIFORM_BUFFER, bezpbofs[0], sizeof(GLint), &bp->dim );
    glBufferSubData ( GL_UNIFORM_BUFFER, bezpbofs[1], sizeof(GLint), &bp->udeg );
    glBufferSubData ( GL_UNIFORM_BUFFER, bezpbofs[2], sizeof(GLint), &bp->vdeg );
    glBufferSubData ( GL_UNIFORM_BUFFER, bezpbofs[3], sizeof(GLint), &bp->stride_u );
    glBufferSubData ( GL_UNIFORM_BUFFER, bezpbofs[4], sizeof(GLint), &bp->stride_v );
    glBufferSubData ( GL_UNIFORM_BUFFER, bezpbofs[5], sizeof(GLint), &bp->stride_p );
    glBufferSubData ( GL_UNIFORM_BUFFER, bezpbofs[6], sizeof(GLint), &bp->stride_q );
    glBufferSubData ( GL_UNIFORM_BUFFER, bezpbofs[7], sizeof(GLint), &nq );
    zero = 0;
    glBufferSubData ( GL_UNIFORM_BUFFER, bezpbofs[8], sizeof(GLint), &zero );
    glBufferSubData ( GL_UNIFORM_BUFFER, bezpbofs[9], 4*sizeof(GLfloat), colour );

    size = ncp*dim*sizeof(GLfloat);
    bp->buf[1] = NewUniformBlockObject ( cpbofs[0]+size, cpbbp );
    glBufferSubData ( GL_UNIFORM_BUFFER, cpbofs[0], size, cp );
    ExitIfGLError ( "EnterBezierPatch" );
  }
  return bp;
} /*EnterBezierPatches*/

BezierPatchObjf* EnterBezierPatchesElem ( GLint udeg, GLint vdeg, GLint dim,
                      int npatches, int ncp,
                      const GLfloat *cp, const GLint *ind,
                      const GLfloat *colour )
{
  BezierPatchObjf *bp;
  GLint           size, one;

  if ( dim < 2 || dim > 4 || npatches < 1 ||
       udeg < 1 || udeg > MAX_PATCH_DEGREE ||
       vdeg < 1 || vdeg > MAX_PATCH_DEGREE )
    return NULL;
  bp = malloc ( sizeof(BezierPatchObjf) );
  if ( bp ) {
    memset ( bp, 0, sizeof(BezierPatchObjf) );
    bp->udeg = udeg;  bp->vdeg = vdeg;  bp->dim = dim;
    bp->npatches = npatches;
    bp->buf[0] = NewUniformBlockObject ( bezpbsize, bezpbbp );
    glBufferSubData ( GL_UNIFORM_BUFFER, bezpbofs[0], sizeof(GLint), &bp->dim );
    glBufferSubData ( GL_UNIFORM_BUFFER, bezpbofs[1], sizeof(GLint), &bp->udeg );
    glBufferSubData ( GL_UNIFORM_BUFFER, bezpbofs[2], sizeof(GLint), &bp->vdeg );
    bp->stride_p = (udeg+1)*(vdeg+1);
    glBufferSubData ( GL_UNIFORM_BUFFER, bezpbofs[5], sizeof(GLint), &bp->stride_p );
    one = 1;
    glBufferSubData ( GL_UNIFORM_BUFFER, bezpbofs[6], sizeof(GLint), &one );
    glBufferSubData ( GL_UNIFORM_BUFFER, bezpbofs[8], sizeof(GLint), &one );
    glBufferSubData ( GL_UNIFORM_BUFFER, bezpbofs[9], 4*sizeof(GLfloat), colour );

    size = ncp*dim*sizeof(GLfloat);
    bp->buf[1] = NewUniformBlockObject ( cpbofs[0]+size, cpbbp );
    glBufferSubData ( GL_UNIFORM_BUFFER, cpbofs[0], size, cp );

    size = (udeg+1)*(vdeg+1)*npatches*sizeof(GLint);
    bp->buf[2] = NewUniformBlockObject ( size, cpibbp );
    glBufferSubData ( GL_UNIFORM_BUFFER, cpibofs[0], size, ind );
    ExitIfGLError ( "EnterBezierPatchesInd" );
  }
  return bp;
} /*EnterBezierPatchesElem*/

void SetBezierPatchOptions ( GLuint program_id, GLint normals, GLint tesslevel )
{
  glUseProgram ( program_id );
  glUniform1i ( ubeznloc, normals );
  glUniform1i ( ubeztloc, tesslevel );
  ExitIfGLError ( "SetBezierPatchOptions" );
} /*SetBezierPatchOptions*/

void DrawBezierPatches ( BezierPatchObjf *bp )
{
  if ( bp ) {
    glBindBufferBase ( GL_UNIFORM_BUFFER, bezpbbp, bp->buf[0] );
    glBindBufferBase ( GL_UNIFORM_BUFFER, cpbbp, bp->buf[1] );
    if ( bp->buf[2] )
      glBindBufferBase ( GL_UNIFORM_BUFFER, cpibbp, bp->buf[2] );
    glBindVertexArray ( empty_vao );
    glPatchParameteri ( GL_PATCH_VERTICES, 4 );
    glDrawArraysInstanced ( GL_PATCHES, 0, 4, bp->npatches );
    ExitIfGLError ( "DrawBezierPatch" );
  }
} /*DrawBezierPatch*/

void DrawBezierNets ( BezierPatchObjf *bp )
{
  int nlines;

  if ( bp ) {
    glBindBufferBase ( GL_UNIFORM_BUFFER, bezpbbp, bp->buf[0] );
    glBindBufferBase ( GL_UNIFORM_BUFFER, cpbbp, bp->buf[1] );
    if ( bp->buf[2] )
      glBindBufferBase ( GL_UNIFORM_BUFFER, cpibbp, bp->buf[2] );
    glBindVertexArray ( empty_vao );
    nlines = 2*bp->udeg*bp->vdeg + bp->udeg + bp->vdeg;
    glDrawArraysInstanced ( GL_LINES, 0, 2, bp->npatches*nlines );
    ExitIfGLError ( "DrawBezierNets" );
  }
} /*DrawBezierNets*/

void DeleteBezierPatches ( BezierPatchObjf *bp )
{
  if ( bp ) {
    glDeleteBuffers ( 4, bp->buf );
    free ( bp );
  }
} /*DeleteBezierPatches*/
