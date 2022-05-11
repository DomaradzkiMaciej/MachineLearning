
GLfloat UnitCircle[7][3];

BezierPatchObjf* EnterRSphericalProduct ( int eqdeg, int eqarcs, int eqstride,
                                          GLfloat eqcp[][3],
                                          int mdeg, int marcs, int mstride,
                                          GLfloat mcp[][3],
                                          GLfloat *colour );

BezierPatchObjf* EnterTorus ( float R, float r, GLfloat *colour );

