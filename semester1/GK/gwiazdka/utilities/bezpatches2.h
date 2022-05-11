
#define MAX_PATCH_DEGREE 10

typedef struct BezierPatchObjf {
    GLint  udeg, vdeg, dim, stride_u, stride_v, stride_p, stride_q, npatches;
    GLuint buf[4];
  } BezierPatchObjf;

extern GLuint bezpbi, bezpbbp,
              cpbi, cpbbp, cpibi, cpibbp,  /* dostep do opisu plata Beziera */
              txcbi, txcbp;
extern GLint  bezpbsize, bezpbofs[10], cpbofs[1], cpibofs[1], txcofs[1];
extern GLuint ubeznloc, ubeztloc;

extern GLuint pd_vao;              /* dziedzina plata */

void ConstructEmptyVAO ( void );
void DeleteEmptyVAO ( void );

BezierPatchObjf* EnterBezierPatches ( GLint udeg, GLint vdeg, GLint dim,
                      GLint np, GLint nq, GLint ncp, const GLfloat *cp,
                      GLint stride_p, GLint stride_q, GLint stride_u, GLint stride_v,
                      const GLfloat *colour );
BezierPatchObjf* EnterBezierPatchesElem ( GLint udeg, GLint vdeg, GLint dim,
                      int npatches, int ncp,
                      const GLfloat *cp, const GLint *ind,
                      const GLfloat *colour );

void SetBezierPatchOptions ( GLuint program_id, GLint normals, GLint tesslevel );

void DrawBezierPatches ( BezierPatchObjf *bp );
void DrawBezierNets ( BezierPatchObjf *bp );

void DeleteBezierPatches ( BezierPatchObjf *bp );

