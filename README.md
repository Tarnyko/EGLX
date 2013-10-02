EGLX 0.1
========

	A X11/GLX Wrapper for EGL/Wayland

	By Tarnyko <tarnyko@tarnyko.net>


## Description

 EGLX wraps basic X11/GLX calls, used by popular Unix OpenGL applications, to EGL/Wayland ones.

 Coupled with the [jwzGLES wrapper](http://www.jwz.org/blog/2012/06/i-have-ported-xscreensaver-to-the-iphone/), it permits to run legacy OpenGL applications on a Wayland compositor, eventually using hardware acceleration. 

![EGLX in action](http://www.tarnyko.net/repo/EGLX.png)

## Prerequisites

 Wayland 1.2, Mesa EGL >= 9.0.3, Mesa GLESv1 >= 9.0.3

## Usage

 Create libEGLX.so by running :
./compile_eglx.sh
 Or for some early GTK+3/X11 wrapping, better run :
./compile_eglx-withgtk3.sh

 Put the resulting libEGLX.so and all EGLX*.h files in the same directory as the GLX application you want to wrap.
 Then take this application source code, glxgears.c e.g., and replace GLX headers such as :
```
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <GL/glxext.h>
```
 by :
```
#include "EGLX.h"
```

 Then, in the app "main()" function, add at the beginning :
```
EGLX_main(0);
```

 Then compile your application with this kind of command :
<strong>gcc glxgears.c -o glxgears `pkg-config --cflags --libs wayland-client wayland-egl egl` libEGLX.so</strong>

 If the application uses desktop GL, thus pulling X11 :
```
#include <GL/gl.h>
```
 and you happen to have the jwzGLES wrapper handy, do :
```
#include <GLES/gh.>
#include "jwzgles.h"
```

 And then compile using :
<strong>gcc glxgears.c -o glxgears `pkg-config --cflags --libs wayland-client wayland-egl egl glesv1_cm` libEGLX.so libjwzgles.so -DHAVE_JWZGLES -DGL_VERSION_ES_CM_1_0</strong>

 If the app does some GTK+3 X11 calls (GtkGLArea3 e.g.), don't forget to comment :
```
#include <gdk/gdkx.h>
```
 and to add on your compilation line :
<strong>`pkg-config --cflags --libs gtk+-3.0`</strong>

## Examples

 Most GLX famous examples (GLXDemo, GLXGears, GLXHeads, ManyWin, Offset) work with the wrapper.
 To test with GLXGears, once you've got "libEGLX.so" ; copy it to the "examples" folder and run :
<strong>./compile_glxgears.sh</strong>


## Licensing

 See COPYING ; except for "EGLX-eventdef.h" and "EGLX-keysymdef", which are more and less copies from X11's ones (and therefore, under the terms of its MIT license) ; and "examples/jwzgles*" files which are under the terms of their author.
