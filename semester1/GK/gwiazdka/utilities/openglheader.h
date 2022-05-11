
#define USE_GL3W

#ifdef USE_GL3W
#include <GL/gl3w.h>
#define ENABLE_SPIRV
#else
#include <GL/glew.h>
#define USE_GLEW
#endif

