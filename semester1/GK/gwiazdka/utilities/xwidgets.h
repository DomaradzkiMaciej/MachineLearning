
#define false 0
#define true  1

#define WDGSTATE_INACTIVE         -1
#define WDGSTATE_DEFAULT           0
#define WDGSTATE_MOVING_SLIDE      1
#define WDGSTATE_EDITING           2

#define XWMSG_NONE                 0
#define XWMSG_UNKNOWN              1
#define XWMSG_ENTERING             2
#define XWMSG_LEAVING              3
#define XWMSG_BUTTON_PRESS         4
#define XWMSG_BUTTON_RELEASE       5
#define XWMSG_MOUSE_MOTION         6
#define XWMSG_KEY_PRESS            7
#define XWMSG_KEY_RELEASE          8
#define XWMSG_SPECIAL_KEY_PRESS    9
#define XWMSG_SPECIAL_KEY_RELEASE 10
#define XWMSG_CLIENT_MESSAGE      11

#define WDGMSG_RECONFIGURE        12
#define WDGMSG_BUTTON_PRESS       13
#define WDGMSG_SWITCH_CHANGE      14
#define WDGMSG_SLIDEBAR_CHANGE    15
#define WDGMSG_EDITOR_ENTER       16
#define WDGMSG_EDITOR_ESCAPE      17

#define PALETTE_LENGTH 30
#define XWP_BLACK      wmcolour[0]
#define XWP_RED_25     wmcolour[1]
#define XWP_RED_50     wmcolour[2]
#define XWP_RED_75     wmcolour[3]
#define XWP_RED        wmcolour[4]
#define XWP_GREEN_25   wmcolour[5]
#define XWP_GREEN_50   wmcolour[6]
#define XWP_GREEN_75   wmcolour[7]
#define XWP_GREEN      wmcolour[8]
#define XWP_BLUE_25    wmcolour[9]
#define XWP_BLUE50     wmcolour[10]
#define XWP_BLUE75     wmcolour[11]
#define XWP_BLUE       wmcolour[12]
#define XWP_CYAN_25    wmcolour[13]
#define XWP_CYAN_50    wmcolour[14]
#define XWP_CYAN_75    wmcolour[15]
#define XWP_CYAN       wmcolour[16]
#define XWP_MAGENTA_25 wmcolour[17]
#define XWP_MAGENTA_50 wmcolour[18]
#define XWP_MAGENTA_75 wmcolour[19]
#define XWP_MAGENTA    wmcolour[20]
#define XWP_YELLOW_25  wmcolour[21]
#define XWP_YELLOW_50  wmcolour[22]
#define XWP_YELLOW_75  wmcolour[23]
#define XWP_YELLOW     wmcolour[24]
#define XWP_GREY_10    wmcolour[25]
#define XWP_GREY_25    wmcolour[26]
#define XWP_GREY_50    wmcolour[27]
#define XWP_GREY_75    wmcolour[28]
#define XWP_WHITE      wmcolour[29]

#define XWP_MENU_BACKGROUND_COLOUR XWP_GREY_50
#define XWP_TEXT_COLOUR            XWP_WHITE
#define XWP_BUTTON_COLOUR          XWP_GREY_25
#define XWP_SWITCH_COLOUR          XWP_GREY_25
#define XWP_SLIDEBAR_COLOUR        XWP_GREY_25
#define XWP_ACTIVE_SLIDEBAR_COLOUR XWP_GREY_10
#define XWP_EDITOR_COLOUR          XWP_GREY_25
#define XWP_ACTIVE_EDITOR_COLOUR   XWP_GREY_10
#define XWP_EDITOR_CURSOR_COLOUR   XWP_GREY_50

typedef struct {
    struct xwidget *prev, *next;
  } xwlink;

typedef struct xwidget {
    int        id;
    XRectangle r;
    int        state;
    char       (*input)(struct xwidget *wdg, int msg, int key, int x, int y);
    void       (*redraw)(struct xwidget *wdg);
    struct     xwinmenu *wm;
    xwlink     link;
    void       *data0, *data1;
  } xwidget;

typedef struct xwinmenu {
    Window       window;
    Pixmap       pixmap;
    XRectangle   r;
    int          prevx, prevy;
    unsigned int prevmask;
    char         changed, expose_sent;
    void         *data;
    xwidget      *empty, *focus;
    XEvent       *ev;
    void        (*redraw)(struct xwinmenu *wm);
    void        (*callback)(struct xwidget *wdg, int msg, int key, int x, int y);
    xwlink       wlist;
  } xwinmenu;

typedef void (*xmredraw)(struct xwinmenu *wm);
typedef void (*xcallback)(struct xwidget *wdg, int msg, int key, int x, int y);
typedef char (*xwinput)(struct xwidget *wdg, int msg, int key, int x, int y);
typedef void (*xwredraw)(struct xwidget *wdg);


extern unsigned int wmcolour[];


void InitRGBXColourmap ( void );
unsigned int RGBXColour ( float r, float g, float b );
void InitWinMenuPalette ( void );

void TranslateEventMsg ( XEvent *ev, int *msg, int *key, int *x, int *y );

xwinmenu *NewWinMenu ( Window window, int w, int h, int x, int y,
                       void *data, xmredraw redraw, xcallback callback );
void WinMenuInput ( xwinmenu *wm, XEvent *ev );
void WinMenuRedraw ( xwinmenu *wm );
void GrabInput ( xwidget *wdg );
void UngrabInput ( xwidget *wdg );
void DestroyWinMenu ( xwinmenu *wm );

xwidget *NewWidget ( struct xwinmenu *wm, int size, int id,
                     int w, int h, int x, int y,
                     xwinput input, xwredraw redraw, void *data0, void *data1 );

xwidget *NewEmptyWidget ( xwinmenu *wm, int id );

xwidget *NewButton ( xwinmenu *wm, int id,
                     int w, int h, int x, int y, char *title );

xwidget *NewSwitch ( xwinmenu *wm, int id,
                     int w, int h, int x, int y, char *title, char *sw );

xwidget *NewSlidebarf ( xwinmenu *wm, int id,
                        int w, int h, int x, int y, float *data );

void LeaveEditingState ( xwidget *wdg );
xwidget *NewLineEditor ( xwinmenu *wm, int id, int w, int h, int x, int y,
                         int maxlength, char *txtbuf );

void PostMenuExposeEvent ( xwinmenu *wm );
void PostExposeEvent ( Window win, int width, int height );
void PostClientMessageEvent ( Window win, Atom message_type,
                              int format, void *data );

