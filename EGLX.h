#ifndef __EGLX_H__
#define __EGLX_H__

 /* to understand the "NULL" keyword */
#include <stdlib.h>

 /* for all the XK_<key> definitions */ 
#include "EGLX-keysymdef.h"


 /* simple type definitions */
#define Bool			int
#define True			1
#define False			0

#define None			0L
#define AllocNone		0
#define InputOutput		1

#define USPosition			(1L << 0)
#define USSize				(1L << 1)

#define CWBackPixel			(1L<<1)
#define CWBorderPixel		(1L<<3)

#define CWColormap			(1L<<13)

#define KeyPressMask		(1L<<0)
#define CWEventMask			(1L<<11)
#define ButtonPressMask		(1L<<2)
#define ButtonMotionMask	(1L<<3)
#define ExposureMask		(1L<<15)
#define StructureNotifyMask	(1L<<17)

#define VisualIDMask 		0x1
#define VisualScreenMask	0x2
#define VisualDepthMask		0x4

#define MotionNotify		6

#define XLookupKeySym		3

#define PropModeReplace		0
#define PropModePrepend		1
#define PropModeAppend		2
#define XA_ATOM ((Atom) 4)

#define GLX_USE_GL		1
#define GLX_BUFFER_SIZE		2
#define GLX_LEVEL		3
#define GLX_RGBA		4
#define GLX_DOUBLEBUFFER 5
#define GLX_STEREO		6
#define GLX_AUX_BUFFERS		7
#define GLX_RED_SIZE	8
#define GLX_GREEN_SIZE	9
#define GLX_BLUE_SIZE	10
#define GLX_ALPHA_SIZE	11
#define GLX_DEPTH_SIZE	12
#define GLX_STENCIL_SIZE 13
#define GLX_ACCUM_RED_SIZE 14
#define GLX_ACCUM_GREEN_SIZE 15
#define GLX_ACCUM_BLUE_SIZE 16
#define GLX_ACCUM_ALPHA_SIZE 17
#define GLX_SAMPLE_BUFFERS 100000
#define GLX_SAMPLES		100001

#define GLX_VENDOR		1
#define GLX_VERSION		2
#define GLX_EXTENSIONS	3

#define GLX_VERSION_1_1		1
#define GLX_VERSION_1_2		1

#define GL_BACK_LEFT	0x0402
#define GL_BACK_RIGHT	0x0403
#define GL_BACK			0x0405

#define GLX_RGBA_TYPE	0x8014

typedef unsigned long int XID;
typedef XID Window;
typedef XID Drawable;
typedef XID Colormap;
typedef XID Pixmap;
typedef XID KeySym;
typedef unsigned long Atom;
typedef char *XPointer;

typedef unsigned long Time;
typedef unsigned long VisualID;

typedef XID GLXPixmap;
typedef XID GLXDrawable;
typedef XID GLXContextID;

#define Status int


 /* complex type (empty) pre-definitions */
struct SCREEN;
struct DEPTH;
struct VISUAL;

 /* complex type definitions */
struct _XDisplay
{
	int proto_major_version;
	int proto_minor_version;
	char *vendor;
	char *display_name;
	int default_screen;
	int nscreens;
	struct SCREEN *screens;
};
typedef struct _XDisplay Display;

typedef struct SCREEN
{
	struct _XDisplay display;
	Window root;
	int width, height;
	int mwidth, mheight;
	int ndepths;
	struct DEPTH *depths;
	int root_depth;
	struct VISUAL *root_visual;
	//GC default_gc;
	Colormap cmap;
	unsigned long white_pixel;
	unsigned long black_pixel;
} Screen;

typedef struct DEPTH
{
	int depth;
	int nvisuals;
	struct VISUAL *visuals;

} Depth;

typedef struct VISUAL
{
	VisualID visualid;
#if defined (__cplusplus) || defined (c_plusplus)
	int c_class;
#else
	int class;
#endif
	unsigned long red_mask;
	unsigned long green_mask;
	unsigned long blue_mask;

} Visual;

typedef struct
{
	Visual *visual;
	VisualID visualid;
	int screen;
	int depth;
#if defined (__cplusplus) || defined (c_plusplus)
	int c_class;
#else
	int class;
#endif
	unsigned long red_mask;
	unsigned long green_mask;
	unsigned long blue_mask;
	int colormap_size;
	int bits_per_rgb;
} XVisualInfo;

typedef struct
{
	unsigned long background_pixel;
	unsigned long border_pixel;
	Colormap colormap;
	long event_mask;
} XSetWindowAttributes;

typedef struct
{
	long flags;
	int x, y;
	int width, height, min_width, min_height, max_width, max_height;
	int width_inc, height_inc, base_width, base_height;
	int win_gravity;
	struct
	{
		int x, y;
	} min_aspect, max_aspect;
} XSizeHints;

#include "EGLX-eventdef.h"
typedef struct
{
	int type;
    XAnyEvent xany;
    XKeyEvent xkey;
    XButtonEvent xbutton;
    XMotionEvent xmotion;
    XCrossingEvent xcrossing;
    XFocusChangeEvent xfocus;
    XExposeEvent xexpose;
    XGraphicsExposeEvent xgraphicsexpose;
    XNoExposeEvent xnoexpose;
    XVisibilityEvent xvisibility;
    XCreateWindowEvent xcreatewindow;
    XDestroyWindowEvent xdestroywindow;
    XUnmapEvent xunmap;
    XMapEvent xmap;
    XMapRequestEvent xmaprequest;
    XReparentEvent xreparent;
    XConfigureEvent xconfigure;
    XGravityEvent xgravity;
    XResizeRequestEvent xresizerequest;
    XConfigureRequestEvent xconfigurerequest;
    XCirculateEvent xcirculate;
    XCirculateRequestEvent xcirculaterequest;
    XPropertyEvent xproperty;
    XSelectionClearEvent xselectionclear;
    XSelectionRequestEvent xselectionrequest;
    XSelectionEvent xselection;
    XColormapEvent xcolormap;
    XClientMessageEvent xclient;
    XMappingEvent xmapping;
    XErrorEvent xerror;
    XKeymapEvent xkeymap;
    XGenericEvent xgeneric;
    XGenericEventCookie xcookie;
    long pad[24];
} XEvent;

struct _XComposeStatus {
    XPointer compose_ptr;       /* state table pointer */
    int chars_matched;          /* match state */
};
typedef struct _XComposeStatus XComposeStatus;

struct __GLXFBConfigRec {
	VisualID vid;
#if defined (__cplusplus) || defined (c_plusplus)
	int c_class;
#else
	int class;
#endif
	Bool rgba;
	int redSize, greenSize, blueSize, alphaSize;
	unsigned long redMask, greenMask, blueMask, alphaMask;
	int accumRedSize, accumGreenSize, accumBlueSize, accumAlphaSize;
	Bool doubleBuffer;
	Bool stereo;
	int bufferSize;
	int depthSize;
	int stencilSize;
	int auxBuffers;
	int level;
	/* put extended visual properties there ? */
};
typedef struct __GLXFBConfigRec *GLXFBConfig;

/*struct __GLXContextRec {
	struct __GLXContextRec *last;
	struct __GLXContextRec *next;
	struct __GLXContextRec *nextPriv;
	// __GLContextModes *modes;
	XID id;
	XID share_id;
	VisualID vid;
	// GLint screen;
	// GLboolean idExists;
	// GLboolean isCurrent;
	// GLboolean isDirect;
	// GLuint pendingState;
	// GLboolean hasUnflushedCommands;
	// GLenum renderMode;
};
typedef struct __GLXContextRec *GLXContext;*/
typedef unsigned int GLXContext; /* very bad, but makes it work !*/


/*                PROTOTYPES					*/
/* -------------------------------------------- */

extern int EGLX_main (Bool placement);
extern int EGLX_XParseGeometry (char *geometry, int *x, int *y, unsigned int *width, unsigned int *height);
extern char* EGLX_XDisplayName (char *string);
extern int EGLX_XStoreName (Display *dpy, Window win, char *wname);
extern Display* EGLX_XOpenDisplay (char *dpy);
extern unsigned int EGLX_XCreateWindow (Display *dpy, Window win, int x, int y, unsigned int width, unsigned int height, unsigned int border_width, int depth, unsigned int clas, Visual *visual, unsigned long mask, XSetWindowAttributes *attrs);
extern unsigned int EGLX_XCreateSimpleWindow (Display *dpy, Window win, int x, int y, unsigned int width, unsigned int height, unsigned int border_width, unsigned long border, unsigned long background);
extern void EGLX_XSetNormalHints (Display *dpy, Window win, XSizeHints *hints);
extern void EGLX_XSetStandardProperties (Display *dpy, Window win, char *wname, char *iname, Pixmap ipixmap, char **argv, int argc, XSizeHints *hints);
extern int EGLX_XMapWindow (Display *dpy, Window win);
extern int EGLX_XClearWindow (Display *dpy, Window win);
extern int EGLX_XClearArea (Display *dpy, Window win, int x, int y, unsigned int width, unsigned int height, Bool exposures);
extern int EGLX_XDestroyWindow (Display *dpy, Window win);
extern int EGLX_XCloseDisplay (Display *dpy);
extern int EGLX_XPending (Display *dpy);
extern int EGLX_XNextEvent (Display *dpy, XEvent *evt);
extern Atom EGLX_XInternAtom (Display *dpy, char *atom_name, Bool only_if_exists);
extern int EGLX_XChangeProperty (Display *dpy, Window win, Atom prop, Atom type, int format, int mode, unsigned char *data, int nelts);
extern int EGLX_XLookupString (XKeyEvent *event, char *buffer_return, int bytes_buffer, KeySym *keysym_return, XComposeStatus *status_in_out);
extern KeySym EGLX_XLookupKeysym (XKeyEvent *event, int index);
extern Status EGLX_XGetGeometry (Display *dpy, Drawable d, Window *root_ret, int *x_ret, int *y_ret, unsigned int *width_ret, unsigned int *height_ret, unsigned int *border_ret, unsigned int *depth_ret);
extern XVisualInfo* EGLX_XGetVisualInfo (Display *dpy, long vinfo_mask, XVisualInfo *vinfo_templ, int *nitems_ret);
extern VisualID EGLX_XVisualIDFromVisual (Visual *visual);

extern XVisualInfo* EGLX_glXChooseVisual (Display *dpy, int scr, int *attrs);
extern GLXContext EGLX_glXCreateContext (Display *dpy, XVisualInfo *info, GLXContext list, Bool direct);
extern GLXContext EGLX_glXCreateNewContext (Display *dpy, GLXFBConfig config, int render_type, GLXContext list, Bool direct);
extern Bool EGLX_glXMakeCurrent (Display *dpy, GLXDrawable drawable, GLXContext ctx);
extern void EGLX_glXSwapBuffers (Display *dpy, GLXDrawable drawable);
extern void EGLX_glXDestroyContext (Display *dpy, GLXContext ctx);
extern void EGLX_glXWaitX (void);
extern void EGLX_glXWaitGL (void);
extern int EGLX_glXGetConfig (Display *dpy, XVisualInfo *info, int attr, int *value);
extern GLXContext EGLX_glXGetCurrentContext (void);
extern Bool EGLX_glXQueryVersion (Display *dpy, int *major, int *minor);
extern Bool EGLX_glXQueryExtension (Display *dpy, int *error, int *event);
extern const char* EGLX_glXQueryExtensionsString (Display *dpy, int scr);
extern const char* EGLX_glXGetClientString (Display *dpy, int value);
extern void* EGLX_glXGetProcAddressARB (const unsigned char *procname);

extern struct wl_surface* ELGX_wl_surface_get (int win);
extern struct wl_shell_surface* EGLX_wl_shell_surface_get (int win);
extern struct wl_egl_window* EGLX_wl_egl_window_get (int win);


/*                     X11                       */
/* --------------------------------------------- */

#define XParseGeometry			EGLX_XParseGeometry
#define XDisplayName			EGLX_XDisplayName
#define XStoreName			EGLX_XStoreName
#define XOpenDisplay			EGLX_XOpenDisplay
#define XCreateWindow			EGLX_XCreateWindow
#define XCreateSimpleWindow		EGLX_XCreateSimpleWindow
#define XSetNormalHints			EGLX_XSetNormalHints
#define XSetStandardProperties	EGLX_XSetStandardProperties
#define XMapWindow				EGLX_XMapWindow
#define XClearWindow			EGLX_XClearWindow
#define XClearArea				EGLX_XClearArea
#define XDestroyWindow			EGLX_XDestroyWindow
#define XCloseDisplay			EGLX_XCloseDisplay
#define XPending				EGLX_XPending
#define XNextEvent				EGLX_XNextEvent
#define XInternAtom				EGLX_XInternAtom
#define XChangeProperty			EGLX_XChangeProperty
#define XLookupString			EGLX_XLookupString
#define XLookupKeysym			EGLX_XLookupKeysym
#define XGetGeometry			EGLX_XGetGeometry
#define XGetVisualInfo			EGLX_XGetVisualInfo
#define XVisualIDFromVisual		EGLX_XVisualIDFromVisual

#define XCreateColormap(D,W,V,A) 46137345
#define XFree(R)				NULL

#define RootWindow(D,S)			DefaultRootWindow(D)
#define DefaultRootWindow(D)	0	// RootWindow doesn't exist under Wayland

#define DefaultScreen(D)		0 // ICI ON DEVRAIT UTILISER L'ATTRIBUT
#define DisplayWidth(D,S)		1024 // ICI ON DEVRAIT UTILISER L'ATTRIBUT
#define DisplayHeight(D,S)		768 // ICI ON DEVRAIT UTILISER L'ATTRIBUT

#define KeyPress		2
#define ButtonPress		4
#define Expose			12
#define GraphicsExpose	13
#define NoExpose		14
#define ConfigureNotify	22


/*                     GLX                       */
/* --------------------------------------------- */

#define glXChooseVisual			EGLX_glXChooseVisual
#define glXCreateContext		EGLX_glXCreateContext
#define glXCreateNewContext		EGLX_glXCreateNewContext
#define glXMakeCurrent			EGLX_glXMakeCurrent
#define glXSwapBuffers			EGLX_glXSwapBuffers
#define glXDestroyContext		EGLX_glXDestroyContext
#define glXWaitX				EGLX_glXWaitGL
#define glXWaitGL				EGLX_glXWaitGL
#define glXGetConfig			EGLX_glXGetConfig
#define glXGetCurrentContext		EGLX_glXGetCurrentContext
#define glXQueryVersion			EGLX_glXQueryVersion
#define glXQueryExtension		EGLX_glXQueryExtension
#define glXQueryExtensionsString EGLX_glXQueryExtensionsString
#define glXGetClientString		EGLX_glXGetClientString
#define glXGetProcAddressARB		EGLX_glXGetProcAddressARB




#endif
