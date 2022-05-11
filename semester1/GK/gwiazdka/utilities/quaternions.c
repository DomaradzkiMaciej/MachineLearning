
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "openglheader.h"

#include "utilities.h"
#include "quaternions.h"

void QuatMultf ( float q[4], const float q1[4], const float q2[4] )
{
  q[0] = q1[0]*q2[0] - q1[1]*q2[1] - q1[2]*q2[2] - q1[3]*q2[3];
  q[1] = q1[1]*q2[0] + q1[0]*q2[1] - q1[3]*q2[2] + q1[2]*q2[3];
  q[2] = q1[2]*q2[0] + q1[3]*q2[1] + q1[0]*q2[2] - q1[1]*q2[3];
  q[3] = q1[3]*q2[0] - q1[2]*q2[1] + q1[1]*q2[2] + q1[0]*q2[3];
} /*QuatMultf*/

float QuatAbsf ( float q[4] )
{
  return sqrt ( V4DotProductf ( q, q ) );
} /*QuatAbsf*/

float QuatArgf ( float q[4] )
{
  return atan2 ( sqrt ( V3DotProductf ( &q[1], &q[1]) ), q[0] );
} /*QuatArgf*/

void QuatLDivf ( float q[4], const float q2[4], const float q1[4] )
{
  float q2i[4], s;

  s = V4DotProductf ( q2, q2 );
  if ( s > 0.0 ) {
    q2i[0] = q2[0]/s;  q2i[1] = -q2[1]/s;  q2i[2] = -q2[2]/s;  q2i[3] = -q2[3]/s;
    QuatMultf ( q, q2i, q1 );
  }
} /*QuatLDivf*/

void QuatRDivf ( float q[4], const float q1[4], const float q2[4] )
{
  float q2i[4], s;

  s = V4DotProductf ( q2, q2 );
  if ( s > 0.0 ) {
    q2i[0] = q2[0]/s;  q2i[1] = -q2[1]/s;  q2i[2] = -q2[2]/s;  q2i[3] = -q2[3]/s;
    QuatMultf ( q, q1, q2i );
  }
} /*QuatRDivf*/

void Mat4x4QuatRotationf ( GLfloat a[16], float q[4] )
{
  float s;

  s = sqrt ( V3DotProductf ( &q[1], &q[1] ) );
  if ( s > 0.0 )
    M4x4RotateVf ( a, q[1]/s, q[2]/s, q[3]/s, 2.0*atan2 ( s, q[0] ) );
  else
    M4x4Identf ( a );
} /*Mat3x3QuatRotationf*/

void QuatRotVf ( float q[4], const float v[3], float phi )
{
#define TOL 1.0e-6
  float d;

  d = V3DotProductf ( v, v );
  if ( d > TOL*TOL ) {
    d = sin ( 0.5*phi )/sqrt ( d );
    q[0] = cos ( 0.5*phi );
    q[1] = d*v[0];  q[2] = d*v[1];  q[3] = d*v[2];
  }
  else
    { q[0] = 1.0, q[1] = q[2] = q[3] = 0.0; }
} /*QuatRotVf*/

void RotVQuatf ( float v[3], float *phi, const float q[4] )
{
  float s, d;

  if ( (s = V3DotProductf ( &q[1], &q[1] )) < TOL*TOL ||
       (d = q[0]*q[0] + s) < TOL*TOL )
  { v[0] = 1.0, v[1] = v[2] = 0.0; *phi = 0.0; }
  else {
    s = sqrt ( s );
    *phi = 2.0*atan2 ( s, q[0] );
    d = 1.0/(sqrt ( d )*s);
    v[0] = d*q[1];  v[1] = d*q[2];  v[2] = d*q[3];
  }
#undef TOL
} /*RotVQuatf*/

void QuatAnglef ( float *psi, float *spsi, float *cpsi, float q0[4], float q1[4] )
{
  float a[4], p, r;

  V4Addf ( a, q0, q1 );
  p = sqrt ( V4DotProductf ( a, a ) );
  V4Subtractf ( a, q0, q1 );
  r = sqrt ( V4DotProductf ( a, a ) );
  if ( psi )  *psi = 2.0*atan2 ( r, p );
  if ( spsi ) *spsi = 0.5*p*r;
  if ( cpsi ) *cpsi = 0.25*( p*p - r*r );
} /*QuatAnglef*/

void QuatArcInterpf ( float qt[4], const float q0[4], const float q1[4],
                      float psi, float spsi, float t )
{
  float stp, s1tp;

  stp = sin ( t*psi );  s1tp = sin ( (1.0-t)*psi );
  qt[0] = (s1tp*q0[0] + stp*q1[0]) / spsi;
  qt[1] = (s1tp*q0[1] + stp*q1[1]) / spsi;
  qt[2] = (s1tp*q0[2] + stp*q1[2]) / spsi;
  qt[3] = (s1tp*q0[3] + stp*q1[3]) / spsi;
} /*QuatArcInterp*/

void QuatSlerpf ( float qt[4], float q0[4], float q1[4], float t )
{
  float psi, spsi;

  QuatAnglef ( &psi, &spsi, NULL, q0, q1 );
  if ( spsi > 0.0 )
    QuatArcInterpf ( qt, q0, q1, psi, spsi, t );
  else
    memcpy ( qt, q0, 4*sizeof(float) );
} /*QuatSlerpf*/

