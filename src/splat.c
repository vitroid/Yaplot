#include "splat.h"
#include "gui.h"
#include <math.h>
#include <stdlib.h>

/*
 * Always 3 splat layers (L = 0, 1, 2) when w->splat is 1..3:
 *   depth_factor  = 2^L
 *   radius_factor = 2 * 2^L
 * Opacity scales with w->splat level d (1..3):
 *   d=3: full (total alpha 0.125 across 3 layers)
 *   d=2: 1/2 of d=3
 *   d=1: 1/4 of d=3
 */
#define SPLAT_D3_ALPHA (0.125f / 3.0f)
#define SPLAT_MAX_LAYERS 3
#define SPLAT_RADIUS_QUANTUM 2

static float splat_layer_alpha(int level)
{
	if (level == 1)
		return SPLAT_D3_ALPHA * 0.25f;
	if (level == 2)
		return SPLAT_D3_ALPHA * 0.5f;
	return SPLAT_D3_ALPHA;
}

#ifndef __EMSCRIPTEN__
#define SPLAT_MASK_CACHE_ENTRIES 64
#define SPLAT_MASK_CACHE_BYTES (16u * 1024u * 1024u)
#define SPLAT_MASK_MAX_RADIUS 512

typedef struct {
	int radius;
	size_t bytes;
	unsigned long age;
	cairo_surface_t *surface;
} SplatMaskEntry;

static SplatMaskEntry splat_mask_cache[SPLAT_MASK_CACHE_ENTRIES];
static size_t splat_mask_cache_bytes;
static unsigned long splat_mask_cache_age;

static cairo_surface_t *create_circle_mask(int radius, size_t *bytes)
{
	int size = 2 * radius + 3;
	cairo_surface_t *surface =
	    cairo_image_surface_create(CAIRO_FORMAT_A8, size, size);
	cairo_t *cr;

	if (cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS) {
		cairo_surface_destroy(surface);
		return NULL;
	}
	cr = cairo_create(surface);
	if (cairo_status(cr) != CAIRO_STATUS_SUCCESS) {
		cairo_destroy(cr);
		cairo_surface_destroy(surface);
		return NULL;
	}
	cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
	cairo_paint(cr);
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
	cairo_set_antialias(cr, CAIRO_ANTIALIAS_FAST);
	cairo_set_source_rgba(cr, 1, 1, 1, 1);
	cairo_arc(cr, radius + 1, radius + 1, radius, 0, 2 * M_PI);
	cairo_fill(cr);
	cairo_destroy(cr);
	cairo_surface_flush(surface);
	*bytes = (size_t)cairo_image_surface_get_stride(surface) * (size_t)size;
	return surface;
}

static cairo_surface_t *get_circle_mask(int radius)
{
	int i, slot = -1, oldest = -1;
	unsigned long oldest_age = ~0ul;
	size_t bytes = 0;
	cairo_surface_t *surface;

	if (radius < 1 || radius > SPLAT_MASK_MAX_RADIUS)
		return NULL;
	splat_mask_cache_age++;
	for (i = 0; i < SPLAT_MASK_CACHE_ENTRIES; i++) {
		SplatMaskEntry *entry = &splat_mask_cache[i];
		if (entry->surface != NULL && entry->radius == radius) {
			entry->age = splat_mask_cache_age;
			return entry->surface;
		}
		if (entry->surface == NULL && slot < 0)
			slot = i;
		if (entry->surface != NULL && entry->age < oldest_age) {
			oldest_age = entry->age;
			oldest = i;
		}
	}

	surface = create_circle_mask(radius, &bytes);
	if (surface == NULL || bytes > SPLAT_MASK_CACHE_BYTES) {
		if (surface != NULL)
			cairo_surface_destroy(surface);
		return NULL;
	}
	while (splat_mask_cache_bytes + bytes > SPLAT_MASK_CACHE_BYTES ||
	       slot < 0) {
		SplatMaskEntry *entry;
		if (oldest < 0)
			break;
		entry = &splat_mask_cache[oldest];
		splat_mask_cache_bytes -= entry->bytes;
		cairo_surface_destroy(entry->surface);
		entry->surface = NULL;
		slot = oldest;
		oldest = -1;
		oldest_age = ~0ul;
		for (i = 0; i < SPLAT_MASK_CACHE_ENTRIES; i++) {
			entry = &splat_mask_cache[i];
			if (entry->surface != NULL && entry->age < oldest_age) {
				oldest_age = entry->age;
				oldest = i;
			}
		}
	}
	if (slot < 0) {
		cairo_surface_destroy(surface);
		return NULL;
	}
	splat_mask_cache[slot].radius = radius;
	splat_mask_cache[slot].bytes = bytes;
	splat_mask_cache[slot].age = splat_mask_cache_age;
	splat_mask_cache[slot].surface = surface;
	splat_mask_cache_bytes += bytes;
	return surface;
}
#endif /* !__EMSCRIPTEN__ */

typedef struct {
	Pinfo *p;
	float sortkey;
	char is_splat;
	char layer; /* 0..n-1 when is_splat */
} PaintItem;

static int paint_compar(const void *a, const void *b)
{
	float ka = ((const PaintItem *)a)->sortkey;
	float kb = ((const PaintItem *)b)->sortkey;
	if (ka > kb)
		return -1;
	if (ka < kb)
		return 1;
	/* Splat before solid; among splats, farther layer (higher L) first. */
	if (((const PaintItem *)a)->is_splat !=
	    ((const PaintItem *)b)->is_splat)
		return (int)((const PaintItem *)b)->is_splat -
		       (int)((const PaintItem *)a)->is_splat;
	return (int)((const PaintItem *)b)->layer -
	       (int)((const PaintItem *)a)->layer;
}

static int is_splattable(char id)
{
	if (id == 'c' || id == 'p')
		return 1;
	if (id >= '3' && id <= '6')
		return 1;
	return 0;
}

static float layer_depth_factor(int layer)
{
	return (float)(1 << layer);
}

static float layer_radius_factor(int layer)
{
	return 2.0f * (float)(1 << layer);
}

static void set_splat_rgba(Winfo *w, float alpha)
{
	setrgba(w, (float)w->colortable[1].red / 65536.0f,
		(float)w->colortable[1].green / 65536.0f,
		(float)w->colortable[1].blue / 65536.0f, alpha);
}

static void eye_basis_exyz(Winfo *w, float *ex, float *ey, float *ez)
{
	*ex = w->h[0] * w->eyep[0] + w->h[1] * w->eyep[1] +
	      w->h[2] * w->eyep[2];
	*ey = w->u[0] * w->eyep[0] + w->u[1] * w->eyep[1] +
	      w->u[2] * w->eyep[2];
	*ez = w->e[0] * w->eyep[0] + w->e[1] * w->eyep[1] +
	      w->e[2] * w->eyep[2];
}

static void project_xy(Winfo *w, float xx, float yy, float zoom, float ex,
		       float ey, int *ix, int *iy)
{
	*ix = (int)(zoom * (xx - ex)) + w->screenwidth / 2;
	*iy = -(int)(zoom * (yy - ey)) + w->screenheight / 2;
}

static float circle_char_r(Pinfo *qq)
{
	return qq->r;
}

static float poly_char_r(Pinfo *qq, float *cx, float *cy, float *cz)
{
	int n = qq->nvertex - 1;
	int i;
	float sum2 = 0;

	*cx = *cy = *cz = 0;
	if (n < 3)
		return -1.0f;
	for (i = 0; i < n; i++) {
		Vertex *v = qq->vertex[i];
		*cx += v->xx;
		*cy += v->yy;
		*cz += v->zz;
	}
	*cx /= n;
	*cy /= n;
	*cz /= n;
	for (i = 0; i < n; i++) {
		Vertex *v = qq->vertex[i];
		float dx = v->xx - *cx;
		float dy = v->yy - *cy;
		float dz = v->zz - *cz;
		sum2 += dx * dx + dy * dy + dz * dz;
	}
	return sqrtf(sum2 / n);
}

static float circle_splat_key(Pinfo *qq, int layer)
{
	return qq->vertex[0]->eyedist +
	       layer_depth_factor(layer) * circle_char_r(qq);
}

static float poly_splat_key(Pinfo *qq, float ez, int layer)
{
	float cx, cy, cz, r;
	r = poly_char_r(qq, &cx, &cy, &cz);
	if (r < 0)
		return -1.0f;
	return (cz - ez) + layer_depth_factor(layer) * r;
}

static void draw_circle_splat(Winfo *w, Pinfo *qq, float ex, float ey,
			      int layer)
{
	Vertex *v = qq->vertex[0];
	float r = circle_char_r(qq);
	float df = layer_depth_factor(layer);
	float rf = layer_radius_factor(layer);
	float eyedist_s = v->eyedist + df * r;
	float zoom_s;
	int ix, iy, rr;
#ifndef __EMSCRIPTEN__
	cairo_surface_t *mask;
#endif

	if (eyedist_s <= 0)
		return;
	zoom_s = w->screendepth / eyedist_s;
	project_xy(w, v->xx, v->yy, zoom_s, ex, ey, &ix, &iy);
	rr = (int)(zoom_s * rf * r);
	if (rr < 1)
		rr = 1;
	rr = ((rr + SPLAT_RADIUS_QUANTUM / 2) / SPLAT_RADIUS_QUANTUM) *
	     SPLAT_RADIUS_QUANTUM;
	if (ix + rr < 0 || iy + rr < 0 || ix - rr >= w->screenwidth ||
	    iy - rr >= w->screenheight)
		return;
#ifndef __EMSCRIPTEN__
	mask = get_circle_mask(rr);
	if (mask != NULL) {
		cairo_mask_surface(w->cr, mask, ix - rr - 1, iy - rr - 1);
		return;
	}
#endif
	drawcircle2fb(w, ix, iy, rr, TRUE);
}

static void draw_poly_splat(Winfo *w, Pinfo *qq, float ex, float ey, float ez,
			    int layer)
{
	int n = qq->nvertex - 1;
	int i;
	float cx, cy, cz, r;
	float eyedist_s, zoom_s;
	float scx = 0, scy = 0;
	float df = layer_depth_factor(layer);
	float rf = layer_radius_factor(layer);
	int ix, iy;
	ivector2 *poly;

	r = poly_char_r(qq, &cx, &cy, &cz);
	if (r < 0 || n < 3)
		return;

	for (i = 0; i < n; i++) {
		scx += (float)qq->points[i].x;
		scy += (float)qq->points[i].y;
	}
	scx /= n;
	scy /= n;

	eyedist_s = (cz - ez) + df * r;
	if (eyedist_s <= 0)
		return;
	zoom_s = w->screendepth / eyedist_s;
	project_xy(w, cx, cy, zoom_s, ex, ey, &ix, &iy);

	poly = malloc((size_t)n * sizeof(ivector2));
	if (poly == NULL)
		return;
	for (i = 0; i < n; i++) {
		float ox = (float)qq->points[i].x - scx;
		float oy = (float)qq->points[i].y - scy;
		poly[i].x = ix + (int)(rf * ox);
		poly[i].y = iy + (int)(rf * oy);
	}
	drawpoly2fb(w, poly, n, TRUE);
	free(poly);
}

int paint_with_splats(Winfo *w, NewOinfo *o, SplatSolidFn draw_solid)
{
	int i, n, L, nlay;
	float ex, ey, ez;
	float alpha;
	PaintItem *items;

	if (w->nrealize == 0)
		return 0;
	nlay = SPLAT_MAX_LAYERS;
	alpha = splat_layer_alpha((int)w->splat);

	items = malloc((size_t)w->nrealize * (1 + nlay) * sizeof(PaintItem));
	if (items == NULL)
		return 0;

	eye_basis_exyz(w, &ex, &ey, &ez);
	n = 0;
	for (i = 0; i < w->nrealize; i++) {
		Pinfo *qq = ((Pinfo **)o->prims->a)[i];

		if (qq->sortkey < 0)
			break;
		if (is_splattable(qq->id)) {
			for (L = 0; L < nlay; L++) {
				float sk;
				if (qq->id == 'p')
					sk = poly_splat_key(qq, ez, L);
				else
					sk = circle_splat_key(qq, L);
				if (sk <= 0)
					continue;
				items[n].p = qq;
				items[n].sortkey = sk;
				items[n].is_splat = 1;
				items[n].layer = (char)L;
				n++;
			}
		}
		items[n].p = qq;
		items[n].sortkey = qq->sortkey;
		items[n].is_splat = 0;
		items[n].layer = 0;
		n++;
	}

	qsort(items, (size_t)n, sizeof(PaintItem), paint_compar);

	for (i = 0; i < n; i++) {
		Pinfo *qq = items[i].p;
		if (((i % 1000) == 0) && existevent(NULL)) {
			free(items);
			return 1;
		}
		if (items[i].is_splat) {
			int layer = (int)items[i].layer;
			set_splat_rgba(w, alpha);
			if (qq->id == 'p')
				draw_poly_splat(w, qq, ex, ey, ez, layer);
			else
				draw_circle_splat(w, qq, ex, ey, layer);
		} else {
			draw_solid(w, qq);
		}
	}

	free(items);
	return 0;
}
