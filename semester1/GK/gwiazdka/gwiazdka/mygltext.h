
typedef struct {
    GLint  chw, chh;  /* szerokosc i wysokosc znaku */
    GLuint ubo;
  } myFont;

typedef struct {
    GLuint vao, buf[2];
    int    maxlength;
    myFont *font;
  } myTextObject;


char LoadTextShaders ( void );
void SetupTextFrame ( GLint w, GLint h );
void SetTextForeground ( GLfloat fg[4] );
void SetTextBackground ( GLfloat bk[4] );

myFont *NewFontObject ( GLint chw, GLint chh, GLint chf, GLint chl,
                        int size, GLvoid *glyphs );
void DeleteFontObject ( myFont *font );

myTextObject *NewTextObject ( int maxlength );
void SetTextObjectContents ( myTextObject *to,
                             GLchar *text, GLint x, GLint y, myFont *font );
void DisplayTextObject ( myTextObject *to );
void DeleteTextObject ( myTextObject *to );

void TextCleanup ( void );


myFont *NewFont12x6 ( void );
myFont *NewFont18x10 ( void );

