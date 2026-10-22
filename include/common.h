#ifndef _COMMON_H
#define _COMMON_H
#include "cache.h"
#include "varray.h"
#include <stdio.h>
#include <string.h>
#ifndef HELPFILE
#define HELPFILE "/opt/homebrew/share/yaplot/help.yap"
#endif
#ifndef DEFAULTPALETTE
#define DEFAULTPALETTE "/opt/homebrew/share/yaplot/yaplot.col"
#endif
#ifndef VIEWFILE
#define VIEWFILE "./.yapviewstack"
#endif
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <cairo.h>
#ifdef win32
#define M_PI 3.1415926535897932384L
#define rint(x) floor((x) + 0.5)
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
#define VERSION "5.3"

typedef struct _Vertex {
	struct _Vertex *next;
	float x, y, z;	  /*original*/
	float rx, ry, rz; /*relative to center*/
	float zoom;
	int layermask;
	int ix, iy;	  /*coordinate on the screen*/
	float xx, yy, zz; /*after rotation*/
	float eyedist;
} Vertex;

typedef struct {
	int x, y;
} ivector2;

// Primitive info
typedef struct {
	char id;
	char color;
	int layermask;
	int nvertex;
	Vertex **vertex;
	ivector2 *points;
	float r;
	float sortkey;
	char *label;
	// effective to the sticks only.
	int arrowtype;
} Pinfo;

typedef struct {
	float x, y;
} vector2;

typedef struct {
	float x, y, z;
} vector3;

#define REDRAW 1
#define DEFAULTX 512
#define DEFAULTY 512
#define PI 3.1415926535897932
/* 1行の最大長（t コマンドの長い文字列など） */
#define YAP_LINE_MAX 65536
#define BUFFER0 240
#define BUFFER1 15
#define MAXREALITY 4
#define MAXLAYER 30
#define MAXVERBOSE 3
#define HASHSIZE 24000007
#define LASTFRAME 999999
/*入力データの精度。小数点以下をここで打ち切り、一致する点は同一とみなす。*/
#define ACCURACY 1024.0
#define IACC (1.0 / ACCURACY)

/* GTK に依存しない色型（0..65535、既存の << 8 と互換） */
typedef struct {
	unsigned short red, green, blue;
} YapColor;

/*すべての窓に共通の情報*/
typedef struct GINFO {
	char commandname[256];
	int numColors;
	int verbose;

	/*窓の数*/
	int nwindow;
	/*clipping distance*/
	float clip;
	/*色パレット（表示バックエンド非依存）*/
	YapColor mastercolortable[256];
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
typedef struct OINFO {
	varray *prims;	/*Set of primitives*/
	Vertex *vertex; /*Chain of vertice*/
	void **svgs;	/*SVG overlays of this frame (svg_open handles)*/
	int nsvg;
} NewOinfo;

/*アニメーションの情報*/
typedef struct AINFO {
	/*以下は共通*/
	int cachesize;
} Ainfo;

/*ファイル(窓)に固有の情報*/
typedef struct WINFO {
	FILE *inputfile; /*窓ごとに別*/
#ifdef FFMPEG
	// pipe to ffmpeg
	FILE *pffmpeg;
#endif
	varray *framepos; /*窓ごとに別*/
	/*もっとも描画要素が多かったフレームの要素数*/
	int maxprim;

	char inputfilename[256];
	NewOinfo **oo; /*EventLoop内で割り当てられ、開放されない。*/
	/*Cache*/
	Cache *c;
	/*asynchronous mode*/
	int async;
	int status;
	/*実際に表示する要素の個数*/
	int nrealize;
	/*number of non-text primitives to be displayed*/
	int nscalable;
	int screenwidth, screenheight;
	float screendepth;
	int currentframe;
	float depthratio;

	/*Items to be synced with W_Sync*/

	/*new*/
	int df;
	float realcurrent;
	char reality;
	char splat; /* depth-cue levels 0..3 (key d), Reality 2–4 */
	float eyep[3], lookp[3], up[3];
	float e[3], u[3], h[3];
	float fov;
	int thick;
	// for global centering
	// Center of the visual vertices.  It is set when "c" is pressed, and
	// won't be updated
	float centerx, centery, centerz;
	/*表示するレイヤ*/
	int layermask;
	int AbsoluteRotationMode;
	/*accumulated rotation angle (for the next frame)*/
	float accump, accumh, accumb;
	/*rotation speed*/
	float wp, wh, wb;
	/*animating mode*/
	int RecordMode;
	int lastColor;
	/*表示バックエンド用ハンドル（中身はバックエンド実装のみが知る）*/
	void *window, *drawarea;
	cairo_surface_t *surface;
	cairo_t *cr;
	YapColor colortable[256];
	// GdkColormap *colormap;
	/*user clipping vector*/
	int slice;
	float slicev[4]; // [:3] : orientation unit vector; [3]:depth
	/* global ブロック由来のフレーム共通デフォルト */
	float def_radius;
	int def_layer;
	int def_fillcolor;
	int def_linecolor;
	int def_arrowtype;
	/* global ブロック由来の全フレーム共通 SVG オーバーレイ */
	void **svg_global;
	int nsvg_global;
			 /*ここまで*/
} Winfo;

#endif
