#include "common.h"
#include "gui.h"
#include "splat.h"
#include <math.h>

extern int debug;
extern vector2 polygon[6][4];

static float r2_thickness, r2_thickdelta;

static void reality2_solid(Winfo *w, Pinfo *qq)
{
	int r;
	ivector2 poly[7];
	char ch = qq->id;

	if (ch != 't') {
		r2_thickness += r2_thickdelta;
		setlinewidth(w, r2_thickness);
	}
	if (ch >= '3' && ch <= '6') {
		int n, j;
		float rr;

		n = ch - '0';
		rr = qq->vertex[0]->zoom * qq->r;
		for (j = 0; j < n; j++) {
			poly[j].x =
			    qq->points[0].x + (int)(polygon[j][n - 3].x * rr);
			poly[j].y =
			    qq->points[0].y + (int)(polygon[j][n - 3].y * rr);
		}
		poly[n].x = poly[0].x;
		poly[n].y = poly[0].y;
		setfgcolor(w, qq->color);
		drawpoly2fb(w, poly, n, FALSE);
		return;
	}
	switch (ch) {
	case 't':
		setfgcolor(w, qq->color);
		drawstring2fb(w, qq->points[0].x, qq->points[0].y, qq->label,
			      strlen(qq->label));
		break;
	case 'l':
		setfgcolor(w, qq->color);
		drawline2fb(w, qq->points[0].x, qq->points[0].y, qq->points[1].x,
			    qq->points[1].y);
		break;
	case 'c':
		r = (int)(qq->vertex[0]->zoom * qq->r);
		settpcolor(w, qq->color);
		drawcircle2fb(w, qq->points[0].x, qq->points[0].y, r, TRUE);
		setlinewidth(w, 1);
		drawcircle2fb(w, qq->points[0].x, qq->points[0].y, r, FALSE);
		setlinewidth(w, r2_thickness);
		break;
	case 'o':
		r = (int)(qq->vertex[0]->zoom * qq->r);
		setfgcolor(w, qq->color);
		drawcircle2fb(w, qq->points[0].x, qq->points[0].y, r, FALSE);
		break;
	case 's':
		r = (int)(qq->vertex[0]->zoom * qq->r);
		if (qq->arrowtype <= 1) {
			setfgcolor(w, qq->color);
			drawstick2fb(w, qq->points[0].x, qq->points[0].y,
				     qq->points[1].x, qq->points[1].y, r, FALSE,
				     qq->arrowtype);
		} else {
			settpcolor(w, qq->color);
			drawstick2fb(w, qq->points[0].x, qq->points[0].y,
				     qq->points[1].x, qq->points[1].y, r, TRUE,
				     qq->arrowtype);
			setlinewidth(w, 1);
			drawstick2fb(w, qq->points[0].x, qq->points[0].y,
				     qq->points[1].x, qq->points[1].y, r, FALSE,
				     qq->arrowtype);
			setlinewidth(w, r2_thickness);
		}
		break;
	case 'p':
		settpcolor(w, qq->color);
		drawpoly2fb(w, qq->points, qq->nvertex - 1, TRUE);
		setlinewidth(w, 1);
		drawpoly2fb(w, qq->points, qq->nvertex - 1, FALSE);
		setlinewidth(w, r2_thickness);
		break;
	}
}

int Reality2(NewOinfo *o, Winfo *w)
{
	int i;

	if (debug)
		fprintf(stderr, "Nrealize: %d\n", w->nrealize);
	if (w->nrealize == 0)
		return 0;

	r2_thickness = 0.5;
	r2_thickdelta =
	    w->thick * 0.001 * 2.0 * w->screenwidth / (float)w->nscalable;
	setlinewidth(w, r2_thickness);

	if (w->splat) {
		paint_with_splats(w, o, reality2_solid);
		return 0;
	}

	for (i = 0; i < w->nrealize; i++) {
		Pinfo *qq = ((Pinfo **)o->prims->a)[i];
		if (((i % 1000) == 0) && existevent(NULL))
			return 1;
		if (qq->sortkey < 0)
			return 0;
		reality2_solid(w, qq);
	}
	return 0;
}
