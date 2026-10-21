
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

HEADERS = $(filter-out include/common.h,$(wildcard include/*.h)) include/common.h

.PHONY: all
all: $(PROG)

include/common.h: common.h.in Makefile
	sed -e sX%%pkgdatadir%%X$(PKGDATADIR)X -e sX%%version%%X$(VERSION)X $< > $@

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

$(OBJS): $(HEADERS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

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
