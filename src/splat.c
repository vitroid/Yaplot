#include "splat.h"
#include "gui.h"
#include <math.h>
#include <stdlib.h>

/* Tunable depth-splat parameters */
#define SPLAT_DEPTH_FACTOR 1.0f
#define SPLAT_RADIUS_FACTOR 2.0f
/* Shadow: palette 1 (background) at 50% via settpcolor(). */

typedef struct {
	Pinfo *p;
	float sortkey;
	char is_splat;
} PaintItem;

static int paint_compar(const void *a, const void *b)
{
	float ka = ((const PaintItem *)a)->sortkey;
	float kb = ((const PaintItem *)b)->sortkey;
	/* far → near (larger eyedist first) */
	if (ka > kb)
		return -1;
	if (ka < kb)
		return 1;
	/* At equal depth, paint splat before solid (shadow behind). */
	return (int)((const PaintItem *)b)->is_splat -
	       (int)((const PaintItem *)a)->is_splat;
}

static int is_splattable(char id)
{
	if (id == 'c' || id == 'p')
		return 1;
	if (id >= '3' && id <= '6')
		return 1;
	return 0;
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

static float circle_splat_key(Pinfo *qq)
{
	return qq->vertex[0]->eyedist + SPLAT_DEPTH_FACTOR * qq->r;
}

static float poly_splat_key(Pinfo *qq, float ez)
{
	int n = qq->nvertex - 1;
	int i;
	float cx = 0, cy = 0, cz = 0;
	float sum2 = 0;
	float r;

	if (n < 3)
		return -1.0f;
	for (i = 0; i < n; i++) {
		Vertex *v = qq->vertex[i];
		cx += v->xx;
		cy += v->yy;
		cz += v->zz;
	}
	cx /= n;
	cy /= n;
	cz /= n;
	for (i = 0; i < n; i++) {
		Vertex *v = qq->vertex[i];
		float dx = v->xx - cx;
		float dy = v->yy - cy;
		float dz = v->zz - cz;
		sum2 += dx * dx + dy * dy + dz * dz;
	}
	r = sqrtf(sum2 / n);
	return (cz - ez) + SPLAT_DEPTH_FACTOR * r;
}

static void draw_circle_splat(Winfo *w, Pinfo *qq, float ex, float ey)
{
	Vertex *v = qq->vertex[0];
	float r = qq->r;
	float eyedist_s = v->eyedist + SPLAT_DEPTH_FACTOR * r;
	float zoom_s;
	int ix, iy, rr;

	if (eyedist_s <= 0)
		return;
	zoom_s = w->screendepth / eyedist_s;
	project_xy(w, v->xx, v->yy, zoom_s, ex, ey, &ix, &iy);
	rr = (int)(zoom_s * SPLAT_RADIUS_FACTOR * r);
	if (rr < 1)
		rr = 1;
	drawcircle2fb(w, ix, iy, rr, TRUE);
}

static void draw_ngon_splat(Winfo *w, Pinfo *qq, float ex, float ey)
{
	/* Same push as circle; use screen disk for markers 3–6. */
	draw_circle_splat(w, qq, ex, ey);
}

static void draw_poly_splat(Winfo *w, Pinfo *qq, float ex, float ey, float ez)
{
	int n = qq->nvertex - 1;
	int i;
	float cx = 0, cy = 0, cz = 0;
	float sum2 = 0;
	float r, eyedist_c, eyedist_s, zoom_s;
	float scx = 0, scy = 0;
	int ix, iy;
	ivector2 *poly;

	if (n < 3)
		return;

	/* Eye-space centroid + RMS size (for depth push). */
	for (i = 0; i < n; i++) {
		Vertex *v = qq->vertex[i];
		cx += v->xx;
		cy += v->yy;
		cz += v->zz;
		scx += (float)qq->points[i].x;
		scy += (float)qq->points[i].y;
	}
	cx /= n;
	cy /= n;
	cz /= n;
	scx /= n;
	scy /= n;

	for (i = 0; i < n; i++) {
		Vertex *v = qq->vertex[i];
		float dx = v->xx - cx;
		float dy = v->yy - cy;
		float dz = v->zz - cz;
		sum2 += dx * dx + dy * dy + dz * dz;
	}
	r = sqrtf(sum2 / n);
	eyedist_c = cz - ez;
	eyedist_s = eyedist_c + SPLAT_DEPTH_FACTOR * r;
	if (eyedist_s <= 0)
		return;
	zoom_s = w->screendepth / eyedist_s;
	project_xy(w, cx, cy, zoom_s, ex, ey, &ix, &iy);

	/*
	 * Expand in *screen* space about the solid centroid by
	 * SPLAT_RADIUS_FACTOR, then recenter on the depth-pushed projection.
	 * Eye-space 1.2× was cancelled by perspective shrink and looked
	 * almost the same size as the body.
	 */
	poly = malloc((size_t)n * sizeof(ivector2));
	if (poly == NULL)
		return;
	for (i = 0; i < n; i++) {
		float ox = (float)qq->points[i].x - scx;
		float oy = (float)qq->points[i].y - scy;
		poly[i].x = ix + (int)(SPLAT_RADIUS_FACTOR * ox);
		poly[i].y = iy + (int)(SPLAT_RADIUS_FACTOR * oy);
	}
	drawpoly2fb(w, poly, n, TRUE);
	free(poly);
}

void paint_with_splats(Winfo *w, NewOinfo *o, SplatSolidFn draw_solid)
{
	int i, n;
	float ex, ey, ez;
	PaintItem *items;

	if (w->nrealize == 0)
		return;

	items = malloc((size_t)w->nrealize * 2 * sizeof(PaintItem));
	if (items == NULL)
		return;

	eye_basis_exyz(w, &ex, &ey, &ez);
	n = 0;
	for (i = 0; i < w->nrealize; i++) {
		Pinfo *qq = ((Pinfo **)o->prims->a)[i];
		float sk;

		if (qq->sortkey < 0)
			break;
		if (is_splattable(qq->id)) {
			if (qq->id == 'p')
				sk = poly_splat_key(qq, ez);
			else
				sk = circle_splat_key(qq);
			if (sk > 0) {
				items[n].p = qq;
				items[n].sortkey = sk;
				items[n].is_splat = 1;
				n++;
			}
		}
		items[n].p = qq;
		items[n].sortkey = qq->sortkey;
		items[n].is_splat = 0;
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
			settpcolor(w, 1);
			if (qq->id == 'p')
				draw_poly_splat(w, qq, ex, ey, ez);
			else if (qq->id >= '3' && qq->id <= '6')
				draw_ngon_splat(w, qq, ex, ey);
			else
				draw_circle_splat(w, qq, ex, ey);
		} else {
			draw_solid(w, qq);
		}
	}

	free(items);
}
