#ifndef _GTKWRAP_H
#define _GTKWRAP_H
#include "common.h"
void drawline2fb(Winfo *w,int x0,int y0,int x1,int y1);
void drawcircle2fb(Winfo *w,int x,int y,int r,int fill);
void drawpoly2fb(Winfo *w,ivector2 *poly,int n,int fill);
void drawstick2fb(Winfo* w, int x0, int y0, int x1, int y1, int r, int fill, int arrowtype);
void setlinewidth(Winfo *w,float thick);
void waituntilflush();
void setfgcolor(Winfo *w,int palette);
void settpcolor(Winfo *w,int palette);
void drawstring2fb(Winfo *w,int x,int y,char *str,int length);
void clearfb(Winfo *w);
void exposefb(Winfo *w);
void createfb(Winfo *w);
void createwindow(Ginfo *g,Winfo *w);
void G_loadpalettes(Ginfo *g);
void G_Init2(Ginfo *g);
void W_Init2(Winfo *w,Ginfo *g);
int existevent(Ginfo *g);
void handleevent(Ginfo *g,Winfo w[]);
void G_Init0(int *argc,char **argv[]);
void overridepalette(Winfo *w,int palette,int red,int green,int blue);
#ifdef RECORD
void W_SaveSnapShot(Winfo *w,int i);
void W_StartRecording(Winfo *w);
void W_StopRecording(Winfo *w);
#endif

#endif
