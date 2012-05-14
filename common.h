#ifndef _COMMON_H
#define _COMMON_H
#include <stdio.h>
#include <string.h>
#include "varray.h"
#include "cache.h"
#ifndef HELPFILE
#define HELPFILE "/usr/local/share/yaplot/help.yap"
#endif
#ifndef DEFAULTPALETTE
#define DEFAULTPALETTE "/usr/local/share/yaplot/yaplot.col"
#endif
#ifndef VIEWFILE
#define VIEWFILE "./.yapviewstack"
#endif
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef YAPLOT_GTK
#include <gtk/gtk.h>
#else
#include <X11/Xlib.h>
#endif
#ifdef win32
#define FONT "-unknown-*-*-r-normal-*-12-100-*-*-p-*-windows-shiftjis"
#else
#define FONT "-adobe-helvetica-medium-r-normal-*-12-*"
#endif
#ifdef win32
#define M_PI 3.1415926535897932384L
#define rint(x) floor((x)+0.5)
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 0
#endif

typedef struct _Vertex
{
  struct _Vertex *next;
  float x,y,z,zoom;
  int layermask;
  int ix,iy;/*coordinate on the screen*/
  float zz;/*depth after rotation*/
}
Vertex;

typedef struct
{
  short x,y;
}
ivector2;

typedef struct
{
  char id;
  char color;
  int layermask;
  int nvertex;
  Vertex **vertex;
  ivector2 *points;
  float r;
  float sortkey;
  char *label;
}
Pinfo;

typedef struct
{
  float x,y;
}
vector2;

typedef struct
{
  float x,y,z;
}
vector3;


#define REDRAW   1
#define DEFAULTX 512
#define DEFAULTY 512
#define PI 3.1415926535897932
#define BUFFER0 240
#define BUFFER1 15
#define MAXREALITY 4
#define MAXLAYER 12
#define MAXVERBOSE 3
#define HASHSIZE 240007
#define LASTFRAME 999999
/*$BF~NO%G!<%?$N@:EY!#>.?tE@0J2<$r$3$3$GBG$A@Z$j!"0lCW$9$kE@$OF10l$H$_$J$9!#(B*/
#define ACCURACY 1024.0
#define IACC     (1.0/ACCURACY)

/*$B$9$Y$F$NAk$K6&DL$N>pJs(B*/
typedef struct GINFO
{
  char commandname[256];
  int numColors;
  int verbose;
  
  /*$BAk$N?t(B*/
  int nwindow;
  /*clipping distance*/
  float clip;
  /*X11$B$K0MB8$9$kJQ?t(B*/
  /*colormap$B$O(BGinfo$B$G0lC6=i4|2=$7!"3F%&%#%s%I%&MQ$K%3%T!<$7$F;HMQ$9$k!#(B*/
#ifdef YAPLOT_GTK
  GdkColor mastercolortable[256];
#else
  XColor mastercolortable[256];
  Window root;
  int screen;
#endif
  /*GdkColormap *colormap;*/
  /*UNIX$B$K0MB8$9$kJQ?t(B*/
  FILE *palettefile;
} Ginfo;
/*
#define MAXPRIM 30000
typedef struct OINFO
{
    Pinfo *prims[MAXPRIM];
    int nprims;
} Oinfo;
*/
/*typedef varray Oinfo;*/
typedef struct OINFO
{
  varray *prims;/*Set of primitives*/
  Vertex *vertex;/*Chain of vertice*/
}
NewOinfo;

/*$B%"%K%a!<%7%g%s$N>pJs(B*/
typedef struct AINFO
{
  /*$B0J2<$O6&DL(B*/
  int cachesize;
} Ainfo;

/*$B%U%!%$%k(B($BAk(B)$B$K8GM-$N>pJs(B*/
typedef struct WINFO
{
  FILE *inputfile;/*$BAk$4$H$KJL(B*/
  varray *framepos;/*$BAk$4$H$KJL(B*/
  /*$B$b$C$H$bIA2hMWAG$,B?$+$C$?%U%l!<%`$NMWAG?t(B*/
  int maxprim;


  char inputfilename[256];
  NewOinfo **oo;/*EventLoop$BFb$G3d$jEv$F$i$l!"3+J|$5$l$J$$!#(B*/
  /*Cache*/
  Cache *c;
  /*asynchronous mode*/
  int async;
  int status;
  /*$B<B:]$KI=<($9$kMWAG$N8D?t(B*/
  int nrealize;
  int screenwidth,screenheight;
  float screendepth;
  int currentframe;
  float depthratio;
  
  /*Items to be synced with W_Sync*/

  /*new*/
  int df;
  float realcurrent;
  char reality;
  float eyep[3],lookp[3],up[3];
  float e[3],u[3],h[3];
  float fov;
  int thick;
  /*$BI=<($9$k%l%$%d(B*/
  int layermask;
  int AbsoluteRotationMode;
  /*accumulated rotation angle (for the next frame)*/
  float accump,accumh,accumb;
  /*rotation speed*/
  int wp,wh,wb;
  /*animating mode*/
#ifdef RECORD
  int RecordMode;
#endif
  int lastColor;
  /*X11$B$K0MB8$9$kItJ,(B*/
#ifdef YAPLOT_GTK
  GtkWidget *window,*drawarea;
  GdkPixmap *pixmap;
  //for stipples
  GdkPixmap *bitmap[16];
  GdkGC *gc;
  GdkFont *font;
  GdkColor colortable[256];
  GdkColormap *colormap;
#else
  XColor colortable[256];
  Pixmap pixmap;
  Window window;
  GC gc;
  Colormap colormap;
#endif
  /*$B$3$3$^$G(B*/
}
Winfo;



#endif
