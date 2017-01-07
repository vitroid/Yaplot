#requires pkg-config
PROG=yaplot
SRC=Hash.c reality0.c reality2.c reality4.c varray.c cache.c  gtk.c  reality1.c reality3.c unixwrap.c yaplot.c
OBJS=$(patsubst %.c,%.o,$(SRC))
DEPENDS=$(patsubst %.c,%.d,$(SRC))
GTK_CFLAGS=$(shell pkg-config gtk+-2.0 --cflags)
GTK_LDFLAGS=$(shell pkg-config gtk+-2.0 --libs)
PNG_LDFLAGS=$(shell pkg-config libpng --libs)
CFLAGS=-g -Wall -Werror -DRECORD -I/opt/X11/include $(GTK_CFLAGS)
LDFLAGS=$(GTK_LDFLAGS) $(PNG_LDFLAGS)
PKGDATADIR=/usr/local/share/yaplot
BINDIR=/usr/local/bin

%.h: %.h.in
	sed -e sX@pkgdatadir@X$(PKGDATADIR)X $< > $@
%.o: common.h
$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LIBS) $(LDFLAGS)
install:
	install yaplot $(BINDIR)
	install yaplot.col help.yap $(PKGDATADIR)
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