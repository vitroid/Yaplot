#requires pkg-config

OBJS=varray.o yaplot.o reality0.o reality1.o reality2.o reality3.o reality4.o unixwrap.o cache.o Hash.o gtk.o
GTK_CFLAGS=$(shell pkg-config gtk+-2.0 --cflags)
GTK_LDFLAGS=$(shell pkg-config gtk+-2.0 --libs)
PNG_LDFLAGS=$(shell pkg-config libpng --libs)
CFLAGS=-g -Wall -Werror -DRECORD -I/opt/X11/include $(GTK_CFLAGS)
LDFLAGS=$(GTK_LDFLAGS) $(PNG_LDFLAGS)
PKGDATADIR=/usr/local/share/yaplot

%.h: %.h.in
	sed -e sX@pkgdatadir@X$(PKGDATADIR)X $< > $@
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
yaplot: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LIBS) $(LDFLAGS)
install:
	
clean:
	-rm *.o
	-rm *~
	-rm common.h
	-rm yaplot
