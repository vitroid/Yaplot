
PROG = yaplot

SRC  = $(wildcard src/*.c)
OBJS = $(SRC:.c=.o)

SDL2_CFLAGS  = $(shell pkg-config sdl2 --cflags)
SDL2_LDFLAGS = $(shell pkg-config sdl2 --libs)
CAIRO_CFLAGS = $(shell pkg-config cairo --cflags)
CAIRO_LDFLAGS = $(shell pkg-config cairo --libs)
PNG_LDFLAGS  = $(shell pkg-config libpng --libs)
LDFLAGS      = $(SDL2_LDFLAGS) $(CAIRO_LDFLAGS) $(PNG_LDFLAGS) -lm

PKGDATADIR = /opt/homebrew/share/yaplot
BINDIR     = /opt/homebrew/bin

VERSION = 4.2

CFLAGS  = -g -Wall -Werror -Werror=vla -I./include
CFLAGS += $(SDL2_CFLAGS) $(CAIRO_CFLAGS)
CFLAGS += -DFFMPEG=\"'$(BINDIR)/ffmpeg -r %d -i - -y -pix_fmt yuv420p'\"

.PHONY: all
all: gen_common $(PROG)

gen_common:
	sed -e sX%%pkgdatadir%%X$(PKGDATADIR)X -e sX%%version%%X$(VERSION)X common.h.in > include/common.h

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

install: $(PROG)
	install $(PROG) $(BINDIR)
	install -d $(PKGDATADIR)
	install yaplot.col help.yap $(PKGDATADIR)

tarball:
	mkdir Yaplot-$(VERSION)
	cp $(SRC) Makefile Yaplot-$(VERSION)/
	tar zcf Yaplot-$(VERSION).tar.gz Yaplot-$(VERSION)

.PHONY: clean format

clean:
	$(RM) $(PROG) $(OBJS) include/common.h

format:
	fd -e h -e c -x clang-format -i {}
	clang-format -i common.h.in
