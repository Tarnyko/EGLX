#ifndef __EGLX_GTK_H__
#define __EGLX_GTK_H__

#include <gtk/gtk.h>
#include "EGLX.h"


/*                PROTOTYPES					*/
/* -------------------------------------------- */

extern Display* gdk_x11_get_default_xdisplay (void);
extern Display* gdk_x11_display_get_xdisplay (GdkDisplay *d);
extern GdkVisual* gdk_x11_screen_lookup_visual (GdkScreen *screen, VisualID xvid);
extern XID gdk_x11_window_get_xid (GdkWindow *w);
extern Visual* gdk_x11_visual_get_xvisual (GdkVisual *visual);


/*                DEFINITIONS					*/
/* -------------------------------------------- */

#define GDK_VISUAL_XVISUAL	gdk_x11_visual_get_xvisual
#define GDK_DISPLAY_XDISPLAY(D)	gdk_x11_display_get_xdisplay (D)
#define GDK_WINDOW_XDISPLAY(W)	gdk_window_get_display (W)
#define GDK_WINDOW_XID(W)	gdk_x11_window_get_xid (W)

#endif
