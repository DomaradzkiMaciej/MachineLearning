
char M3diagLUDecompf ( int n, float *a, float *b, float *c, float *d, char *p );
void M3diagLUSolvef ( int n, float *a, float *b, float *c, float *d, char *p,
                      int m, float *e );

void EvaluateBSplinesf ( float *bfv, int n, int k, const float *knots, float t );
char ConstructCubicInterpBSplinef ( int *N, float *knots, float *cp,
                                    int M, float *ikn, int dim, float *p );
char BSCdeBoorf ( int n, int lkn, float *knots, int dim, float *cp,
                  float t, float *p );

void QuatSlerpdeBoorf ( int n, int lkn, float *knots, float *cp, float t,
                        float *p );
char ConstructQuaternionInterpSplinef ( int *N, float *knots, float *qcp,
                                        int M, float *ikn, float *qt );
