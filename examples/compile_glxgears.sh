#!/bin/bash

gcc glxgears.c jwzgles.c -o glxgears `pkg-config --cflags --libs wayland-client wayland-egl egl glesv1_cm` libEGLX.so -DHAVE_JWZGLES -DGL_VERSION_ES_CM_1_0 -lm 
