#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "EGLX.h"
#include "EGLX-gtkx11.h"


/*                   GDK                         */
/* --------------------------------------------- */

Display* gdk_x11_get_default_xdisplay (void)
{
	Display *dpy = EGLX_XOpenDisplay (NULL);
	return dpy;
}

Display* gdk_x11_display_get_xdisplay (GdkDisplay *d)
{
	Display *dpy = EGLX_XOpenDisplay (NULL);
	return dpy;
}

GdkVisual* gdk_x11_screen_lookup_visual (GdkScreen *screen, VisualID xvid)
{
	if (xvid == xvi->visualid)
		return gdk_screen_get_system_visual (screen);
	else
		return NULL;
}

XID gdk_x11_window_get_xid (GdkWindow *w)
{
	return xwin;
}

Visual* gdk_x11_visual_get_xvisual (GdkVisual *v)
{
	return xvi->visual;
}
