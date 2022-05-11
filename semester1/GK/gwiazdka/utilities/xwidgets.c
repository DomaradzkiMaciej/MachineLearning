#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "xvariables.h"
#include "xwidgets.h"

/* ////////////////////////////////////////////////////////////////////////// */
static xwidget *lastinput = NULL;

static struct {
    float r_bits,  g_bits,  b_bits;
    char  r_shift, g_shift, b_shift;
  } cmap;

unsigned int wmcolour[PALETTE_LENGTH];

static void parse_colourmask ( int mask, float *bits, char *shift )
{
  char sh;

  for ( sh = 0;  !(mask & 0x01);  mask = mask >> 1, sh++ )
    ;
  *shift = sh;
  *bits = (float)mask;
} /*parse_colourmask*/

void InitRGBXColourmap ( void )
{
  parse_colourmask ( xvisual->red_mask, &cmap.r_bits, &cmap.r_shift );
  parse_colourmask ( xvisual->green_mask, &cmap.g_bits, &cmap.g_shift );
  parse_colourmask ( xvisual->blue_mask, &cmap.b_bits, &cmap.b_shift );
} /*InitRGBXColourmap*/

unsigned int RGBXColour ( float r, float g, float b )
{
  unsigned int ir, ig, ib;

  if ( r < 0.0 ) ir = 0;
  else if ( r > 1.0 ) ir = cmap.r_bits;
  else ir = (unsigned int)(r*cmap.r_bits);
  if ( g < 0.0 ) ig = 0;
  else if ( g > 1.0 ) ig = cmap.g_bits;
  else ig = (unsigned int)(g*cmap.g_bits);
  if ( b < 0.0 ) ib = 0;
  else if ( b > 1.0 ) ib = cmap.b_bits;
  else ib = (unsigned int)(b*cmap.b_bits);
  return (ir << cmap.r_shift) + (ig << cmap.g_shift) + (ib << cmap.b_shift);
} /*RGBXColour*/

static const float rgbpalette[PALETTE_LENGTH][3] =
  {{0.0,0.0,0.0},{0.25,0.0,0.0},{0.5,0.0,0.0},{0.75,0.0,0.0},{1.0,0.0,0.0},
   {0.0,0.25,0.0},{0.0,0.5,0.0},{0.0,0.75,0.0},{0.0,1.0,0.0},
   {0.0,0.0,0.25},{0.0,0.0,0.5},{0.0,0.0,0.75},{0.0,0.0,1.0},
   {0.0,0.25,0.25},{0.0,0.5,0.5},{0.0,0.75,0.75},{0.0,1.0,1.0},
   {0.25,0.0,0.25},{0.5,0.0,0.5},{0.75,0.0,0.75},{1.0,0.0,1.0},
   {0.25,0.25,0.0},{0.5,0.5,0.0},{0.75,0.75,0.0},{1.0,1.0,0.0},
   {0.1,0.1,0.1},
   {0.25,0.25,0.25},{0.5,0.5,0.5},{0.75,0.75,0.75},{1.0,1.0,1.0}};

void InitWinMenuPalette ( void )
{
  int i;

  for ( i = 0; i < PALETTE_LENGTH; i++ )
    wmcolour[i] = RGBXColour ( rgbpalette[i][0], rgbpalette[i][1],
                               rgbpalette[i][2] );
} /*InitWinMenuPalette*/

/* ////////////////////////////////////////////////////////////////////////// */
void WinMenuRedraw ( xwinmenu *wm )
{
  xwidget *wdg;

  XSetForeground ( xdisplay, xgc, XWP_MENU_BACKGROUND_COLOUR );
  XFillRectangle ( xdisplay, wm->pixmap, xgc,
                   /*wm->r.x, wm->r.y,*/ 0, 0, wm->r.width, wm->r.height );
  for ( wdg = wm->wlist.next;  wdg;  wdg = wdg->link.next )
    if ( wdg->state != WDGSTATE_INACTIVE )
      wdg->redraw ( wdg );
} /*WinMenuRedraw*/

xwinmenu *NewWinMenu ( Window window, int w, int h, int x, int y,
                       void *data, xmredraw redraw, xcallback callback )
{
  xwinmenu *wm;

  if ( (wm = malloc ( sizeof(xwinmenu) )) ) {
    memset ( wm, 0, sizeof(xwinmenu) );
    wm->window = window;
    wm->r.width = w;  wm->r.height = h;  wm->r.x = x;  wm->r.y = y;
    wm->data = data;
    wm->redraw = redraw ? redraw : WinMenuRedraw;
    if ( !redraw )
      wm->pixmap = XCreatePixmap ( xdisplay, window, w, h, 24 );
    wm->callback = callback;
    wm->empty = NewEmptyWidget ( wm, 0 );
    wm->changed = true;  wm->expose_sent = false;
  }
  return wm;
} /*NewWinMenu*/

void TranslateEventMsg ( XEvent *ev, int *msg, int *key, int *x, int *y )
{
  char   chr;
  KeySym ks;

  switch ( ev->xany.type ) {
case ButtonPress:
    *msg = XWMSG_BUTTON_PRESS;
    *key = ev->xbutton.button;
    *x = ev->xbutton.x;  *y = ev->xbutton.y;
    break;
case ButtonRelease:
    *msg = XWMSG_BUTTON_RELEASE;
    *key = ev->xbutton.button;
    *x = ev->xbutton.x;  *y = ev->xbutton.y;
    break;
case MotionNotify:
    *msg = XWMSG_MOUSE_MOTION;
    *key = ev->xmotion.state;
    *x = ev->xmotion.x;  *y = ev->xmotion.y;
    break;
case KeyPress:
    *msg = XWMSG_KEY_PRESS;
    goto decode_key;
case KeyRelease:
    *msg = XWMSG_KEY_RELEASE;
decode_key:
    XLookupString ( &ev->xkey, &chr, 1, &ks, NULL );
/*printf ( "chr = %d, keysym = %d\n", chr, (int)ks );*/
    if ( !chr ) {  /* not ASCII */
      *msg = ev->xany.type == KeyPress ?
               XWMSG_SPECIAL_KEY_PRESS : XWMSG_SPECIAL_KEY_RELEASE;
      *key = ks;
    }
    else
      *key = chr;
    *x = ev->xkey.x;  *y = ev->xkey.y;
    break;
default:
    *msg = XWMSG_UNKNOWN;
    *x = *y = -1;
    break;
  }
} /*TranslateEventMsg*/

static char XYInside ( xwidget *wdg, int x, int y )
{
  return x >= wdg->r.x && x < wdg->r.x+wdg->r.width &&
         y >= wdg->r.y && y < wdg->r.y+wdg->r.height;
} /*XYInside*/

void WinMenuInput ( xwinmenu *wm, XEvent *ev )
{
  int     msg, key;
  int     x, y;
  xwidget *wdg;
  char    inp, found;
  Window  root, child;

  wm->ev = ev;
  switch ( ev->xany.type ) {
case Expose:
    if ( ev->xexpose.count == 0 ) {
      if ( wm->changed || !wm->pixmap ) {
        wm->redraw ( wm );
        wm->changed = wm->expose_sent = false;
      }
      if ( wm->pixmap )
        XCopyArea ( xdisplay, wm->pixmap, wm->window, xgc,
                    0, 0, wm->r.width, wm->r.height, 0, 0 );
    }
    return;
case ConfigureNotify:
    wm->r.x = ev->xconfigure.x;      wm->r.y = ev->xconfigure.y;
    wm->r.width = ev->xconfigure.width;  wm->r.height = ev->xconfigure.height;
    if ( wm->pixmap ) {
      XFreePixmap ( xdisplay, wm->pixmap );
      wm->pixmap = XCreatePixmap ( xdisplay, wm->window, wm->r.width, wm->r.height, 24 );
    }
    wm->callback ( wm->empty, WDGMSG_RECONFIGURE, 0, wm->r.width, wm->r.height );
    wm->changed = true;
    PostMenuExposeEvent ( wm );
    break;
case ClientMessage:
    wm->callback ( wm->wlist.next, XWMSG_CLIENT_MESSAGE,
                   ev->xclient.message_type, -1, -1 );
    break;
case EnterNotify:
    for ( wdg = wm->wlist.prev;  wdg;  wdg = wdg->link.prev )
      if ( XYInside ( wdg, ev->xcrossing.x, ev->xcrossing.y ) ) {
        wdg->input ( wdg, XWMSG_ENTERING, 0, ev->xcrossing.x, ev->xcrossing.y );
        lastinput = wdg;
        break;
      }
    break;
case LeaveNotify:
    if ( lastinput ) {
      lastinput->input ( lastinput, XWMSG_LEAVING, 0,
                         ev->xcrossing.x, ev->xcrossing.y );
      lastinput = NULL;
    }
    break;
case GraphicsExpose:
case NoExpose:
    wm->callback ( wm->wlist.next, XWMSG_UNKNOWN, 0, 0, 0 );
    break;
default:
    inp = found = false;
    TranslateEventMsg ( ev, &msg, &key, &x, &y );
    if ( (wdg = wm->focus) ) {
      inp = wdg->input ( wdg, msg, key, x, y );
      if ( !wm->focus && !XYInside ( wdg, x, y ) ) {
        wdg->input ( wdg, XWMSG_LEAVING, 0, x, y );
        lastinput = NULL;
      }
    }
    else {
      for ( wdg = wm->wlist.prev;  wdg;  wdg = wdg->link.prev ) {
        if ( XYInside ( wdg, x, y ) ) {
          found = true;
          if ( wdg != lastinput ) {
            if ( lastinput )
              lastinput->input ( lastinput, XWMSG_LEAVING, 0, x, y );
            wdg->input ( wdg, XWMSG_ENTERING, 0, x, y );
            lastinput = wdg;
          }
          if ( (inp = wdg->input ( wdg, msg, key, x, y )) )
            break;
        }
      }
      if ( !found && lastinput ) {
        lastinput->input ( lastinput, XWMSG_LEAVING, 0, x, y );
        lastinput = NULL;
      }
    }
    if ( !inp )
      wm->callback ( wm->wlist.next, msg, key, x, y );
    if ( wm->changed )
      PostMenuExposeEvent ( wm );
  }
  XQueryPointer ( xdisplay, wm->window, &root, &child,
                  &x, &y, &wm->prevx, &wm->prevy, &wm->prevmask );
  return;
} /*WinMenuInput*/

void GrabInput ( xwidget *wdg )
{
  wdg->wm->focus = wdg;
} /*GrabInput*/

void UngrabInput ( xwidget *wdg )
{
  wdg->wm->focus = NULL;
} /*UngrabInput*/

void DestroyWinMenu ( xwinmenu *wm )
{
  xwidget *wdg, *w;

  for ( wdg = wm->wlist.next; wdg; ) {
    w = wdg;  wdg = w->link.next;
    free ( w );
  }
  if ( wm->pixmap )
    XFreePixmap ( xdisplay, wm->pixmap );
  free ( wm );
} /*DestroyWinMenu*/

/* ////////////////////////////////////////////////////////////////////////// */
xwidget *NewWidget ( struct xwinmenu *wm, int size, int id,
                     int w, int h, int x, int y,                 
                     xwinput input, xwredraw redraw, void *data0, void *data1 )
{
  xwidget *wdg;

  if ( size < sizeof(xwidget) )
    size = sizeof(xwidget);
  if ( (wdg = malloc ( size )) ) {
    memset ( wdg, 0, size );
    if ( !wm->wlist.prev )
      wm->wlist.prev = wm->wlist.next = wdg;
    else {
      wdg->link.prev = wm->wlist.prev;
      wdg->link.prev->link.next = wm->wlist.prev = wdg;
    }
    wdg->id = id;
    wdg->r.width = w;  wdg->r.height = h;  wdg->r.x = x;  wdg->r.y = y;
    wdg->input = input;  wdg->redraw = redraw;
    wdg->data0 = data0;  wdg->data1 = data1;
    wdg->wm = wm;
    wdg->state = WDGSTATE_DEFAULT;
  }
  return wdg;
} /*NewWidget*/

/* ////////////////////////////////////////////////////////////////////////// */
void PostMenuExposeEvent ( xwinmenu *wm )
{
  if ( !wm->expose_sent ) {
    PostExposeEvent ( wm->window, wm->r.width, wm->r.height );
    wm->expose_sent = true;
  }
} /*PostMenuExposeEvent*/

void PostExposeEvent ( Window win, int width, int height )
{
  XExposeEvent ev;

  memset ( &ev, 0, sizeof(XExposeEvent) );
  ev.type = Expose;
  ev.send_event = True;
  ev.display = xdisplay;
  ev.window = win;
  ev.width = width;  ev.height = height;
  XSendEvent ( xdisplay, win, True, ExposureMask, (XEvent*)&ev );
} /*PostExposeEvent*/

void PostClientMessageEvent ( Window win, Atom message_type,
                              int format, void *data )
{
  XClientMessageEvent ev;

  memset ( &ev, 0, sizeof(XClientMessageEvent) );
  ev.type = ClientMessage;
  ev.send_event = True;
  ev.display = xdisplay;
  ev.window = win;
  ev.message_type = message_type;
  ev.format = format;
  if ( data )
    memcpy ( ev.data.b, data, 20*sizeof(char) );
  XSendEvent ( xdisplay, win, True, 0, (XEvent*)&ev );
} /*PostClientMessageEvent*/

/* ////////////////////////////////////////////////////////////////////////// */
static char EmptyInput ( struct xwidget *wdg,
                         int msg, int key, int x, int y )
{
  return false;
} /*EmptyInput*/

static void EmptyRedraw ( struct xwidget *wdg )
{
} /*EmptyRedraw*/

xwidget *NewEmptyWidget ( xwinmenu *wm, int id )
{
  return NewWidget ( wm, sizeof(xwidget), id, 0, 0, 0, 0,
                     EmptyInput, EmptyRedraw, NULL, NULL );
} /*NewEmptyWidget*/

/* ////////////////////////////////////////////////////////////////////////// */
static char ButtonInput ( struct xwidget *wdg,
                          int msg, int key, int x, int y )
{
  switch ( msg ) {
case XWMSG_BUTTON_PRESS:
    if ( key == Button1 )
      goto issue_command;
    break;
case XWMSG_KEY_PRESS:
    if ( key == 0x0D ) {
issue_command:
      wdg->wm->callback ( wdg, WDGMSG_BUTTON_PRESS, 0, x, y );
      return true;
    }
    break;
default:
    break;
  }
  return false;
} /*ButtonInput*/

static void ButtonRedraw ( struct xwidget *wdg )
{
  char *title;

  XSetForeground ( xdisplay, xgc, XWP_BUTTON_COLOUR );
  XFillRectangle ( xdisplay, wdg->wm->pixmap, xgc,
                   wdg->r.x, wdg->r.y, wdg->r.width-1, wdg->r.height-1 );
  XSetForeground ( xdisplay, xgc, XWP_TEXT_COLOUR );
  XDrawRectangle ( xdisplay, wdg->wm->pixmap, xgc,
                   wdg->r.x, wdg->r.y, wdg->r.width-1, wdg->r.height-1 );
  if ( (title = (char*)wdg->data0) ) {
    XSetBackground ( xdisplay, xgc, XWP_BUTTON_COLOUR );
    XDrawString ( xdisplay, wdg->wm->pixmap, xgc,
                  wdg->r.x+2, wdg->r.y+13, title, strlen ( title ) );
  }
} /*ButtonRedraw*/

xwidget *NewButton ( xwinmenu *wm, int id,
                     int w, int h, int x, int y, char *title )
{
  return NewWidget ( wm, sizeof(xwidget), id, w, h, x, y,
                     ButtonInput, ButtonRedraw, (void*)title, NULL );
} /*NewButton*/

/* ////////////////////////////////////////////////////////////////////////// */
static char SwitchInput ( struct xwidget *wdg,
                          int msg, int key, int x, int y )
{
  char *sw, s;

  switch ( msg ) {
case XWMSG_BUTTON_PRESS:
    if ( key == Button1 )
      goto issue_command;
    break;
case XWMSG_KEY_PRESS:
    if ( key == 0x0D ) {
issue_command:
      sw = ((char*)wdg->data1);  s = *sw;
      wdg->wm->callback ( wdg, WDGMSG_SWITCH_CHANGE, *sw = !s, x, y );
      wdg->wm->changed |= *sw != s;
      return true;
    }
    break;
default:
    break;
  }
  return false;
} /*SwitchInput*/

static void SwitchRedraw ( struct xwidget *wdg )
{
  char *title;

  XSetForeground ( xdisplay, xgc, XWP_SWITCH_COLOUR );
  XFillRectangle ( xdisplay, wdg->wm->pixmap, xgc,
                   wdg->r.x, wdg->r.y, wdg->r.height-1, wdg->r.height-1 );
  XSetForeground ( xdisplay, xgc, XWP_TEXT_COLOUR );
  XDrawRectangle ( xdisplay, wdg->wm->pixmap, xgc,
                   wdg->r.x, wdg->r.y, wdg->r.height-1, wdg->r.height-1 );
  if ( (title = (char*)wdg->data0) ) {
    XSetBackground ( xdisplay, xgc, XWP_MENU_BACKGROUND_COLOUR );
    XDrawString ( xdisplay, wdg->wm->pixmap, xgc,
                  wdg->r.x+wdg->r.height+2, wdg->r.y+13, title, strlen ( title ) );
  }
  if ( *((char*)wdg->data1) )
    XFillRectangle ( xdisplay, wdg->wm->pixmap, xgc,
                     wdg->r.x+4, wdg->r.y+4, wdg->r.height-8, wdg->r.height-8 );
} /*SwitchRedraw*/

xwidget *NewSwitch ( xwinmenu *wm, int id,
                     int w, int h, int x, int y, char *title, char *sw )
{
  return NewWidget ( wm, sizeof(xwidget), id, w, h, x, y,
                     SwitchInput, SwitchRedraw, (void*)title, (void*)sw );
} /*NewSwitch*/

/* ////////////////////////////////////////////////////////////////////////// */
static char SlidebarfInput ( struct xwidget *wdg,
                             int msg, int key, int x, int y )
{
  float z, *slipos;

  slipos = wdg->data0;
  switch ( wdg->state ) {
case WDGSTATE_DEFAULT:
    switch ( msg ) {
  case XWMSG_BUTTON_PRESS:
      if ( key == Button1 ) {
        if ( x < wdg->r.x+5 ) x = (int)(wdg->r.x+5);
        else if ( x > wdg->r.x+wdg->r.width-5 ) x = (int)(wdg->r.x+wdg->r.width-5);
        wdg->state = WDGSTATE_MOVING_SLIDE;
        GrabInput ( wdg );
        wdg->wm->changed = true;
        goto update;
      }
      break;
  default:
      break;
    }
    break;

case WDGSTATE_MOVING_SLIDE:
    switch ( msg ) {
  case XWMSG_MOUSE_MOTION:
      if ( ((XMotionEvent*)wdg->wm->ev)->state & Button1Mask ) {
        if ( x < wdg->r.x+5 ) x = (int)(wdg->r.x+5);
        else if ( x > wdg->r.x+wdg->r.width-5 ) x = (int)(wdg->r.x+wdg->r.width-5);
update:
        z = (float)(x-wdg->r.x-5)/(float)(wdg->r.width-10);
        if ( z != *slipos ) {
          *slipos = z;
          wdg->wm->callback ( wdg, WDGMSG_SLIDEBAR_CHANGE, 0, x, y );
          wdg->wm->changed = true;
        }
      }
      else
        goto release;
      return true;
  case XWMSG_BUTTON_RELEASE:
      if ( key == Button1 ) {
release:
        wdg->state = WDGSTATE_DEFAULT;
        UngrabInput ( wdg );
        wdg->wm->changed |= true;
        return true;
      }
      break;
  default:
      break;
    }
    break;

default:
    break;
  }
  return false;
} /*SlidebarfInput*/

static void SlidebarfRedraw ( struct xwidget *wdg )
{
  int   x;
  float *slipos;

  slipos = wdg->data0;
  if ( wdg->state == WDGSTATE_MOVING_SLIDE )
    XSetForeground ( xdisplay, xgc, XWP_ACTIVE_SLIDEBAR_COLOUR );
  else
    XSetForeground ( xdisplay, xgc, XWP_SLIDEBAR_COLOUR );
  XFillRectangle ( xdisplay, wdg->wm->pixmap, xgc,
                   wdg->r.x, wdg->r.y, wdg->r.width-1, wdg->r.height-1 );
  XSetForeground ( xdisplay, xgc, XWP_TEXT_COLOUR );
  XDrawRectangle ( xdisplay, wdg->wm->pixmap, xgc,
                   wdg->r.x, wdg->r.y, wdg->r.width-1, wdg->r.height-1 );
  x = wdg->r.x + 2 + (int)((*slipos)*(float)(wdg->r.width - 10));
  XFillRectangle ( xdisplay, wdg->wm->pixmap, xgc,
                   x, wdg->r.y+2, 6, 6 );
} /*SlidebarfRedraw*/

xwidget *NewSlidebarf ( xwinmenu *wm, int id,
                        int w, int h, int x, int y, float *data )
{
  return NewWidget ( wm, sizeof(xwidget), id, w, h, x, y,
                     SlidebarfInput, SlidebarfRedraw, (void*)data, NULL );
} /*NewSlidebarf*/

/* ////////////////////////////////////////////////////////////////////////// */
typedef struct xLineEditor {
    xwidget wdg;
    int     maxlength,  /* maximal string length */
            chdisp,     /* number of characters displayed */
            start,      /* first character displayed */
            pos;        /* text cursor position */
  } xLineEditor;

static char insert = true;

void LeaveEditingState ( xwidget *wdg )
{
  wdg->state = WDGSTATE_DEFAULT;
  UngrabInput ( wdg );
  wdg->wm->changed = true;
} /*LeaveEditingState*/

static void CorrectCursorPos ( xLineEditor *xed )
{
  if ( xed->pos < 0 )
    xed->pos = 0;
  else if ( xed->pos > xed->maxlength )
    xed->pos = xed->maxlength;
  if ( xed->pos < xed->start )
    xed->start = xed->pos;
  else if ( xed->pos >= xed->start+xed->chdisp ) 
    xed->start = (short)(xed->pos-xed->chdisp+1);
} /*CorrectCursorPos*/

static char LineEditorInput ( struct xwidget *wdg,
                             int msg, int key, int x, int y )
{
  xLineEditor *xed;
  char        *text;
  int         lgt, pos;

  xed = (xLineEditor*)wdg;
  lgt = strlen ( text = (char*)wdg->data0 );
  pos = xed->pos;
  switch ( wdg->state ) {
case WDGSTATE_DEFAULT:
    switch ( msg ) {
  case XWMSG_BUTTON_PRESS:
      if ( key == Button1 ) {
        wdg->state = WDGSTATE_EDITING;
        GrabInput ( wdg );
        wdg->wm->changed = true;
      }
      break;
  default:
      break;
    }
    break;

case WDGSTATE_EDITING:
    switch ( msg ) {
  case XWMSG_SPECIAL_KEY_PRESS:
/*printf ( "special key: 0x%x\n", key );*/
      switch ( key ) {
    case XK_Home: case XK_KP_Home:
        if ( pos > 0 ) {
          pos = 0;
          goto redraw_it;
        }
        break;
    case XK_End: case XK_KP_End:
        if ( pos < lgt ) {
          pos = lgt;
          goto redraw_it;
        }
        break;
    case XK_Left: case XK_KP_Left:
        if ( pos > 0 ) {
          pos --;
          goto redraw_it;
        }
        break;
    case XK_Right: case XK_KP_Right:
        if ( pos < xed->maxlength-1 ) {
          pos ++;
          goto redraw_it;
        }
        break;
    case XK_Insert: case XK_KP_Insert:
        insert = !insert;
        goto redraw_it;
    case XK_Delete: case XK_KP_Delete:
        if ( pos < lgt-1 ) {
          memmove ( &text[xed->pos], &text[xed->pos+1], (lgt-pos+1)*sizeof(char) );
          goto redraw_it;
        }
        break;
    default:
        break;
      }
      break;

  case XWMSG_KEY_PRESS:
/*printf ( "key: 0x%x\n", key );*/
      switch ( key ) {
    case 0x0D:  /* Enter */
        LeaveEditingState ( wdg );
        break;
    case 0x08:  /* Backspace */
        if ( pos > lgt ) {
          pos = lgt;
          goto redraw_it;
        }
        else if ( xed->pos > 0 && lgt > 0 ) {
          memmove ( &text[xed->pos-1], &text[xed->pos], (lgt-pos+1)*sizeof(char) );
          pos --;
          goto redraw_it;
        }
        break;
    case 0x1B:  /* Esc */
        wdg->wm->callback ( wdg, WDGMSG_EDITOR_ESCAPE, 0, 0, 0 );
        break;
    default:
        if ( key >= 0x20 && key <= 0x7F ) {  /* ASCII character */
          if ( pos > lgt )
            pos = lgt;
          if ( insert ) {
            if ( lgt >= xed->maxlength )
              lgt --;
            if ( pos < lgt )
              memmove ( &text[pos+1], &text[pos], (lgt-pos)*sizeof(char) );
          }
          text[pos++] = (char)key;
redraw_it:
          xed->pos = pos;
          CorrectCursorPos ( xed );
          wdg->wm->changed = true;
        }
        break;
      }
      break;

  case XWMSG_BUTTON_PRESS:
      if ( key == Button1 ) {
        if ( XYInside ( wdg, x, y ) ) {
          xed->pos = xed->start + (x-wdg->r.x-2) / 6;
          wdg->wm->changed = true;
        }
        else
          LeaveEditingState ( wdg );
      }
      break;

  default:
      break;
    }
    break;

default:
    break;
  }
  return true;
} /*LineEditorInput*/

static void LineEditorRedraw ( struct xwidget *wdg )
{
  xLineEditor *xed;
  char        *text;
  char        buffer[257];
  int         x, h, lgt;

  xed = (xLineEditor*)wdg;
  text = wdg->data0;
  lgt = (short)(strlen(text)-xed->start);
  if ( lgt > xed->maxlength )
    lgt = xed->maxlength;
  if ( wdg->state == WDGSTATE_EDITING ) {
    XSetForeground ( xdisplay, xgc, XWP_ACTIVE_EDITOR_COLOUR );   
    XFillRectangle ( xdisplay, wdg->wm->pixmap, xgc,
                     wdg->r.x+1, wdg->r.y+1, wdg->r.width-2, wdg->r.height-2 );
    CorrectCursorPos ( xed );
        /* draw the text cursor */
    XSetForeground ( xdisplay, xgc, XWP_EDITOR_CURSOR_COLOUR );
    x = (short)(wdg->r.x + 2 + (xed->pos-xed->start)*6);
    h = insert ? 13 / 2 : 13 / 4;
    XFillRectangle ( xdisplay, wdg->wm->pixmap, xgc,
                     x, wdg->r.y+wdg->r.height-2-h, 6, h );
  }
  else {
    XSetForeground ( xdisplay, xgc, XWP_EDITOR_COLOUR );
    XFillRectangle ( xdisplay, wdg->wm->pixmap, xgc,
                     wdg->r.x+1, wdg->r.y+1, wdg->r.width-2, wdg->r.height-2 );
  }
  XSetForeground ( xdisplay, xgc, XWP_TEXT_COLOUR );
  XDrawRectangle ( xdisplay, wdg->wm->pixmap, xgc,
                   wdg->r.x, wdg->r.y, wdg->r.width-1, wdg->r.height-1 );
  if ( lgt > 0 ) {
    if ( lgt > xed->chdisp )
      lgt = xed->chdisp;
    memcpy ( buffer, &text[xed->start], lgt );   
    buffer[lgt] = 0;
    XDrawString ( xdisplay, wdg->wm->pixmap, xgc,
                  wdg->r.x+2, wdg->r.y+wdg->r.height-5, buffer, lgt );
  }
} /*LineEditorRedraw*/

xwidget *NewLineEditor ( xwinmenu *wm, int id, int w, int h, int x, int y,               
                         int maxlength, char *txtbuf )
{
  xLineEditor *xed;

  if ( (xed = (xLineEditor*)NewWidget ( wm, sizeof(xLineEditor), id, w, h, x, y,
                   LineEditorInput, LineEditorRedraw, (void*)txtbuf, NULL )) ) {
    xed->maxlength = maxlength;
    xed->chdisp = (w-2)/6;  /* fixed font */
    xed->start = xed->pos = 0;
  }
  return (xwidget*)xed;
} /*NewLineEditor*/

