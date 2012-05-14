#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
//#ifdef HAVE_LIBGTK
#ifdef YAPLOT_GTK
#include <gtk/gtk.h>
#include "gtk.h"
#else
#include <X11/Xlib.h>
#include "x11wrap.h"
#endif
