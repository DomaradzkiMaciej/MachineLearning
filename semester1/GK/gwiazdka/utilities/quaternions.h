
void QuatMultf ( float q[4], const float q1[4], const float q2[4] );
float QuatAbsf ( float q[4] );
float QuatArgf ( float q[4] );
void QuatLDivf ( float q[4], const float q2[4], const float q1[4] );
void QuatRDivf ( float q[4], const float q1[4], const float q2[4] );
void Mat4x4QuatRotationf ( GLfloat a[16], float q[4] );
void QuatRotVf ( float q[4], const float v[3], float phi );
void RotVQuatf ( float v[3], float *phi, const float q[4] );
void QuatAnglef ( float *psi, float *spsi, float *cpsi, float q0[4], float q1[4] );
void QuatArcInterpf ( float qt[4], const float q0[4], const float q1[4],
                      float psi, float spsi, float t );
void QuatSlerpf ( float qt[4], float q0[4], float q1[4], float t );
