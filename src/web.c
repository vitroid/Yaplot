/*
 * Emscripten + HTML5 Canvas 2D display backend for Yaplot (WASM MVP).
 * Single canvas; no Cairo / SDL2 / ffmpeg.
 */
#include "display.h"
#include "os.h"
#include "yaplot.h"
#include <emscripten.h>
#include <emscripten/html5.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define YAP_SHIFT_MASK 1

extern int debug;
extern int prefix;
extern float Eyex, Eyey, Eyez;
extern float Lookx, Looky, Lookz;
extern float Upx, Upy, Upz;

float lastthick = 1.0f;

static Ginfo *g_internal;
static Winfo *w_internal;

enum {
	EV_KEY = 1,
	EV_MOUSEDOWN,
	EV_MOUSEUP,
	EV_MOUSEMOVE,
	EV_WHEEL,
	EV_RESIZE,
	EV_REDRAW,
	EV_PINCH,
	EV_ROTATE,
	EV_OPEN
};

typedef struct {
	int type;
	int key; /* printable char, or negative special codes */
	int mod;
	int x, y;
	int buttons;
	int wheel;
	int width, height;
	float amount; /* pinch magnification / rotation degrees (clockwise) */
} WebEvent;

/* Special key codes (negative to avoid clash with printable) */
#define WK_LEFT (-1)
#define WK_RIGHT (-2)
#define WK_UP (-3)
#define WK_DOWN (-4)
#define WK_HOME (-5)
#define WK_END (-6)
#define WK_PAGEUP (-7)
#define WK_PAGEDOWN (-8)
#define WK_ESCAPE (-9)
#define WK_TAB (-10)
#define WK_ENTER (-11)
#define WK_SPACE (-12)
#define WK_PAUSE (-13)
#define WK_F1 (-100)

#define EVQ_SIZE 64
static WebEvent evq[EVQ_SIZE];
static int evq_head, evq_tail, evq_count;
static WebEvent saved_event;
static int have_saved_event;

static int originx, originy;
static float stroke_r, stroke_g, stroke_b, stroke_a;

static void evq_push(const WebEvent *e)
{
	if (evq_count >= EVQ_SIZE)
		return;
	evq[evq_tail] = *e;
	evq_tail = (evq_tail + 1) % EVQ_SIZE;
	evq_count++;
}

static int evq_pop(WebEvent *e)
{
	if (evq_count == 0)
		return 0;
	*e = evq[evq_head];
	evq_head = (evq_head + 1) % EVQ_SIZE;
	evq_count--;
	return 1;
}

static void ctx_apply_style(void)
{
	EM_ASM({
		var ctx = Module['yapCtx'];
		if (!ctx) return;
		var a = $3;
		if (a >= 0.99)
			ctx.strokeStyle = ctx.fillStyle =
			    'rgb(' + (($0 * 255) | 0) + ',' + (($1 * 255) | 0) +
			    ',' + (($2 * 255) | 0) + ')';
		else
			ctx.strokeStyle = ctx.fillStyle =
			    'rgba(' + (($0 * 255) | 0) + ',' + (($1 * 255) | 0) +
			    ',' + (($2 * 255) | 0) + ',' + a + ')';
		ctx.lineWidth = $4 > 0 ? $4 : 1;
		ctx.lineJoin = 'round';
		ctx.lineCap = 'round';
		ctx.font = '14px Helvetica, Arial, sans-serif';
	}, stroke_r, stroke_g, stroke_b, stroke_a, (double)lastthick);
}

/* --- Drawing --- */

void drawline2fb(Winfo *w, int x0, int y0, int x1, int y1)
{
	(void)w;
	ctx_apply_style();
	EM_ASM({
		var ctx = Module['yapCtx'];
		ctx.beginPath();
		ctx.moveTo($0, $1);
		ctx.lineTo($2, $3);
		ctx.stroke();
	}, x0, y0, x1, y1);
}

void drawpoly2fb(Winfo *w, ivector2 *poly, int n, int fill)
{
	int i;
	(void)w;
	if (n < 2)
		return;
	ctx_apply_style();
	EM_ASM({
		var ctx = Module['yapCtx'];
		ctx.beginPath();
	});
	EM_ASM({ Module['yapCtx'].moveTo($0, $1); }, poly[n - 1].x,
	       poly[n - 1].y);
	for (i = 0; i < n; i++)
		EM_ASM({ Module['yapCtx'].lineTo($0, $1); }, poly[i].x,
		       poly[i].y);
	EM_ASM({ Module['yapCtx'].closePath(); });
	if (fill) {
		float save = lastthick;
		lastthick = 0;
		ctx_apply_style();
		EM_ASM({ Module['yapCtx'].fill(); });
		lastthick = save;
		ctx_apply_style();
	} else {
		EM_ASM({ Module['yapCtx'].stroke(); });
	}
}

void drawcircle2fb(Winfo *w, int x, int y, int r, int fill)
{
	(void)w;
	ctx_apply_style();
	EM_ASM({
		var ctx = Module['yapCtx'];
		ctx.beginPath();
		ctx.arc($0, $1, $2, 0, Math.PI * 2);
		if ($3)
			ctx.fill();
		else
			ctx.stroke();
	}, x, y, r, fill);
}

void drawstick2fb(Winfo *w, int x0, int y0, int x1, int y1, int r, int fill,
		  int arrowtype)
{
	if (arrowtype == 0) {
		int thick = r * 2;
		float save = lastthick;
		if (thick == 0)
			thick = 1;
		lastthick = (float)thick;
		drawline2fb(w, x0, y0, x1, y1);
		lastthick = save;
		ctx_apply_style();
	} else {
		int dx = x1 - x0;
		int dy = y1 - y0;
		int dd = (int)sqrt(dx * dx + dy * dy);
		if (dd != 0) {
			int rx = r * dx / dd;
			int ry = r * dy / dd;
			if (arrowtype == 1) {
				float save = lastthick;
				lastthick = 1;
				drawline2fb(w, x0, y0, x1, y1);
				drawline2fb(w, x1 - 2 * rx + ry, y1 - 2 * ry - rx,
					    x1, y1);
				drawline2fb(w, x1, y1, x1 - 2 * rx - ry,
					    y1 - 2 * ry + rx);
				lastthick = save;
				ctx_apply_style();
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
	(void)w;
	lastthick = thick;
	if (thick == 0)
		lastthick = 1;
}

void setfgcolor(Winfo *w, int palette)
{
	stroke_r = (float)w->colortable[palette].red / 65536.0f;
	stroke_g = (float)w->colortable[palette].green / 65536.0f;
	stroke_b = (float)w->colortable[palette].blue / 65536.0f;
	stroke_a = 1.0f;
}

void settpcolor(Winfo *w, int palette)
{
	stroke_r = (float)w->colortable[palette].red / 65536.0f;
	stroke_g = (float)w->colortable[palette].green / 65536.0f;
	stroke_b = (float)w->colortable[palette].blue / 65536.0f;
	stroke_a = 0.5f;
}

void setrgba(Winfo *w, float r, float g, float b, float a)
{
	(void)w;
	stroke_r = r;
	stroke_g = g;
	stroke_b = b;
	stroke_a = a;
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
	(void)w;
	(void)length;
	ctx_apply_style();
	EM_ASM({
		var ctx = Module['yapCtx'];
		var s = UTF8ToString($2);
		ctx.fillText(s, $0, $1);
	}, x, y, str);
}

/* --- SVG overlays (decoded asynchronously by the browser as <img>) --- */

typedef struct {
	int id; /* index into Module['yapSvgs'] */
} SvgOverlay;

EMSCRIPTEN_KEEPALIVE void yap_web_svg_loaded(void)
{
	WebEvent e;
	memset(&e, 0, sizeof(e));
	e.type = EV_REDRAW;
	evq_push(&e);
}

void *svg_open(const char *data, size_t len)
{
	SvgOverlay *s;
	int id = EM_ASM_INT({
		var svgs = Module['yapSvgs'] || (Module['yapSvgs'] = []);
		var text = UTF8ToString($0, $1);
		var url = URL.createObjectURL(
		    new Blob([text], {type : 'image/svg+xml'}));
		var img = new Image();
		img.onload = function() { _yap_web_svg_loaded(); };
		img.onerror = function() {
			console.error('svg ignored: failed to decode');
		};
		img.src = url;
		svgs.push({img : img, url : url});
		return svgs.length - 1;
	}, data, (int)len);
	s = calloc(1, sizeof(SvgOverlay));
	s->id = id;
	return s;
}

void svg_close(void *svg)
{
	SvgOverlay *s = (SvgOverlay *)svg;
	if (!s)
		return;
	EM_ASM({
		var e = Module['yapSvgs'][$0];
		if (e) {
			URL.revokeObjectURL(e.url);
			Module['yapSvgs'][$0] = null;
		}
	}, s->id);
	free(s);
}

void drawsvg2fb(Winfo *w, void *svg)
{
	SvgOverlay *s = (SvgOverlay *)svg;
	if (!s)
		return;
	/* Fit into the whole window, keeping aspect ratio (like rsvg). */
	EM_ASM({
		var e = Module['yapSvgs'][$0];
		var ctx = Module['yapCtx'];
		if (!e || !ctx || !e.img.complete || !e.img.naturalWidth)
			return;
		var iw = e.img.naturalWidth;
		var ih = e.img.naturalHeight;
		var k = Math.min($1 / iw, $2 / ih);
		var dw = iw * k;
		var dh = ih * k;
		ctx.drawImage(e.img, ($1 - dw) / 2, ($2 - dh) / 2, dw, dh);
	}, s->id, w->screenwidth, w->screenheight);
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
	ctx_apply_style();
	EM_ASM({
		var ctx = Module['yapCtx'];
		var c = Module['canvas'];
		if (!ctx || !c) {
			console.error('clearfb: missing ctx/canvas');
			return;
		}
		ctx.fillRect(0, 0, c.width, c.height);
	});
}

void exposefb(Winfo *w)
{
	(void)w;
	/* Canvas 2D draws immediately; nothing to present. */
}

/* --- Events --- */

static void handle_key(int i, int key, int modstatus)
{
	Winfo *w = w_internal;
	Ginfo *g = g_internal;

	if (debug)
		fprintf(stderr, "Key: %d mod: %d\n", key, modstatus);

	if (key >= '0' && key <= '9' && !(modstatus & YAP_SHIFT_MASK)) {
		ClearLastCommand();
		prefix = prefix * 10 + (key - '0');
		{
			int j;
			for (j = 0; j < g->nwindow; j++)
				w[j].status |= REDRAW;
		}
		return;
	}

	if (w[i].RecordMode) {
		if (key == 'r' || key == 'R') {
			eStopRecording(g, w);
			return;
		}
		if (key == 'q') {
			eStopRecording(g, w);
			eQuit(g, w, i);
		}
		return;
	}

	switch (key) {
	case 'q':
		(void)eQuit(g, w, i);
		SetLastCommand(0, "q", "quit");
		break;
	case 'g':
	case WK_ENTER: {
		int p = (modstatus && prefix == 0) ? LASTFRAME : prefix;
		(void)eGotoFrame(g, w, i, p);
		SetLastCommand(p, "g", "goto frame %d");
		break;
	}
	case WK_PAGEDOWN:
	case 'n': {
		int n = prefix ? prefix : 1;
		(void)eGoRelativeFrame(g, w, i, n, modstatus);
		if (modstatus)
			SetLastCommand(n, "N", "proceed at %d fps");
		else
			SetLastCommand(n, "n", "proceed %d frames");
		break;
	}
	case 'c':
		(void)eToggleCentering(g, w, i);
		SetLastCommand(0, "c", "toggle center");
		break;
	case '%':
		(void)eToggleSlicing(g, w, i);
		SetLastCommand(0, "% key", "toggle slice");
		break;
	case ']':
		(void)(modstatus ? eSliceMove(g, w, i, 1) : eWiden(g, w, i, -1));
		SetLastCommand(0, "] key",
			       modstatus ? "shift slice right" : "widen in");
		break;
	case '[':
		(void)(modstatus ? eSliceMove(g, w, i, -1) : eWiden(g, w, i, 1));
		SetLastCommand(0, "[ key",
			       modstatus ? "shift slice left" : "widen out");
		break;
	case 'r':
		(void)eStartRecording(g, w, 1, 1);
		SetLastCommand(0, "r", "start recording");
		break;
	case 'u':
		(void)ePushView(g, w, i);
		SetLastCommand(0, "u", "push view");
		break;
	case 'o':
		(void)ePopView(g, w, i);
		SetLastCommand(0, "o", "pop view");
		break;
	case 'v':
		if (prefix > 0) {
			g->verbose = (prefix > MAXVERBOSE) ? MAXVERBOSE : prefix;
			SetLastCommand(g->verbose, "v", "verbosity %d");
		} else {
			(void)eToggleVerbosity(g, w, i);
			SetLastCommand(0, "v", "toggle verbosity");
		}
		break;
	case 'd':
		if (prefix > 0) {
			char sval = (char)((prefix > 3) ? 3 : prefix);
			int j;
			for (j = 0; j < g->nwindow; j++) {
				if (w[i].async && j != i)
					continue;
				w[j].splat = sval;
				if (sval && w[j].reality < 2)
					w[j].reality = 3;
			}
			SetLastCommand(sval, "d", "depth splat %d");
		} else {
			(void)eCycleSplat(g, w, i);
			SetLastCommand(0, "d", "cycle depth splat");
		}
		break;
	case WK_PAGEUP:
	case 'p': {
		int n = prefix ? prefix : 1;
		(void)eGoRelativeFrame(g, w, i, -n, modstatus);
		if (modstatus)
			SetLastCommand(n, "P", "go back at %d fps");
		else
			SetLastCommand(n, "p", "go back %d frames");
		break;
	}
	case WK_HOME:
		(void)eGotoFrame(g, w, i, 0);
		SetLastCommand(0, "Home", "goto frame 0");
		break;
	case WK_END:
		(void)eGotoFrame(g, w, i, LASTFRAME);
		SetLastCommand(0, "End", "goto last frame");
		break;
	case WK_SPACE:
		(void)eStopMotion(g, w, i);
		SetLastCommand(0, "Space", "stop motion");
		break;
	case WK_PAUSE:
	case '!':
		(void)eStopRotation(g, w, i);
		SetLastCommand(0, "Pause", "stop rotation");
		break;
	case '*': {
		int n = prefix ? prefix : 1;
		(void)eZoom(g, w, i, n);
		SetLastCommand(n, "* key", "zoom in %d steps");
		break;
	}
	case '/': {
		int n = prefix ? prefix : 1;
		(void)eZoom(g, w, i, -n);
		SetLastCommand(n, "/ key", "zoom out %d steps");
		break;
	}
	case 'h':
	case WK_LEFT: {
		int n = prefix ? prefix : 1;
		(void)eHeading(g, w, i, n, modstatus);
		SetLastCommand(n, "h", "heading left %d");
		break;
	}
	case 'l':
	case WK_RIGHT: {
		int n = prefix ? prefix : 1;
		(void)eHeading(g, w, i, -n, modstatus);
		SetLastCommand(n, "l", "heading right %d");
		break;
	}
	case 'k':
	case WK_UP: {
		int n = prefix ? prefix : 1;
		(void)eBanking(g, w, i, n, modstatus);
		SetLastCommand(n, "k", "banking %d");
		break;
	}
	case 'j':
	case WK_DOWN: {
		int n = prefix ? prefix : 1;
		(void)eBanking(g, w, i, -n, modstatus);
		SetLastCommand(n, "j", "banking -%d");
		break;
	}
	case WK_ESCAPE:
		ClearLastCommand();
		break;
	case 's':
		(void)eToggleSync(g, w, i);
		SetLastCommand(0, "s", "toggle sync");
		break;
	case 'f': {
		int layer = prefix ? prefix : 1;
		(void)eToggleLayer(g, w, i, layer);
		SetLastCommand(layer, "f", "toggle layer %d");
		break;
	}
	case 'x':
		(void)(modstatus ? eResetView(g, w, i, +40, 0, 0)
				 : eResetView(g, w, i, -40, 0, 0));
		SetLastCommand(0, "x", "reset view X");
		break;
	case 'y':
		(void)(modstatus ? eResetView(g, w, i, 0, +40, 0)
				 : eResetView(g, w, i, 0, -40, 0));
		SetLastCommand(0, "y", "reset view Y");
		break;
	case 'z':
		(void)(modstatus ? eResetView(g, w, i, 0, 0, +40)
				 : eResetView(g, w, i, 0, 0, -40));
		SetLastCommand(0, "z", "reset view Z");
		break;
	case ')': {
		int n = prefix ? prefix : 1;
		(void)eRelativeThickness(g, w, i, n);
		SetLastCommand(n, ") key", "thickness +%d");
		break;
	}
	case '(': {
		int n = prefix ? prefix : 1;
		(void)eRelativeThickness(g, w, i, -n);
		SetLastCommand(n, "( key", "thickness -%d");
		break;
	}
	case '+':
		(void)eRelativeReality(g, w, i, 1);
		SetLastCommand(0, "+ key", "reality +1");
		break;
	case '-':
		(void)eRelativeReality(g, w, i, -1);
		SetLastCommand(0, "- key", "reality -1");
		break;
	case WK_TAB:
		(void)eResetView(g, w, i, Eyex, Eyey, Eyez);
		SetLastCommand(0, "Tab", "reset view");
		break;
	case WK_F1:
	case WK_F1 + 1:
	case WK_F1 + 2:
	case WK_F1 + 3:
	case WK_F1 + 4:
	case WK_F1 + 5:
	case WK_F1 + 6:
	case WK_F1 + 7:
	case WK_F1 + 8:
	case WK_F1 + 9:
	case WK_F1 + 10:
	case WK_F1 + 11: {
		int layer = key - WK_F1 + 1;
		(void)eToggleLayer(g, w, i, layer);
		SetLastCommand(0, "Fn", "toggle layer");
		break;
	}
	default:
		break;
	}
	prefix = 0;
	{
		int j;
		for (j = 0; j < g->nwindow; j++)
			w[j].status |= REDRAW;
	}
}

static void handle_mouse_motion(int i, int x, int y, int buttons)
{
	Winfo *w = w_internal;
	Ginfo *g = g_internal;
	if (buttons & 1) {
		if (originx != x || originy != y) {
			float tick = 10.0f / 400.0f;
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

/* File dropped on the page: JS has written it to DROP_PATH in MEMFS. */
#define DROP_PATH "/dropped.yap"

EMSCRIPTEN_KEEPALIVE void yap_web_file_dropped(void)
{
	WebEvent e;
	memset(&e, 0, sizeof(e));
	e.type = EV_OPEN;
	evq_push(&e);
}

static void open_dropped_file(Ginfo *g, Winfo w[], int i)
{
	char name[256];
	FILE *file = fopen(DROP_PATH, "r");

	if (!file) {
		fprintf(stderr, "drop: cannot open %s\n", DROP_PATH);
		return;
	}
	/* the previous drop is still open via w->inputfile; MEMFS keeps it */
	unlink(DROP_PATH);
	EM_ASM({ stringToUTF8(Module['yapDroppedName'] || 'dropped.yap', $0,
			      $1); },
	       name, (int)sizeof(name));
	W_ReplaceInput(g, &w[i], file, name);
	fprintf(stderr, "opened %s\n", name);
}

void handleevent(Ginfo *g, Winfo w[])
{
	WebEvent *e = &saved_event;
	int i = 0;

	switch (e->type) {
	case EV_KEY:
		handle_key(i, e->key, e->mod);
		break;
	case EV_MOUSEDOWN:
		originx = e->x;
		originy = e->y;
		if (w[i].async)
			w[i].AbsoluteRotationMode = 1;
		else {
			int j;
			for (j = 0; j < g->nwindow; j++)
				w[j].AbsoluteRotationMode = 1;
		}
		break;
	case EV_MOUSEUP:
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
		break;
	case EV_MOUSEMOVE:
		handle_mouse_motion(i, e->x, e->y, e->buttons);
		break;
	case EV_WHEEL:
		if (e->wheel != 0)
			eWiden(g, w, i, e->wheel > 0 ? 1 : -1);
		break;
	case EV_RESIZE:
		w[i].screenwidth = e->width;
		w[i].screenheight = e->height;
		EM_ASM({
			var c = Module['canvas'];
			c.width = $0;
			c.height = $1;
		}, e->width, e->height);
		w[i].status |= REDRAW;
		break;
	case EV_PINCH:
		if (e->amount != 0.0f)
			(void)eZoomFactor(g, w, i, expf(e->amount));
		break;
	case EV_ROTATE:
		if (e->amount != 0.0f) {
			float pitch = e->amount * (float)M_PI / 180.0f;
			int j;
			for (j = 0; j < g->nwindow; j++)
				if (!w[i].async || j == i)
					WindowRoll(&w[j], pitch);
		}
		break;
	case EV_OPEN:
		open_dropped_file(g, w, i);
		break;
	case EV_REDRAW: {
		int j;
		for (j = 0; j < g->nwindow; j++)
			w[j].status |= REDRAW;
		break;
	}
	default:
		break;
	}
	have_saved_event = 0;
}

/*
 * イベント有無のみ返す。描画中の割り込み判定からも呼ぶ。
 * ここで handleevent は呼ばない。
 */
int existevent(Ginfo *g)
{
	(void)g;
	if (have_saved_event)
		return 1;
	if (evq_pop(&saved_event)) {
		have_saved_event = 1;
		return 1;
	}
	return 0;
}

static int map_dom_key(const EmscriptenKeyboardEvent *ke, int *out_key)
{
	const char *k = ke->key;
	if (strcmp(k, "ArrowLeft") == 0) {
		*out_key = WK_LEFT;
		return 1;
	}
	if (strcmp(k, "ArrowRight") == 0) {
		*out_key = WK_RIGHT;
		return 1;
	}
	if (strcmp(k, "ArrowUp") == 0) {
		*out_key = WK_UP;
		return 1;
	}
	if (strcmp(k, "ArrowDown") == 0) {
		*out_key = WK_DOWN;
		return 1;
	}
	if (strcmp(k, "Home") == 0) {
		*out_key = WK_HOME;
		return 1;
	}
	if (strcmp(k, "End") == 0) {
		*out_key = WK_END;
		return 1;
	}
	if (strcmp(k, "PageUp") == 0) {
		*out_key = WK_PAGEUP;
		return 1;
	}
	if (strcmp(k, "PageDown") == 0) {
		*out_key = WK_PAGEDOWN;
		return 1;
	}
	if (strcmp(k, "Escape") == 0) {
		*out_key = WK_ESCAPE;
		return 1;
	}
	if (strcmp(k, "Tab") == 0) {
		*out_key = WK_TAB;
		return 1;
	}
	if (strcmp(k, "Enter") == 0) {
		*out_key = WK_ENTER;
		return 1;
	}
	if (strcmp(k, " ") == 0 || strcmp(k, "Spacebar") == 0) {
		*out_key = WK_SPACE;
		return 1;
	}
	if (strcmp(k, "Pause") == 0) {
		*out_key = WK_PAUSE;
		return 1;
	}
	if (k[0] == 'F' && k[1] >= '1' && k[1] <= '9' && k[2] == '\0') {
		*out_key = WK_F1 + (k[1] - '1');
		return 1;
	}
	if (strcmp(k, "F10") == 0) {
		*out_key = WK_F1 + 9;
		return 1;
	}
	if (strcmp(k, "F11") == 0) {
		*out_key = WK_F1 + 10;
		return 1;
	}
	if (strcmp(k, "F12") == 0) {
		*out_key = WK_F1 + 11;
		return 1;
	}
	/* Single printable character */
	if (k[0] != '\0' && k[1] == '\0') {
		*out_key = (unsigned char)k[0];
		return 1;
	}
	return 0;
}

/* Toggles / digits must not auto-repeat; motion and FOV keys may. */
static int key_allows_repeat(int key)
{
	switch (key) {
	case '[':
	case ']':
	case WK_LEFT:
	case WK_RIGHT:
	case WK_UP:
	case WK_DOWN:
	case 'h':
	case 'j':
	case 'k':
	case 'l':
	case '*':
	case '/':
	case 'n':
	case 'p':
	case WK_PAGEUP:
	case WK_PAGEDOWN:
		return 1;
	default:
		return 0;
	}
}

static EM_BOOL on_key_down(int type, const EmscriptenKeyboardEvent *ke,
			   void *user)
{
	WebEvent e;
	int key;
	(void)type;
	(void)user;
	if (ke->ctrlKey || ke->metaKey || ke->altKey)
		return EM_FALSE; /* leave browser shortcuts alone */
	memset(&e, 0, sizeof(e));
	if (!map_dom_key(ke, &key))
		return EM_FALSE;
	/* DOM reports Shift+n as "N"; handlers expect 'n' plus the shift bit. */
	if (key >= 'A' && key <= 'Z')
		key += 'a' - 'A';
	if (ke->repeat && !key_allows_repeat(key))
		return EM_TRUE;
	e.type = EV_KEY;
	e.key = key;
	e.mod = ke->shiftKey ? YAP_SHIFT_MASK : 0;
	evq_push(&e);
	return EM_TRUE;
}

static EM_BOOL on_mouse_down(int type, const EmscriptenMouseEvent *me,
			     void *user)
{
	WebEvent e;
	(void)type;
	(void)user;
	memset(&e, 0, sizeof(e));
	e.type = EV_MOUSEDOWN;
	e.x = me->targetX;
	e.y = me->targetY;
	e.buttons = me->buttons;
	evq_push(&e);
	return EM_TRUE;
}

static EM_BOOL on_mouse_up(int type, const EmscriptenMouseEvent *me, void *user)
{
	WebEvent e;
	(void)type;
	(void)user;
	memset(&e, 0, sizeof(e));
	e.type = EV_MOUSEUP;
	e.x = me->targetX;
	e.y = me->targetY;
	e.buttons = me->buttons;
	evq_push(&e);
	return EM_TRUE;
}

static EM_BOOL on_mouse_move(int type, const EmscriptenMouseEvent *me,
			     void *user)
{
	WebEvent e;
	(void)type;
	(void)user;
	if (!(me->buttons & 1))
		return EM_FALSE;
	memset(&e, 0, sizeof(e));
	e.type = EV_MOUSEMOVE;
	e.x = me->targetX;
	e.y = me->targetY;
	e.buttons = me->buttons;
	evq_push(&e);
	return EM_TRUE;
}

static EM_BOOL on_wheel(int type, const EmscriptenWheelEvent *we, void *user)
{
	WebEvent e;
	static double last_wheel_ms;
	double now;
	(void)type;
	(void)user;
	memset(&e, 0, sizeof(e));
	if (we->mouse.ctrlKey) {
		/* Chromium/Firefox deliver trackpad pinch as ctrl+wheel. */
		double dy = we->deltaY;
		if (we->deltaMode == DOM_DELTA_LINE)
			dy *= 16.0;
		e.type = EV_PINCH;
		e.amount = (float)(-dy * 0.01);
		evq_push(&e);
		return EM_TRUE;
	}
	if (we->deltaY == 0)
		return EM_TRUE;
	now = emscripten_get_now();
	if (last_wheel_ms != 0 && now - last_wheel_ms < 100)
		return EM_TRUE;
	last_wheel_ms = now;
	e.type = EV_WHEEL;
	e.wheel = (we->deltaY < 0) ? 1 : -1;
	evq_push(&e);
	return EM_TRUE;
}

/* Safari trackpad gestures and two-finger touch, forwarded from JS. */
EMSCRIPTEN_KEEPALIVE void yap_web_pinch(float magnification)
{
	WebEvent e;
	memset(&e, 0, sizeof(e));
	e.type = EV_PINCH;
	e.amount = magnification;
	evq_push(&e);
}

EMSCRIPTEN_KEEPALIVE void yap_web_rotate(float degrees_clockwise)
{
	WebEvent e;
	memset(&e, 0, sizeof(e));
	e.type = EV_ROTATE;
	e.amount = degrees_clockwise;
	evq_push(&e);
}

static EM_BOOL on_resize(int type, const EmscriptenUiEvent *ui, void *user)
{
	WebEvent e;
	double css_w, css_h;
	(void)type;
	(void)ui;
	(void)user;
	emscripten_get_element_css_size("#canvas", &css_w, &css_h);
	memset(&e, 0, sizeof(e));
	e.type = EV_RESIZE;
	e.width = (int)css_w;
	e.height = (int)css_h;
	if (e.width < 64)
		e.width = DEFAULTX;
	if (e.height < 64)
		e.height = DEFAULTY;
	evq_push(&e);
	return EM_TRUE;
}

/* --- Init / palette / snapshot --- */

void G_Init0(int *argc, char **argv[])
{
	(void)argc;
	(void)argv;
	have_saved_event = 0;
	evq_head = evq_tail = evq_count = 0;
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
		g->mastercolortable[g->numColors].red =
		    (unsigned short)(rr << 8);
		g->mastercolortable[g->numColors].green =
		    (unsigned short)(gg << 8);
		g->mastercolortable[g->numColors].blue =
		    (unsigned short)(bb << 8);
		g->numColors++;
	}
	fclose(g->palettefile);
}

void G_Init2(Ginfo *g)
{
	G_loadpalettes(g);
}

/* Safari trackpad gestures and two-finger touch -> yap_web_pinch/rotate */
EM_JS(void, install_gesture_listeners, (void), {
	var canvas = Module['canvas'];
	canvas.style.touchAction = 'none';

	/* Safari: trackpad pinch/rotate (cumulative scale, degrees cw) */
	var gs = {scale : 1, rot : 0};
	canvas.addEventListener('gesturestart', function(ev) {
		ev.preventDefault();
		gs.scale = ev.scale;
		gs.rot = ev.rotation;
	});
	canvas.addEventListener('gesturechange', function(ev) {
		ev.preventDefault();
		if (ev.scale > 0 && gs.scale > 0)
			_yap_web_pinch(Math.log(ev.scale / gs.scale));
		_yap_web_rotate(ev.rotation - gs.rot);
		gs.scale = ev.scale;
		gs.rot = ev.rotation;
	});
	canvas.addEventListener('gestureend',
				function(ev) { ev.preventDefault(); });

	/* Touch screens: two-finger pinch and twist */
	var tp = null;
	function two(ev) {
		var a = ev.touches[0];
		var b = ev.touches[1];
		var dx = b.clientX - a.clientX;
		var dy = b.clientY - a.clientY;
		return {d : Math.hypot(dx, dy), ang : Math.atan2(dy, dx)};
	}
	canvas.addEventListener('touchstart', function(ev) {
		if (ev.touches.length == 2) {
			ev.preventDefault();
			tp = two(ev);
		}
	}, {passive : false});
	canvas.addEventListener('touchmove', function(ev) {
		if (ev.touches.length != 2 || !tp)
			return;
		ev.preventDefault();
		var t = two(ev);
		var da = t.ang - tp.ang;
		if (da > Math.PI)
			da -= 2 * Math.PI;
		if (da < -Math.PI)
			da += 2 * Math.PI;
		if (tp.d > 0 && t.d > 0)
			_yap_web_pinch(Math.log(t.d / tp.d));
		_yap_web_rotate(da * 180 / Math.PI);
		tp = t;
	}, {passive : false});
	canvas.addEventListener('touchend', function(ev) {
		if (ev.touches.length < 2)
			tp = null;
	});
});

/* Drag & drop a .yap anywhere on the page to replace the displayed file */
EM_JS(void, install_drop_listener, (void), {
	window.addEventListener('dragover', function(ev) {
		ev.preventDefault();
		ev.dataTransfer.dropEffect = 'copy';
	});
	window.addEventListener('drop', function(ev) {
		ev.preventDefault();
		var f = ev.dataTransfer.files[0];
		if (!f)
			return;
		f.arrayBuffer().then(function(buf) {
			FS.writeFile('/dropped.yap', new Uint8Array(buf));
			Module['yapDroppedName'] = f.name;
			_yap_web_file_dropped();
		});
	});
});

void W_Init2(Winfo *w, Ginfo *g)
{
	int i, j;
	double css_w = 0, css_h = 0;

	g_internal = g;
	w_internal = w;

	fprintf(stderr, "W_Init2: begin nwindow=%d numColors=%d\n", g->nwindow,
		g->numColors);

	EM_ASM({
		var canvas = document.getElementById('canvas');
		if (!canvas) {
			canvas = document.createElement('canvas');
			canvas.id = 'canvas';
			document.body.appendChild(canvas);
		}
		Module['canvas'] = canvas;
		Module['yapCtx'] = canvas.getContext('2d');
		if (!Module['yapCtx'])
			console.error('yapCtx is null');
	});
	install_gesture_listeners();
	install_drop_listener();

	emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, NULL, 1,
					on_key_down);
	emscripten_set_mousedown_callback("#canvas", NULL, 1, on_mouse_down);
	emscripten_set_mouseup_callback("#canvas", NULL, 1, on_mouse_up);
	emscripten_set_mousemove_callback("#canvas", NULL, 1, on_mouse_move);
	emscripten_set_wheel_callback("#canvas", NULL, 1, on_wheel);
	emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, NULL, 1,
				       on_resize);

	emscripten_get_element_css_size("#canvas", &css_w, &css_h);
	/* flex layout may report 0 before first paint; fall back to window */
	if (css_w < 64 || css_h < 64) {
		css_w = EM_ASM_DOUBLE({ return window.innerWidth || 512; });
		css_h = EM_ASM_DOUBLE({
			return (window.innerHeight || 512) - 40;
		});
	}

	for (i = 0; i < g->nwindow; i++) {
		w[i].lastColor = g->numColors - 1;
		for (j = 0; j < g->numColors; j++) {
			w[i].colortable[j].red = g->mastercolortable[j].red;
			w[i].colortable[j].green = g->mastercolortable[j].green;
			w[i].colortable[j].blue = g->mastercolortable[j].blue;
		}
		if (css_w >= 64 && css_h >= 64) {
			w[i].screenwidth = (int)css_w;
			w[i].screenheight = (int)css_h;
		}
		EM_ASM({
			var c = Module['canvas'];
			c.width = $0;
			c.height = $1;
		}, w[i].screenwidth, w[i].screenheight);
		w[i].window = NULL;
		w[i].drawarea = NULL;
		w[i].status |= REDRAW;
		/* Prove the 2D context works: fill palette index 1 (gray100). */
		clearfb(&w[i]);
		fprintf(stderr, "W_Init2: canvas %d %dx%d cleared\n", i,
			w[i].screenwidth, w[i].screenheight);
	}
}

void W_SaveSnapShot(Winfo *w, int windowid)
{
	(void)w;
	(void)windowid;
}

void W_StartRecording(Winfo *w, int mode, int windowid, int fps)
{
	(void)mode;
	(void)windowid;
	(void)fps;
	w->RecordMode = 0; /* no-op under WASM MVP */
}

void W_StopRecording(Winfo *w)
{
	w->RecordMode = 0;
}
