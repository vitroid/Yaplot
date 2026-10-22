PROG = yaplot

BREW_PREFIX ?= /opt/homebrew
PKG_CONFIG  ?= $(BREW_PREFIX)/bin/pkg-config

SDL2_CFLAGS  = $(shell $(PKG_CONFIG) sdl2 --cflags)
SDL2_LDFLAGS = $(shell $(PKG_CONFIG) sdl2 --libs)
CAIRO_CFLAGS = $(shell $(PKG_CONFIG) cairo --cflags)
CAIRO_LDFLAGS = $(shell $(PKG_CONFIG) cairo --libs)
PNG_LDFLAGS  = $(shell $(PKG_CONFIG) libpng --libs)
RSVG_CFLAGS  = $(shell $(PKG_CONFIG) librsvg-2.0 --cflags)
RSVG_LDFLAGS = $(shell $(PKG_CONFIG) librsvg-2.0 --libs)
LDFLAGS      = $(SDL2_LDFLAGS) $(CAIRO_LDFLAGS) $(PNG_LDFLAGS) $(RSVG_LDFLAGS) -lm

PKGDATADIR = $(BREW_PREFIX)/share/yaplot
BINDIR     = $(BREW_PREFIX)/bin
FFMPEG_BINDIR = $(BREW_PREFIX)/bin

VERSION = 5.5

CFLAGS  = -g -Wall -Werror -Werror=vla -I./include
CFLAGS += $(SDL2_CFLAGS) $(CAIRO_CFLAGS) $(RSVG_CFLAGS)
CFLAGS += -DFFMPEG=\"'$(FFMPEG_BINDIR)/ffmpeg -r %d -i - -y -pix_fmt yuv420p'\"

UNAME_S := $(shell uname -s)
C_SRC = $(filter-out src/macos_gestures_stub.c src/web.c,$(wildcard src/*.c))
WASM_SRC = $(filter-out src/sdl2.c src/macos_gestures_stub.c,$(wildcard src/*.c))

ifeq ($(UNAME_S),Darwin)
  GESTURE_OBJ = src/macos_gestures.o
  LDFLAGS += -framework AppKit
else
  GESTURE_OBJ = src/macos_gestures_stub.o
endif

OBJS = $(C_SRC:.c=.o) $(GESTURE_OBJ)

HEADERS = $(filter-out include/common.h,$(wildcard include/*.h)) include/common.h

WEBDIR = web
EMCC ?= emcc
WASM_CFLAGS = -O2 -Wall -Werror -Werror=vla -I./include
WASM_CFLAGS += -DHELPFILE=\"/help.yap\" -DDEFAULTPALETTE=\"/yaplot.col\"
WASM_LDFLAGS = -s ALLOW_MEMORY_GROWTH=1 -sSTACK_SIZE=8MB
WASM_LDFLAGS += -sEXPORTED_RUNTIME_METHODS=UTF8ToString,stringToUTF8,FS
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

src/macos_gestures.o: src/macos_gestures.m
	$(CC) $(CFLAGS) -fobjc-arc -c $< -o $@

install: $(PROG)
	install $(PROG) $(BINDIR)
	install -d $(PKGDATADIR)
	install yaplot.col help.yap $(PKGDATADIR)

tarball:
	mkdir Yaplot-$(VERSION)
	cp $(C_SRC) src/macos_gestures.m src/macos_gestures_stub.c Makefile Yaplot-$(VERSION)/
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
