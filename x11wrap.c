#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <math.h>
#ifdef RECORD
#endif
#include "x11wrap.h"
#include "unixwrap.h"
#include "yaplot.h"

#define stipple_width 16
#define stipple_height 16
static unsigned char stipple_bits[]=
{
  0x88,0x88,
  0x00,0x00,
  0x00,0x00,
  0x00,0x00,
  0x88,0x88,
  0x00,0x00,
  0x00,0x00,
  0x00,0x00,
  0x88,0x88,
  0x00,0x00,
  0x00,0x00,
  0x00,0x00,
  0x88,0x88,
  0x00,0x00,
  0x00,0x00,
  0x00,0x00,
};

extern int debug;
extern float Eyex,Eyey,Eyez;
extern float Lookx,Looky,Lookz;
extern float Upx,Upy,Upz;

/*global*/
extern int jumpto;
int        crawl=0;
int        originx=0,originy=0;
Display*   display;

void drawline2fb( Winfo *w, int x0, int y0, int x1, int y1 )
{
    XDrawLine(display,w->pixmap,w->gc,x0,y0,x1,y1);
}

void drawpoly2fb(Winfo *w,ivector2 *poly,int n,int fill)
{
  if(fill){
    XFillPolygon(display,w->pixmap,w->gc,(XPoint *)poly,n,Complex,CoordModeOrigin);
  }
  else{
    poly[n].x=poly[0].x;
    poly[n].y=poly[0].y;
    XDrawLines(display,w->pixmap,w->gc,(XPoint *)poly,n+1,CoordModeOrigin);
  }
}

void drawcircle2fb(Winfo *w,int x,int y,int r,int fill)
{
  if(fill){
    XFillArc(display,w->pixmap,w->gc,x - r,y - r,r+r,r+r,0,360*64);
  }else{
    XDrawArc(display,w->pixmap,w->gc,x - r,y - r,r+r,r+r,0,360*64);
  }
}

void setthickness(Winfo *w,int thick)
{
  XSetLineAttributes(display,w->gc,thick,LineSolid,CapRound,JoinRound);
}

void waituntilflush(Ginfo *g)
{
  XSync(display,0);
}

void setfgcolor(Winfo *w,int palette)
{
  XSetForeground(display,w->gc,w->colortable[palette].pixel);
}

void overridepalette(Winfo *w,int palette,int red,int green,int blue)
{
  if(w->colortable[palette].pixel){
    XFreeColors(display,w->colormap,&w->colortable[palette].pixel,1,-1);
  }
  w->colortable[palette].red=red<<8;
  w->colortable[palette].green=green<<8;
  w->colortable[palette].blue=blue<<8;
  XAllocColor(display,w->colormap,&w->colortable[palette]);
  if(palette>w->lastColor){
    w->lastColor=palette;
  }
}

void drawstring2fb(Winfo *w,int x,int y,char *str,int length)
{
  XDrawString(display,w->pixmap,w->gc,x,y,str,length);
}

void setsolidfill(Winfo *w)
{
  XSetFillStyle(display,w->gc,FillSolid);
}

void setstippledfill(Winfo *w)
{
  XSetFillStyle(display,w->gc,FillStippled);
}

void clearfb(Winfo *w)
{
  setsolidfill(w);
  setfgcolor(w,1);
  XFillRectangle(display,w->pixmap,w->gc,0,0,w->screenwidth,w->screenheight);
}

void exposefb(Winfo *w)
{
  XSetFillStyle(display,w->gc,FillTiled);
  XSetTile(display,w->gc,w->pixmap);
  XFillRectangle(display,w->window,w->gc,0,0,w->screenwidth,w->screenheight);
  /*make expose event*/
}

/*
void createfb(Winfo *w)
{
  w->pixmap = XCreatePixmap(display,w->window,w->screenwidth,w->screenheight,DefaultDepth(display,g->screen));
}
*/

void W_Init2(Winfo *w,Ginfo *g)
{
  XSetWindowAttributes setattr;
  int border_width;
  unsigned long border,background;
  XEvent ev;
  int i;
    
  border_width = 0;
  border = 0;
  background = 1;
  for(i=0;i<g->nwindow;i++){
    int j;
    w[i].colormap=DefaultColormap(display,g->screen);
    w[i].lastColor=g->numColors-1;
    for(j=0;j<g->numColors;j++){
      w[i].colortable[j].red=g->mastercolortable[j].red;
      w[i].colortable[j].green=g->mastercolortable[j].green;
      w[i].colortable[j].blue=g->mastercolortable[j].blue;
      XAllocColor(display,w[i].colormap,&w[i].colortable[j]);
    }
    w[i].window = XCreateSimpleWindow(display,g->root,0,0,w->screenwidth,w->screenheight,border_width,border,background);
    /* make pixmap */
    w[i].pixmap = XCreatePixmap(display,w[i].window,w->screenwidth,w->screenheight,DefaultDepth(display,g->screen));
    XSelectInput(display,w[i].window,StructureNotifyMask|ExposureMask);
    XMapWindow(display,w[i].window);
  }
  while(1)
    {
      XNextEvent(display,&ev);
      if(ev.type==MapNotify)break;
    }
  for(i=0;i<g->nwindow;i++){
    w[i].gc = XCreateGC(display,w[i].window,0,0);
    XSetLineAttributes(display,w[i].gc,5,LineSolid,CapRound,JoinRound);
    setattr.backing_store=Always;
    XChangeWindowAttributes(display,w[i].window,CWBackingStore,&setattr);
    XSetFont(display,w[i].gc,XLoadFont(display,FONT));
      
    XSelectInput(display,w[i].window,ButtonReleaseMask|ButtonPressMask|Button1MotionMask|KeyPressMask|ExposureMask|StructureNotifyMask/*ResizeRedirectMask*/);
    XSetStipple(display,w[i].gc,XCreateBitmapFromData(display,w[i].window,stipple_bits,stipple_width,stipple_height));
  }
}

void
G_loadpalettes(Ginfo *g)
{
  g->numColors=0;
  while(!feof(g->palettefile))
    {
      int rr,gg,bb;
      char string[4096];
      int cpos;
      
      fgets(string,sizeof(string),g->palettefile);
      cpos=0;
      while((string[cpos]==' ')||(string[cpos]=='\t')){
	cpos++;
      }
      sscanf(string,"%d %d %d",&rr,&gg,&bb);
      g->mastercolortable[g->numColors].red=rr<<8;
      g->mastercolortable[g->numColors].green=gg<<8;
      g->mastercolortable[g->numColors].blue=bb<<8;
      g->numColors++;
    }
  fclose(g->palettefile);
}

void G_Init2(Ginfo *g)
{
  if ((display = (Display *)XOpenDisplay("")) == NULL) {
    fprintf(stderr, "%s: Can't open display\n", g->commandname);
    exit(1);
  }
  g->screen = DefaultScreen(display);
  g->root   = DefaultRootWindow(display);
  /*g->colormap = DefaultColormap(display,g->screen);*/
  G_loadpalettes(g);
}

void G_Init0(int *argc,char **argv[])
{
}


int existevent(Ginfo *g)
{
  return XEventsQueued(display,QueuedAfterFlush);
}

int WhichWindow(XEvent *ev,Ginfo *g,Winfo *w)
{
  int i;
  for(i=0;i<g->nwindow;i++){
    if(w[i].window==((XAnyEvent *)ev)->window){
      return i;
    }
  }
  exit(1);
}



void handleevent(Ginfo *g,Winfo w[])
{
  int i;
  
  XEvent ev;
  XNextEvent(display,&ev);
  i=WhichWindow(&ev,g,w);
  switch(ev.type)
    {
    case Expose:
      {
	w[i].status|=REDRAW;
	break;
      }
    /*case ResizeRequest:*/
    case ConfigureNotify:
      {
	XConfigureEvent *cev;
	cev=(XConfigureEvent *)&ev;
	/*printf("ConfigureNotify\n");*/
	w[i].screenwidth = cev->width;
	w[i].screenheight = cev->height;
	XFreePixmap(display,w[i].pixmap);
	w[i].pixmap = XCreatePixmap(display,w[i].window, w[i].screenwidth,w[i].screenheight,DefaultDepth(display,g->screen));
	w[i].status|=REDRAW;
	/*平成１０年１１月２日(月)現在の方法では、今様の */
	/*ウィンドウマネージャー(real timeで再描画する) */
	/*で不都合が生じる。平成１１年３月８日(月)修正*/
	/*XResizeWindow(display,g->window,g->screenwidth,g->screenheight);*/
	break;
      }
    case MotionNotify:
      {
	if((originx!=ev.xmotion.x)||(originy!=ev.xmotion.y))
	  {
	    float head,bank;
	    bank = +(originy - ev.xmotion.y)*(10.0/400.0);
	    head = +(originx - ev.xmotion.x)*(10.0/400.0);
	    originx = ev.xmotion.x;
	    originy = ev.xmotion.y;
	    if(w[i].async){
	      WindowMotion(&w[i],head,bank);
	    }else{
	      for(i=0;i<g->nwindow;i++){
		WindowMotion(&w[i],head,bank);
	      }
	    }
	  }
      }
    break;
    case ButtonRelease:
      {
	if(w[i].async){
	  w[i].accump=w[i].accumb=w[i].accumh=0;
	  w[i].AbsoluteRotationMode=0;
	  w[i].status |=REDRAW;
	}else{
	  for(i=0;i<g->nwindow;i++){
	    w[i].accump=w[i].accumb=w[i].accumh=0;
	    w[i].AbsoluteRotationMode=0;
	    w[i].status |=REDRAW;
	  }
	}
      }
    break;
    case ButtonPress:
      {
	originx = ev.xmotion.x;
	originy = ev.xmotion.y;
	if(w[i].async){
	  w[i].AbsoluteRotationMode=1;
	}else{
	  for(i=0;i<g->nwindow;i++){
	    w[i].AbsoluteRotationMode=1;
	  }
	}
      }
    break;
    case KeyPress:
      {
	char text[10];
	KeySym key;
	int len = XLookupString((XKeyEvent *)&ev,text,10,&key,0);
	int modstatus = ev.xkey.state;
	if(debug){
	  fprintf(stderr,"Key: %ld/Status: %d\n",key,modstatus);
	}
	/*このあたりの処理はすべてのコマンドに拡張すべき。*/
	if((len==1)&&(text[0]>='0')&&(text[0]<='9')){
	  jumpto = jumpto*10+text[0]-'0';
	  if(g->verbose>=2){
	    for(i=0;i<g->nwindow;i++){
	      w[i].status|=REDRAW;
	    }
	  }
#ifdef RECORD
	}else if(w[i].RecordMode){
	  switch(key){
	  case XK_R:
	  case XK_r:
	    eStopRecording(g,w,i);
	    break;
	  case XK_Q:
	  case XK_q:
	  case XK_Break:
	    eStopRecording(g,w,i);
	    exit(0);
	    /*go absolute*/
	  }
#endif
	}else{
	  int processed;
	  switch(key)
	    {
	      
	      /*仕様を大幅に変更しよう*/
	    case XK_q:
	    case XK_Break:
	      processed=eQuit(g,w,i);
	      break;
	      /*go absolute*/
	    case XK_g:
	    case XK_G:
	    case XK_Return:
	    case XK_KP_Enter:
	      processed=eGotoFrame(g,w,i,(((modstatus&ShiftMask)&&(jumpto==0))?LASTFRAME:jumpto));
	      break;
	      /*go forward*/
	    case XK_Page_Down:
	    case XK_n:
	    case XK_N:
	    case XK_KP_Page_Down:
	      processed=eGoRelativeFrame(g,w,i,(jumpto?+jumpto:+1),(modstatus&ShiftMask));
	      break;
	      /*go backward*/
	      /*case XK_c:
		case XK_C:*/
	      /*crawl=!crawl;*/
	      /*break;*/
#ifdef RECORD
	    case XK_R:
	    case XK_r:
	      processed=eStartRecording(g,w,i);
	      break;
#endif
#ifndef win32
	    case XK_u:
	    case XK_U:
	      processed=ePushView(g,w,i);
	      break;
	    case XK_o:
	    case XK_O:
	      processed=ePopView(g,w,i);
	      break;
#endif
	    case XK_v:
	    case XK_V:
	      processed=eToggleVerbosity(g,w,i);
	      break;
	    case XK_Page_Up:
	    case XK_p:
	    case XK_P:
	    case XK_KP_Page_Up:
	      processed=eGoRelativeFrame(g,w,i,(jumpto?-jumpto:-1),(modstatus&ShiftMask));
	      break;
	      /*first frame*/
	    case XK_Home:
	    case XK_KP_Home:
	      processed=eGotoFrame(g,w,i,0);
	      break;
	      /*last frame*/
	    case XK_End:
	    case XK_KP_End:
	      processed=eGotoFrame(g,w,i,LASTFRAME);
	      break;
	      /*Stop motion*/
	    case XK_space:
	    case XK_KP_Space:
	      processed=eStopMotion(g,w,i);
	      break;
	      /*Stop rotation*/
	    case XK_Pause:
	    case XK_exclam:
	      processed=eStopRotation(g,w,i);
	      break;
	      /*telescopic*/
	    case XK_bracketright:
	      processed=eWiden(g,w,i,(jumpto?-jumpto:-1));
	      break;
	    case XK_asterisk:
	    case XK_KP_Multiply:
	      processed=eZoom(g,w,i,(jumpto?+jumpto:+1));
	      break;
	    /*wide*/
	    case XK_bracketleft:
	      processed=eWiden(g,w,i,(jumpto?+jumpto:+1));
	      break;
	    case XK_slash:
	    case XK_KP_Divide:
	      processed=eZoom(g,w,i,(jumpto?-jumpto:-1));
	      break;
	    /*heading rotation*/
	    case XK_h:
	    case XK_H:
	    case XK_Left:
	    case XK_KP_Left:
	      processed=eHeading(g,w,i,(jumpto?+jumpto:+1));
	      break;
	      /*heading rotation*/
	    case XK_L:
	    case XK_l:
	    case XK_Right:
	    case XK_KP_Right:
	      processed=eHeading(g,w,i,(jumpto?-jumpto:-1));
	      break;
	      /*banking rotation*/
	    case XK_K:
	    case XK_k:
	    case XK_Up:
	    case XK_KP_Up:
	      processed=eBanking(g,w,i,(jumpto?+jumpto:+1));
	      break;
	      /*banking rotation*/
	    case XK_J:
	    case XK_j:
	    case XK_Down:
	    case XK_KP_Down:
	      processed=eBanking(g,w,i,(jumpto?-jumpto:-1));
	      break;
	/*flush jumpto*/
	    case XK_Escape:
	      processed=1;
	      break;
	    case XK_s:
	    case XK_S:
	      processed=eToggleSync(g,w,i);
	      break;
	    case XK_f:
	    case XK_F:
	      processed=eToggleLayer(g,w,i,(jumpto?jumpto:1));
	      break;
	    case XK_Insert:
	    case XK_KP_Insert:
	    case XK_parenright:
	      processed=eRelativeThickness(g,w,i,(jumpto?+jumpto:+1));
	      break;
	    case XK_Delete:
	    case XK_KP_Delete:
	    case XK_parenleft:
	      processed=eRelativeThickness(g,w,i,(jumpto?-jumpto:-1));
	      break;
	      /*better quality*/
	    case XK_plus:
	    case XK_KP_Add:
	      processed=eRelativeReality(g,w,i,+1);
	      break;
	      /*less quality*/
	    case XK_minus:
	    case XK_KP_Subtract:
	      processed=eRelativeReality(g,w,i,-1);
	      break;
	    case XK_Tab:
	    case XK_KP_Tab:
	      processed=eResetView(g,w,i);
	      break;
	    case XK_F1:
	      processed=eToggleLayer(g,w,i,1);
	      break;
	    case XK_F2:
	      processed=eToggleLayer(g,w,i,2);
	      break;
	    case XK_F3:
	      processed=eToggleLayer(g,w,i,3);
	      break;
	    case XK_F4:
	      processed=eToggleLayer(g,w,i,4);
	      break;
	    case XK_F5:
	      processed=eToggleLayer(g,w,i,5);
	      break;
	    case XK_F6:
	      processed=eToggleLayer(g,w,i,6);
	      break;
	    case XK_F7:
	      processed=eToggleLayer(g,w,i,7);
	      break;
	    case XK_F8:
	      processed=eToggleLayer(g,w,i,8);
	      break;
	    case XK_F9:
	      processed=eToggleLayer(g,w,i,9);
	      break;
	    case XK_F10:
	      processed=eToggleLayer(g,w,i,10);
	      break;
	    case XK_F11:
	      processed=eToggleLayer(g,w,i,11);
	      break;
	    case XK_F12:
	      processed=eToggleLayer(g,w,i,12);
	      break;
	    default:
	      processed=0;
	      break;
	    }
	  if(processed){
	    jumpto=0;
	  }
	}
      }
    break;
    default:
      break;
    }
}
