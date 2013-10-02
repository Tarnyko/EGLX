#!/bin/sh

gcc -c -fPIC EGLX.c -o EGLX.o `pkg-config --cflags --libs wayland-client wayland-egl egl` 
gcc -shared -Wl,-soname,libEGLX.so -o libEGLX.so EGLX.o -lc
rm EGLX.o
