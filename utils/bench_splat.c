#include <cairo.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 512
#define HEIGHT 512
#define OBJECTS 1846
#define LAYERS 3
#define FRAMES 30

typedef struct {
	int x;
	int y;
	int r;
} Circle;

static double now_seconds(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
}

static cairo_surface_t *make_circle_mask(int r)
{
	int size = 2 * r + 3;
	cairo_surface_t *surface =
	    cairo_image_surface_create(CAIRO_FORMAT_A8, size, size);
	cairo_t *cr = cairo_create(surface);

	cairo_set_antialias(cr, CAIRO_ANTIALIAS_FAST);
	cairo_set_source_rgba(cr, 1, 1, 1, 1);
	cairo_arc(cr, r + 1, r + 1, r, 0, 2 * 3.14159265358979323846);
	cairo_fill(cr);
	cairo_destroy(cr);
	return surface;
}

static void clear_surface(cairo_t *cr)
{
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
	cairo_set_source_rgb(cr, 0.72, 0.88, 0.98);
	cairo_paint(cr);
	cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
}

static void draw_direct(cairo_t *cr, const Circle *circles, int n,
			double alpha)
{
	int i;
	cairo_set_source_rgba(cr, 0.15, 0.25, 0.18, alpha);
	for (i = 0; i < n; i++) {
		cairo_new_sub_path(cr);
		cairo_arc(cr, circles[i].x, circles[i].y, circles[i].r, 0,
			  2 * 3.14159265358979323846);
		cairo_fill(cr);
	}
}

static void draw_masked(cairo_t *cr, const Circle *circles, int n,
			cairo_surface_t **masks, double alpha)
{
	int i;
	cairo_set_source_rgba(cr, 0.15, 0.25, 0.18, alpha);
	for (i = 0; i < n; i++) {
		int layer = i % LAYERS;
		int r = circles[i].r;
		cairo_mask_surface(cr, masks[layer], circles[i].x - r - 1,
				   circles[i].y - r - 1);
	}
}

static uint32_t checksum(cairo_surface_t *surface)
{
	unsigned char *data;
	int stride;
	uint32_t sum = 0;
	int x, y;

	cairo_surface_flush(surface);
	data = cairo_image_surface_get_data(surface);
	stride = cairo_image_surface_get_stride(surface);
	for (y = 0; y < HEIGHT; y += 17) {
		uint32_t *row = (uint32_t *)(data + y * stride);
		for (x = 0; x < WIDTH; x += 17)
			sum = sum * 33u + row[x];
	}
	return sum;
}

static void run_case(const char *name, cairo_t *cr, cairo_surface_t *surface,
		     const Circle *circles, int n, cairo_surface_t **masks,
		     double alpha, int masked)
{
	double start;
	double elapsed;
	uint32_t sum;
	int frame;

	for (frame = 0; frame < 3; frame++) {
		clear_surface(cr);
		if (masked)
			draw_masked(cr, circles, n, masks, alpha);
		else
			draw_direct(cr, circles, n, alpha);
	}

	start = now_seconds();
	for (frame = 0; frame < FRAMES; frame++) {
		clear_surface(cr);
		if (masked)
			draw_masked(cr, circles, n, masks, alpha);
		else
			draw_direct(cr, circles, n, alpha);
	}
	elapsed = now_seconds() - start;
	sum = checksum(surface);
	printf("%-24s %7.2f ms/frame  %7.1f fps  checksum=%08x\n", name,
	       elapsed * 1000.0 / FRAMES, FRAMES / elapsed, sum);
}

int main(void)
{
	const int radii[LAYERS] = {6, 12, 24};
	const int n = OBJECTS * LAYERS;
	Circle *circles = malloc((size_t)n * sizeof(*circles));
	cairo_surface_t *masks[LAYERS];
	cairo_surface_t *surface;
	cairo_t *cr;
	uint32_t state = 1;
	int i, layer;

	if (circles == NULL)
		return 1;
	for (i = 0; i < OBJECTS; i++) {
		state = state * 1664525u + 1013904223u;
		for (layer = 0; layer < LAYERS; layer++) {
			Circle *c = &circles[i * LAYERS + layer];
			c->x = (int)((state + (uint32_t)(layer * 37)) % WIDTH);
			c->y =
			    (int)(((state >> 9) + (uint32_t)(layer * 53)) %
				  HEIGHT);
			c->r = radii[layer];
		}
	}
	for (layer = 0; layer < LAYERS; layer++)
		masks[layer] = make_circle_mask(radii[layer]);

	surface =
	    cairo_image_surface_create(CAIRO_FORMAT_ARGB32, WIDTH, HEIGHT);
	cr = cairo_create(surface);
	cairo_set_antialias(cr, CAIRO_ANTIALIAS_FAST);

	printf("%d objects x %d layers = %d circles, %dx%d, %d frames\n",
	       OBJECTS, LAYERS, n, WIDTH, HEIGHT, FRAMES);
	run_case("direct opaque", cr, surface, circles, n, masks, 1.0, 0);
	run_case("direct translucent", cr, surface, circles, n, masks,
		 0.125 / 3.0, 0);
	run_case("cached mask opaque", cr, surface, circles, n, masks, 1.0, 1);
	run_case("cached mask translucent", cr, surface, circles, n, masks,
		 0.125 / 3.0, 1);

	cairo_destroy(cr);
	cairo_surface_destroy(surface);
	for (layer = 0; layer < LAYERS; layer++)
		cairo_surface_destroy(masks[layer]);
	free(circles);
	return 0;
}
