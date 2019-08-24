#ifndef _YAPLOT_H
#define _YAPLOT_H
#include "common.h"
/*prototypes for yaplot.c and related sources.*/
int Reality0(NewOinfo *o,Winfo *w);
int Reality1(NewOinfo *o,Winfo *w);
int Reality2(NewOinfo *o,Winfo *w);
int Reality3(NewOinfo *o,Winfo *w);
int Reality4(NewOinfo *o,Winfo *w);
void WindowMotion(Winfo *w,float head,float bank);
void W_Sync(Winfo *to,Winfo *from);
void W_setvectors(Winfo *w);
Winfo *W_Init(int n);
void EyeMotion(Winfo *w,float prop);
#ifndef HAVE_POWIF
float powi(float x,int n);
#endif
int eWiden(Ginfo *g,Winfo w[],int i,int jumpto);
int eGoRelativeFrame(Ginfo *g,Winfo w[],int i,int jumpto,int keepmoving);
int eBanking(Ginfo *g,Winfo w[],int i,int jumpto, int fine);
int eHeading(Ginfo *g,Winfo w[],int i,int jumpto, int fine);
int eToggleSync(Ginfo *g,Winfo w[],int i);
int eRelativeThickness(Ginfo *g,Winfo w[],int i,int jumpto);
int eZoom(Ginfo *g,Winfo w[],int i,int jumpto);
int eStopRotation(Ginfo *g,Winfo w[],int i);
int eStopMotion(Ginfo *g,Winfo w[],int i);
int eQuit(Ginfo *g,Winfo w[],int i);
int eStartRecording(Ginfo *g,Winfo w[],int mode);
int eStopRecording(Ginfo *g,Winfo w[]);
int eToggleVerbosity(Ginfo *g,Winfo w[],int i);
int eGotoFrame(Ginfo *g,Winfo w[],int i,int jumpto);
int eRelativeReality(Ginfo *g,Winfo w[],int i,int jumpto);
int eResetView(Ginfo *g,Winfo w[],int i, float eyex, float eyey, float eyez);
int eToggleLayer(Ginfo *g,Winfo w[],int i,int layer);
int eToggleCentering(Ginfo* g, Winfo w[], int i);
int eToggleSlicing(Ginfo* g, Winfo w[], int i);
int eSliceMove(Ginfo *g,Winfo w[],int i,float delta);
void W_setcenter(Winfo* w);
void W_resetcenter(Winfo* w);
#endif
