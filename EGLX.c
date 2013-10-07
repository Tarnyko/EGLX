#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <wayland-client.h>
#include <wayland-egl.h>
#include <EGL/egl.h>
#include <EGL/eglmesaext.h>

#include "EGLX.h"


 /* Wayland display structure */
struct display {
	struct wl_display *display;
	struct wl_compositor *compositor;
	struct wl_registry *registry;
	struct wl_shell *shell;
	struct wl_output *output;
};

 /* EGL window structure */
struct window {
	struct wl_egl_window *ewindow;
	struct wl_surface *surface;
	struct wl_shell_surface *shell_surface;
	EGLSurface esrf;
	struct wl_callback *callback;
	int x, y, width, height;
	char *title;
};
EGLDisplay edpy;
EGLConfig ecfg;
EGLContext ectx;
 int window_count;
 Bool window_first;

 /* EGL configuration & initialization attributes */
static const EGLint ecfgattrs[] = {
	EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
	EGL_RED_SIZE, 1,
	EGL_GREEN_SIZE, 1,
	EGL_BLUE_SIZE, 1,
	EGL_ALPHA_SIZE, 0,
	EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
	EGL_NONE
};

static const EGLint ectxattrs[] = {
	EGL_CONTEXT_CLIENT_VERSION, 1,
	EGL_NONE
};

 /* Fake XWindow structures */
Display *xdpy;
Screen *xscr;
XVisualInfo *xvi;
Window *xwin;

 /* Real Wayland structures */
struct display *display;
struct window *window;

 /* keep track if MapWindow() and MakeCurrent() have already been run once */ 
Bool *mapwindowdone;
Bool *makecurrentdone;
Bool createcontextdone;
 /* keep track of the current and last updated context */
XID currentcontext;
XID lastupdatedcontext;

 /* how should we position our surfaces */
#define EGLX_AUTOPLACEMENT False
#define EGLX_MANUALPLACEMENT True
Bool mplacement;


/*                   COMMON                      */
/* --------------------------------------------- */

static void display_handle_geometry (void *data, struct wl_output *output, int x, int y, int physical_width, int physical_height, int subpixel, const char *make, const char *model, int transform)
{
	 /* get screen width and height in millimeters */
    xscr->mwidth = physical_width;
    xscr->mheight = physical_height;
}

static void display_handle_mode (void *data, struct wl_output *output, uint32_t flags, int width, int height, int refresh)
{
	 /* get screen width and height in pixels */
    xscr->width = width;
    xscr->height = height;
}

static const struct wl_output_listener output_listener = {
	display_handle_geometry,
	display_handle_mode
};

static void registry_handle_global (void *data, struct wl_registry *registry, uint32_t id, const char *interface, uint32_t version)
{
	struct display *d = data;

	if (strcmp (interface, "wl_compositor") == 0) {
		d->compositor = wl_registry_bind (registry, id, &wl_compositor_interface, 1);
	} else if (strcmp(interface, "wl_shell") == 0) {
		d->shell = wl_registry_bind (registry, id, &wl_shell_interface, 1);
	} else if (strcmp(interface, "wl_output") == 0) {
		d->output = wl_registry_bind (registry, id, &wl_output_interface, 1);
		wl_output_add_listener (d->output, &output_listener, d);
	}
}

static void registry_handle_global_remove (void *data, struct wl_registry *registry, uint32_t name)
{
}

static const struct wl_registry_listener registry_listener = {
	registry_handle_global,
	registry_handle_global_remove
};

int EGLX_main (Bool placement)
{
	mplacement = placement;

	 /* we start with only one window */
	window_count = 1;
	window_first = True;

	 /* pre-allocates one X Window */
	xwin = malloc (sizeof(Window)*window_count);
	xwin[window_count-1] = window_count;	/* first Window has ID 1 */

	 /* pre-allocates one Wayland window */
    window = malloc (sizeof(*window)*window_count);

	 /* for first window, MapWindow() and MakeCurrent() haven't been run yet */
	mapwindowdone = malloc (sizeof(Bool)*window_count);
	makecurrentdone = malloc (sizeof(Bool)*window_count);
	mapwindowdone[window_count-1] = False;
	makecurrentdone[window_count-1] = False;

	 /* CreateContext() hasn't been run yet */
	createcontextdone = False;

	 /* pre-allocates all other global structures */
	xdpy = malloc (sizeof *xdpy);
	xscr = malloc (sizeof *xscr);
	xvi = malloc (sizeof *xvi);
    display = malloc (sizeof *display);

	/* CONNECT TO THE WAYLAND DISPLAY */
	display->display = wl_display_connect (NULL);
	assert (display->display != NULL);
	/* register the main event listener */
	display->registry = wl_display_get_registry (display->display);
	wl_registry_add_listener (display->registry, &registry_listener, display);
	/* iterate once */
	wl_display_roundtrip (display->display);

	/* DEFINE SOME DEFAULT WINDOW ATTRIBUTES */
	window[window_count-1].width  = 320;
	window[window_count-1].height = 240;
	window[window_count-1].title = "";

	/* CREATE A FAKE XWINDOW DISPLAY WITH STANDARD VALUES */
	xdpy->proto_major_version = 11;
	xdpy->proto_minor_version = 6;
	xdpy->vendor = "X.Org";
	xdpy->display_name = "XLib Display";
	xdpy->default_screen = 0;
	xdpy->nscreens = 1;

	/* CREATE A FAKE XWINDOW SCREEN WITH STANDARD VALUES */
	xscr->display = *xdpy;
	xscr->root = xwin[window_count-1]; 
	/* xscr->width = 
	   xscr->height = 
	   xscr->mwidth = 
	   xscr->mheight = */ /* these values are read in the wl_output listener cb */
	xscr->ndepths = 7;
	xscr->root_depth = 32; // GET THE WESTON BUFFER DEPTH ?
	xscr->white_pixel = 167772151;
	xscr->black_pixel = 0;

	/*xvi->visual->visualid = 233; // RANDOM...
	xvi->visual->class = 4;
	xvi->visual->red_mask = 16711680;
	xvi->visual->green_mask = 65280;
	xvi->visual->blue_mask = 255;*/ // ENABLING THAT SEGFAULTS !
	xvi->visualid = 233; //RANDOM
	xvi->screen = 0;
	xvi->depth = 32; // IS THERE A WESTON DEPTH SETTING ?
	xvi->class = 4; // WHATIS ?
	xvi->red_mask = 16711680;
	xvi->green_mask = 65280;
	xvi->blue_mask = 255;
	xvi->colormap_size = 8;

	return 0;
}



/*                     X11                       */
/* --------------------------------------------- */

  /* parses a string such as "320x240+100-100" */
int EGLX_XParseGeometry (char *geometry, int *x, int *y, unsigned int *width, unsigned int *height)
{
	printf ("XParseGeometry() not implemented yet ; sorry\n");

	/* SHOULD RETURN A MASK DEPENDING ON WHICH VALUES WERE ASSIGNED */
	return 0;
}

  /* retrieves the friendly display name */
char* EGLX_XDisplayName (char *string)
{
	return ":0.0";
}

  /* sets the potential Window title */
int EGLX_XStoreName (Display *dpy, Window win, char *wname)
{
	window[win-1].title = wname;

	return 1;
}

  /* connects to the current display */
Display* EGLX_XOpenDisplay (char *dpy)
{ 
	edpy = eglGetDisplay ((EGLNativeDisplayType) display->display);
	assert (edpy != NULL);

	EGLint major, minor, n;
	EGLBoolean result;
	result = eglInitialize (edpy, &major, &minor);
	assert (result == EGL_TRUE);

	result = eglBindAPI (EGL_OPENGL_ES_API);
	assert (result == EGL_TRUE);

	return xdpy;
}

  /* create a Window, for which only dimensions matter */
unsigned int EGLX_XCreateWindow (Display *dpy, Window win,
								 int x, int y, unsigned int width, unsigned int height,
								 unsigned int border_width, int depth, unsigned int class,
								 Visual *visual, unsigned long mask,
								 XSetWindowAttributes *attrs)
{
	 /* if not the first one, create a new window */
	if (window_first == False)
	{
		window_count++;
		window = realloc (window, sizeof(*window)*window_count);
		xwin = realloc (xwin, sizeof(Window)*window_count);
		xwin[window_count-1] = window_count;
		mapwindowdone = realloc (mapwindowdone, sizeof(Bool)*window_count);
		makecurrentdone = realloc (makecurrentdone, sizeof(Bool)*window_count);
		mapwindowdone[window_count-1] = False;
		makecurrentdone[window_count-1] = False;
	}

	window_first = False;

	window[window_count-1].width = width;
	window[window_count-1].height = height;

	return xwin[window_count-1];
}

  /* create a Window with less arguments, for which only dimensions matter */
unsigned int EGLX_XCreateSimpleWindow (Display *dpy, Window win, int x, int y, unsigned int width, unsigned int height, unsigned int border_width, unsigned long border, unsigned long background)
{
	 /* if not the first one, create a new window */
	if (window_first == False)
	{
		window_count++;
		window = realloc (window, sizeof(*window)*window_count);
		xwin = realloc (xwin, sizeof(Window)*window_count);
		xwin[window_count-1] = window_count;
		mapwindowdone = realloc (mapwindowdone, sizeof(Bool)*window_count);
		makecurrentdone = realloc (makecurrentdone, sizeof(Bool)*window_count);
		mapwindowdone[window_count-1] = False;
		makecurrentdone[window_count-1] = False;
	}

	window_first = False;

	window[window_count-1].width = width;
	window[window_count-1].height = height;

	return xwin[window_count-1];
}

  /* sets a Window size, same thing here */
void EGLX_XSetNormalHints (Display *dpy, Window win, XSizeHints *hints)
{
	window[win-1].width = hints->width;
	window[win-1].height = hints->height;	
}

 /* sets a Window size, same thing here, but with title */
void EGLX_XSetStandardProperties (Display *dpy, Window win, char *wname, char *iname, Pixmap ipixmap, char **argv, int argc, XSizeHints *hints)
{
	window[win-1].width = hints->width;
	window[win-1].height = hints->height;
	window[win-1].title = wname;
}

 /* *really* displays the Window on the screen. Very important function */
extern int EGLX_XMapWindow (Display *dpy, Window win)
{
	if (mapwindowdone[win-1] == False)
	{
		/* create the compositor surface */
		window[win-1].surface = wl_compositor_create_surface (display->compositor);
		window[win-1].shell_surface = wl_shell_get_shell_surface (display->shell, window[win-1].surface);

		/* it's here we really use width, height, and title */
		window[win-1].ewindow = wl_egl_window_create (window[win-1].surface, window[win-1].width, window[win-1].height);
		window[win-1].esrf = eglCreateWindowSurface (edpy, ecfg, window[win-1].ewindow, NULL);
		wl_shell_surface_set_title (window[win-1].shell_surface, window[win-1].title);

		 /* if it is not the first window, it should have a different position  */
		/* when using manual placement, you HAVE to position them from within the application, otherwise nothing will show */
		if (mplacement == False)
		{
			if (win > 1)
			 wl_shell_surface_set_transient (window[win-1].shell_surface, window[win-2].surface, 100, 100, 0);
			else
		 	 wl_shell_surface_set_toplevel (window[win-1].shell_surface);
		}

		/* remembers MapWindow() has been run on this window */
		mapwindowdone[win-1] = True;
	}

	return 1;
}

 /* deletes the whole window content */
int EGLX_XClearWindow (Display *dpy, Window win)
{
	/* 	struct wl_buffer *buffer;
	! PUT BLACK COLOR IN BUFFER HERE !
	wl_surface_attach (window->surface, buffer, 0, 0);
	wl_surface_damage (window->surface, 0, 0, window->width, window->height);
	wl_surface_commit (); */ /* VALID IN SHM MODE, BUT EGL MODE ? */

	return 1;
}

 /* deletes part of the window content */
int EGLX_XClearArea (Display *dpy, Window win, int x, int y, unsigned int width, unsigned int height, Bool exposures)
{
	/* struct wl_buffer *buffer;
	! PUT BLACK COLOR IN BUFFER HERE !
	wl_surface_attach (window->surface, buffer, 0, 0);
	wl_surface_damage (window->surface, x, y, width, height);
	wl_surface_commit () */ /* VALID IN SHM MODE, BUT EGL MODE ? */

	return 1;
}

 /* makes the window disappear */
int EGLX_XDestroyWindow (Display *dpy, Window win)
{
	/* destroys the compositor surface */
	wl_surface_destroy (window[win-1].surface);

	return 1;
}

 /* disconnects from the current display */
int EGLX_XCloseDisplay (Display *dpy)
{
	if (window_count <= 1)
	{
		eglTerminate (edpy);
		eglReleaseThread ();

		wl_display_flush (display->display);
		wl_display_disconnect (display->display);
	}

	return 0;
}

 /* returns the number of events still in queue for the "main loop" */
int EGLX_XPending (Display *dpy)
{
	/* DON'T USE DISPATCH ! IT WILL ALWAYS RETURN 0 WITH
		MULTIPLE CONTEXTS ! */
	int result = wl_display_roundtrip (display->display);

	if (result == -1)
	 return 1;
	else
	 return 0;
}

 /* iterates to the next event (i.e. loops) and fills in its type */
int EGLX_XNextEvent (Display *dpy, XEvent *evt)
{
	wl_display_roundtrip (display->display);

	/* always put the X "Expose" event in the struct, so the wrapped app refreshes */
	evt->type = Expose;

	return 0;
}

 /* returns an identifier matching a string, or "None" */
Atom EGLX_XInternAtom (Display *dpy, char *atom_name, Bool only_if_exists)
{
	/*
	if (strcmp(atom_name,"_NET_WM_STATE") == 0)
		return 290;
	if (strcmp(atom_name,"_NET_WM_STATE_FULLSCREEN") == 0)
		return 293;
	*/

	return None;
}

 /* sets some window-manager-specific hints, like maximized, fullscreen... */
int EGLX_XChangeProperty (Display *dpy, Window win, Atom prop, Atom type, int format, int mode, unsigned char *data, int nelts)
{
	/* IMPLEMENTATION STILL TO DO xxxx */

	return 1;
}

 /* translates a XKeyEvent into a readable KeySym */
int EGLX_XLookupString (XKeyEvent *event, char *buffer_return, int bytes_buffer, KeySym *keysym_return, XComposeStatus *status_in_out)
{
	/* IMPLEMENTATION STILL TO DO xxxx */

	return 0;
}

 /* same, translates a XKeyEvent into a readable KeySym */
KeySym EGLX_XLookupKeysym (XKeyEvent *event, int index)
{
	/* IMPLEMENTATION STILL TO DO xxxx */

	return 0;
}

 /* retrieves the target drawable position and dimensions */
Status EGLX_XGetGeometry (Display *dpy, Drawable d, Window *root_ret, int *x_ret, int *y_ret, unsigned int *width_ret, unsigned int *height_ret, unsigned int *border_ret, unsigned int *depth_ret)
{
    if (window[d-1].surface != NULL)
	{
		*root_ret = xwin[d-1]; /* there is no root Window in Wayland, so return the current one */
		*x_ret = 0;
		*y_ret = 0;
		*width_ret = window[d-1].width;
		*height_ret = window[d-1].height;
		*border_ret = 0;
		*depth_ret = 32;		/* retrieve the Weston buffer depth here ? */
	}
	 // CHECK WHAT'S FILLED IF NOTHING MATCHES !

	return 1;
}

 /* retrieves list of visuals matching the given mask and template  */
XVisualInfo* EGLX_XGetVisualInfo (Display *dpy, long vinfo_mask, XVisualInfo *vinfo_templ, int *nitems_ret)
{
	if ((*vinfo_templ).screen == xvi->screen)
	 { if (nitems_ret != NULL) *nitems_ret = 1;
	   return xvi; }
	else if ((*vinfo_templ).depth == xvi->depth)
	 { if (nitems_ret != NULL) *nitems_ret = 1;
	   return xvi; }
	else
	 { if (nitems_ret != NULL) *nitems_ret = 0;
	   return NULL; } /* not very pro, but X11 really does that... */
}

VisualID EGLX_XVisualIDFromVisual (Visual *visual)
{
	return xvi->visualid;
}


/*                     GLX                       */
/* --------------------------------------------- */

 /* returns a (predefined) XVisual */
XVisualInfo* EGLX_glXChooseVisual (Display *dpy, int scr, int *attrs)
{
	EGLBoolean result;
	EGLint numconfigs;
	result = eglChooseConfig (edpy, ecfgattrs, &ecfg, 1, &numconfigs);
	assert (result && numconfigs ==1);

	return xvi;
}

 /* creates the GL context */
GLXContext EGLX_glXCreateContext (Display *dpy, XVisualInfo *info, GLXContext list, Bool direct)
{ 
	if (createcontextdone == False)
	{
		ectx = eglCreateContext (edpy, ecfg, EGL_NO_CONTEXT, ectxattrs);
		assert (ectx != NULL);

		createcontextdone = True;
	}

	 /* return the context ID starting from 1 (0 means "failed")  */
	return window_count;
}

 /* creates the GL context with configuration options */
GLXContext EGLX_glXCreateNewContext (Display *dpy, GLXFBConfig config, int render_type, GLXContext list, Bool direct)
{
	if (createcontextdone == False)
	{
		ectx = eglCreateContext (edpy, ecfg, EGL_NO_CONTEXT, ectxattrs);
		assert (ectx != NULL);

		createcontextdone = True;
	}

	 /* return the context ID starting from 1 (0 means "failed")  */
	return window_count;
}

 /* synchronizes the GL context with the display (refresh) */
Bool EGLX_glXMakeCurrent (Display *dpy, GLXDrawable drawable, GLXContext ctx)
{
	 /* rerunning MakeCurrent() multiple times on the same context makes the display hang */
	if (lastupdatedcontext != ctx)
	{
		/* makes sure the surface has been created before, otherwise create it */
		if (mapwindowdone[ctx-1] == False)
			EGLX_XMapWindow (xdpy, xwin[ctx-1]);
		//if (mapwindowdone[drawable-1] == False)
		//	EGLX_XMapWindow (xdpy, xwin[drawable-1]);

		EGLBoolean result;
		result = eglMakeCurrent (edpy, window[ctx-1].esrf, window[ctx-1].esrf, ectx);
		//result = eglMakeCurrent (edpy, window[drawable-1].esrf, window[drawable-1].esrf, ectx);
		assert (result == EGL_TRUE);

		 /* remember it has already been run */
		makecurrentdone[ctx-1] = True;

		 /* remember we did all this */ 
		lastupdatedcontext = ctx;
	}

	 /* remember the current context */
	currentcontext = ctx;

	return True;
}

 /* destroys the GL context */
void EGLX_glXDestroyContext (Display *dpy, GLXContext ctx)
{
	if (window_count <= 1)
	{ 
		EGLBoolean result;
		result = eglDestroyContext (edpy, ectx);
		assert (result == EGL_TRUE);
	}
}

 /* refreshes the GL context */
void EGLX_glXSwapBuffers (Display *dpy, GLXDrawable drawable)
{
	int win = EGLX_glXGetCurrentContext();
	//int win = drawable;
	eglSwapBuffers (edpy, window[win-1].esrf);
}

 /* makes sure all queued XEvents are processed before continuing */
void EGLX_glXWaitX (void)
{
	wl_display_roundtrip (display->display);
}

 /* makes sure all queued GL Events are processed before continuing */
void EGLX_glXWaitGL (void)
{
	eglWaitClient ();
}

 /* retrieves the current configuration */
int EGLX_glXGetConfig (Display *dpy, XVisualInfo *info, int attr, int *value)
{
	if (value != NULL)
	{
		if (attr == GLX_USE_GL)
			*value = 1;
		else if (attr ==  GLX_BUFFER_SIZE)
			*value = 24;
		else if (attr == GLX_LEVEL)
			*value = 0;
		else if (attr == GLX_RGBA)
			*value = 1;
		else 
			*value = 0;
	}

	/* WE SHOULD TAKE VALUES FROM THE ARRAY PASSED TO GLXCHOOSEVISUAL */

	return 0;
}

 /* retrieves the current context */
GLXContext EGLX_glXGetCurrentContext (void)
{
	return currentcontext;
}

 /* returns the fake GLX version info */
Bool EGLX_glXQueryVersion (Display* dpy, int *major, int *minor)
{
	if (major != NULL) *major = 1;
	if (major != NULL) *minor = 2;
	return True;
}

 /* returns if GLX is supported or not */
Bool EGLX_glXQueryExtension (Display *dpy, int *error, int *event)
{
	if (error != NULL) *error = 168;
	if (event != NULL) *event = 104;
	return True;  /* always True of course */
}

 /* get supported extensions ; returns EGL extensions names ; should we wrap that ?*/
const char* EGLX_glXQueryExtensionsString (Display *dpy, int screen)
{
	const char *extensions = NULL;
	 /* when the display hasn't been created yet ? To investigate
	  EGLDisplay edisplay = eglGetDisplay (EGL_DEFAULT_DISPLAY); */
	extensions = eglQueryString (edpy, EGL_EXTENSIONS);

	 /* if returns null, we should at least report Mesa */
	if (extensions == NULL)
	 extensions = "EGL_MESA_screen_surface";

	return extensions;
}

 /* returns a value corresponding to GLX_VENDOR, GLX_VERSION, GLX_EXTENSIONS */
const char* EGLX_glXGetClientString (Display *dpy, int value)
{
	const char *c_string;

	if (value == GLX_VENDOR)
		c_string = "Mesa Project and SGI";
	else if (value == GLX_VERSION)
		c_string = "1.2"; /* return a low version so apps don't try exotic calls */
	else if (value == GLX_EXTENSIONS)
		c_string = "GLX_ARB_create_context GLX_ARB_create_context_profile GLX_ARB_create_context_robustness GLX_ARB_get_proc_address GLX_ARB_multisample";

	return c_string;
}

 /* returns the address of a hardware subroutine ; we don't support this */
void* EGLX_glXGetProcAddressARB(const unsigned char *procname)
{
	procname = NULL;
}

 /* wrapper-specific functions, needed to access the wrapped window position */
 /* needed by GtkGLArea and wxWidgets */
struct wl_surface* EGLX_wl_surface_get (int win)
{
	return window[win-1].surface;
}

struct wl_shell_surface* EGLX_wl_shell_surface_get (int win)
{
	return window[win-1].shell_surface;
}

struct wl_egl_window* EGLX_wl_egl_window_get (int win)
{
	return window[win-1].ewindow;
}


/* ------------------------------ */
/* Let's merge the GTK additions... */

#ifdef WITH_GTK

#include "EGLX-gtkx11.h"
#include "EGLX-gtkx11.c"

#endif
