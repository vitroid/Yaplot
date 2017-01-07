#include <gtk/gtk.h>
#include <stdio.h>
#include <math.h>
#include <gdk/gdkkeysyms.h>
#ifdef RECORD
#include <gdk-pixbuf/gdk-pixbuf.h>
#endif
#include "gtk.h"
#include "yaplot.h"
#include "os.h"
#include "stipples.h"

extern int debug;
extern float Eyex,Eyey,Eyez;
extern float Lookx,Looky,Lookz;
extern float Upx,Upy,Upz;
int lastthick;

void drawline2fb(Winfo *w,int x0,int y0,int x1,int y1)
{
  gdk_draw_line(w->pixmap,w->gc,x0,y0,x1,y1);
}

void drawpoly2fb(Winfo *w,ivector2 *poly,int n,int fill)
{
  gdk_draw_polygon(w->pixmap,w->gc,fill,(GdkPoint *)poly,n);
}

void drawcircle2fb(Winfo *w,int x,int y,int r,int fill)
{
  gdk_draw_arc(w->pixmap,w->gc,fill,x - r,y - r,r+r,r+r,0,360*64);
}

void drawstick2fb(Winfo* w, int x0, int y0, int x1, int y1, int r, int fill, int arrowtype)
{
  if ( arrowtype == 0){
    gdk_gc_set_line_attributes(w->gc,r*2,GDK_LINE_SOLID,GDK_CAP_BUTT,GDK_JOIN_ROUND);
    gdk_draw_line(w->pixmap,w->gc,x0,y0,x1,y1);
    setthickness(w, lastthick);
  }
  else{
    int dx = x1 - x0;
    int dy = y1 - y0;
    int dd = (int)sqrt(dx*dx + dy*dy);
    if ( dd != 0 ){
      int rx = r * dx / dd;
      int ry = r * dy / dd;
      if ( arrowtype == 1 ){
	//normal arrow with a head.
	gdk_draw_line(w->pixmap,w->gc,x0,y0,x1,y1);
	gdk_draw_line(w->pixmap,w->gc,x1,y1,x1-2*rx+ry,y1-2*ry-rx);
	gdk_draw_line(w->pixmap,w->gc,x1,y1,x1-2*rx-ry,y1-2*ry+rx);
      }
      else if ( arrowtype == 2 ){
	//dart
	ivector2 p[3];
	p[0].x = x1;
	p[0].y = y1;
	p[1].x = x0 + ry;
	p[1].y = y0 - rx;
	p[2].x = x0 - ry;
	p[2].y = y0 + rx;
	drawpoly2fb(w,p,3,fill);
      }
    }
  }
}


void setthickness(Winfo *w,int thick)
{
  lastthick = thick;
  gdk_gc_set_line_attributes(w->gc,thick,GDK_LINE_SOLID,GDK_CAP_ROUND,GDK_JOIN_ROUND);
}

void waituntilflush()
{
  gdk_flush();
}

void setfgcolor(Winfo *w,int palette)
{
  gdk_gc_set_foreground(w->gc,&w->colortable[palette]);
  if(debug){
    fprintf(stderr,"set fg color to palette %d: %x %x %x\n",
	    palette, 
	    w->colortable[palette].red,
	    w->colortable[palette].green,
	    w->colortable[palette].blue);
  }
}

void overridepalette(Winfo *w,int palette,int red,int green,int blue)
{
  if(w->colortable[palette].pixel){
    gdk_colormap_free_colors(w->colormap,&w->colortable[palette],1);
  }
  w->colortable[palette].red=red<<8;
  w->colortable[palette].green=green<<8;
  w->colortable[palette].blue=blue<<8;
  gdk_colormap_alloc_color(w->colormap,&w->colortable[palette],FALSE,FALSE);
  if(palette>w->lastColor){
    w->lastColor=palette;
  }
}

void drawstring2fb(Winfo *w,int x,int y,char *str,int length)
{
  gdk_draw_text(w->pixmap,
		w->font,
		w->gc,
		x,y,str,length);
}

void setsolidfill(Winfo *w)
{
  gdk_gc_set_fill(w->gc,GDK_SOLID);
}

void setstippledfill(Winfo *w)
{
  gdk_gc_set_fill(w->gc,GDK_STIPPLED);
}


void setstipple(Winfo* w, gint i)
{
  gdk_gc_set_stipple(w->gc,w->bitmap[i]);
}


void clearfb(Winfo *w)
{
  setsolidfill(w);
  setfgcolor(w,1);
  gdk_draw_rectangle(w->pixmap,w->gc,
		     TRUE,0,0,
		     w->drawarea->allocation.width,
		     w->drawarea->allocation.height);
}

void exposefb(Winfo *w)
{
  GdkRectangle rect;
  gdk_draw_pixmap(w->drawarea->window,
		  w->drawarea->style->fg_gc[GTK_WIDGET_STATE(w->drawarea)],
		  w->pixmap,
		  0,0,
		  0,0,
		  w->drawarea->allocation.width,
		  w->drawarea->allocation.height);
  if(debug)
    fprintf(stderr,"REQ::Expose::%d %d\n",
	  w->drawarea->allocation.width,
	  w->drawarea->allocation.height);
  /*make expose event*/
  rect.x=0;
  rect.y=0;
  rect.width=w->drawarea->allocation.width;
  rect.height=w->drawarea->allocation.height;
  gtk_widget_draw(w->window,&rect);
  
}

Ginfo *g_internal;
Winfo *w_internal;

int WhichWidget(GtkWidget *widget)
{
  int i;
  for(i=0;i<g_internal->nwindow;i++){
    if(w_internal[i].drawarea==widget||w_internal[i].window==widget){
      return i;
    }
  }
  exit(1);
}
  
static gint expose_event(GtkWidget *widget,
			 GdkEventExpose *event)
{
  int i=WhichWidget(widget);
  if(debug)
    fprintf(stderr,"Expose[%d]::%d %d\n",
	  i,		  event->area.width,event->area.height);
  gdk_draw_pixmap(widget->window,
		  widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
		  w_internal[i].pixmap,
		  event->area.x,event->area.y,
		  event->area.x,event->area.y,
		  event->area.width,event->area.height);
  w_internal[i].status|=REDRAW;
  return FALSE;
}

static gint configure_event(GtkWidget *widget,
			 GdkEventConfigure *event)
{
  int i=WhichWidget(widget);
  if(w_internal[i].pixmap){
    gdk_pixmap_unref(w_internal[i].pixmap);
  }
  w_internal[i].pixmap=gdk_pixmap_new(widget->window,
				      widget->allocation.width,
				      widget->allocation.height,
				      -1);
  w_internal[i].screenwidth = widget->allocation.width;
  w_internal[i].screenheight = widget->allocation.height;
  w_internal[i].status|=REDRAW;
  return FALSE;
}

/*global*/
extern int jumpto;
int crawl=0;
int originx=0,originy=0;

static gint motion_notify_event(GtkWidget *widget,
				GdkEventMotion *event)
{
  int x,y;
  GdkModifierType state;

  if(event->is_hint){
    gdk_window_get_pointer(event->window,&x,&y,&state);
  }else{
    x=event->x;
    y=event->y;
    state=event->state;
  }
  if(state & GDK_BUTTON1_MASK){
    if( (originx !=x) || (originy != y)){
      int i;
      float head,bank;
      /*fprintf(stderr,"motion::%d %d\n",originx-x,originy-y);*/
      bank = +(originy - y)*(10.0/400.0);
      head = +(originx - x)*(10.0/400.0);
      originx = x;
      originy = y;
      i=WhichWidget(widget);
      if(w_internal[i].async){
	WindowMotion(&w_internal[i],head,bank);
      }else{
	for(i=0;i<g_internal->nwindow;i++){
	  WindowMotion(&w_internal[i],head,bank);
	}
      }
    }
  }
  return FALSE;
}

static gint button_press_event(GtkWidget *widget,
				GdkEventButton *event)
{
  int i;
  
  originx=event->x;
  originy=event->y;
  i=WhichWidget(widget);
  if(w_internal[i].async){
    w_internal[i].AbsoluteRotationMode=1;
  }else{
    for(i=0;i<g_internal->nwindow;i++){
      w_internal[i].AbsoluteRotationMode=1;
    }
  }
  return FALSE;
}

static gint button_release_event(GtkWidget *widget,
				GdkEventButton *event)
{
  int i;
  if(debug)
    fprintf(stderr,"Released::\n");
  i=WhichWidget(widget);
  if(w_internal[i].async){
    w_internal[i].wp=w_internal[i].wb=w_internal[i].wh=0;
    w_internal[i].AbsoluteRotationMode=0;
    /*w_internal[i].status |=REDRAW;*/
  }else{
    for(i=0;i<g_internal->nwindow;i++){
      w_internal[i].wp=w_internal[i].wb=w_internal[i].wh=0;
      w_internal[i].AbsoluteRotationMode=0;
      /*w_internal[i].status |=REDRAW;*/
    }
  }
  return FALSE;
}

#ifdef RECORD
void W_StopRecording(Winfo *w)
{
  /*automatic recording mode; will save each frame to file*/
  w->RecordMode=0;
  gtk_window_set_title((GtkWindow *)w->window,"Done");
}

void W_StartRecording(Winfo *w)
{
  w->RecordMode=1;
  w->df=w->wh=w->wp=w->wb=0;
  gtk_window_set_title((GtkWindow *)w->window,"Recording");
}
#endif

static gint key_press_cb(GtkWidget *widget,
				GdkEventKey *event)
{
  int i=WhichWidget(widget);
  int key=event->keyval;
  int modstatus = event->state;
  Winfo *w=w_internal;/*just for compat*/
  Ginfo *g=g_internal;/*just for compat*/
  if(debug){
    fprintf(stderr,"Key: %d/Status: %d\n",key,modstatus);
  }
  /*このあたりの処理はすべてのコマンドに拡張すべき。*/
  if((event->length==1)&&(event->string[0]>='0')&&(event->string[0]<='9')){
    jumpto = jumpto*10+event->string[0]-'0';
#ifdef RECORD
  }else if(w[i].RecordMode){
    switch(key){
    case GDK_r:
      eStopRecording(g,w,i);
      break;
    case GDK_q:
    case GDK_Break:
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
      case GDK_q:
      case GDK_Break:
	processed=eQuit(g,w,i);
	break;
	/*go absolute*/
      case GDK_g:
      case GDK_G:
      case GDK_Return:
      case GDK_KP_Enter:
	processed=eGotoFrame(g,w,i,(((modstatus&GDK_SHIFT_MASK)&&(jumpto==0))?LASTFRAME:jumpto));
	break;
	/*go forward*/
      case GDK_Page_Down:
      case GDK_n:
      case GDK_N:
      case GDK_KP_Page_Down:
	processed=eGoRelativeFrame(g,w,i,(jumpto?+jumpto:+1),(modstatus&GDK_SHIFT_MASK));
	break;
	/*go backward*/
	/*case GDK_c:
      case GDK_C:*/
	/*crawl=!crawl;*/
	break;
#ifdef RECORD
      case GDK_R:
      case GDK_r:
	processed=eStartRecording(g,w,i);
	break;
#endif
#ifndef win32
      case GDK_u:
      case GDK_U:
	processed=ePushView(g,w,i);
	break;
      case GDK_o:
      case GDK_O:
	processed=ePopView(g,w,i);
	break;
#endif
      case GDK_v:
      case GDK_V:
	processed=eToggleVerbosity(g,w,i);
	break;
      case GDK_Page_Up:
      case GDK_p:
      case GDK_P:
      case GDK_KP_Page_Up:
	processed=eGoRelativeFrame(g,w,i,(jumpto?-jumpto:-1),(modstatus&GDK_SHIFT_MASK));
	break;
	/*first frame*/
      case GDK_Home:
      case GDK_KP_Home:
	processed=eGotoFrame(g,w,i,0);
	break;
	/*last frame*/
      case GDK_End:
      case GDK_KP_End:
	processed=eGotoFrame(g,w,i,LASTFRAME);
	break;
	/*Stop motion*/
      case GDK_space:
      case GDK_KP_Space:
	processed=eStopMotion(g,w,i);
	break;
	/*Stop rotation*/
      case GDK_Pause:
      case GDK_exclam:
	processed=eStopRotation(g,w,i);
	break;
	/*telescopic*/
      case GDK_bracketright:
	processed=eWiden(g,w,i,(jumpto?-jumpto:-1));
	break;
      case GDK_asterisk:
      case GDK_KP_Multiply:
	processed=eZoom(g,w,i,(jumpto?+jumpto:+1));
	break;
	/*wide*/
      case GDK_bracketleft:
	processed=eWiden(g,w,i,(jumpto?+jumpto:+1));
	break;
      case GDK_slash:
      case GDK_KP_Divide:
	processed=eZoom(g,w,i,(jumpto?-jumpto:-1));
	break;
	/*heading rotation*/
      case GDK_H:
      case GDK_h:
      case GDK_Left:
      case GDK_KP_Left:
	processed=eHeading(g,w,i,(jumpto?+jumpto:+1));
	break;
	/*heading rotation*/
      case GDK_L:
      case GDK_l:
      case GDK_Right:
      case GDK_KP_Right:
	processed=eHeading(g,w,i,(jumpto?-jumpto:-1));
	break;
	/*banking rotation*/
      case GDK_K:
      case GDK_k:
      case GDK_Up:
      case GDK_KP_Up:
	processed=eBanking(g,w,i,(jumpto?+jumpto:+1));
	break;
	/*banking rotation*/
      case GDK_J:
      case GDK_j:
      case GDK_Down:
      case GDK_KP_Down:
	processed=eBanking(g,w,i,(jumpto?-jumpto:-1));
	break;
	/*flush jumpto*/
      case GDK_Escape:
	processed=1;
	break;
      case GDK_s:
      case GDK_S:
	processed=eToggleSync(g,w,i);
	break;
      case GDK_f:
      case GDK_F:
	processed=eToggleLayer(g,w,i,(jumpto?jumpto:1));
	break;
	/*if(crawl){*/
	  /*float dx,dy,dz;
	    dx=g->e[0]*0.2;
	    dy=g->e[1]*0.2;
	    dz=g->e[2]*0.2;
	    g->lookp[0]+=dx;
	    g->lookp[1]+=dy;
	    g->lookp[2]+=dz;
	    g->eyep[0]+=dx;
	    g->eyep[1]+=dy;
	    g->eyep[2]+=dz;
	    for(i=0;i<g->nwindow;i++)
	    w[i].status|=REDRAW;
	    */
      case GDK_Insert:
      case GDK_KP_Insert:
      case GDK_parenright:
	processed=eRelativeThickness(g,w,i,(jumpto?+jumpto:+1));
	break;
      case GDK_Delete:
      case GDK_KP_Delete:
      case GDK_parenleft:
	processed=eRelativeThickness(g,w,i,(jumpto?-jumpto:-1));
	break;
	/*better quality*/
      case GDK_plus:
      case GDK_KP_Add:
	processed=eRelativeReality(g,w,i,+1);
	break;
	/*less quality*/
      case GDK_minus:
      case GDK_KP_Subtract:
	processed=eRelativeReality(g,w,i,-1);
	break;
      case GDK_Tab:
      case GDK_KP_Tab:
	processed=eResetView(g,w,i);
	break;
      case GDK_F1:
	processed=eToggleLayer(g,w,i,1);
	break;
      case GDK_F2:
	processed=eToggleLayer(g,w,i,2);
	break;
      case GDK_F3:
	processed=eToggleLayer(g,w,i,3);
	break;
      case GDK_F4:
	processed=eToggleLayer(g,w,i,4);
	break;
      case GDK_F5:
	processed=eToggleLayer(g,w,i,5);
	break;
      case GDK_F6:
	processed=eToggleLayer(g,w,i,6);
	break;
      case GDK_F7:
	processed=eToggleLayer(g,w,i,7);
	break;
      case GDK_F8:
	processed=eToggleLayer(g,w,i,8);
	break;
      case GDK_F9:
	processed=eToggleLayer(g,w,i,9);
	break;
      case GDK_F10:
	processed=eToggleLayer(g,w,i,10);
	break;
      case GDK_F11:
	processed=eToggleLayer(g,w,i,11);
	break;
      case GDK_F12:
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
  return FALSE;
}

		  
void W_Init2(Winfo *w,Ginfo *g)
{
    /*XSetWindowAttributes setattr;*/
    int i,j;
    g_internal=g;
    w_internal=w;
    
    for(i=0;i<g->nwindow;i++){
        /*Ginfoの内容をコピーする。*/
        w[i].colormap=gdk_colormap_new(gdk_visual_get_system(),FALSE);
	//Color map is now unavailable in some Macs (I do not know why).
        w[i].lastColor=g->numColors-1;
        for(j=0;j<g->numColors;j++){
            w[i].colortable[j].red=g->mastercolortable[j].red;
            w[i].colortable[j].green=g->mastercolortable[j].green;
            w[i].colortable[j].blue=g->mastercolortable[j].blue;
            gdk_colormap_alloc_color(w[i].colormap,&w[i].colortable[j],FALSE,FALSE);
        }
        w[i].window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        w[i].drawarea=gtk_drawing_area_new();
        gtk_container_add(GTK_CONTAINER(w[i].window),w[i].drawarea);
        gtk_drawing_area_size(GTK_DRAWING_AREA(w[i].drawarea),w[i].screenwidth,w[i].screenheight);
        /*fprintf(stderr,"%d %d\n",w[i].screenwidth,w[i].screenheight);*/
        gtk_widget_show(w[i].drawarea);
        gtk_signal_connect(GTK_OBJECT(w[i].drawarea),"expose_event",(GtkSignalFunc)expose_event,NULL);
        gtk_signal_connect(GTK_OBJECT(w[i].drawarea),"configure_event",(GtkSignalFunc)configure_event,NULL);
        gtk_signal_connect(GTK_OBJECT(w[i].window),"motion_notify_event",(GtkSignalFunc)motion_notify_event,NULL);
        gtk_signal_connect(GTK_OBJECT(w[i].window),"button_press_event",(GtkSignalFunc)button_press_event,NULL);
        gtk_signal_connect(GTK_OBJECT(w[i].window),"button_release_event",(GtkSignalFunc)button_release_event,NULL);
        gtk_signal_connect(GTK_OBJECT(w[i].window),"key_press_event",(GtkSignalFunc)key_press_cb,NULL);
        /*
          gtk_signal_connect(GTK_OBJECT(w[i].window),"configure_event",(GtkSignalFunc)configure_event,NULL);
          gtk_signal_connect(GTK_OBJECT(w[i].window),"expose_event",(GtkSignalFunc)expose_event,NULL);
          gtk_signal_connect(GTK_OBJECT(w[i].window),"button_press_event",(GtkSignalFunc)button_press_event,NULL);
        */
        gtk_widget_set_events(w[i].window,
                              GDK_STRUCTURE_MASK
                              | GDK_EXPOSURE_MASK
                              | GDK_BUTTON_PRESS_MASK
                              | GDK_BUTTON_RELEASE_MASK
                              | GDK_KEY_PRESS_MASK
                              | GDK_BUTTON1_MOTION_MASK);
        gtk_widget_show(w[i].window);
        gtk_window_set_title((GtkWindow *)w[i].window,w[i].inputfilename);
    }
    /*
      while(1)
      {
      XEvent ev;
      XNextEvent(g->display,&ev);
      if(ev.type==MapNotify)break;
      }*/
    
    
    for(i=0;i<g->nwindow;i++){
        w[i].pixmap=gdk_pixmap_new(w[i].window->window,w[i].screenwidth,w[i].screenheight,-1);
        w[i].gc=gdk_gc_new(w[i].pixmap);
        w[i].font=gdk_font_load(FONT);
	//gdk_gc_set_stipple(w[i].gc,gdk_bitmap_create_from_data(w[i].window->window,stipple_bits,stipple_width,stipple_height));
        for(j=0;j<16;j++){
	  w[i].bitmap[j] = gdk_bitmap_create_from_data(w[i].window->window,stipple_bits[j],stipple_width,stipple_height);
	}
    }
}

void
G_loadpalettes(Ginfo *g)
{
    g->numColors=0;
    while(!feof(g->palettefile)){
        int rr,gg,bb;
        char string[4096];
        int cpos;
        
        fgets(string,sizeof(string),g->palettefile);
        cpos=0;
        while((string[cpos]==' ')||(string[cpos]=='\t')){
            cpos++;
        }
        /*named color is not available now.*/
        sscanf(string,"%d %d %d",&rr,&gg,&bb);
        g->mastercolortable[g->numColors].red=rr<<8;
        g->mastercolortable[g->numColors].green=gg<<8;
        g->mastercolortable[g->numColors].blue=bb<<8;
        g->numColors++;
    }
    fclose(g->palettefile);
}

void G_Init0(int *argc,char **argv[])
{
    gtk_init(argc,argv);
}

void G_Init2(Ginfo *g)
{
    G_loadpalettes(g);
}



int existevent(Ginfo *g)
{
    return gtk_events_pending();
}



void handleevent(Ginfo *g,Winfo w[])
{
  gtk_main_iteration();
}

#ifdef RECORD
#include <png.h>
//#include <jpeglib.h>

typedef struct{
  int    width, height;
  //JSAMPARRAY rgb;
  unsigned char**  rgb;
} sRGBImage;


void rgbimage_done( sRGBImage* img )
{
  int y;
  for(y=0;y<img->height;y++){
    free( img->rgb[y] );
  }
  free( img->rgb );
  free( img );
}



sRGBImage* rgbimage_new( int width, int height )
{
  sRGBImage* rgbimage = (sRGBImage*) malloc( sizeof(sRGBImage) );
  int y;
  rgbimage->width  = width;
  rgbimage->height = height;
  rgbimage->rgb = (unsigned char**) malloc( sizeof(char*)*height );
  for( y=0; y<height; y++ ){
    rgbimage->rgb[y] = (unsigned char*) calloc( width*3, sizeof(char) );
  }
  return rgbimage;
}



void png_save( sRGBImage* img, FILE* fp )
{
  
  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL,NULL,NULL);
  png_infop info_ptr = png_create_info_struct(png_ptr);
  png_set_IHDR(png_ptr, info_ptr, img->width, img->height,
               8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
               PNG_FILTER_TYPE_BASE);
  png_init_io(png_ptr, fp);
  png_write_info(png_ptr, info_ptr);
  png_write_image(png_ptr, img->rgb);
  png_write_end(png_ptr, info_ptr);
  png_destroy_write_struct(&png_ptr, &info_ptr);
}



void W_SaveSnapShot(Winfo *w,int windowid)
{
  GdkPixbuf *gp;
  unsigned char *buf;
  char filename[1024];
  int j;
  sRGBImage* image = rgbimage_new(w->window->allocation.width,
				  w->window->allocation.height);
  //sprintf(filename,"ppmquant 256 | pnmtopng > yaplot%02d_%05d.png",i,w->currentframe);
  //file=popen(filename,"w");
  //sprintf(filename,"yaplot%02d_%05d.ppm",i,w->currentframe);
  //file=fopen(filename,"w");
  gp=gdk_pixbuf_get_from_drawable(NULL,w->pixmap,gdk_drawable_get_colormap(w->pixmap),
				  0,0,0,0,
				  w->window->allocation.width,
				  w->window->allocation.height);
  //fprintf(file,"P3\n%d %d\n255\n",
  //	  w->window->allocation.width,
  //	  w->window->allocation.height);
  int rowstride = gdk_pixbuf_get_rowstride( gp );
  buf = gdk_pixbuf_get_pixels(gp);
  for(j=0; j<w->window->allocation.height;j++){
    int i;
    for(i=0;i<w->window->allocation.width*3; i++){
      image->rgb[j][i] = buf[i+j*rowstride];
    }
  }
  //gdk_pixbuf_unref(gp);
  g_object_unref(gp);
  //pclose(file);
  sprintf(filename,"yaplot%02d_%05d.png",windowid,w->currentframe);
  FILE* file=fopen(filename,"w");
  png_save( image, file );
  fclose(file);
  rgbimage_done( image );
}
#endif
