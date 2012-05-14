#SUBDIRS		= utils samples
bin_PROGRAMS	= @YAPLOT@
EXTRA_DIST	= common.h.in help.yap yaplot.col README.html WelcomeJ.html Makefile.dos README README.j yaplot.spec.in
pkgdata_DATA	= help.yap yaplot.col
EXTRA_PROGRAMS	= yaplot #yaplot_gtk #yaplot_glut yaplot gnome_yaplot gaplot_example

#gaplot_example_SOURCES	= gaplot.h gaplot.c gaplot_example.c
yaplot_CFLAGS	= -Wall -Werror # -g -pg
yaplot_SOURCES	= unixwrap.c varray.c reality0.c reality1.c reality2.c reality3.c reality4.c Hash.c cache.c yaplot.c Hash.h      os.h        varray.h    yaplot.h common.h   cache.h     gui.h       unixwrap.h gtk.h gtk.c
#yaplot_glut_SOURCES	= unixwrap.c varray.c reality0.c reality1.c reality2.c reality3.c reality4.c Hash.c cache.c yaplot.c Hash.h      os.h        varray.h    yaplot.h common.h   cache.h     gui.h       unixwrap.h glut.h glut.c
#yaplot_SOURCES	= unixwrap.c varray.c reality0.c reality1.c reality2.c reality3.c reality4.c Hash.c cache.c yaplot.c Hash.h      os.h        varray.h    yaplot.h  common.h   cache.h     gui.h       unixwrap.h x11wrap.c x11wrap.h
#gnome_yaplot_SOURCES	= gnome_yaplot.c
#gnome_yaplot_LDADD	= #$(GNOME_LIBDIR) $(GNOMEUI_LIBS) #$(INTLLIBS)
#EXTRA_yaplot_SOURCES = win32wrap.c win32wrap.h

#yaplot_CFLAGS	= -g -Wall -Werror
INCLUDES	= @X_CFLAGS@ -I$(includedir) -I/usr/lib/gnome-libs/include
LDADD		= @X_LIBS@ -lX11 
CLEANFILES	= *~
DISTCLEANFILES	= .deps/*.P

pkgdatadir = $(datadir)/@PACKAGE@
pkglibdir = $(libdir)/@PACKAGE@
pkgincludedir = $(includedir)/@PACKAGE@
package = @PACKAGE@
version = @VERSION@

%.h: %.h.in
	sed -e 's!@pkgdatadir@!$(pkgdatadir)!g' $< > $@
%.spec: %.spec.in
	sed -e 's!@pkgdatadir@!$(pkgdatadir)!g' -e 's!@version@!$(version)!g' -e 's!@package@!$(package)!g' $< > $@
yaplot_gtk.spec.in: yaplot.spec.in
	sed -e 's!--without-gtk!!g' -e 's!Name: @package@!Name: @package@-gtk!g' $< > $@
README: README.html
	lynx -dump $< > $@
README.j: WelcomeJ.html
	lynx -dump $< > $@

@PACKAGE@-dos.zip: $(wildcard *.dll) yaplot.exe yaplot.col help.yap
	zip $@ $^
