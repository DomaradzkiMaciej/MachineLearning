
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "openglheader.h"
#include "utilities.h"
#include "quaternions.h"
#include "ispline.h"

/* ////////////////////////////////////////////////////////////////////////// */
char M3diagLUDecompf ( int n, float *a, float *b, float *c, float *d, char *p )
{
  int   i;
  float l;
#define SWAP(x,y) { l = x;  x = y;  y = l; }

  memset ( d, 0, (n-2)*sizeof(float) );
  for ( i = 0; i < n-2; i++ ) {
    if ( (p[i] = fabs(a[i+1]) > fabs(b[i])) ) {
      SWAP ( a[i+1], b[i] )  SWAP ( b[i+1], c[i] )
      d[i] = c[i+1];  c[i+1] = 0.0;
    }
    if ( b[i] == 0.0 ) return false;
    a[i+1] = l = a[i+1]/b[i];
    b[i+1] -= l*c[i];  c[i+1] -= l*d[i];
  }
  if ( (p[n-2] = fabs(a[n-1]) > fabs(b[n-2])) )
    { SWAP ( a[n-1], b[n-2] )  SWAP ( b[n-1], c[n-2] ) }
  if ( b[n-2] == 0.0 ) return false;
  a[n-1] = l = a[n-1]/b[n-2];
  b[n-1] -= l*c[n-2];
  return b[n-1] != 0.0;
} /*M3diagLUDecompf*/

void M3diagLUSolvef ( int n, float *a, float *b, float *c, float *d, char *p,
                      int m, float *e )
{
  int   i, j, k;
  float l, s, t;

  for ( i = k = 0;  i < n-1;  i++, k += m ) {
    if ( p[i] )
      for ( j = 0; j < m; j++ ) SWAP ( e[k+j], e[k+m+j] )
    for ( l = a[i+1], j = 0;  j < m;  j++ ) e[k+m+j] -= l*e[k+j];
  }
  for ( l = b[n-1], j = 0, k = (n-1)*m;  j < m;  j++ )
    e[k+j] /= l;
  for ( l = b[n-2], s = c[n-2], j = 0, k = (n-2)*m;  j < m;  j++ )
    e[k+j] = (e[k+j] - s*e[k+m+j])/l;
  for ( i = n-3, k = (n-3)*m;  i >= 0;  i--, k -= m )
    for ( l = b[i], s = c[i], t = d[i], j = 0;  j < m;  j++ )
      e[k+j] = (e[k+j] - s*e[k+m+j] - t*e[k+m+m+j])/l;
#undef SWAP
} /*M3diagLUSolvef*/

void EvaluateBSplinesf ( float *bfv, int n, int k, const float *knots, float t )
{
  int i, j, l;
  float alpha, beta;

  l = k-n;
  bfv[n] = 1.0;
  for ( j = 1; j <= n; j++ ) {
    beta = (knots[k+1]-t)/(knots[k+1]-knots[k-j+1]);
    bfv[n-j] = beta*bfv[n-j+1];
    for ( i = k-j+1; i < k; i++ ) {
      alpha = 1.0-beta;
      beta = (knots[i+j+1]-t)/(knots[i+j+1]-knots[i+1]);
      bfv[i-l] = alpha*bfv[i-l] + beta*bfv[i+1-l];
    }
    bfv[n] *= 1.0-beta;
  }
} /*EvaluateBSplinesf*/

char ConstructCubicInterpBSplinef ( int *N, float *knots, float *cp,
                                    int M, float *ikn, int dim, float *p )
{
  int   i, lkn;
  float *a, *b, *c, *d, bfv[4], t0, t1;
  char  *permut;

  *N = lkn = M+6;
  if ( !(a = malloc ( (4*(lkn-3)-3)*sizeof(float) + (lkn-4)*sizeof(char) )) )
    return false;
  b = &a[lkn-3];  c = &b[lkn-3];  d = &c[lkn-4];  permut = (char*)&d[lkn-5];
        /* utworz ciag wezlow funkcji sklejanej */
  knots[0] = knots[1] = knots[2] = ikn[0];
  memcpy ( &knots[3], ikn, (M+1)*sizeof(float) );
  knots[lkn-2] = knots[lkn-1] = knots[lkn] = ikn[M];
        /* utworz macierz ukladu */
  b[0] = 1.0;  c[0] = 0.0;
  t0 = knots[4]-knots[1];  t1 = knots[5]-knots[2];
  a[1] = t1;  b[1] = -(t0+t1);  c[1] = t0;
  for ( i = 2; i <= M; i++ ) {
    EvaluateBSplinesf ( bfv, 3, i+2, knots, knots[i+2] );
    a[i] = bfv[0];  b[i] = bfv[1];  c[i] = bfv[2];
  }
  t0 = knots[lkn-2]-knots[lkn-5];  t1 = knots[lkn-1]-knots[lkn-4];
  a[lkn-5] = t1;  b[lkn-5] = -(t0+t1);  c[lkn-5] = t0;
  a[lkn-4] = 0.0;  b[lkn-4] = 1.0;
        /* utworz prawa strone */
  memcpy ( cp, p, dim*sizeof(float) );
  memset ( &cp[dim], 0, dim*sizeof(float) );
  memcpy ( &cp[dim+dim], &p[dim], (M-1)*dim*sizeof(float) );
  memset ( &cp[(M+1)*dim], 0, dim*sizeof(float) );
  memcpy ( &cp[(M+2)*dim], &p[dim*M], dim*sizeof(float) );
        /* rozwiaz uklad rownan*/
  if ( !M3diagLUDecompf ( M+3, a, b, c, d, permut ) ) {
    free ( a );
    return false;
  }
  M3diagLUSolvef ( M+3, a, b, c, d, permut, dim, cp );
  free ( a );
  return true;
} /*ConstructCubicInterpBSplinef*/

char BSCdeBoorf ( int n, int lkn, float *knots, int dim, float *cp,
                  float t, float *p )
{
  int   i, j, k, l;
  float *d, alpha;

  if ( !(d = malloc ( (n+1)*dim*sizeof(float) )) )
    return false;
  for ( k = n, j = lkn-n;  j-k > 1; ) {
    i = k + (j-k)/2;
    if ( t >= knots[i] ) k = i; else j = i;
  }
  memcpy ( d, &cp[(k-n)*dim], (n+1)*dim*sizeof(float) );
  for ( j = 1; j <= n; j++ )
    for ( i = k-n+j; i <= k; i++ ) {
      alpha = (t-knots[i])/(knots[i+n+1-j]-knots[i]);
      for ( l = 0; l < dim; l++ )
        d[(i-k+n-j)*dim+l] = (1.0-alpha)*d[(i-k+n-j)*dim+l] +
                             alpha*d[(i-k+n-j+1)*dim+l];
    }
  memcpy ( p, d, dim*sizeof(float) );
  free ( d );
  return true;
} /*BSCdeBoorf*/

/* ////////////////////////////////////////////////////////////////////////// */
void QuatSlerpdeBoorf ( int n, int lkn, float *knots, float *cp, float t,
                        float *p )
{
  int   i, j, k;
  float d[16], alpha;

  for ( k = n, j = lkn-n;  j-k > 1; ) {
    i = k + (j-k)/2;
    if ( t >= knots[i] ) k = i; else j = i;
  }
  memcpy ( d, &cp[(k-n)*4], (n+1)*4*sizeof(float) );
  for ( j = 1; j <= n; j++ )
    for ( i = k-n+j; i <= k; i++ ) {
      alpha = (t-knots[i])/(knots[i-j+4]-knots[i]);
      QuatSlerpf ( &d[(i-k-j+n)*4], &d[(i-k-j+n)*4], &d[(i-k-j+n+1)*4], alpha );
    }
  memcpy ( p, d, 4*sizeof(float) );
  V4Normalisef ( p );
} /*QuatSlerpdeBoorf*/

static void ModifyQuatCPf ( int N, float *knots, int i, float *qcp, float *qt )
{
  float qf[4], qd[4];

  QuatSlerpdeBoorf ( 3, N, knots, qcp, knots[i+3], qf );
  QuatRDivf ( qd, &qt[4*i], qf );
  QuatMultf ( qf, qd, &qcp[4*(i+1)] );
  memcpy ( &qcp[4*(i+1)], qf, 4*sizeof(float) );
} /*ModifyQuatCPf*/

char ConstructQuaternionInterpSplinef ( int *N, float *knots, float *qcp,
                                        int M, float *ikn, float *qt )
{
#define TOL 1.0e-5
  int   i, lkn, itn;
  float qf[4], qd[4], dist, ldist, d;

  if ( !ConstructCubicInterpBSplinef ( N, knots, qcp, M, ikn, 4, qt ) )
    return false;
  for ( i = 0, lkn = *N;  i < lkn-3;  i++ )
    V4Normalisef ( &qcp[4*i] );
  for ( ldist = 4.0, itn = 0;  ;  ldist = dist, itn++ ) {
    for ( dist = 0.0, i = 1;  i < M; i++ ) {
      QuatSlerpdeBoorf ( 3, lkn, knots, qcp, ikn[i], qf );
      V4Subtractf ( qd, &qt[4*i], qf );
      if ( (d = V4DotProductf ( qd, qd )) > dist ) dist = d;
    }
    if ( dist >= ldist )
      return false;
    if ( dist <= TOL*TOL )
      break;
    ModifyQuatCPf ( lkn, knots, 1, qcp, qt );
    d = knots[4]-knots[1];  d /= (d+knots[5]-knots[2] );
    QuatSlerpf ( &qcp[4], &qcp[0], &qcp[8], d );
    for ( i = 2; i < M; i++ )
      ModifyQuatCPf ( lkn, knots, i, qcp, qt );
    d = knots[lkn-2]-knots[lkn-5];  d /= (d+knots[lkn-1]-knots[lkn-4]);
    QuatSlerpf ( &qcp[4*(M+1)], &qcp[4*M], &qcp[4*(M+2)], d );
  }
/*printf ( "itn = %d\n", itn );*/
  return true;
#undef TOL
} /*ConstructQuaternionInterpSplinef*/

