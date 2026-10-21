#include "splat.h"
#include "gui.h"
#include <math.h>
#include <stdlib.h>

/* Tunable depth-splat parameters */
#define SPLAT_DEPTH_FACTOR 1.0f
#define SPLAT_RADIUS_FACTOR 2.0f
#define SPLAT_POLY_SCALE 1.2f
#define SPLAT_R 0.4f
#define SPLAT_G 0.4f
#define SPLAT_B 0.4f
#define SPLAT_A 0.15f

typedef struct {
	Pinfo *p;
	float sortkey_s;
} SplatItem;

static int splat_compar(const void *a, const void *b)
{
	float ka = ((const SplatItem *)a)->sortkey_s;
	float kb = ((const SplatItem *)b)->sortkey_s;
	/* far → near (larger eyedist first) */
	if (ka > kb)
		return -1;
	if (ka < kb)
		return 1;
	return 0;
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
	drawcircle2fb(w, ix, iy, rr, 1);
}

static void draw_poly_splat(Winfo *w, Pinfo *qq, float ex, float ey, float ez)
{
	int n = qq->nvertex - 1;
	int i;
	float cx = 0, cy = 0, cz = 0;
	float sum2 = 0;
	float r, eyedist_c, eyedist_s, zoom_s;
	ivector2 *poly;

	if (n < 3)
		return;

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
	eyedist_c = cz - ez;
	eyedist_s = eyedist_c + SPLAT_DEPTH_FACTOR * r;
	if (eyedist_s <= 0)
		return;
	zoom_s = w->screendepth / eyedist_s;

	poly = malloc((size_t)n * sizeof(ivector2));
	if (poly == NULL)
		return;
	for (i = 0; i < n; i++) {
		Vertex *v = qq->vertex[i];
		float sx = cx + SPLAT_POLY_SCALE * (v->xx - cx);
		float sy = cy + SPLAT_POLY_SCALE * (v->yy - cy);
		project_xy(w, sx, sy, zoom_s, ex, ey, &poly[i].x, &poly[i].y);
	}
	drawpoly2fb(w, poly, n, 1);
	free(poly);
}

void draw_all_splats(Winfo *w, NewOinfo *o)
{
	int i, ns;
	float ex, ey, ez;
	SplatItem *items;

	if (w->nrealize == 0)
		return;

	items = malloc((size_t)w->nrealize * sizeof(SplatItem));
	if (items == NULL)
		return;

	eye_basis_exyz(w, &ex, &ey, &ez);
	ns = 0;
	for (i = 0; i < w->nrealize; i++) {
		Pinfo *qq = ((Pinfo **)o->prims->a)[i];
		float sortkey_s;

		if (!(qq->layermask & w->layermask))
			continue;
		if (qq->sortkey < 0)
			continue;
		if (!is_splattable(qq->id))
			continue;

		if (qq->id == 'p') {
			int n = qq->nvertex - 1;
			int j;
			float cx = 0, cy = 0, cz = 0;
			float sum2 = 0;
			float r, eyedist_c;

			if (n < 3)
				continue;
			for (j = 0; j < n; j++) {
				Vertex *v = qq->vertex[j];
				cx += v->xx;
				cy += v->yy;
				cz += v->zz;
			}
			cx /= n;
			cy /= n;
			cz /= n;
			for (j = 0; j < n; j++) {
				Vertex *v = qq->vertex[j];
				float dx = v->xx - cx;
				float dy = v->yy - cy;
				float dz = v->zz - cz;
				sum2 += dx * dx + dy * dy + dz * dz;
			}
			r = sqrtf(sum2 / n);
			eyedist_c = cz - ez;
			sortkey_s = eyedist_c + SPLAT_DEPTH_FACTOR * r;
		} else {
			/* circle or n-gon marker */
			sortkey_s = qq->vertex[0]->eyedist +
				    SPLAT_DEPTH_FACTOR * qq->r;
		}
		if (sortkey_s <= 0)
			continue;
		items[ns].p = qq;
		items[ns].sortkey_s = sortkey_s;
		ns++;
	}

	if (ns == 0) {
		free(items);
		return;
	}

	qsort(items, (size_t)ns, sizeof(SplatItem), splat_compar);
	setrgba(w, SPLAT_R, SPLAT_G, SPLAT_B, SPLAT_A);

	for (i = 0; i < ns; i++) {
		Pinfo *qq = items[i].p;
		if (qq->id == 'p')
			draw_poly_splat(w, qq, ex, ey, ez);
		else
			draw_circle_splat(w, qq, ex, ey);
	}

	free(items);
}
