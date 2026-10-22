#ifndef _SPLAT_H
#define _SPLAT_H
#include "common.h"

typedef void (*SplatSolidFn)(Winfo *w, Pinfo *qq);

/*
 * Depth-sorted paint of drop-shadows (palette 1 @ 50%) merged with solids
 * so shadows can fog farther content. draw_solid is called for each solid.
 */
int paint_with_splats(Winfo *w, NewOinfo *o, SplatSolidFn draw_solid);

#endif
