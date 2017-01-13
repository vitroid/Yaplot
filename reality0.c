#include <math.h>
#include "common.h"
#include "gui.h"

extern int debug;
extern vector2 polygon[6][4];

int
Reality0(NewOinfo *o,Winfo *w)
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
    thickdelta=w->thick*0.001*2.0*w->screenwidth/(float)w->nrealize;
    ithickness=thickness;
    oldthickness=ithickness;
    setthickness(w,ithickness);
    
    if(debug)
        fprintf(stderr,"nrealize:%d\n",w->nrealize);
    for(i=0;i<w->nrealize;i++){
        Pinfo *qq;
        char ch;
        
        thickness+=thickdelta;
        ithickness=thickness;
        if(oldthickness!=ithickness){
            oldthickness=ithickness;
            setthickness(w,ithickness);
        }
        qq = ((Pinfo **)o->prims->a)[i];
        
        ch = qq->id;
        if(ch>='3'&&ch<='6'){
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
            drawpoly2fb(w,poly,n,FALSE);
        }
        else
            switch (ch){
            case 'o':
            case 'c':
                r = (int)(qq->vertex[0]->zoom*qq->r);
                setfgcolor(w,qq->color);
                drawcircle2fb(w,qq->points[0].x,qq->points[0].y,r,FALSE);
                
                break;
            case 't':
                setfgcolor(w,qq->color);
                drawstring2fb(w,qq->points[0].x,qq->points[0].y,qq->label,strlen(qq->label));
                break;
            case 'l':
            case 's':
                setfgcolor(w,qq->color);
                drawline2fb(w,qq->points[0].x,qq->points[0].y,qq->points[1].x,qq->points[1].y);
                break;
            case 'p':
                setfgcolor(w,qq->color);
                drawpoly2fb(w,qq->points,qq->nvertex-1,FALSE);
                break;
            }
    }
    return 0;
}
