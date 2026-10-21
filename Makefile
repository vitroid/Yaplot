
PROG = yaplot

ALL_SRC = $(wildcard src/*.c)
DESKTOP_SRC = $(filter-out src/web.c,$(ALL_SRC))
WASM_SRC = $(filter-out src/sdl2.c,$(ALL_SRC))
OBJS = $(DESKTOP_SRC:.c=.o)

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

VERSION = 5.1

CFLAGS  = -g -Wall -Werror -Werror=vla -I./include
CFLAGS += $(SDL2_CFLAGS) $(CAIRO_CFLAGS)
CFLAGS += -DFFMPEG=\"'$(BINDIR)/ffmpeg -r %d -i - -y -pix_fmt yuv420p'\"

HEADERS = $(filter-out include/common.h,$(wildcard include/*.h)) include/common.h

WEBDIR = web
EMCC ?= emcc
WASM_CFLAGS = -O2 -Wall -Werror -Werror=vla -I./include
WASM_CFLAGS += -DHELPFILE=\"/help.yap\" -DDEFAULTPALETTE=\"/yaplot.col\"
WASM_LDFLAGS = -s ALLOW_MEMORY_GROWTH=1
WASM_LDFLAGS += -sEXPORTED_RUNTIME_METHODS=UTF8ToString
WASM_LDFLAGS += --preload-file yaplot.col@/yaplot.col
WASM_LDFLAGS += --preload-file help.yap@/help.yap
WASM_LDFLAGS += --preload-file test/global.yap@/global.yap
WASM_OUT = $(WEBDIR)/yaplot.js

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
	cp $(DESKTOP_SRC) Makefile Yaplot-$(VERSION)/
	tar zcf Yaplot-$(VERSION).tar.gz Yaplot-$(VERSION)

.PHONY: wasm
wasm: include/common.h
	mkdir -p $(WEBDIR)
	$(EMCC) $(WASM_CFLAGS) $(WASM_SRC) -o $(WASM_OUT) $(WASM_LDFLAGS)

.PHONY: clean format

clean:
	$(RM) $(PROG) $(OBJS) include/common.h
	$(RM) $(WEBDIR)/yaplot.js $(WEBDIR)/yaplot.wasm $(WEBDIR)/yaplot.data

format:
	fd -e h -e c -x clang-format -i {}
	clang-format -i common.h.in
