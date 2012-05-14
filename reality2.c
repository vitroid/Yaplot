#include <math.h>
#include "common.h"
#include "gui.h"

extern int debug;
extern vector2 polygon[6][4];

int
Reality2(NewOinfo *o,Winfo *w)
{
  int r,i;
  ivector2 poly[7];
    
  float thickness,thickdelta;
  int ithickness,oldthickness;
    
  /*本来は、可変長配列の要素数は概数なので、このような指定は正しくない。*/
  /*if(o->n==0)return 0;*/
  if(debug){
    fprintf(stderr,"Nrealize: %d\n",w->nrealize);
  }
  if(w->nrealize==0)
    return 0;
    
  thickness=0.5;
  /*thickdelta=w->thick*0.001*2.0*w->screenwidth/(float)o->n;    */
  thickdelta=w->thick*0.001*2.0*w->screenwidth/(float)w->nrealize;
  ithickness=thickness;
  oldthickness=ithickness;
  setthickness(w,ithickness);
  for(i=0;i<w->nrealize;i++)
    {
      Pinfo *qq;
      char ch;
	
      thickness+=thickdelta;
      ithickness=thickness;
      if(oldthickness!=ithickness){
	oldthickness=ithickness;
	setthickness(w,ithickness);
      }

      qq = ((Pinfo **)o->prims->a)[i];
      /*if(((i%1000)==0)&&XPending(g->display))return 1;*/
      if(qq->sortkey<0)return 0;
      ch = qq->id;
      if(ch>='3'&&ch<='6')
	{
	  int n,j;
	  float rr;
		      
	  n = ch - '0';
	  rr = qq->vertex[0]->zoom*qq->r;
	  for(j=0;j<n;j++)
	    {
	      poly[j].x = qq->points[0].x + (int)(polygon[j][n-3].x * rr);
	      poly[j].y = qq->points[0].y + (int)(polygon[j][n-3].y * rr);
	    }
	  poly[n].x = poly[0].x;
	  poly[n].y = poly[0].y;
	  setfgcolor(w,qq->color);
	  /*		      XSetFillStyle(g->display,w->gc,FillStippled);
			      XFillPolygon(g->display,w->pixmap,w->gc,poly,n,Nonconvex,CoordModeOrigin);
			      XSetFillStyle(g->display,w->gc,FillSolid);*/
	  /* it might be time-consuming. */
	  setstipple(w, i%16);
	  drawpoly2fb(w,poly,n,FALSE);
	}
      else
	switch (ch)
	  {
	  case 't':
	    setfgcolor(w,qq->color);
	    drawstring2fb(w,qq->points[0].x,qq->points[0].y,qq->label,strlen(qq->label));
	    break;
	  case 'l':
	    setfgcolor(w,qq->color);
	    drawline2fb(w,qq->points[0].x,qq->points[0].y,qq->points[1].x,qq->points[1].y);
	    break;
	  case 'c':
	    r = (int)(qq->vertex[0]->zoom*qq->r);
	    setfgcolor(w,qq->color);
	    setstipple(w, i%16);
	    setstippledfill(w);
	    drawcircle2fb(w,qq->points[0].x,qq->points[0].y,r,TRUE);
	    setsolidfill(w);
	    setthickness(w,1);
	    drawcircle2fb(w,qq->points[0].x,qq->points[0].y,r,FALSE);
	    setthickness(w,ithickness);
	    break;
	  case 'o':
	    r = (int)(qq->vertex[0]->zoom*qq->r);
	    setfgcolor(w,qq->color);
	    drawcircle2fb(w,qq->points[0].x,qq->points[0].y,r,FALSE);
	    break;
	  case 's':
	    r = (int)(qq->vertex[0]->zoom*qq->r);
	    setfgcolor(w,qq->color);
	    drawstick2fb(w,qq->points[0].x,qq->points[0].y,
			 qq->points[1].x,qq->points[1].y,r);
	    break;
	  case 'p':
	    setfgcolor(w,qq->color);
	    setstipple(w, i%16);
	    setstippledfill(w);
	    drawpoly2fb(w,qq->points,qq->nvertex-1,TRUE);
	    setsolidfill(w);
	    setthickness(w,1);
	    drawpoly2fb(w,qq->points,qq->nvertex-1,FALSE);
	    setthickness(w,ithickness);
	    break;
	  }
    }
  return 0;
}
