#!/bin/sh

gcc -c -fPIC EGLX.c -o EGLX.o -DWITH_GTK `pkg-config --cflags --libs wayland-client wayland-egl egl gtk+-3.0`
gcc -shared -Wl,-soname,libEGLX.so -o libEGLX.so EGLX.o -lc
rm EGLX.o
