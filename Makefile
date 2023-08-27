PROG = yaplot

SRC  = $(wildcard src/*.c)
OBJS = $(SRC:.c=.o)

GTK_CFLAGS  = $(shell pkg-config gtk+-3.0 --cflags)
GTK_LDFLAGS = $(shell pkg-config gtk+-3.0 --libs)
PNG_LDFLAGS = $(shell pkg-config libpng --libs)
LDFLAGS     = $(GTK_LDFLAGS) $(PNG_LDFLAGS) -lm

PKGDATADIR = /opt/homebrew/share/yaplot
BINDIR     = /opt/homebrew/bin

VERSION = 4.1.2

CFLAGS  = -g -Wall -Werror -Werror=vla -I/opt/X11/include -I./include
CFLAGS += $(GTK_CFLAGS) -DGTK_DISABLE_SINGLE_INCLUDES -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -DGSEAL_ENABLE
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
