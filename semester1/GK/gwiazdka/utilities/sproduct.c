#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "openglheader.h"

#include "utilities.h"
#include "bezpatches.h"
#include "sproduct.h"

#define SQRT3 1.7320508

GLfloat UnitCircle[7][3] =
  {{0.5,0.5*SQRT3,1.0},{-0.5,0.5*SQRT3,0.5},{-1.0,0.0,1.0},{-0.5,-0.5*SQRT3,0.5},
   {0.5,-0.5*SQRT3,1.0},{1.0,0.0,0.5},{0.5,0.5*SQRT3,1.0}};

BezierPatchObjf* EnterRSphericalProduct ( int eqdeg, int eqarcs, int eqstride,
                                          GLfloat eqcp[][3],
                                          int mdeg, int marcs, int mstride,
                                          GLfloat mcp[][3], 
                                          GLfloat *colour )
{
  GLfloat         *cp;
  int             i, j, k, nw, nk;
  BezierPatchObjf *spr;

  nw = (eqarcs-1)*eqstride + eqdeg + 1;
  nk = (marcs-1)*mstride + mdeg + 1;
  if ( !(cp = malloc ( nw*nk*4*sizeof(GLfloat)) ) )
    return NULL;
  for ( i = k = 0;  i < nw;  i++ )
    for ( j = 0;  j < nk;  j++, k += 4 ) {
      cp[k+0] = eqcp[i][0]*mcp[j][0];
      cp[k+1] = eqcp[i][1]*mcp[j][0];
      cp[k+2] = eqcp[i][2]*mcp[j][1];
      cp[k+3] = eqcp[i][2]*mcp[j][2];
    }
  spr = EnterBezierPatches ( eqdeg, mdeg, 4, eqarcs, marcs, nw*nk, cp,
                             nk*eqstride*4, mstride*4, 4*nk, 4, colour );
  free ( cp );
  return spr;
} /*EnterRSphericalProduct*/

BezierPatchObjf* EnterTorus ( float R, float r, GLfloat *colour )
{
  GLfloat circ[7][3];
  int     i;

  for ( i = 0; i < 7; i++ ) {
    circ[i][0] = r*UnitCircle[i][0] + R*UnitCircle[i][2];
    circ[i][1] = r*UnitCircle[i][1];
    circ[i][2] = UnitCircle[i][2];
  }
  return EnterRSphericalProduct ( 2, 3, 2, UnitCircle, 2, 3, 2, circ, colour );
} /*EnterTorus*/

