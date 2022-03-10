#requires pkg-config and gtk+ via github
PROG=yaplot
SRC=Hash.c reality0.c reality2.c reality4.c varray.c cache.c  gtk.c  reality1.c reality3.c unixwrap.c yaplot.c
OBJS=$(patsubst %.c,%.o,$(SRC))
DEPENDS=$(patsubst %.c,%.d,$(SRC))
GTK_CFLAGS=$(shell pkg-config gtk+-3.0 --cflags)
GTK_LDFLAGS=$(shell pkg-config gtk+-3.0 --libs)
PNG_LDFLAGS=$(shell pkg-config libpng --libs)
LDFLAGS=$(GTK_LDFLAGS) $(PNG_LDFLAGS) -lm
PKGDATADIR=/opt/homebrew/share/yaplot
BINDIR=/opt/homebrew/bin
# FFMPEG=
FFMPEG=-DFFMPEG=\"'$(BINDIR)/ffmpeg -r %d -i - -y -pix_fmt yuv420p'\"
VERSION=4.1.2
CFLAGS=-g -Wall -Werror -Werror=vla  -I/opt/X11/include $(GTK_CFLAGS) -DGTK_DISABLE_SINGLE_INCLUDES -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -DGSEAL_ENABLE $(FFMPEG)

%.h: %.h.in Makefile
	sed -e sX%%pkgdatadir%%X$(PKGDATADIR)X -e sX%%version%%X$(VERSION)X $< > $@
%.rb: %.rb.in Makefile tarball
	set `shasum docs/tarballs/Yaplot-$(VERSION).tar.gz`; \
	    sed -e sX%%sha%%X$$1X -e sX%%version%%X$(VERSION)X $< > $@
%.o: common.h
$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LIBS) $(LDFLAGS)
install: yaplot
	install yaplot $(BINDIR)
	install -d $(PKGDATADIR)
	install yaplot.col help.yap $(PKGDATADIR)
tarball:
	-mkdir Yaplot-$(VERSION)
	cd Yaplot-$(VERSION); cp ../*.h ../*.c ../Makefile .
	tar zcf docs/tarballs/Yaplot-$(VERSION).tar.gz Yaplot-$(VERSION)
.PHONY: clean depend distclean
clean:
	-$(RM) $(PROG) $(OBJS) $(DEPENDS)
distclean: clean
	-rm *~
	-rm common.h
%.d: %.c common.h
	@set -e; $(CC) -MM $(CFLAGS) $< \
		| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
		[ -s $@ ] || rm -f $@
-include $(DEPENDS)
