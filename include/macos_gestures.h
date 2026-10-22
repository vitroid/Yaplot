#ifndef MACOS_GESTURES_H
#define MACOS_GESTURES_H

/* Darwin: AppKit local monitor. Non-Apple: no-op stub. */
void yap_macos_gestures_start(void);

/* Implemented in sdl2.c; invoked from the Cocoa monitor on Darwin. */
void yap_on_pinch(float magnification);
void yap_on_rotate(float degrees_clockwise);
void yap_on_scroll(float precise_y);

#endif
