/*
 * SDL2 + Cairo display backend for Yaplot.
 * Window and event handling: SDL2. Drawing: Cairo (same as before).
 */
#include "display.h"
#include "os.h"
#include "yaplot.h"
#include <cairo.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>

#define YAP_SHIFT_MASK 1

extern int debug;
extern int prefix;
extern float Eyex, Eyey, Eyez;
extern float Lookx, Looky, Lookz;
extern float Upx, Upy, Upz;

float lastthick;

typedef struct {
	SDL_Renderer *renderer;
	SDL_Texture *texture;
} SDL2Context;

static Ginfo *g_internal;
static Winfo *w_internal;
static SDL_Event saved_event;
static int have_saved_event;

static int window_id_to_index(Ginfo *g, Winfo *w, Uint32 windowID)
{
	int i;
	for (i = 0; i < g->nwindow; i++) {
		if (SDL_GetWindowID((SDL_Window *)w[i].window) == windowID)
			return i;
	}
	return -1;
}

/* --- Drawing (Cairo, unchanged from GTK backend) --- */

void drawline2fb(Winfo *w, int x0, int y0, int x1, int y1)
{
	cairo_move_to(w->cr, x0, y0);
	cairo_line_to(w->cr, x1, y1);
	cairo_stroke(w->cr);
}

static void drawpoly2fb0(Winfo *w, ivector2 *poly, int n)
{
	int i;
	cairo_move_to(w->cr, poly[n - 1].x, poly[n - 1].y);
	for (i = 0; i < n; i++) {
		cairo_line_to(w->cr, poly[i].x, poly[i].y);
	}
	cairo_close_path(w->cr);
	cairo_stroke_preserve(w->cr);
}

static void drawpoly2fb1(Winfo *w, ivector2 *poly, int n)
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
		if (thick == 0)
			thick = 1;
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
				setlinewidth(w, 1);
				cairo_move_to(w->cr, x0, y0);
				cairo_line_to(w->cr, x1, y1);
				cairo_move_to(w->cr, x1 - 2 * rx + ry, y1 - 2 * ry - rx);
				cairo_line_to(w->cr, x1, y1);
				cairo_line_to(w->cr, x1 - 2 * rx - ry, y1 - 2 * ry + rx);
				cairo_stroke(w->cr);
				setlinewidth(w, lastthick);
			} else if (arrowtype == 2) {
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
	if (thick == 0)
		thick = 1;
	cairo_set_line_width(w->cr, thick);
}

void setfgcolor(Winfo *w, int palette)
{
	cairo_set_source_rgb(w->cr,
			    (float)w->colortable[palette].red / 65536.0,
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
	w->colortable[palette].red = (unsigned short)(red << 8);
	w->colortable[palette].green = (unsigned short)(green << 8);
	w->colortable[palette].blue = (unsigned short)(blue << 8);
	if (palette > w->lastColor)
		w->lastColor = palette;
}

void drawstring2fb(Winfo *w, int x, int y, char *str, int length)
{
	(void)length;
	cairo_move_to(w->cr, x, y);
	cairo_show_text(w->cr, str);
}

void setsolidfill(Winfo *w)
{
	(void)w;
}

void setstippledfill(Winfo *w)
{
	(void)w;
}

void setstipple(Winfo *w, int i)
{
	(void)w;
	(void)i;
}

void clearfb(Winfo *w)
{
	setfgcolor(w, 1);
	cairo_paint(w->cr);
}

static void present_window(Winfo *w)
{
	SDL2Context *ctx = (SDL2Context *)w->drawarea;
	if (!ctx || !ctx->renderer || !ctx->texture || !w->surface)
		return;
	cairo_surface_flush(w->surface);
	SDL_UpdateTexture(ctx->texture, NULL,
			  cairo_image_surface_get_data(w->surface),
			  cairo_image_surface_get_stride(w->surface));
	SDL_RenderClear(ctx->renderer);
	SDL_RenderCopy(ctx->renderer, ctx->texture, NULL, NULL);
	SDL_RenderPresent(ctx->renderer);
}

void exposefb(Winfo *w)
{
	if (debug)
		fprintf(stderr, "exposefb: %d %d\n", w->screenwidth,
			w->screenheight);
	present_window(w);
}

/* --- Event handling --- */

static int originx, originy;

static void handle_key(int i, SDL_Keycode key, Uint16 modstate)
{
	Winfo *w = w_internal;
	Ginfo *g = g_internal;
	int modstatus = (modstate & KMOD_SHIFT) ? YAP_SHIFT_MASK : 0;

	if (debug)
		fprintf(stderr, "Key: %d mod: %d\n", (int)key, modstatus);

	/* Digit prefix */
	if (key >= SDLK_0 && key <= SDLK_9) {
		prefix = prefix * 10 + (int)(key - SDLK_0);
		return;
	}

	if (w[i].RecordMode) {
		if (key == SDLK_r) {
			eStopRecording(g, w);
			return;
		}
		if (key == SDLK_q) {
			eStopRecording(g, w);
			exit(0);
		}
		return;
	}

	{
		int processed = 0;
		switch (key) {
		case SDLK_q:
			processed = eQuit(g, w, i);
			break;
		case SDLK_g:
		case SDLK_RETURN:
		case SDLK_KP_ENTER:
			processed = eGotoFrame(g, w, i,
					       (modstatus && prefix == 0)
						   ? LASTFRAME
						   : prefix);
			break;
		case SDLK_PAGEDOWN:
		case SDLK_n:
			processed =
			    eGoRelativeFrame(g, w, i, prefix ? prefix : 1,
					     modstatus);
			break;
		case SDLK_c:
			processed = eToggleCentering(g, w, i);
			break;
		case SDLK_PERCENT:
			processed = eToggleSlicing(g, w, i);
			break;
		case SDLK_RIGHTBRACKET:
			processed = (modstate & KMOD_SHIFT)
				       ? eSliceMove(g, w, i, 1)
				       : eWiden(g, w, i, -1);
			break;
		case SDLK_LEFTBRACKET:
			processed = (modstate & KMOD_SHIFT)
				       ? eSliceMove(g, w, i, -1)
				       : eWiden(g, w, i, 1);
			break;
		case SDLK_r:
			if (modstate & KMOD_SHIFT) {
#ifdef FFMPEG
				processed = eStartRecording(g, w, 2,
							prefix ? prefix : 10);
#endif
			} else {
				processed = eStartRecording(g, w, 1, 1);
			}
			break;
#ifndef win32
		case SDLK_u:
			processed = ePushView(g, w, i);
			break;
		case SDLK_o:
			processed = ePopView(g, w, i);
			break;
#endif
		case SDLK_v:
			processed = eToggleVerbosity(g, w, i);
			break;
		case SDLK_PAGEUP:
		case SDLK_p:
			processed =
			    eGoRelativeFrame(g, w, i, prefix ? -prefix : -1,
					     modstatus);
			break;
		case SDLK_HOME:
			processed = eGotoFrame(g, w, i, 0);
			break;
		case SDLK_END:
			processed = eGotoFrame(g, w, i, LASTFRAME);
			break;
		case SDLK_SPACE:
			processed = eStopMotion(g, w, i);
			break;
		case SDLK_PAUSE:
		case SDLK_EXCLAIM:
			processed = eStopRotation(g, w, i);
			break;
		case SDLK_KP_MULTIPLY:
		case SDLK_ASTERISK:
			processed = eZoom(g, w, i, prefix ? prefix : 1);
			break;
		case SDLK_KP_DIVIDE:
		case SDLK_SLASH:
			processed = eZoom(g, w, i, prefix ? -prefix : -1);
			break;
		case SDLK_h:
		case SDLK_LEFT:
			processed = eHeading(g, w, i, prefix ? prefix : 1,
					    modstatus);
			break;
		case SDLK_l:
		case SDLK_RIGHT:
			processed = eHeading(g, w, i, prefix ? -prefix : -1,
					    modstatus);
			break;
		case SDLK_k:
		case SDLK_UP:
			processed = eBanking(g, w, i, prefix ? prefix : 1,
					    modstatus);
			break;
		case SDLK_j:
		case SDLK_DOWN:
			processed = eBanking(g, w, i, prefix ? -prefix : -1,
					    modstatus);
			break;
		case SDLK_ESCAPE:
			processed = 1;
			break;
		case SDLK_s:
			processed = eToggleSync(g, w, i);
			break;
		case SDLK_f:
			processed = eToggleLayer(g, w, i, prefix ? prefix : 1);
			break;
		case SDLK_x:
			processed = (modstate & KMOD_SHIFT)
					? eResetView(g, w, i, +40, 0, 0)
					: eResetView(g, w, i, -40, 0, 0);
			break;
		case SDLK_y:
			processed = (modstate & KMOD_SHIFT)
					? eResetView(g, w, i, 0, +40, 0)
					: eResetView(g, w, i, 0, -40, 0);
			break;
		case SDLK_z:
			processed = (modstate & KMOD_SHIFT)
					? eResetView(g, w, i, 0, 0, +40)
					: eResetView(g, w, i, 0, 0, -40);
			break;
		case SDLK_INSERT:
		case SDLK_RIGHTPAREN:
			processed =
			    eRelativeThickness(g, w, i, prefix ? prefix : 1);
			break;
		case SDLK_DELETE:
		case SDLK_LEFTPAREN:
			processed =
			    eRelativeThickness(g, w, i, prefix ? -prefix : -1);
			break;
		case SDLK_KP_PLUS:
		case SDLK_PLUS:
			processed = eRelativeReality(g, w, i, 1);
			break;
		case SDLK_KP_MINUS:
		case SDLK_MINUS:
			processed = eRelativeReality(g, w, i, -1);
			break;
		case SDLK_TAB:
			processed = eResetView(g, w, i, Eyex, Eyey, Eyez);
			break;
		case SDLK_F1:
			processed = eToggleLayer(g, w, i, 1);
			break;
		case SDLK_F2:
			processed = eToggleLayer(g, w, i, 2);
			break;
		case SDLK_F3:
			processed = eToggleLayer(g, w, i, 3);
			break;
		case SDLK_F4:
			processed = eToggleLayer(g, w, i, 4);
			break;
		case SDLK_F5:
			processed = eToggleLayer(g, w, i, 5);
			break;
		case SDLK_F6:
			processed = eToggleLayer(g, w, i, 6);
			break;
		case SDLK_F7:
			processed = eToggleLayer(g, w, i, 7);
			break;
		case SDLK_F8:
			processed = eToggleLayer(g, w, i, 8);
			break;
		case SDLK_F9:
			processed = eToggleLayer(g, w, i, 9);
			break;
		case SDLK_F10:
			processed = eToggleLayer(g, w, i, 10);
			break;
		case SDLK_F11:
			processed = eToggleLayer(g, w, i, 11);
			break;
		case SDLK_F12:
			processed = eToggleLayer(g, w, i, 12);
			break;
		default:
			break;
		}
		if (processed)
			prefix = 0;
	}
}

static void handle_mouse_motion(int i, int x, int y, Uint32 state)
{
	Winfo *w = w_internal;
	Ginfo *g = g_internal;
	if (state & SDL_BUTTON_LMASK) {
		if (originx != x || originy != y) {
			float tick = (state & KMOD_SHIFT) ? (1.0f / 400.0f)
							: (10.0f / 400.0f);
			float bank = (float)(originy - y) * tick;
			float head = (float)(originx - x) * tick;
			originx = x;
			originy = y;
			if (w[i].async)
				WindowMotion(&w[i], head, bank);
			else {
				int j;
				for (j = 0; j < g->nwindow; j++)
					WindowMotion(&w[j], head, bank);
			}
		}
	}
}

static void handle_mousewheel(int i, int y)
{
	if (y != 0) {
		int fov_change = (y > 0) ? 1 : -1;
		eWiden(g_internal, w_internal, i, fov_change);
	}
}

static void handle_window_event(Ginfo *g, Winfo *w, const SDL_WindowEvent *ev)
{
	int i = window_id_to_index(g, w, ev->windowID);
	if (i < 0)
		return;
	if (ev->event == SDL_WINDOWEVENT_RESIZED) {
		SDL2Context *ctx = (SDL2Context *)w[i].drawarea;
		if (w[i].surface)
			cairo_surface_destroy(w[i].surface);
		if (w[i].cr)
			cairo_destroy(w[i].cr);
		if (ctx && ctx->texture)
			SDL_DestroyTexture(ctx->texture);
		w[i].screenwidth = ev->data1;
		w[i].screenheight = ev->data2;
		w[i].surface = cairo_image_surface_create(
		    CAIRO_FORMAT_ARGB32, w[i].screenwidth, w[i].screenheight);
		w[i].cr = cairo_create(w[i].surface);
		cairo_set_line_join(w[i].cr, CAIRO_LINE_JOIN_ROUND);
		cairo_set_line_cap(w[i].cr, CAIRO_LINE_CAP_ROUND);
		cairo_set_font_size(w[i].cr, 14);
		cairo_select_font_face(w[i].cr, "Helvetica",
				       CAIRO_FONT_SLANT_NORMAL,
				       CAIRO_FONT_WEIGHT_NORMAL);
#ifdef CAIRO_ANTIALIAS_FAST
		cairo_set_antialias(w[i].cr, CAIRO_ANTIALIAS_FAST);
#endif
		if (ctx && ctx->renderer) {
			ctx->texture = SDL_CreateTexture(
			    ctx->renderer, SDL_PIXELFORMAT_ARGB8888,
			    SDL_TEXTUREACCESS_STREAMING, w[i].screenwidth,
			    w[i].screenheight);
		}
		w[i].status |= REDRAW;
	}
}

void handleevent(Ginfo *g, Winfo w[])
{
	SDL_Event *e = &saved_event;
	int i;

	switch (e->type) {
	case SDL_QUIT:
		eQuit(g, w, 0);
		break;
	case SDL_KEYDOWN:
		i = window_id_to_index(g, w, e->key.windowID);
		if (i >= 0 && !e->key.repeat)
			handle_key(i, e->key.keysym.sym, e->key.keysym.mod);
		break;
	case SDL_MOUSEBUTTONDOWN:
		if (e->button.button == SDL_BUTTON_LEFT) {
			i = window_id_to_index(g, w, e->button.windowID);
			if (i >= 0) {
				originx = e->button.x;
				originy = e->button.y;
				if (w[i].async)
					w[i].AbsoluteRotationMode = 1;
				else {
					int j;
					for (j = 0; j < g->nwindow; j++)
						w[j].AbsoluteRotationMode = 1;
				}
			}
		}
		break;
	case SDL_MOUSEBUTTONUP:
		if (e->button.button == SDL_BUTTON_LEFT) {
			i = window_id_to_index(g, w, e->button.windowID);
			if (i >= 0) {
				if (w[i].async) {
					w[i].wp = w[i].wb = w[i].wh = 0;
					w[i].AbsoluteRotationMode = 0;
				} else {
					int j;
					for (j = 0; j < g->nwindow; j++) {
						w[j].wp = w[j].wb = w[j].wh = 0;
						w[j].AbsoluteRotationMode = 0;
					}
				}
			}
		}
		break;
	case SDL_MOUSEMOTION:
		i = window_id_to_index(g, w, e->motion.windowID);
		if (i >= 0)
			handle_mouse_motion(i, e->motion.x, e->motion.y,
					   e->motion.state);
		break;
	case SDL_MOUSEWHEEL:
		i = window_id_to_index(g, w, e->wheel.windowID);
		if (i >= 0)
			handle_mousewheel(i, e->wheel.y);
		break;
	case SDL_WINDOWEVENT:
		handle_window_event(g, w, &e->window);
		break;
	default:
		break;
	}
	have_saved_event = 0;
}

/* --- Init / palette / snapshot --- */

void G_Init0(int *argc, char **argv[])
{
	(void)argc;
	(void)argv;
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
		exit(1);
	}
	have_saved_event = 0;
}

void G_loadpalettes(Ginfo *g)
{
	g->numColors = 0;
	while (!feof(g->palettefile)) {
		int rr, gg, bb;
		char string[4096];
		int cpos;

		if (!fgets(string, (int)sizeof(string), g->palettefile))
			break;
		cpos = 0;
		while ((string[cpos] == ' ') || (string[cpos] == '\t'))
			cpos++;
		if (sscanf(string + cpos, "%d %d %d", &rr, &gg, &bb) < 3)
			continue;
		g->mastercolortable[g->numColors].red = (unsigned short)(rr << 8);
		g->mastercolortable[g->numColors].green =
		    (unsigned short)(gg << 8);
		g->mastercolortable[g->numColors].blue = (unsigned short)(bb << 8);
		g->numColors++;
	}
	fclose(g->palettefile);
}

void G_Init2(Ginfo *g)
{
	G_loadpalettes(g);
}

void W_Init2(Winfo *w, Ginfo *g)
{
	int i, j;
	SDL2Context *ctx;

	g_internal = g;
	w_internal = w;

	for (i = 0; i < g->nwindow; i++) {
		w[i].lastColor = g->numColors - 1;
		for (j = 0; j < g->numColors; j++) {
			w[i].colortable[j].red = g->mastercolortable[j].red;
			w[i].colortable[j].green = g->mastercolortable[j].green;
			w[i].colortable[j].blue = g->mastercolortable[j].blue;
		}
		w[i].window = SDL_CreateWindow(
		    w[i].inputfilename,
		    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		    w[i].screenwidth, w[i].screenheight,
		    SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
		if (!w[i].window) {
			fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
			exit(1);
		}
		ctx = (SDL2Context *)malloc(sizeof(SDL2Context));
		if (!ctx) {
			fprintf(stderr, "malloc SDL2Context\n");
			exit(1);
		}
		ctx->renderer = SDL_CreateRenderer((SDL_Window *)w[i].window, -1,
						   SDL_RENDERER_ACCELERATED);
		if (!ctx->renderer) {
			ctx->renderer = SDL_CreateRenderer(
			    (SDL_Window *)w[i].window, -1, 0);
		}
		if (!ctx->renderer) {
			fprintf(stderr, "SDL_CreateRenderer: %s\n",
				SDL_GetError());
			exit(1);
		}
		ctx->texture = SDL_CreateTexture(
		    ctx->renderer, SDL_PIXELFORMAT_ARGB8888,
		    SDL_TEXTUREACCESS_STREAMING, w[i].screenwidth,
		    w[i].screenheight);
		if (!ctx->texture) {
			fprintf(stderr, "SDL_CreateTexture: %s\n",
				SDL_GetError());
			exit(1);
		}
		w[i].drawarea = ctx;

		w[i].surface = cairo_image_surface_create(
		    CAIRO_FORMAT_ARGB32, w[i].screenwidth, w[i].screenheight);
		w[i].cr = cairo_create(w[i].surface);
		cairo_set_line_join(w[i].cr, CAIRO_LINE_JOIN_ROUND);
		cairo_set_line_cap(w[i].cr, CAIRO_LINE_CAP_ROUND);
		cairo_set_font_size(w[i].cr, 14);
		cairo_select_font_face(w[i].cr, "Helvetica",
				       CAIRO_FONT_SLANT_NORMAL,
				       CAIRO_FONT_WEIGHT_NORMAL);
#ifdef CAIRO_ANTIALIAS_FAST
		cairo_set_antialias(w[i].cr, CAIRO_ANTIALIAS_FAST);
#endif
		if (debug)
			fprintf(stderr, "SDL2 window %d: %dx%d\n", i,
				w[i].screenwidth, w[i].screenheight);
	}
}

int existevent(Ginfo *g)
{
	(void)g;
	if (have_saved_event)
		return 1;
	if (SDL_PollEvent(&saved_event)) {
		have_saved_event = 1;
		return 1;
	}
	return 0;
}

static cairo_status_t my_write_function(void *closure, unsigned char *data,
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
	{
		char filename[1024];
		snprintf(filename, sizeof(filename), "yaplot%02d_%05d.png",
			 windowid, (int)w->currentframe);
		cairo_surface_write_to_png(w->surface, filename);
	}
}

void W_StartRecording(Winfo *w, int mode, int windowid, int fps)
{
	w->RecordMode = mode;
	w->df = w->wh = w->wp = w->wb = 0;
	SDL_SetWindowTitle((SDL_Window *)w->window, "Recording");
#ifdef FFMPEG
	if (mode == 2) {
		char output0[512];
		char output[512];
		snprintf(output0, sizeof(output0), FFMPEG, fps);
		snprintf(output, sizeof(output), "%s yaplot%02d.mp4", output0,
			 windowid);
		w->pffmpeg = popen(output, "w");
	}
#endif
	(void)windowid;
	(void)fps;
}

void W_StopRecording(Winfo *w)
{
#ifdef FFMPEG
	if (w->RecordMode == 2 && w->pffmpeg)
		pclose(w->pffmpeg);
#endif
	w->RecordMode = 0;
	SDL_SetWindowTitle((SDL_Window *)w->window, "Done");
}
