#include "gtk.h"
#include "os.h"
#include "yaplot.h"
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <math.h>
#include <stdio.h>

extern int debug;
extern float Eyex, Eyey, Eyez;
extern float Lookx, Looky, Lookz;
extern float Upx, Upy, Upz;
float lastthick;

void drawline2fb(Winfo *w, int x0, int y0, int x1, int y1)
{
	cairo_move_to(w->cr, x0, y0);
	cairo_line_to(w->cr, x1, y1);
	cairo_stroke(w->cr);
}

void drawpoly2fb0(Winfo *w, ivector2 *poly, int n)
{
	int i;
	cairo_move_to(w->cr, poly[n - 1].x, poly[n - 1].y);
	for (i = 0; i < n; i++) {
		cairo_line_to(w->cr, poly[i].x, poly[i].y);
	}
	cairo_close_path(w->cr);
	cairo_stroke_preserve(w->cr);
}

void drawpoly2fb1(Winfo *w, ivector2 *poly, int n)
{
	int i;
	cairo_move_to(w->cr, poly[n - 1].x, poly[n - 1].y);
	for (i = 0; i < n; i++) {
		cairo_line_to(w->cr, poly[i].x, poly[i].y);
	}
	cairo_close_path(w->cr);
	cairo_stroke(w->cr);
}

void drawpoly2fb(Winfo *w, ivector2 *poly, int n, int fill)
{
	if (fill) {
		cairo_set_line_width(w->cr, 0);
		drawpoly2fb0(w, poly, n);
		cairo_fill(w->cr);
		setlinewidth(w, lastthick);
	} else {
		drawpoly2fb1(w, poly, n);
	}
}

void drawcircle2fb(Winfo *w, int x, int y, int r, int fill)
{
	cairo_move_to(w->cr, x + r, y);
	if (fill) {
		cairo_set_line_width(w->cr, 0);
		cairo_arc(w->cr, x, y, r, 0, 2 * M_PI);
		cairo_fill(w->cr);
		setlinewidth(w, lastthick);
	} else {
		cairo_arc(w->cr, x, y, r, 0, 2 * M_PI);
		cairo_stroke(w->cr);
	}
}

void drawstick2fb(Winfo *w, int x0, int y0, int x1, int y1, int r, int fill,
		  int arrowtype)
{
	if (arrowtype == 0) {
		int thick = r * 2;
		if (thick == 0) {
			thick = 1;
		}
		cairo_set_line_width(w->cr, thick);
		drawline2fb(w, x0, y0, x1, y1);
		setlinewidth(w, lastthick);
	} else {
		int dx = x1 - x0;
		int dy = y1 - y0;
		int dd = (int)sqrt(dx * dx + dy * dy);
		if (dd != 0) {
			int rx = r * dx / dd;
			int ry = r * dy / dd;
			if (arrowtype == 1) {
				// normal arrow with a head.
				setlinewidth(w, 1);
				cairo_move_to(w->cr, x0, y0);
				cairo_line_to(w->cr, x1, y1);
				cairo_move_to(w->cr, x1 - 2 * rx + ry,
					      y1 - 2 * ry - rx);
				cairo_line_to(w->cr, x1, y1);
				cairo_line_to(w->cr, x1 - 2 * rx - ry,
					      y1 - 2 * ry + rx);
				cairo_stroke(w->cr);
				setlinewidth(w, lastthick);
			} else if (arrowtype == 2) {
				// dart
				ivector2 p[3];
				p[0].x = x1;
				p[0].y = y1;
				p[1].x = x0 + ry;
				p[1].y = y0 - rx;
				p[2].x = x0 - ry;
				p[2].y = y0 + rx;
				drawpoly2fb(w, p, 3, fill);
			}
		}
	}
}

void setlinewidth(Winfo *w, float thick)
{
	lastthick = thick;
	if (thick == 0) {
		thick = 1;
	}
	cairo_set_line_width(w->cr, thick);
}

void setfgcolor(Winfo *w, int palette)
{
	cairo_set_source_rgb(w->cr, (float)w->colortable[palette].red / 65536.0,
			     (float)w->colortable[palette].green / 65536.0,
			     (float)w->colortable[palette].blue / 65536.0);
}

void settpcolor(Winfo *w, int palette)
{
	cairo_set_source_rgba(
	    w->cr, (float)w->colortable[palette].red / 65536.0,
	    (float)w->colortable[palette].green / 65536.0,
	    (float)w->colortable[palette].blue / 65536.0, 0.5);
}

void overridepalette(Winfo *w, int palette, int red, int green, int blue)
{
	w->colortable[palette].red = red << 8;
	w->colortable[palette].green = green << 8;
	w->colortable[palette].blue = blue << 8;
	if (palette > w->lastColor) {
		w->lastColor = palette;
	}
}

void drawstring2fb(Winfo *w, int x, int y, char *str, int length)
{
	cairo_move_to(w->cr, x, y);
	cairo_show_text(w->cr, str);
}

void clearfb(Winfo *w)
{
	setfgcolor(w, 1);
	cairo_paint(w->cr);
}

void exposefb(Winfo *w)
{
	cairo_t *cr = cairo_create(w->surface);
	cairo_set_source_surface(cr, w->surface, 0, 0);

	GtkAllocation g_alloc;
	gtk_widget_get_allocation(w->drawarea, &g_alloc);
	if (debug) {
		fprintf(stderr, "Forced expose::%d %d\n", g_alloc.width,
			g_alloc.height);
	}
	cairo_paint(cr);
	gtk_widget_queue_draw_area(w->drawarea, 0, 0, g_alloc.width,
				   g_alloc.height);
	cairo_destroy(cr);
}

Ginfo *g_internal;
Winfo *w_internal;

int WhichWidget(GtkWidget *widget)
{
	int i;
	for (i = 0; i < g_internal->nwindow; i++) {
		if (w_internal[i].drawarea == widget ||
		    w_internal[i].window == widget) {
			return i;
		}
	}
	exit(1);
}

static gboolean draw_cb(GtkWidget *widget, cairo_t *cr, gpointer data)
{
	int i = WhichWidget(widget);
	if (debug) {
		GtkAllocation g_alloc;
		gtk_widget_get_allocation(widget, &g_alloc);
		fprintf(stderr, "Expose[%d]::%d %d\n", i, g_alloc.width,
			g_alloc.height);
	}
	cairo_set_source_surface(cr, w_internal[i].surface, 0, 0);
	cairo_paint(cr);
	// cairo_paint(w_internal[i].cr);

	// w_internal[i].status|=REDRAW;
	return FALSE;
}

static gint configure_event(GtkWidget *widget, GdkEventConfigure *event)
{
	int i = WhichWidget(widget);
	if (w_internal[i].surface) {
		cairo_surface_finish(w_internal[i].surface);
	}
	GtkAllocation g_alloc;
	gtk_widget_get_allocation(widget, &g_alloc);
	// drawarea and surface are combined here....2019-06
	w_internal[i].surface = cairo_image_surface_create(
	    CAIRO_FORMAT_ARGB32, g_alloc.width, g_alloc.height);
	cairo_t *cr = cairo_create(w_internal[i].surface);
	w_internal[i].cr = cr;
	cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
	cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
	cairo_set_font_size(cr, 14);
	cairo_select_font_face(cr, "Helvetica", CAIRO_FONT_SLANT_ITALIC,
			       CAIRO_FONT_WEIGHT_NORMAL);
#ifdef CAIRO_ANTIALIAS_FAST
	cairo_set_antialias(cr, CAIRO_ANTIALIAS_FAST); // for speed
#endif
	w_internal[i].screenwidth = g_alloc.width;
	w_internal[i].screenheight = g_alloc.height;
	w_internal[i].status |= REDRAW;
	return FALSE;
}

/*global*/
extern int prefix;
int crawl = 0;
int originx = 0, originy = 0;

static gint motion_notify_event(GtkWidget *widget, GdkEventMotion *event)
{
	int x, y;
	GdkModifierType state;
	if (debug)
		fprintf(stderr, "Motion::\n");

	if (event->is_hint) {
		gdk_window_get_device_position(event->window, event->device, &x,
					       &y, &state);
	} else {
		x = event->x;
		y = event->y;
		state = event->state;
	}
	if (state & GDK_BUTTON1_MASK) {
		if ((originx != x) || (originy != y)) {
			int i;
			float head, bank;
			float tick;
			if (state & GDK_SHIFT_MASK) {
				tick = 1.0 / 400.0;
			} else {
				tick = 10.0 / 400.0;
			}
			bank = +(originy - y) * tick;
			head = +(originx - x) * tick;
			originx = x;
			originy = y;
			i = WhichWidget(widget);
			if (w_internal[i].async) {
				WindowMotion(&w_internal[i], head, bank);
			} else {
				for (i = 0; i < g_internal->nwindow; i++) {
					WindowMotion(&w_internal[i], head,
						     bank);
				}
			}
		}
	}
	return FALSE;
}

static gint button_press_event(GtkWidget *widget, GdkEventButton *event)
{
	int i;
	if (debug)
		fprintf(stderr, "Pressed::\n");

	originx = event->x;
	originy = event->y;
	i = WhichWidget(widget);
	if (w_internal[i].async) {
		w_internal[i].AbsoluteRotationMode = 1;
	} else {
		for (i = 0; i < g_internal->nwindow; i++) {
			w_internal[i].AbsoluteRotationMode = 1;
		}
	}
	return FALSE;
}

static gint button_release_event(GtkWidget *widget, GdkEventButton *event)
{
	int i;
	if (debug)
		fprintf(stderr, "Released::\n");
	i = WhichWidget(widget);
	if (w_internal[i].async) {
		w_internal[i].wp = w_internal[i].wb = w_internal[i].wh = 0;
		w_internal[i].AbsoluteRotationMode = 0;
		/*w_internal[i].status |=REDRAW;*/
	} else {
		for (i = 0; i < g_internal->nwindow; i++) {
			w_internal[i].wp = w_internal[i].wb = w_internal[i].wh =
			    0;
			w_internal[i].AbsoluteRotationMode = 0;
			/*w_internal[i].status |=REDRAW;*/
		}
	}
	return FALSE;
}

void W_StopRecording(Winfo *w)
{
#ifdef FFMPEG
	if (w->RecordMode == 2) {
		// close the pipe to FFMPEG
		pclose(w->pffmpeg);
	}
#endif
	w->RecordMode = 0;
	gtk_window_set_title((GtkWindow *)w->window, "Done");
}

void W_StartRecording(Winfo *w, int mode, int windowid, int fps)
{
	/*automatic recording mode; will save each frame to file*/
	w->RecordMode = mode; // 1: still pictures PNG 2: video h264
	w->df = w->wh = w->wp = w->wb = 0;
	gtk_window_set_title((GtkWindow *)w->window, "Recording");
#ifdef FFMPEG
	if (mode == 2) {
		// open the pipe to FFMPEG
		char output0[1000];
		char output[1000];
		sprintf(output0, FFMPEG, fps);
		// sprintf(output, "%s yaplot%02d.mp4", output0, windowid);
		snprintf(output, sizeof(output), "%.*s yaplot%02d.mp4",
			 (int)(sizeof(output) - strlen(output0) - 11), output0,
			 windowid);
		w->pffmpeg = popen(output, "w");
	}
#endif
}

static gint key_press_cb(GtkWidget *widget, GdkEventKey *event)
{
	int i = WhichWidget(widget);
	int key = event->keyval;
	int modstatus = event->state;
	Winfo *w = w_internal; /*just for compat*/
	Ginfo *g = g_internal; /*just for compat*/
	if (debug) {
		fprintf(stderr, "Key: %d/Status: %d\n", key, modstatus);
	}
	/*このあたりの処理はすべてのコマンドに拡張すべき。*/
	if ((event->length == 1) && (event->string[0] >= '0') &&
	    (event->string[0] <= '9')) {
		prefix = prefix * 10 + event->string[0] - '0';
	} else if (w[i].RecordMode) {
		switch (key) {
		case GDK_KEY_r:
			eStopRecording(g, w);
			break;
		case GDK_KEY_q:
		case GDK_KEY_Break:
			eStopRecording(g, w);
			exit(0);
			/*go absolute*/
		}
	} else {
		int processed;
		switch (key) {
			/*仕様を大幅に変更しよう*/
		case GDK_KEY_q:
		case GDK_KEY_Break:
			processed = eQuit(g, w, i);
			break;
		case GDK_KEY_g:
		case GDK_KEY_G:
		case GDK_KEY_Return:
		case GDK_KEY_KP_Enter:
			/*go absolute*/
			processed = eGotoFrame(
			    g, w, i,
			    (((modstatus & GDK_SHIFT_MASK) && (prefix == 0))
				 ? LASTFRAME
				 : prefix));
			break;
		case GDK_KEY_Page_Down:
		case GDK_KEY_n:
		case GDK_KEY_N:
		case GDK_KEY_KP_Page_Down:
			/*go forward*/
			processed =
			    eGoRelativeFrame(g, w, i, (prefix ? +prefix : +1),
					     (modstatus & GDK_SHIFT_MASK));
			break;
		case GDK_KEY_c:
		case GDK_KEY_C:
			// centering
			processed = eToggleCentering(g, w, i);
			break;
		case GDK_KEY_percent:
			// slicing
			processed = eToggleSlicing(g, w, i);
			break;
		case GDK_KEY_braceright:
			processed = eSliceMove(g, w, i, 1);
			break;
		case GDK_KEY_braceleft:
			processed = eSliceMove(g, w, i, -1);
			break;
		case GDK_KEY_R:
#ifdef FFMPEG
			// if FFMPEG is defined, key R records the pictures into
			// a video prefix value is used as the frame rate
			processed =
			    eStartRecording(g, w, 2, (prefix ? +prefix : +10));
			break;
#endif
		case GDK_KEY_r:
			processed = eStartRecording(g, w, 1, 1);
			break;
#ifndef win32
		case GDK_KEY_u:
		case GDK_KEY_U:
			processed = ePushView(g, w, i);
			break;
		case GDK_KEY_o:
		case GDK_KEY_O:
			processed = ePopView(g, w, i);
			break;
#endif
		case GDK_KEY_v:
		case GDK_KEY_V:
			processed = eToggleVerbosity(g, w, i);
			break;
		case GDK_KEY_Page_Up:
		case GDK_KEY_p:
		case GDK_KEY_P:
		case GDK_KEY_KP_Page_Up:
			processed =
			    eGoRelativeFrame(g, w, i, (prefix ? -prefix : -1),
					     (modstatus & GDK_SHIFT_MASK));
			break;
			/*first frame*/
		case GDK_KEY_Home:
		case GDK_KEY_KP_Home:
			processed = eGotoFrame(g, w, i, 0);
			break;
			/*last frame*/
		case GDK_KEY_End:
		case GDK_KEY_KP_End:
			processed = eGotoFrame(g, w, i, LASTFRAME);
			break;
			/*Stop motion*/
		case GDK_KEY_space:
		case GDK_KEY_KP_Space:
			processed = eStopMotion(g, w, i);
			break;
			/*Stop rotation*/
		case GDK_KEY_Pause:
		case GDK_KEY_exclam:
			processed = eStopRotation(g, w, i);
			break;
			/*telescopic*/
		case GDK_KEY_bracketright:
			processed = eWiden(g, w, i, (prefix ? -prefix : -1));
			break;
		case GDK_KEY_asterisk:
		case GDK_KEY_KP_Multiply:
			processed = eZoom(g, w, i, (prefix ? +prefix : +1));
			break;
			/*wide*/
		case GDK_KEY_bracketleft:
			processed = eWiden(g, w, i, (prefix ? +prefix : +1));
			break;
		case GDK_KEY_slash:
		case GDK_KEY_KP_Divide:
			processed = eZoom(g, w, i, (prefix ? -prefix : -1));
			break;
			/*heading rotation*/
		case GDK_KEY_H:
		case GDK_KEY_h:
		case GDK_KEY_Left:
		case GDK_KEY_KP_Left:
			processed = eHeading(g, w, i, (prefix ? +prefix : +1),
					     (modstatus & GDK_SHIFT_MASK));
			break;
			/*heading rotation*/
		case GDK_KEY_L:
		case GDK_KEY_l:
		case GDK_KEY_Right:
		case GDK_KEY_KP_Right:
			processed = eHeading(g, w, i, (prefix ? -prefix : -1),
					     (modstatus & GDK_SHIFT_MASK));
			break;
			/*banking rotation*/
		case GDK_KEY_K:
		case GDK_KEY_k:
		case GDK_KEY_Up:
		case GDK_KEY_KP_Up:
			processed = eBanking(g, w, i, (prefix ? +prefix : +1),
					     (modstatus & GDK_SHIFT_MASK));
			break;
			/*banking rotation*/
		case GDK_KEY_J:
		case GDK_KEY_j:
		case GDK_KEY_Down:
		case GDK_KEY_KP_Down:
			processed = eBanking(g, w, i, (prefix ? -prefix : -1),
					     (modstatus & GDK_SHIFT_MASK));
			break;
			/*flush prefix*/
		case GDK_KEY_Escape:
			processed = 1;
			break;
		case GDK_KEY_s:
		case GDK_KEY_S:
			processed = eToggleSync(g, w, i);
			break;
		case GDK_KEY_f:
		case GDK_KEY_F:
			processed =
			    eToggleLayer(g, w, i, (prefix ? prefix : 1));
			break;
		case GDK_KEY_x:
			processed = eResetView(g, w, i, -40, 0, 0);
			break;
		case GDK_KEY_X:
			processed = eResetView(g, w, i, +40, 0, 0);
			break;
		case GDK_KEY_y:
			processed = eResetView(g, w, i, 0, -40, 0);
			break;
		case GDK_KEY_Y:
			processed = eResetView(g, w, i, 0, +40, 0);
			break;
		case GDK_KEY_z:
			processed = eResetView(g, w, i, 0, 0, -40);
			break;
		case GDK_KEY_Z:
			processed = eResetView(g, w, i, 0, 0, +40);
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
		case GDK_KEY_Insert:
		case GDK_KEY_KP_Insert:
		case GDK_KEY_parenright:
			processed = eRelativeThickness(g, w, i,
						       (prefix ? +prefix : +1));
			break;
		case GDK_KEY_Delete:
		case GDK_KEY_KP_Delete:
		case GDK_KEY_parenleft:
			processed = eRelativeThickness(g, w, i,
						       (prefix ? -prefix : -1));
			break;
			/*better quality*/
		case GDK_KEY_plus:
		case GDK_KEY_KP_Add:
			processed = eRelativeReality(g, w, i, +1);
			break;
			/*less quality*/
		case GDK_KEY_minus:
		case GDK_KEY_KP_Subtract:
			processed = eRelativeReality(g, w, i, -1);
			break;
		case GDK_KEY_Tab:
		case GDK_KEY_KP_Tab:
			processed = eResetView(g, w, i, Eyex, Eyey, Eyez);
			break;
		case GDK_KEY_F1:
			processed = eToggleLayer(g, w, i, 1);
			break;
		case GDK_KEY_F2:
			processed = eToggleLayer(g, w, i, 2);
			break;
		case GDK_KEY_F3:
			processed = eToggleLayer(g, w, i, 3);
			break;
		case GDK_KEY_F4:
			processed = eToggleLayer(g, w, i, 4);
			break;
		case GDK_KEY_F5:
			processed = eToggleLayer(g, w, i, 5);
			break;
		case GDK_KEY_F6:
			processed = eToggleLayer(g, w, i, 6);
			break;
		case GDK_KEY_F7:
			processed = eToggleLayer(g, w, i, 7);
			break;
		case GDK_KEY_F8:
			processed = eToggleLayer(g, w, i, 8);
			break;
		case GDK_KEY_F9:
			processed = eToggleLayer(g, w, i, 9);
			break;
		case GDK_KEY_F10:
			processed = eToggleLayer(g, w, i, 10);
			break;
		case GDK_KEY_F11:
			processed = eToggleLayer(g, w, i, 11);
			break;
		case GDK_KEY_F12:
			processed = eToggleLayer(g, w, i, 12);
			break;
		default:
			processed = 0;
			break;
		}
		if (processed) {
			prefix = 0;
		}
	}
	return FALSE;
}

void W_Init2(Winfo *w, Ginfo *g)
{
	/*XSetWindowAttributes setattr;*/
	int i, j;
	g_internal = g;
	w_internal = w;

	for (i = 0; i < g->nwindow; i++) {
		w[i].lastColor = g->numColors - 1;
		for (j = 0; j < g->numColors; j++) {
			w[i].colortable[j].red = g->mastercolortable[j].red;
			w[i].colortable[j].green = g->mastercolortable[j].green;
			w[i].colortable[j].blue = g->mastercolortable[j].blue;
		}
		w[i].window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
		if (debug)
			fprintf(stderr, "Setup window X %d.\n", i);
		if (debug)
			fprintf(stderr, "Setup window Y %d.\n", i);
		w[i].drawarea = gtk_drawing_area_new();
		//
		gtk_container_add(GTK_CONTAINER(w[i].window), w[i].drawarea);

		gtk_widget_set_size_request(w[i].drawarea, w[i].screenwidth,
					    w[i].screenheight);
		// if(debug)fprintf(stderr,"Setup window %d
		// %d.\n",i,(int)w[i].drawarea);
		gtk_widget_show(w[i].drawarea);
		g_signal_connect(w[i].drawarea, "draw", G_CALLBACK(draw_cb),
				 NULL);
		g_signal_connect(w[i].drawarea, "configure_event",
				 G_CALLBACK(configure_event), NULL);
		if (debug)
			fprintf(stderr, "Setup window A0 %d.\n", i);
		g_signal_connect(w[i].window, "motion_notify_event",
				 G_CALLBACK(motion_notify_event), NULL);
		if (debug)
			fprintf(stderr, "Setup window A1 %d.\n", i);
		g_signal_connect(w[i].window, "button_press_event",
				 G_CALLBACK(button_press_event), NULL);
		if (debug)
			fprintf(stderr, "Setup window A2 %d.\n", i);
		g_signal_connect(w[i].window, "button_release_event",
				 G_CALLBACK(button_release_event), NULL);
		if (debug)
			fprintf(stderr, "Setup window A3 %d.\n", i);
		g_signal_connect(w[i].window, "key_press_event",
				 G_CALLBACK(key_press_cb), NULL);
		if (debug)
			fprintf(stderr, "Setup window A4 %d.\n", i);
		gtk_widget_set_events(
		    w[i].window,
		    GDK_STRUCTURE_MASK | GDK_EXPOSURE_MASK |
			GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK |
			GDK_KEY_PRESS_MASK | GDK_POINTER_MOTION_MASK |
			GDK_BUTTON1_MOTION_MASK); // this causes an error.
		if (debug)
			fprintf(stderr, "Setup window C %d.\n", i);
		gtk_widget_show(w[i].window);
		if (debug)
			fprintf(stderr, "Setup window D %d.\n", i);
		gtk_window_set_title((GtkWindow *)w[i].window,
				     w[i].inputfilename);
	}
	for (i = 0; i < g->nwindow; i++) {
		if (debug)
			fprintf(stderr, "Setup window E %d.\n", i);
		w[i].surface = cairo_image_surface_create(
		    CAIRO_FORMAT_ARGB32, w[i].screenwidth, w[i].screenheight);
		// if(debug)fprintf(stderr,"Setup window F %d
		// %d.\n",i,(int)w[i].surface);
		cairo_t *cr = cairo_create(w[i].surface);
		// if(debug)fprintf(stderr,"Setup window G %d %d.\n",i,
		// (int)cr);
		w[i].cr = cr;
		if (debug)
			fprintf(stderr, "Setup window H %d.\n", i);
		cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND); // THIS FAILS
		if (debug)
			fprintf(stderr, "Setup window I %d.\n", i);
		cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
		if (debug)
			fprintf(stderr, "Setup window J %d.\n", i);
		cairo_set_font_size(cr, 14);
		cairo_select_font_face(cr, "Helvetica", CAIRO_FONT_SLANT_ITALIC,
				       CAIRO_FONT_WEIGHT_NORMAL);
#ifdef CAIRO_ANTIALIAS_FAST
		cairo_set_antialias(cr, CAIRO_ANTIALIAS_FAST); // for speed
#endif
		if (debug)
			fprintf(stderr, "Setup window L %d.\n", i);
	}
	if (debug)
		fprintf(stderr, "Done settingup windows.\n");
}

void G_loadpalettes(Ginfo *g)
{
	g->numColors = 0;
	while (!feof(g->palettefile)) {
		int rr, gg, bb;
		char string[4096];
		int cpos;

		fgets(string, sizeof(string), g->palettefile);
		cpos = 0;
		while ((string[cpos] == ' ') || (string[cpos] == '\t')) {
			cpos++;
		}
		sscanf(string, "%d %d %d", &rr, &gg, &bb);
		g->mastercolortable[g->numColors].red = rr << 8;
		g->mastercolortable[g->numColors].green = gg << 8;
		g->mastercolortable[g->numColors].blue = bb << 8;
		g->numColors++;
	}
	fclose(g->palettefile);
}

void G_Init0(int *argc, char **argv[]) { gtk_init(argc, argv); }

void G_Init2(Ginfo *g) { G_loadpalettes(g); }

int existevent(Ginfo *g) { return gtk_events_pending(); }

void handleevent(Ginfo *g, Winfo w[]) { gtk_main_iteration(); }

cairo_status_t my_write_function(void *closure, unsigned char *data,
				 unsigned int length)
{
	fwrite(data, 1, length, (FILE *)closure);
	return CAIRO_STATUS_SUCCESS;
}

void W_SaveSnapShot(Winfo *w, int windowid)
{
#ifdef FFMPEG
	if (w->RecordMode == 2) {
		cairo_surface_write_to_png_stream(
		    w->surface, (cairo_write_func_t)my_write_function,
		    w->pffmpeg);
		return;
	}
#endif
	char filename[1024];
	sprintf(filename, "yaplot%02d_%05d.png", windowid, w->currentframe);
	cairo_surface_write_to_png(w->surface, filename);
}
