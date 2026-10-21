#include "splat.h"
#include "gui.h"
#include <math.h>
#include <stdlib.h>

/*
 * Layer L (0-based), with n = w->splat layers (1..3):
 *   depth_factor  = 2^L
 *   radius_factor = 2 * 2^L
 *   alpha        = base(n) / n
 *     n=1: base 0.25
 *     n=2: base 0.25*(2/3)
 *     n=3: base 0.25*(1/2)
 */
#define SPLAT_BASE_ALPHA 0.25f
#define SPLAT_MAX_LAYERS 3

static float splat_base_alpha(int n)
{
	if (n == 2)
		return SPLAT_BASE_ALPHA * (2.0f / 3.0f);
	if (n == 3)
		return SPLAT_BASE_ALPHA * 0.5f;
	return SPLAT_BASE_ALPHA;
}

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

	if (eyedist_s <= 0)
		return;
	zoom_s = w->screendepth / eyedist_s;
	project_xy(w, v->xx, v->yy, zoom_s, ex, ey, &ix, &iy);
	rr = (int)(zoom_s * rf * r);
	if (rr < 1)
		rr = 1;
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

void paint_with_splats(Winfo *w, NewOinfo *o, SplatSolidFn draw_solid)
{
	int i, n, L, nlay;
	float ex, ey, ez;
	float alpha;
	PaintItem *items;

	if (w->nrealize == 0)
		return;
	nlay = (int)w->splat;
	if (nlay < 1)
		nlay = 1;
	if (nlay > SPLAT_MAX_LAYERS)
		nlay = SPLAT_MAX_LAYERS;
	alpha = splat_base_alpha(nlay) / (float)nlay;

	items = malloc((size_t)w->nrealize * (1 + nlay) * sizeof(PaintItem));
	if (items == NULL)
		return;

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
			return;
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
}
