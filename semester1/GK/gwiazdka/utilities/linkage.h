
/* possible articulation methods */
#define KL_ART_OTHERHALF -1
#define KL_ART_NONE       0
#define KL_ART_TRANS_X    1
#define KL_ART_TRANS_Y    2
#define KL_ART_TRANS_Z    3
#define KL_ART_TRANS_XYZ  4
#define KL_ART_SCALE_X    5
#define KL_ART_SCALE_Y    6
#define KL_ART_SCALE_Z    7
#define KL_ART_SCALE_XYZ  8
#define KL_ART_ROT_X      9
#define KL_ART_ROT_Y     10
#define KL_ART_ROT_Z     11
#define KL_ART_ROT_V     12

#define KL_MAX_NAME_LENGTH 64

typedef struct kl_object {
    int     nvc;                   /* number of vertex coordinates, 3 or 4 */
    int     nvert;                 /* number of vertices */
    GLfloat *vert, *tvert;         /* pointers to vertices */
    GLfloat Etr[16];               /* extra object transformation */
    void    *usrdata;              /* pointer to object specific data */
                                   /* object specific procedures */
    void    (*transform)(struct kl_object*,int,GLfloat*,int,int*,void*);
    void    (*postprocess)(struct kl_object*,void*);
    void    (*redraw)(struct kl_object*,void*);        /* drawing */
    void    (*destroy)(struct kl_object*,void*);       /* deallocation */
  } kl_object;

typedef char (*kl_obj_init)(kl_object*,void*,void*);
typedef void (*kl_obj_transform)(kl_object*,int,GLfloat*,int,int*,void*);
typedef void (*kl_obj_postprocess)(kl_object*,void*);
typedef void (*kl_obj_redraw)(kl_object*,void*);
typedef void (*kl_obj_destroy)(kl_object*,void*);

typedef struct kl_obj_ref {
    int     on;     /* object number */
    int     nextr;  /* next reference */
    int     nv;     /* number of associated vertices */
    int     *vn;    /* vertex numbers */
  } kl_obj_ref;

typedef struct kl_link {
    int     fref;      /* first object reference */
    int     fhj;       /* first half-joint */
    char    tag;
  } kl_link;

typedef struct kl_halfjoint {
    int     l0, l1;     /* numbers of links - beginning and end */
    int     otherhalf;  /* number of the other half */
    int     nexthj;     /* next halfjoint */
    int     pnum;       /* number of articulation parameter */
    int     art;        /* type of articulation */
    GLfloat Ftr[16];    /* front location transformation */
    GLfloat Rtr[16];    /* articulation transformation */
    GLfloat Btr[16];    /* back location transformation */
  } kl_halfjoint;

typedef struct kl_linkage {
    int          maxobj,    nobj;     /* number of geometric objects */
    int          maxorefs,  norefs;   /* number of object references */
    int          maxlinks,  nlinks;   /* number of links */
    int          maxhj,     nhj;      /* number of halfjoints */
    int          maxartpar, nartpar;  /* number of articulation parameters */
    kl_object  *obj;
    kl_obj_ref   *oref;
    kl_link      *link;
    kl_halfjoint *hj;
    GLfloat      *artp, *prevartp;
    int          current_root;
    GLfloat      current_root_tr[16];
    void         *usrdata;
  } kl_linkage;

kl_linkage *kl_NewLinkage ( int maxo, int maxl, int maxr,
                            int maxj, int maxp, void *usrdata );
void kl_DestroyLinkage ( kl_linkage *linkage );

int kl_NewObject ( kl_linkage *linkage, int nvc, int nvert,
                   const GLfloat etrans[16], void *usrdata,
                   kl_obj_init init,
                   kl_obj_transform transform,
                   kl_obj_postprocess postprocess,
                   kl_obj_redraw redraw,
                   kl_obj_destroy destroy );
int kl_NewLink ( kl_linkage *linkage );
int kl_NewObjRef ( kl_linkage *linkage, int lkn, int on, int nv, int *vn );
int kl_NewJoint ( kl_linkage *linkage, int l0, int l1, int art, int pnum );
void kl_SetJointFtr ( kl_linkage *linkage, int jn, GLfloat tr[16], char back );
void kl_SetJointBtr ( kl_linkage *linkage, int jn, GLfloat tr[16], char front );

void kl_SetArtParam ( kl_linkage *linkage, int pno, int nump, GLfloat *par );
void kl_Articulate ( kl_linkage *linkage );
void kl_Redraw ( kl_linkage *linkage );

void kl_DefaultTransform ( kl_object *obj, int refn, GLfloat *tr,
                           int nv, int *vn, void *lkgusrdata );

/* stub procedures */
void kl_obj_stub ( kl_object *obj, void *lkgusrdata );

