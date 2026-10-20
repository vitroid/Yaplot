
PROG = yaplot

SRC  = $(wildcard src/*.c)
OBJS = $(SRC:.c=.o)

BREW_PREFIX ?= /opt/homebrew
PKG_CONFIG  ?= $(BREW_PREFIX)/bin/pkg-config

SDL2_CFLAGS  = $(shell $(PKG_CONFIG) sdl2 --cflags)
SDL2_LDFLAGS = $(shell $(PKG_CONFIG) sdl2 --libs)
CAIRO_CFLAGS = $(shell $(PKG_CONFIG) cairo --cflags)
CAIRO_LDFLAGS = $(shell $(PKG_CONFIG) cairo --libs)
PNG_LDFLAGS  = $(shell $(PKG_CONFIG) libpng --libs)
LDFLAGS      = $(SDL2_LDFLAGS) $(CAIRO_LDFLAGS) $(PNG_LDFLAGS) -lm

PKGDATADIR = /opt/homebrew/share/yaplot
BINDIR     = /opt/homebrew/bin

VERSION = 5.0

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
