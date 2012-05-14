#ifndef _X11WRAP_H
#define _X11WRAP_H
#include "common.h"
void G_Init0(int *argc,char **argv[]);
void drawline2fb(Winfo *w,int x0,int y0,int x1,int y1);
void drawcircle2fb(Winfo *w,int x,int y,int r,int fill);
void drawpoly2fb(Winfo *w,ivector2 *poly,int n,int fill);
void setthickness(Winfo *w,int thick);
void waituntilflush();
void setfgcolor(Winfo *w,int palette);
void drawstring2fb(Winfo *w,int x,int y,char *str,int length);
void setsolidfill(Winfo *w);
void setstippledfill(Winfo *w);
void clearfb(Winfo *w);
void exposefb(Winfo *w);
void createfb(Winfo *w);
void createwindow(Ginfo *g,Winfo *w);
void G_loadpalettes(Ginfo *g);
void G_Init2(Ginfo *g);
void W_Init2(Winfo *w,Ginfo *g);
int existevent(Ginfo *g);
void handleevent(Ginfo *g,Winfo w[]);
void overridepalette(Winfo *w,int palette,int red,int green,int blue);
#endif
