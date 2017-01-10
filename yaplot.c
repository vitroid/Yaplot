/*
 * Author:      Doritos Matto (matto@aqua.chem.nagoya-u.ac.jp)
 *
 * Copyright (C) 1992-1995, Masakazu Matsumoto.
 *
 * Permission limited to the use, copy, display, distribute without
 * charging for a fee, and produce derivative works of "yaplot" and
 * its documentation for not-for-profit purpose is hereby granted by
 * the Author, provided that the above copyright notice appears in
 * all copies made of "yaplot" and that both the copyright notice
 * and this permission notice appear in supporting documentation,
 * and that the name of the Author not be used in advertising or
 * publicity pertaining to distribution of the software without
 * specific, written prior permission.  The Author makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied
 * warranty.  All other rights (including, but not limited to, the
 * right to sell "yaplot", the right to sell derivative works of
 * "yaplot", and the right to distribute "yaplot" for a fee) are
 * reserved by the Author.
 *
 * THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
/* mol2 + pseudo double buffer = mol3 */
/*$Id: yaplot.c,v 1.10 2004-05-17 06:56:28 matto Exp $
  $Log: yaplot.c,v $
  Revision 1.10  2004-05-17 06:56:28  matto
  glut.c is added, but OpenGL version should be developed in separated branch.

  Revision 1.9  2003/04/25 02:25:33  matto
  *** empty log message ***

  Revision 1.8  2002/01/22 06:48:03  matto
  automakeに対応。

  Revision 1.7  2001/07/23 09:35:51  matto
  Autoconf-compatible.
  GTK and no-GTK version can be used.

  Revision 1.6  2001/07/18 03:40:56  matto
  Compiled on windows, too.

  Revision 1.5  2001/07/16 08:09:16  matto
  OS- and GUI- dependent codes are separated.

  Revision 1.4  2001/07/16 02:22:42  matto
  Redrawing action is refined.
  
  Revision 1.3  2001/06/16 07:50:25  matto
  Jerky motion is refined.
  
  Revision 1.2  1999/08/25 03:40:59  matto
  This is the real first edition of Yaplot 2.
  
  Revision 1.22  1999/08/24 10:09:13  matto
  Crawling mode is disabled.
  
  Revision 1.21  1999/08/24 08:34:57  matto
  re-indented.
  
  Revision 1.20  1999/08/24 08:25:41  matto
  Hash is renewed.
  Accuracy is introduced.
  
  Revision 1.19  1999/08/24 03:26:56  matto
  Zoom ratio can be specified independently for each window.
  
  Revision 1.18  1999/08/24 03:01:22  matto
  Many actions can be achieved asynchronously with async mode.
  
  Revision 1.17  1999/08/24 01:00:47  matto
  CVS test from flash memory card.
  
  Revision 1.15  1999/08/20 10:02:25  matto
  Winfo is introduced. Now it is ready to hold multiwindows.
  
  Revision 1.14  1999/08/20 03:40:03  matto
  yaplot.c is divided.
  
  Revision 1.13  1999/08/20 02:04:18  matto
  * IntHash is added for eliminating degenerate vertices.
  * Structure Vertex has chain property, coordinate on the screen, and
    depth information.
  * IREALITY definition is removed.
  * double variables are changed to float.
  * Pinfo does not keep its vertice but pointer to vertice.
  * Oinfo has a pointer to vertice chain.
  * vertice are rotated independently from primitives.
  * Interruption handler is added before and after quick sorting.
  * verbose information is increased.
  * variable g->accuracy is introduced but not implemented.
  * "[" and "]" are allocated for fov control.
  
  Revision 1.12  1999/08/19 12:47:11  matto
  *** empty log message ***
  
  Revision 1.11  1999/08/19 09:02:51  matto
  Verboseness can be changed by "v" key.
  Layer is introduced. Default layer is 1st, and you can change layer by
  "y" command in the input file.
  Each layer can be displayed or hided by function keys.
  
  Revision 1.10  1999/08/19 07:45:52  matto
  It became that each vertex has no information about its radius
  but has its zoom factor instead.
  This modification is required for united vertex list.
  
  Revision 1.9  1999/08/19 07:11:11  matto
  Some trivial changes.
  
  Revision 1.8  1999/08/05 04:08:53  matto
  Now you chan change fov without changing the image size w/ S-* and S-/.
  
  Revision 1.7  1999/08/05 04:04:20  matto
  *** empty log message ***
  
 *Revision 1.6  1999/03/08 04:34:33  matto
 *quick sort is replaced and became faster.
 *event management becomes a little bit smarter.
 *
 *Revision 1.5  1998/10/15 09:36:47  matto
 *Cache is added.
 *
 *Revision 1.4  1998/10/15 07:37:12  matto
 *variable length array is introduced for primitives.
 *
 *Revision 1.3  1998/10/15 04:02:34  matto
 *Pinfo is introduced in place of Line
 *
 *Revision 1.2  1998/10/15 03:45:44  matto
 *frame[] is made into variable length array.
 *Ainfo is introduced.
 *
 *Revision 1.1.1.1  1998/09/28 04:40:46  matto
 *First release of Yaplot
 *
 *Revision 1.8  1998/03/02 02:58:23  matto
 *Polygon mark bug was fixed.
 *default palette name is changed to yaplot.col
 *Envvar is also changed.
 *Install path is defined in Imakefile
 *Line thickness control is added.
 *Palette handling in Redraw() is simplified.
 *
 *Revision 1.7  1998/03/01 23:39:47  matto
 *Window size can be resized into larger now.
 *
 *Revision 1.6  1998/02/27 02:04:23  matto
 *Newframe is appropriately reloaded.
 *
 *Revision 1.5  1998/02/27 01:18:06  matto
 *main() is divided into subroutines.
 *Key assignment is sophisticated.
 *
 *Revision 1.4  1998/02/26 10:08:52  matto
 *Key symbol is abstracted.
 *Arrow keys and paging keys are now available.
 *
 *Revision 1.3  1998/02/26 07:57:13  matto
 *Line width is controlled automatically.
 *Help is added.
 *
 *Resize does not work well when the window is stretched.
 *
 *Revision 1.2  1998/02/26 03:11:06  matto
 *tolower is removed. Capital is reserved for future extensions.
 *
 *Revision 1.1  1998/02/26 03:08:49  matto
 *Initial revision
 *
 *Revision 1.6  1997/12/24 01:35:12  matto
 *Thickness of the lines varies linearly.
 *Seek procedure is made smart.
 *
 *Revision 1.5  1997/12/18 09:57:24  matto
 *Spaces at the head of lines becomes to be skipped.
 *
 *Revision 1.4  1997/12/18 09:50:42  matto
 *Nearer objects are drawn by thick line.
 *It is a good idea!
 *
 *Revision 1.3  1997/06/03 11:35:28  matto
 *palette file mol3.col is flexibly searched.
 *
 *Revision 1.2  1997/06/03 11:15:18  matto
 *Line width is changed for objects' depth.
 *It REALLY works well.
 *
 *Revision 1.1  1997/06/03 19:51:36  matto
 *Initial revision
 *
 * 
 * fix resize */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "os.h"
#include "gui.h"
#include "varray.h"
#include "cache.h"
#include "Hash.h"
#include "yaplot.h"
#include "common.h"
vector2 polygon[6][4];
float ddelta;
/*struct itimerval deb0,deb1;*/
/*global (initial camera position*/
int debug=0;
float Eyex=0,Eyey=-40,Eyez=0;
float Lookx=0,Looky=0,Lookz=0;
float Upx=0,Upy=0,Upz=1;
int jumpto=0;

#define DBGTMRSTART if(debug) getitimer(ITIMER_REAL,&deb1);
#define DBGTMRSTOP  if(debug){\
	    getitimer(ITIMER_REAL,&deb0);\
	    ddelta = deb1.it_value.tv_usec - deb0.it_value.tv_usec;\
	    ddelta = ddelta/1000000.0 + deb1.it_value.tv_sec - deb0.it_value.tv_sec;\
	    fprintf(stderr,"%f DBGTMR\n",ddelta);\
	  }


#ifdef HAVE_POWIF
#else
float powif(float x,int n)
{
  float ans=1;
  int mask=1;
  if(n<0){
    x=1.0/x;
    n=-n;
  }
  while(mask<=n){
    if(mask&n){
      ans*=x;
    }
    x*=x;
    mask<<=1;
  }
  return ans;
}
#endif

int eWiden(Ginfo *g,Winfo w[],int i,int jumpto)
{
    float ratio=powif(1.05,jumpto);
    if(w[i].async){
        w[i].fov*=ratio;
        EyeMotion(&w[i],1.0-ratio);
        w[i].status|=REDRAW;
		if (debug) fprintf(stderr,"REDRAW because the perspective is changed (1)[Window %d]\n",i);
    }else{
        for(i=0;i<g->nwindow;i++){
            w[i].fov*=ratio;
            EyeMotion(&w[i],1.0-ratio);
            w[i].status|=REDRAW;
    		if (debug) fprintf(stderr,"REDRAW because the perspective is changed (2)[Window %d]\n",i);
        }
    }
    return TRUE;
}


int eGoRelativeFrame(Ginfo *g,Winfo w[],int i,int jumpto,int keepmoving)
{
    if(w[i].async){
        if(keepmoving)
            w[i].df=+jumpto;
        else{
            w[i].realcurrent+=jumpto;
            w[i].df=0;
        }
    }else{
        for(i=0;i<g->nwindow;i++){
            if(keepmoving){
                w[i].df=+jumpto;
            }else{
                w[i].realcurrent+=jumpto;
                w[i].df=0;
            }
        }
    }
    return TRUE;
}

int eBanking(Ginfo *g,Winfo w[],int i,int jumpto)
{
    /*if(crawl){*/
    /*float deltah;
      float dx,dy,dz,dd;
      dx=g->lookp[0]-g->eyep[0];
      dy=g->lookp[1]-g->eyep[1];
      dz=g->lookp[2]-g->eyep[2];
      dd=sqrt((dx*dx+dy*dy+dz*dz)/(1.0004));
      deltah=+0.02;
      g->e[0] += g->u[0]*deltah;
      g->e[1] += g->u[1]*deltah;
      g->e[2] += g->u[2]*deltah;
      g->lookp[0]=g->eyep[0]+g->e[0]*dd;
      g->lookp[1]=g->eyep[1]+g->e[1]*dd;
      g->lookp[2]=g->eyep[2]+g->e[2]*dd;*/
    /*}else{*/
    if(w[i].async){
        w[i].wb +=jumpto;
        w[i].status|=REDRAW;
		if (debug) fprintf(stderr,"REDRAW because of banking (1)[Window %d]\n",i);
    }else{
        for(i=0;i<g->nwindow;i++){
            w[i].wb +=jumpto;
            w[i].status|=REDRAW;
    		if (debug) fprintf(stderr,"REDRAW because of banking (2)[Window %d]\n",i);
        }
    }
    return TRUE;
}

int eHeading(Ginfo *g,Winfo w[],int i,int jumpto)
{
    /*if(crawl){*/
    /*float deltab;
      float dx,dy,dz,dd;
      dx=g->lookp[0]-g->eyep[0];
      dy=g->lookp[1]-g->eyep[1];
      dz=g->lookp[2]-g->eyep[2];
      dd=sqrt((dx*dx+dy*dy+dz*dz)/(1.0004));
      deltab=+0.02;
      g->e[0] += g->h[0]*deltab;
      g->e[1] += g->h[1]*deltab;
      g->e[2] += g->h[2]*deltab;
      g->lookp[0]=g->eyep[0]+g->e[0]*dd;
      g->lookp[1]=g->eyep[1]+g->e[1]*dd;
      g->lookp[2]=g->eyep[2]+g->e[2]*dd;*/
  /*}else{*/
    if(w[i].async){
        w[i].wh +=jumpto;
        w[i].status|=REDRAW;
		if (debug) fprintf(stderr,"REDRAW because of heading (1)[Window %d]\n",i);
    }else{
        for(i=0;i<g->nwindow;i++){
            w[i].wh +=jumpto;
            w[i].status|=REDRAW;
    		if (debug) fprintf(stderr,"REDRAW because of heading (2)[Window %d]\n",i);
        }
    }
    return TRUE;
}

int eToggleSync(Ginfo *g,Winfo w[],int i)
{
    w[i].async=!w[i].async;
    if(!w[i].async){
        int j=0;
        while(w[j].async){
            j++;
            if(j==g->nwindow)
                break;
        }
        if(j!=g->nwindow){
            W_Sync(&w[i],&w[j]);
        }
    }
    w[i].status|=REDRAW;
    if (debug) fprintf(stderr,"REDRAW because sync mode is changed[Window %d]\n",i);
    return TRUE;
}

int eRelativeThickness(Ginfo *g,Winfo w[],int i,int jumpto)
{
    if(w[i].async){
        w[i].thick+=jumpto;
        if(w[i].thick<0)
            w[i].thick=0;
        w[i].status|=REDRAW;
		if (debug) fprintf(stderr,"REDRAW because line thickness is changed (1)[Window %d]\n",i);
    }else{
        for(i=0;i<g->nwindow;i++){
            w[i].thick+=jumpto;
            if(w[i].thick<0)
                w[i].thick=0;
            w[i].status|=REDRAW;
    		if (debug) fprintf(stderr,"REDRAW because line thickness is changed (2)[Window %d]\n",i);
        }
    }
    return TRUE;
}

int eZoom(Ginfo *g,Winfo w[],int i,int jumpto)
{
    float ratio=powif(1.05,jumpto);
    w[i].depthratio*=ratio;
    w[i].status|=REDRAW;
    if (debug) fprintf(stderr,"REDRAW because zoom is changed [Window %d]\n",i);
    return TRUE;
}

int eStopRotation(Ginfo *g,Winfo w[],int i)
{
    if(w[i].async){
        w[i].wh=w[i].wp=w[i].wb=0;
        w[i].status|=REDRAW;
        if (debug) fprintf(stderr,"REDRAW because rotation is stopped (1)[Window %d]\n",i);
    }else{
        for(i=0;i<g->nwindow;i++){
            w[i].wh=w[i].wp=w[i].wb=0;
            w[i].status|=REDRAW;
            if (debug) fprintf(stderr,"REDRAW because rotation is stopped (2)[Window %d]\n",i);
        }
    }
    return TRUE;
}

int eStopMotion(Ginfo *g,Winfo w[],int i)
{
    if(w[i].async){
        w[i].df=w[i].wh=w[i].wp=w[i].wb=0;
        w[i].status|=REDRAW;
        if (debug) fprintf(stderr,"REDRAW because frame motion is stopped (1)[Window %d]\n",i);
    }else{
        for(i=0;i<g->nwindow;i++){
            w[i].df=w[i].wh=w[i].wp=w[i].wb=0;
            w[i].status|=REDRAW;
            if (debug) fprintf(stderr,"REDRAW because frame motion is stopped (2)[Window %d]\n",i);
        }
    }
    return TRUE;
}

int eQuit(Ginfo *g,Winfo w[],int i)
{
    exit(0);
}

int eStartRecording(Ginfo *g,Winfo w[],int i)
{
    /*全部同時に録画する。*/
    for(i=0;i<g->nwindow;i++){
        W_StartRecording(&w[i]);
        w[i].status|=REDRAW;
        if (debug) fprintf(stderr,"REDRAW because recording is started [Window %d]\n",i);
    }
    return TRUE;
}

int eStopRecording(Ginfo *g,Winfo w[],int i)
{
    /*全部同時に録画する。*/
    for(i=0;i<g->nwindow;i++){
        W_StopRecording(&w[i]);
        w[i].status|=REDRAW;
        if (debug) fprintf(stderr,"REDRAW because recording is stopped [Window %d]\n",i);
    }
    return TRUE;
}


int eToggleCentering(Ginfo* g, Winfo w[], int i)
{
  if ( (w[i].centerx == 0) && (w[i].centery == 0) && (w[i].centerz == 0) ){
    W_setcenter(&w[i]);
  }
  else {
    W_resetcenter(&w[i]);
  }
  return TRUE;
}


int eToggleVerbosity(Ginfo *g,Winfo w[],int i)
{
    g->verbose--;
    if(g->verbose<0)
        g->verbose=MAXVERBOSE;
    for(i=0;i<g->nwindow;i++){
        w[i].status|=REDRAW;
        if (debug) fprintf(stderr,"REDRAW because verbosity is changed [Window %d]\n",i);
    }
    return TRUE;
}

int eGotoFrame(Ginfo *g,Winfo w[],int i,int jumpto)
{
    /*jump to the last frame if only one G is pressed.*/
    if(w[i].async){
        w[i].realcurrent=jumpto;
        w[i].df=0;
    }else{
        for(i=0;i<g->nwindow;i++){
            w[i].realcurrent=jumpto;
            w[i].df=0;
        }
    }
    return TRUE;
}

int eRelativeReality(Ginfo *g,Winfo w[],int i,int jumpto)
{
    if(w[i].async){
        w[i].reality+=jumpto;
        if(w[i].reality>MAXREALITY)
            w[i].reality=MAXREALITY;
        else if (w[i].reality<0)
            w[i].reality=0;
        if(debug)
            fprintf(stderr,"Reality(%d) : %d\n",
                    i,w[i].reality);
        w[i].status|=REDRAW;
        if (debug) fprintf(stderr,"REDRAW because reality is changed (1)[Window %d]\n",i);
    }else{
        for(i=0;i<g->nwindow;i++){
            w[i].reality+=jumpto;
            if(w[i].reality>MAXREALITY)
                w[i].reality=MAXREALITY;
            else if (w[i].reality<0)
                w[i].reality=0;
            if(debug)
                fprintf(stderr,"Reality(%d) : %d\n",
                        i,w[i].reality);
            w[i].status|=REDRAW;
            if (debug) fprintf(stderr,"REDRAW because reality is changed (2)[Window %d]\n",i);
        }
    }
    return TRUE;
}



int eResetViewOne( Winfo* w )
{
    float dx,dy,dz,ox,oy,oz,dd;
    w->wp = w->wh = w->wb = 0;
    w->accump = w->accumh = w->accumb = 0;
    dx = w->eyep[0]-w->lookp[0];
    dy = w->eyep[1]-w->lookp[1];
    dz = w->eyep[2]-w->lookp[2];
    ox = Eyex-Lookx;
    oy = Eyey-Looky;
    oz = Eyez-Lookz;
    dd = sqrt((dx*dx+dy*dy+dz*dz)/(ox*ox+oy*oy+oz*oz));
    w->lookp[0] = Lookx;
    w->lookp[1] = Looky;
    w->lookp[2] = Lookz;
    w->eyep[0] = Lookx+ox*dd;
    w->eyep[1] = Looky+oy*dd;
    w->eyep[2] = Lookz+oz*dd;
    w->up[0] = Upx;
    w->up[1] = Upy;
    w->up[2] = Upz;
    W_setvectors( w );

    w->status |= REDRAW;
    if (debug) fprintf(stderr,"REDRAW because the view is reset");

    return TRUE;
}



int eResetView( Ginfo *g, Winfo w[], int i )
{
    if(w[i].async){
        eResetViewOne( &w[i] );
    }
    else{
        for(i=0;i<g->nwindow;i++)
            eResetViewOne( &w[i] );
    }
    return TRUE;
}



int eToggleLayerOne( Winfo* w, int layermask )
{
    w->layermask ^= layermask;
    w->status    |= REDRAW;
    if (debug) fprintf(stderr,"REDRAW because a layer is toggled");
    return TRUE;
}



int eToggleLayer( Ginfo *g, Winfo w[], int i, int layer )
{
    int layermask;
    if((layer<1)||(layer>MAXLAYER)){
        return TRUE;
    }
    layermask=1<<(layer-1);

    if(w[i].async){
        eToggleLayerOne( &w[i], layermask );
    }
    else{
        for(i=0;i<g->nwindow;i++)
            eToggleLayerOne( &w[i], layermask );
    }
    return TRUE;
}

void
W_Sync(Winfo *to,Winfo *from)
{
  to->df         = from->df;
  to->realcurrent= from->realcurrent;
  to->reality    = from->reality;
  to->eyep[0]    = from->eyep[0];
  to->eyep[1]    = from->eyep[1];
  to->eyep[2]    = from->eyep[2];
  to->lookp[0]   = from->lookp[0];
  to->lookp[1]   = from->lookp[1];
  to->lookp[2]   = from->lookp[2];
  to->up[0]      = from->up[0];
  to->up[1]      = from->up[1];
  to->up[2]      = from->up[2];
  to->e[0]       = from->e[0];
  to->e[1]       = from->e[1];
  to->e[2]       = from->e[2];
  to->u[0]       = from->u[0];
  to->u[1]       = from->u[1];
  to->u[2]       = from->u[2];
  to->h[0]       = from->h[0];
  to->h[1]       = from->h[1];
  to->h[2]       = from->h[2];
  to->fov        = from->fov;
  to->thick      = from->thick;
  to->layermask  = from->layermask;
  to->AbsoluteRotationMode=from->AbsoluteRotationMode;
  to->accump     = from->accump;
  to->accumh     = from->accumh;
  to->accumb     = from->accumb;
  to->wp         = from->wp;
  to->wh         = from->wh;
  to->wb         = from->wb;
}

void WindowMotion(Winfo *w,float head,float bank)
{
  float dx,dy,dz;
  float eyex,eyey,eyez,eyed;
  float deltah,deltab;
  w->wp=w->wh=w->wb=0;
  w->status|=REDRAW;
  if (debug) fprintf(stderr,"REDRAW because window is modified");
  /*rotate eyep around lookp.*/
  dx = w->lookp[0]-w->eyep[0];
  dy = w->lookp[1]-w->eyep[1];
  dz = w->lookp[2]-w->eyep[2];
  deltah=-sin(head);
  deltab=sin(bank);
  eyex = w->e[0] + w->h[0]*deltah + w->u[0]*deltab;
  eyey = w->e[1] + w->h[1]*deltah + w->u[1]*deltab;
  eyez = w->e[2] + w->h[2]*deltah + w->u[2]*deltab;
  eyed = sqrt((dx*dx+dy*dy+dz*dz)/(eyex*eyex+eyey*eyey+eyez*eyez));
  eyex*=eyed;
  eyey*=eyed;
  eyez*=eyed;
  w->eyep[0]=w->lookp[0]-eyex;
  w->eyep[1]=w->lookp[1]-eyey;
  w->eyep[2]=w->lookp[2]-eyez;
  w->accump=w->accumh=w->accumb=0;
}


void
W_setvectors(Winfo *w)
{
  float dd,eyex,eyey,eyez,eyed;
    
  eyex = w->lookp[0]-w->eyep[0];
  eyey = w->lookp[1]-w->eyep[1];
  eyez = w->lookp[2]-w->eyep[2];
  eyed = 1.0L/sqrt(eyex*eyex+eyey*eyey+eyez*eyez);
    
  w->e[0]=eyex*eyed;/*視線単位ベクトル*/
  w->e[1]=eyey*eyed;
  w->e[2]=eyez*eyed;
  w->h[0]=(w->e[1]*w->up[2]-w->up[1]*w->e[2]);
  w->h[1]=(w->e[2]*w->up[0]-w->up[2]*w->e[0]);
  w->h[2]=(w->e[0]*w->up[1]-w->up[0]*w->e[1]);
  dd = 1.0/sqrt(w->h[0]*w->h[0]+w->h[1]*w->h[1]+w->h[2]*w->h[2]);
  w->h[0]*=dd;/*水平単位ベクトル*/
  w->h[1]*=dd;
  w->h[2]*=dd;
  w->u[0]=-(w->e[1]*w->h[2]-w->h[1]*w->e[2]);
  w->u[1]=-(w->e[2]*w->h[0]-w->h[2]*w->e[0]);
  w->u[2]=-(w->e[0]*w->h[1]-w->h[0]*w->e[1]);/*上方単位ベクトル*/
  /*added*/
  w->up[0]=w->u[0];
  w->up[1]=w->u[1];
  w->up[2]=w->u[2];
  /*
    printf("eyep  %f %f %f\n",eyep[0],eyep[1],eyep[2]);
    printf("lookp %f %f %f\n",lookp[0],lookp[1],lookp[2]);
    printf("up    %f %f %f\n",up[0],up[1],up[2]);
    */
}

char *
skipspaces(char *s)
{
  while(*s==' ')
    s++;
  return s;
}


void W_Seek(Winfo *w)
{
  long int *pos;
  char readstr[4096],*string;
    
  if(w->currentframe < 0)
    {
      w->currentframe=0;
      w->realcurrent=0;
      w->df=0;
    }
  pos = varray_ptr(w->framepos,w->currentframe);
  if(*pos>=0)
    {
      /*すでに一度表示したフレームは場所がわかっている*/
      fseek(w->inputfile,*pos,SEEK_SET);
      if(debug)
	fprintf(stderr,"SEEK 1\n");
    }
  else
    {
      int i;
      /*new frame*/
      /*フレーム検索*/
      /*ファイルの末端は？*/
      /*すでに読みこまれた最後のフレームの位置をさがしiに入れる*/
      for(i=w->currentframe;pos=(long int *)varray_ptr(w->framepos,i),*pos<0;i--);
      /*seekする*/
      pos = varray_ptr(w->framepos,i);
      fseek(w->inputfile,*pos,SEEK_SET);
      if(debug)
	fprintf(stderr,"SEEK 2\n");
      while( i != w->currentframe )
	{
	  do
	    {
	      /*もしファイルの末尾まできたら*/
	      if(fgets(readstr,4096,w->inputfile)==NULL)
		{
		  /*あきらめて、i番目のフレームを表示*/
		  w->currentframe=i;
		  w->realcurrent=i;
		  w->df=0;
		  if(w->RecordMode){
		    W_StopRecording(w);
		  }
		  pos = varray_ptr(w->framepos,i);
		  fseek(w->inputfile,*pos,SEEK_SET);
		  if(debug)
		    fprintf(stderr,"SEEK 3\n");
		  /*ファイルの末尾*/
		  return;
		}
	      string=skipspaces(readstr);
	    }
	  while (*string!='\n'); /*空行になるまくりかえす。*/
	  i++;
	  pos = varray_ptr(w->framepos,i);
	  *pos=ftell(w->inputfile);
	}
    }
  /*目的不明なので消した。平成９年１２月２４日(水)
    if(o->frame[o->nextframe]<0)
    return;
    */
}

void P_FreeFields(Pinfo *p)
{
  free(p->label);
  free(p->vertex);
  free(p->points);
}

void O_Done(void *oo)
{
  int i;
  Pinfo **p;
  Vertex *v,*next;
  NewOinfo *o=(NewOinfo *)oo;
  
  if(o==NULL)return;
  v = o->vertex;
  while(v!=NULL){
    next=v->next;
    free(v);
    v=next;
  }
  for(i=0,p=o->prims->a;*p!=NULL;i++,p++)
    {
      P_FreeFields(*p);
      free(*p);
    }
  free(o->prims->a);
  free(o);
}

/*Hashを使うより、voxel分割した方が効率がいい？否。voxel内で結局検索が
  必要になる。その上、voxelはせいぜい64*64*64分割ぐらいしか可能でない。
  (それで十分かも)*/
Vertex *Vertex_Insert(sHash *h,NewOinfo *o,float x,float y,float z,int mask)
{
  int ix,iy,iz;
  Vertex *v,*newv;
  unsigned int key;
  /*
    ix=(int *)&x;
    iy=(int *)&y;
    iz=(int *)&z;
    key = *ix;
    key<<=5;
    key ^= *iy;
    key<<=7;
    key ^= *iz;
    */
  /*もっとよいencodingがあるはず*/
  newv=(Vertex *)malloc(sizeof(Vertex));
  x=rint(x*ACCURACY);
  y=rint(y*ACCURACY);
  z=rint(z*ACCURACY);
  newv->x=x*IACC;
  newv->y=y*IACC;
  newv->z=z*IACC;
  ix=(int)x;
  iy=(int)y;
  iz=(int)z;
  key = ix;
  key<<=10;
  key ^= iy;
  key<<=10;
  key ^= iz;
  key %= h->hashsize;
  key=Hash_QueryElement(h,key,newv);
  v=(Vertex *)h->value[key];
  if(v!=NULL){
    v->layermask|=mask;
    if(debug){
      fputc('*',stderr);
    }
    free(newv);
  }else{
    newv->layermask=mask;
    newv->next=o->vertex;
    o->vertex=newv;
    Hash_RegisterValue(h,key,newv);
    v=newv;
  }
  return v;
}

int hash_compare(const void *a,const void *b)
{
  Vertex *va=(Vertex *)a;
  Vertex *vb=(Vertex *)b;
  /*if(debug)
    fprintf(stderr,"%f <-x-> %f\n",va->x,vb->x);*/
  if(va->x!=vb->x)
    return 1;
  /*if(debug)
    fprintf(stderr,"%f <-y-> %f\n",va->y,vb->y);*/
  if(va->y!=vb->y)
    return 1;
  /*if(debug)
    fprintf(stderr,"%f <-z-> %f\n",va->z,vb->z);*/
  if(va->z!=vb->z)
    return 1;
  return 0;
}

NewOinfo *
O_Load(int defaultsize,FILE *file,Winfo *w)
{
  int i;
  char *string,readstr[4096],ch2,*stringp;
  int ch;
  float x1,y1,z1,x2,y2,z2;
  float currentradius;
  int currentlayer,currentlayermask,currentarrowtype;
  int fillcolor,linecolor;
  int n;
  NewOinfo *o;
  Pinfo *qq,**q;
  int nel;
  sHash *h;

  o = calloc(1,sizeof(NewOinfo));
  o->prims = varray_init(defaultsize,sizeof(Pinfo *),(intptr_t)NULL);
  o->vertex=NULL;
  currentradius = 1;
  currentlayer=1;
  currentarrowtype = 0;
  currentlayermask=1;/*==2**(currentlayer-1)*/
  /*
    fillcolor = 2*(16+1);
    */
  fillcolor = 2;
  linecolor = 0;
    
  nel=0;

  h=Hash_Init(HASHSIZE,hash_compare);
  while(fgets(readstr,4096,file)!=NULL)
    {
      string=skipspaces(readstr);
      if(*string=='\n')
	break;
      switch(ch=*string)
	{
	case 'a':
	  sscanf(string,"%c %d\n",&ch2,&currentarrowtype);
	  break;
	case 'l':
	case 's':
	  q = varray_ptr(o->prims,nel++);
	  *q = malloc(sizeof(Pinfo));
	  qq = *q;
	  sscanf(string,"%c %f %f %f %f %f %f\n",&ch2,&x1,&y1,&z1,&x2,&y2,&z2);
	  qq->nvertex = 2;
	  qq->vertex = calloc(2,sizeof(Vertex *));
	  qq->points = calloc(2,sizeof(ivector2));
	  qq->layermask=currentlayermask;
	  qq->vertex[0]=Vertex_Insert(h,o,x1,y1,z1,currentlayermask);
	  qq->vertex[1]=Vertex_Insert(h,o,x2,y2,z2,currentlayermask);
	  qq->r  = currentradius;
	  qq->id = ch;
	  qq->label = NULL;
	  qq->arrowtype = currentarrowtype;
		
	  if(ch=='l')
	    qq->color = linecolor;
	  else
	    qq->color = fillcolor;
	  break;
	case 'p':
	  q = varray_ptr(o->prims,nel++);
	  *q = malloc(sizeof(Pinfo));
	  qq = *q;
	  strtok(string," \t");
	  n=atoi(strtok(NULL," \t"));
	  if(n<3)break;
	  qq->nvertex = n+1;
	  qq->vertex = calloc(n+1,sizeof(Vertex *));
	  qq->points = calloc(n+1,sizeof(ivector2));
	  qq->layermask=currentlayermask;
	  for(i=0;i<n;i++)
	    {
	      x1=atof(strtok(NULL," \t"));
	      y1=atof(strtok(NULL," \t"));
	      z1=atof(strtok(NULL," \t"));
	      fprintf(stderr,"%f %f %f\n", x1,y1,z1);
	      qq->vertex[i]=Vertex_Insert(h,o,x1,y1,z1,currentlayermask);
	    }
	  qq->vertex[n] = qq->vertex[0];
		
	  qq->id = ch;
	  qq->label = NULL;
	  qq->color = fillcolor;
	  break;
	case 'c':
	case 'o':
	case '3':
	case '4':
	case '5':
	case '6':
	  q = varray_ptr(o->prims,nel++);
	  *q = malloc(sizeof(Pinfo));
	  qq = *q;
	  sscanf(string,"%c %f %f %f\n",&ch2,&x1,&y1,&z1);
	  qq->nvertex = 1;
	  qq->vertex = calloc(1,sizeof(Vertex *));
	  qq->points = calloc(1,sizeof(ivector2));
	  qq->layermask=currentlayermask;
	  qq->vertex[0]=Vertex_Insert(h,o,x1,y1,z1,currentlayermask);
	  qq->id = ch;
	  qq->r  = currentradius;
	  qq->color = fillcolor;
	  qq->label =NULL;
	  break;
          /*
           * 't' : put a string at specified position.
           */
	case 't':
	  q = varray_ptr(o->prims,nel++);
	  *q = malloc(sizeof(Pinfo));
	  qq = *q;
	  strtok(string," \t");
	  x1 = atof(strtok(NULL," \t"));
	  y1 = atof(strtok(NULL," \t"));
	  z1 = atof(strtok(NULL," \t"));
	  stringp = strtok(NULL,"\n");
	  qq->nvertex = 1;
	  qq->vertex = calloc(1,sizeof(Vertex *));
	  qq->points = calloc(1,sizeof(ivector2));
	  qq->layermask=currentlayermask;
	  qq->vertex[0]=Vertex_Insert(h,o,x1,y1,z1,currentlayermask);
	  qq->id = ch;
	  qq->color = linecolor;
	  qq->label = malloc(strlen(stringp)+1);
	  strcpy(qq->label,stringp);
	  break;
	case 'r':
	  sscanf(string,"r %f\n",&currentradius);
	  break;
	case '@':
	  /*平成13年7月18日(水)拡張：引数があと3つある場合はそれらをrgb強度(0-255)とみなし、パレットをその場で変更する。*/
	  {
	    int red,green,blue,narg;
	    narg=sscanf(string,"@ %d %d %d %d\n",&linecolor,&red,&green,&blue);
	    if(narg==4){
	      overridepalette(w,linecolor,red,green,blue);
	    }else if(linecolor>w->lastColor){
	      linecolor=w->lastColor;
	    }
	    /*
	      linecolor *= (16+1);
	      */
	    fillcolor = linecolor;
	  }
	  break;
	case '#':
	  /*puts(string);*/
	  break;
	case 'y':
	  sscanf(string,"y %d\n",&currentlayer);
	  if(currentlayer>MAXLAYER){
	    currentlayer=MAXLAYER;
	  }
	  currentlayermask=1<<(currentlayer-1);
	  break;
	default:
	  //fgets(string,4096,file);
	  break;
	}
    }
  q = varray_ptr(o->prims,nel);
  *q=NULL;
  /*要素の個数が確定した。*/
  o->prims->n=nel;
  Hash_Done(h);
  return o;
}    


/*この算程が最も遅い。全体の半分以上を占める。要するに、点の数を減らせ
  ば単純に速度が向上する。例えば、ball and stickで水を描く場合に、現在
  は7回座標を指定しているが、これを3回にできれば、その分速度が向上する。
  しかし、こういった処理はできれば外部で行ってほしいな(x3dのように)*/
void
RotateAllPrims0(Ginfo *g,Winfo *w)
{
  float zoomfactor;
  float ex,ey,ez;
  float pr00,pr01,pr02;
  float pr10,pr11,pr12;
  float pr20,pr21,pr22;
  Vertex *v;
  NewOinfo *o;
  o=*w->oo;
  w->screendepth = w->depthratio*(w->screenwidth*0.5)*w->fov;
  /*変換行列の構成：
    newcoord = PerspectiveMat*(absolutecoord - eyecoord)
    物体の回転を許容するなら
    newcoord = PerspectiveMat*(Rotator*absolutecoord - eyecoord)
    = (PerspectiveMat*Rotator)*absolutecoord - (PersMat*Eyecoord)
    */
  /*
    pr00 = g->h[0]*g->Rot[0][0] + g->h[1]*g->Rot[1][0] + g->h[2]*g->Rot[2][0];
    pr10 = g->u[0]*g->Rot[0][0] + g->u[1]*g->Rot[1][0] + g->u[2]*g->Rot[2][0];
    pr20 = g->e[0]*g->Rot[0][0] + g->e[1]*g->Rot[1][0] + g->e[2]*g->Rot[2][0];
    pr01 = g->h[0]*g->Rot[0][1] + g->h[1]*g->Rot[1][1] + g->h[2]*g->Rot[2][1];
    pr11 = g->u[0]*g->Rot[0][1] + g->u[1]*g->Rot[1][1] + g->u[2]*g->Rot[2][1];
    pr21 = g->e[0]*g->Rot[0][1] + g->e[1]*g->Rot[1][1] + g->e[2]*g->Rot[2][1];
    pr02 = g->h[0]*g->Rot[0][2] + g->h[1]*g->Rot[1][2] + g->h[2]*g->Rot[2][2];
    pr12 = g->u[0]*g->Rot[0][2] + g->u[1]*g->Rot[1][2] + g->u[2]*g->Rot[2][2];
    pr22 = g->e[0]*g->Rot[0][2] + g->e[1]*g->Rot[1][2] + g->e[2]*g->Rot[2][2];
    ex = g->h[0]*g->eyep[0] + g->h[1]*g->eyep[1] + g->h[2]*g->eyep[2];
    ey = g->u[0]*g->eyep[0] + g->u[1]*g->eyep[1] + g->u[2]*g->eyep[2];
    ez = g->e[0]*g->eyep[0] + g->e[1]*g->eyep[1] + g->e[2]*g->eyep[2];
    */
  pr00 = w->h[0];
  pr10 = w->u[0];
  pr20 = w->e[0];
  pr01 = w->h[1];
  pr11 = w->u[1];
  pr21 = w->e[1];
  pr02 = w->h[2];
  pr12 = w->u[2];
  pr22 = w->e[2];
  ex = w->h[0]*w->eyep[0] + w->h[1]*w->eyep[1] + w->h[2]*w->eyep[2];
  ey = w->u[0]*w->eyep[0] + w->u[1]*w->eyep[1] + w->u[2]*w->eyep[2];
  ez = w->e[0]*w->eyep[0] + w->e[1]*w->eyep[1] + w->e[2]*w->eyep[2];
  /*まずすべての点を回転する。*/
    
  v=o->vertex;
  while(v!=NULL){
    if(v->layermask & w->layermask){
      float rx,ry,rz;
      rx = v->x - w->centerx;
      ry = v->y - w->centery;
      rz = v->z - w->centerz;
      float xx,yy,zz;
      xx = pr00*rx + pr01*ry + pr02*rz - ex;
      yy = pr10*rx + pr11*ry + pr12*rz - ey;
      zz = pr20*rx + pr21*ry + pr22*rz - ez;
      zoomfactor      = w->screendepth / zz;
      v->zoom = zoomfactor;
      v->ix = (int)( zoomfactor * xx) + w->screenwidth/2;
      v->iy =-(int)( zoomfactor * yy) + w->screenheight/2;
      v->zz=zz;
    }
    v=v->next;
  }
}

void
RotateAllPrims(Ginfo *g,Winfo *w)
{
  int i,j,last;
  Vertex *v;
  NewOinfo *o;
  /*変換行列の構成：
    newcoord = PerspectiveMat*(absolutecoord - eyecoord)
    物体の回転を許容するなら
    newcoord = PerspectiveMat*(Rotator*absolutecoord - eyecoord)
    = (PerspectiveMat*Rotator)*absolutecoord - (PersMat*Eyecoord)
    */
  /*
    pr00 = g->h[0]*g->Rot[0][0] + g->h[1]*g->Rot[1][0] + g->h[2]*g->Rot[2][0];
    pr10 = g->u[0]*g->Rot[0][0] + g->u[1]*g->Rot[1][0] + g->u[2]*g->Rot[2][0];
    pr20 = g->e[0]*g->Rot[0][0] + g->e[1]*g->Rot[1][0] + g->e[2]*g->Rot[2][0];
    pr01 = g->h[0]*g->Rot[0][1] + g->h[1]*g->Rot[1][1] + g->h[2]*g->Rot[2][1];
    pr11 = g->u[0]*g->Rot[0][1] + g->u[1]*g->Rot[1][1] + g->u[2]*g->Rot[2][1];
    pr21 = g->e[0]*g->Rot[0][1] + g->e[1]*g->Rot[1][1] + g->e[2]*g->Rot[2][1];
    pr02 = g->h[0]*g->Rot[0][2] + g->h[1]*g->Rot[1][2] + g->h[2]*g->Rot[2][2];
    pr12 = g->u[0]*g->Rot[0][2] + g->u[1]*g->Rot[1][2] + g->u[2]*g->Rot[2][2];
    pr22 = g->e[0]*g->Rot[0][2] + g->e[1]*g->Rot[1][2] + g->e[2]*g->Rot[2][2];
    ex = g->h[0]*g->eyep[0] + g->h[1]*g->eyep[1] + g->h[2]*g->eyep[2];
    ey = g->u[0]*g->eyep[0] + g->u[1]*g->eyep[1] + g->u[2]*g->eyep[2];
    ez = g->e[0]*g->eyep[0] + g->e[1]*g->eyep[1] + g->e[2]*g->eyep[2];
    */
  RotateAllPrims0(g,w);
  o=*w->oo;
  w->nrealize=0;
  w->nscalable=0;//number of scalable elements, i.e. non-text objects
  /*必要の無い要素はこの時点で除いておく。*/
  last=o->prims->n - 1;
  j=0;
  while(j<=last){
    float zsum;
    Pinfo *qq;
    qq =((Pinfo **)o->prims->a)[j];
    zsum = 0;
    if(qq->layermask & w->layermask){
      for(i=0;i<qq->nvertex;i++){
        v = qq->vertex[i];
        zsum += v->zz;
        if(v->zz<g->clip){
          goto skip;
        }
        qq->points[i].x = v->ix;
        qq->points[i].y = v->iy;
      }
      qq->sortkey = zsum/qq->nvertex;
      j++;
      if ( qq->id != 't' ){
        w->nscalable ++;
      }
    }else{
      Pinfo **head,**tail;
    skip:
      head=&(((Pinfo **)o->prims->a)[j]);
      tail=&(((Pinfo **)o->prims->a)[last]);
      *head=*tail;
      *tail=qq;
      last--;
    }
  }
  w->nrealize=last+1;
}


int
Realize (NewOinfo *o,Winfo *w)
{
  switch(w->reality)
    {
    case 0:
      return Reality0(o,w);
    case 1:
      return Reality1(o,w);
    case 2:
      return Reality2(o,w);
    case 3:
      return Reality3(o,w);
    case 4:
      return Reality4(o,w);
    }
  return 1;
}

int
compar(const Pinfo **i,const Pinfo **j)
{
  return ((*i)->sortkey > (*j)->sortkey)?-1:1;
}

/*このアルゴリズムでは、同一の値が並んでいる場合でも不必要なswapを繰り
  返してしまう。現在のバージョンでは、表示する必要のない要素はソートの
  対象にならないので、あまり問題ではない。

  枢軸要素に区間の中央値を使用しているので、あらかじめソートされている
  データに対して最高の性能がでるように思うのだがどうだろうか。
*/
//int numswap;

static void q_sort(Pinfo **a, int first,int last)
{
  int i,j;
  float pivot;
  Pinfo *tmp;
retry:    
  if(first>=last)return;
  if(last-first==1){
    if(a[first]->sortkey<a[last]->sortkey){
      tmp=a[first];
      a[first]=a[last];
      a[last]=tmp;
    }
    return;
  }
  pivot = a[(first+last)/2]->sortkey;
  i=first;
  j=last;
  do{
    while(a[i]->sortkey>pivot)i++;
    while(a[j]->sortkey<pivot)j--;
    if(i<=j){
      tmp=a[i];
      a[i]=a[j];
      a[j]=tmp;
      //numswap++;
      i++;
      j--;
    }
  } while(i<=j);
  //if( last-first > 1000 )
  //    printf("%d\n",100*(j-first)/(last-first));
  q_sort(a,first,j);
  first=i;
  goto retry;
}

void quick_sort(Pinfo **a, int n)
{
    //numswap=0;
  q_sort(a,0,n-1);
}

int number=0;

void InsertFrameInfo(Winfo *w,float framerate,int verbose)
{
  char num[256];
  int i;
  int base=10;
  setfgcolor(w,2);
  if(verbose==1){
    sprintf(num,"%d",w->currentframe);
    drawstring2fb(w,5,base,num,strlen(num));
  }
  if(verbose>=2){//initial verbosity
    if(debug){
#ifdef win32    
      sprintf(num,"Test %d",clock());
#else
      sprintf(num,"Test %d",number++);
#endif
      drawstring2fb(w,5,base,num,strlen(num));
      base+=14;
    }
    sprintf(num,"Frame %d (n p)",w->currentframe);
    drawstring2fb(w,5,base,num,strlen(num));
    base+=14;
    sprintf(num,"Keybuf %d",jumpto);
    drawstring2fb(w,5,base,num,strlen(num));
    base+=14;
    sprintf(num,"Reality %d (+ -)",w->reality);
    drawstring2fb(w,5,base,num,strlen(num));
    base+=14;
    sprintf(num,"Screen Depth Ratio %5.2f (* /)",w->depthratio);
    drawstring2fb(w,5,base,num,strlen(num));
    base+=14;
    sprintf(num,"Center %5.2f %5.2f %5.2f (c)",w->centerx,w->centery,w->centerz);
    drawstring2fb(w,5,base,num,strlen(num));
    base+=14;
    if(w->async)
      setfgcolor(w,2);
    else
      setfgcolor(w,0);
    drawstring2fb(w,5,base,"async (s)",9);
    base+=14;
    for(i=1;i<=MAXLAYER;i++){
      if(w->layermask & (1<<(i-1)))
	setfgcolor(w,2);
      else
	setfgcolor(w,0);
      sprintf(num,"Layer %d (F%d)",i,i);
      drawstring2fb(w,5,base,num,strlen(num));
      base+=14;
    }
  }
  if(verbose>=3){
    setfgcolor(w,2);
    sprintf(num,"Visual Primitves %d",w->nrealize);
    drawstring2fb(w,5,base,num,strlen(num));
    base+=14;
    sprintf(num,"Line width %d (Del Ins)",w->thick);
    drawstring2fb(w,5,base,num,strlen(num));
    base+=14;
    sprintf(num,"Field of view %f ([ ])",360.0*atan(1.0/w->fov)/M_PI);
    drawstring2fb(w,5,base,num,strlen(num));
    base+=14;
    sprintf(num,"Eye point %f %f %f",w->eyep[0],w->eyep[1],w->eyep[2]);
    drawstring2fb(w,5,base,num,strlen(num));
    base+=14;
    sprintf(num,"Look point %f %f %f",w->lookp[0],w->lookp[1],w->lookp[2]);
    drawstring2fb(w,5,base,num,strlen(num));
    base+=14;
    sprintf(num,"Up vector %f %f %f",w->up[0],w->up[1],w->up[2]);
    drawstring2fb(w,5,base,num,strlen(num));
    base+=14;
    sprintf(num,"Frame Rate %f",framerate);
    drawstring2fb(w,5,base,num,strlen(num));
    base+=14;
    //sprintf(num,"Num of swap in qsort: %d",numswap);
    //drawstring2fb(w,5,base,num,strlen(num));
    //base+=14;
  }
}

int
Redraw(NewOinfo *o,Winfo *w,float framerate,int verbose)
{
  quick_sort(o->prims->a,w->nrealize);
  clearfb(w);
  if(Realize(o,w))return 1;
  InsertFrameInfo(w,framerate,verbose);
  exposefb(w);
  return 0;
}


Ginfo *
G_Init()
{
  Ginfo *g = calloc(1,sizeof(Ginfo));
  g->verbose=2;
  g->clip=3;
  if(debug)fprintf(stderr,"G_Init::\n");
  return g;
}


Winfo *
W_Init(int n)
{
  long int *pos;
  int i;
  Winfo *w = calloc(n,sizeof(Winfo));
    
  for(i=0;i<n;i++){
    w[i].framepos = varray_init(1,sizeof(long int),-1);
    pos = varray_ptr(w[i].framepos,0);
    *pos = 0;
    w[i].maxprim=1;
    w[i].eyep[0]=Eyex;
    w[i].eyep[1]=Eyey;
    w[i].eyep[2]=Eyez;
      
    w[i].lookp[0]=Lookx;
    w[i].lookp[1]=Looky;
    w[i].lookp[2]=Lookz;
      
    w[i].up[0]=Upx;
    w[i].up[1]=Upy;
    w[i].up[2]=Upz;
    w[i].depthratio=1.0;
    w[i].screenwidth =  DEFAULTX;
    w[i].screenheight = DEFAULTY;
    w[i].async=0;
    w[i].RecordMode=0;
  }
  return w;
}    

Ainfo *
A_Init()
{
  Ainfo *a = calloc(1,sizeof(Ainfo));
  a->cachesize=1;
  if(debug)fprintf(stderr,"A_Init::\n");
  return a;
}    



void W_Cache(Winfo *w){
  /*Objects*/
  NewOinfo *o;
  W_Seek(w);
  w->oo = (NewOinfo **)Cache_ptr(w->c,w->currentframe);
  if((*w->oo)==NULL)
    *w->oo=O_Load(w->maxprim,w->inputfile,w);
  o=*w->oo;
  if(w->maxprim<o->prims->n)
    w->maxprim=o->prims->n+1;
  
}


void W_resetcenter(Winfo* w)
{
  w->centerx = w->centery = w->centerz = 0;
}



void W_setcenter(Winfo* w)
{
  float xmin=1e99,xmax=-1e99;
  float ymin=1e99,ymax=-1e99;
  float zmin=1e99,zmax=-1e99;
  NewOinfo* o=*w->oo;
  Vertex* v=o->vertex;
  while(v!=NULL){
    if(v->layermask & w->layermask){
      if ( v->x < xmin ) xmin = v->x;
      if ( xmax < v->x ) xmax = v->x;
      if ( v->y < ymin ) ymin = v->y;
      if ( ymax < v->y ) ymax = v->y;
      if ( v->z < zmin ) zmin = v->z;
      if ( zmax < v->z ) zmax = v->z;
    }
    v=v->next;
  }
  w->centerx = (xmax + xmin)/2;
  w->centery = (ymax + ymin)/2;
  w->centerz = (zmax + zmin)/2;
}



void W_InitActionVarsOne(Winfo *w,int cachesize)
{
    w->c = Cache_Init(cachesize,O_Done);
    W_Cache(w);
    w->status = REDRAW;
    w->accump = w->accumh = w->accumb=0;
    w->wp=w->wb=w->wh=0;
    w->AbsoluteRotationMode=0;
    w->currentframe     = 0;
    w->realcurrent=0;
    w->df = 0;
    w->fov=1.0/tan(45.0*PI/360.0);
    w->reality=0;
    w->thick=2;
    w->layermask=0xfff;/*12layers*/
    W_setvectors(w);
    W_resetcenter(w);
}



void W_InitActionVars( Ginfo* g, Winfo w[],int cachesize )
{
    int i;
    
    for(i=0;i<g->nwindow;i++){
        W_InitActionVarsOne( &w[i], cachesize );
    }
}



void EventLoop(Ainfo *a,Ginfo *g,Winfo *w)
{
  /*data read*/
  int i;
  int stopped;
    
  resettimer();
  stopped=1;
  while(1)
    {
      float delta=0.000001*deltatime2();
      if(stopped){
	delta=0;
      }
      if(debug){
	fprintf(stderr,"Refreshed in %f sec.\n",delta);
      }
      stopped=1;
      /*イベントが無いときは自動処理*/
      /*もし動画表示中なら、先へすすめる。*/
      for(i=0;i<g->nwindow;i++){
	if(w[i].RecordMode){
	  w[i].realcurrent++;
	  stopped=0;
	}
	else{
	  if(w[i].df!=0){
	    /*o->nextframe += o->df;*/
	    w[i].realcurrent += w[i].df*delta;
            if(debug){
              fprintf(stderr,"Animate Realcurrnt:%f.\n",w[i].realcurrent);
            }
	  }
	/*もしAbsoluteRotationModeでなければ、自動回転もありう */
	/*る。*/
	  if(w[i].AbsoluteRotationMode==0){
	    if((w[i].wp!=0)||(w[i].wh!=0)||(w[i].wb!=0))
	      {
		w[i].accump += w[i].wp * delta*0.2;
		w[i].accumh += w[i].wh * delta*0.2;
		w[i].accumb += w[i].wb * delta*0.2;
		w[i].status|=REDRAW;
		if (debug) fprintf(stderr,"REDRAW because angular velocity is nonzero: %d %d %d [window %d]\n", w[i].wp, w[i].wh, w[i].wb,i);
		  
		stopped=0;
	      }
	    if(w[i].df!=0){
	      stopped=0;
	    }
	  }
	}
      }
      if(stopped)
	goto wait;
      while(existevent(g)){
      wait:
	handleevent(g,w);
      }
      for(i=0;i<g->nwindow;i++){
	int nextframe;
	nextframe=w[i].realcurrent;
	if(w[i].currentframe!=nextframe)
	  {
	    w[i].currentframe=nextframe;
	    W_Cache(&w[i]);
	    w[i].status|=REDRAW;
		if (debug) fprintf(stderr,"REDRAW for progress in time [Window %d]\n",i);

	  }
	if(!w[i].AbsoluteRotationMode)
	  {
	    float deltah,deltab;
	    float eyex,eyey,eyez,eyed;
	    float dx,dy,dz;
	    /*lookpを中心にeyepを回転させる場合は*/
	    /*horizontal rotation*/
	    dx = w[i].lookp[0]-w[i].eyep[0];
	    dy = w[i].lookp[1]-w[i].eyep[1];
	    dz = w[i].lookp[2]-w[i].eyep[2];
	    deltah=-sin(w[i].accumh);
	    deltab=sin(w[i].accumb);
	    eyex = w[i].e[0] + w[i].h[0]*deltah + w[i].u[0]*deltab;
	    eyey = w[i].e[1] + w[i].h[1]*deltah + w[i].u[1]*deltab;
	    eyez = w[i].e[2] + w[i].h[2]*deltah + w[i].u[2]*deltab;
	    eyed = sqrt((dx*dx+dy*dy+dz*dz)/(eyex*eyex+eyey*eyey+eyez*eyez));
	    eyex*=eyed;
	    eyey*=eyed;
	    eyez*=eyed;
	    w[i].eyep[0]=w[i].lookp[0]-eyex;
	    w[i].eyep[1]=w[i].lookp[1]-eyey;
	    w[i].eyep[2]=w[i].lookp[2]-eyez;
	    w[i].accump=w[i].accumh=w[i].accumb=0;
	    /*end of addition*/
		
	  }
	/*status |= REDRAW;*/
	W_setvectors(&w[i]);
	if(w[i].status & REDRAW){
	  RotateAllPrims(g,&w[i]);
	  if(Redraw(*w[i].oo,&w[i],1.0/delta,g->verbose)){
	    if(debug)
	      fprintf(stderr,"interrupted\n");
	    break;/*interrupted*/
	  }
	  /*バッファの中身がなくなるまで待つ*/
	  /*syncはいつも0.04秒ぐらいしかかかっていない。*/
	  if(w[i].RecordMode){
	    W_SaveSnapShot(&w[i],i);
	  }
	  waituntilflush(g);
	  w[i].status &= ~REDRAW;
	  if (debug) fprintf(stderr,"REDRAW is unset [Window %d]\n",i);
	}
      }
    }
}



void polygon_Init()
{
  int n;
  for(n=3;n<=6;n++)
    {
      int i;
      for(i=0;i<n;i++)
	{
	  polygon[i][n-3].x=sin(2.0*PI*i/n);
	  polygon[i][n-3].y=-cos(2.0*PI*i/n);
	}
    }
    
  if(debug)fprintf(stderr,"polygon_Init::\n");
}



void EyeMotion(Winfo *w,float prop)
{
  float eyex,eyey,eyez;
  eyex = w->lookp[0]-w->eyep[0];
  eyey = w->lookp[1]-w->eyep[1];
  eyez = w->lookp[2]-w->eyep[2];
  w->eyep[0]+=eyex*prop;
  w->eyep[1]+=eyey*prop;
  w->eyep[2]+=eyez*prop;
}


int main(int argc, char *argv[])
{
  Ginfo *g=G_Init();
  Ainfo *a=A_Init();
  Winfo *w;

  polygon_Init();
  w=GetOptions(argc,argv,a,g);
  G_Init2(g);
  W_Init2(w,g);
  /*load the first frame*/
  W_InitActionVars( g, w, a->cachesize );

  EventLoop( a, g, w );
  return(0);
}

