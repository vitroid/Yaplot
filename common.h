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
  int x,y;
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
#define MAXLAYER 30
#define MAXVERBOSE 3
#define HASHSIZE 240007
#define LASTFRAME 999999
/*入力データの精度。小数点以下をここで打ち切り、一致する点は同一とみなす。*/
#define ACCURACY 1024.0
#define IACC     (1.0/ACCURACY)

/*すべての窓に共通の情報*/
typedef struct GINFO
{
  char commandname[256];
  int numColors;
  int verbose;
  
  /*窓の数*/
  int nwindow;
  /*clipping distance*/
  float clip;
  /*X11に依存する変数*/
  /*colormapはGinfoで一旦初期化し、各ウィンドウ用にコピーして使用する。*/
#ifdef YAPLOT_GTK
  GdkColor mastercolortable[256];
#else
  XColor mastercolortable[256];
  Window root;
  int screen;
#endif
  /*GdkColormap *colormap;*/
  /*UNIXに依存する変数*/
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

/*アニメーションの情報*/
typedef struct AINFO
{
  /*以下は共通*/
  int cachesize;
} Ainfo;

/*ファイル(窓)に固有の情報*/
typedef struct WINFO
{
  FILE *inputfile;/*窓ごとに別*/
  varray *framepos;/*窓ごとに別*/
  /*もっとも描画要素が多かったフレームの要素数*/
  int maxprim;


  char inputfilename[256];
  NewOinfo **oo;/*EventLoop内で割り当てられ、開放されない。*/
  /*Cache*/
  Cache *c;
  /*asynchronous mode*/
  int async;
  int status;
  /*実際に表示する要素の個数*/
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
  /*表示するレイヤ*/
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
  /*X11に依存する部分*/
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
  /*ここまで*/
}
Winfo;



#endif
