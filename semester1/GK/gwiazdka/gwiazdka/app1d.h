
#define STATE_NOTHING 0
#define STATE_TURNING 1

typedef struct TransBl {
          GLfloat mm[16], vm[16], pm[16];
          GLfloat eyepos[4];
        } TransBl;

extern GLuint  shader_id[17];
extern GLuint  program_id[5];
extern GLuint  trbi, trbuf, trbbp;
extern GLint   trbsize, trbofs[6];
extern GLuint  icos_vao, icos_vbo[4];
extern float   model_rot_axis[3], model_rot_angle0, model_rot_angle;
extern const float viewer_pos0[4];
extern TransBl trans;
extern myTextObject *vptext;
extern myFont *font;

extern int   WindowHandle;
extern int   win_width, win_height;
extern int   last_xi, last_eta;
extern float left, right, bottom, top, near, far;
extern int   app_state;
extern float viewer_rvec[3];
extern float viewer_rangle;
extern int   option;
extern char  animate, enlight, tesselate;

void SetupMVPMatrix ( void );
void SetupModelMatrix ( float axis[3], float angle );
void InitViewMatrix ( void );
void RotateViewer ( int deltaxi, int deltaeta );
void ConstructIcosahedronVAO ( void );
void DrawIcosahedron ( int opt, char enlight );
void DrawTessIcos ( int opt, char enlight );
void InitLights ( void );
void NotifyViewerPos ( void );

void LoadMyShaders ( void );
void InitMyObject ( void );
void Cleanup ( void );

void ToggleAnimation ( void );
void ToggleLight ( void );
void ToggleTesselation ( void );

void ReshapeFunc ( int width, int height );
void DisplayFunc ( void );
void KeyboardFunc ( unsigned char key, int x, int y );
void MouseFunc ( int button, int state, int x, int y );
void MotionFunc ( int x, int y );
void TimerFunc ( int value );
void IdleFunc ( void );

void Initialise ( int argc, char *argv[] );

