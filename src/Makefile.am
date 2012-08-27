INCLUDES = $(JEFFX_CFLAGS) $(FXSCINTILLA_CFLAGS) $(FOX_CFLAGS) $(LUA_CFLAGS) $(X11_CFLAGS)
LDADD = $(JEFFX_LIBS) $(FXSCINTILLA_LIBS) $(FOX_LIBS) $(LUA_LIBS) $(X11_LIBS) $(LIBINTL)
DEFS += -DLOCALEDIR=\"$(localedir)\"


bin_PROGRAMS=fxite

utildir = $(datadir)/applications
util_DATA = fxite.desktop

icondir = $(prefix)/share/pixmaps
icon_DATA = fxite.xpm

fxite_LDFLAGS=$(FXSCINTILLA_LIBS) $(LUA_LIBS) $(LDFLAGS)

fxite_SOURCES=\
  appmain.cpp appname.cpp appwin_ev.cpp appwin_mk.cpp filer.cpp \
  lang.cpp luacmds.cpp luafuncs.cpp macro.cpp prefdlg.cpp prefs.cpp readtags.c \
  recorder.cpp scidoc.cpp search.cpp shmenu.cpp doctabs.cpp tagread.cpp scisrch.cpp \
  export.cpp help.cpp toolmgr.cpp tooltree.cpp backup.cpp fxasq.cpp fxasq_c.cpp fxasq_lua.c \
  menuspec.cpp isutf8.c winshcut.c luafx.cpp \
  migration.cpp filedlg.cpp theme.cpp prefdlg_ext.cpp \
  prefdlg_sntx.cpp prefdlg_tbar.cpp toolbar.cpp outpane.cpp statusbar.cpp \
  mainmenu.cpp appwin_pub.cpp scidoc_util.cpp foreachtab.cpp cmd_utils.cpp appwin_base.cpp


noinst_HEADERS=\
  appmain.h appwin.h export.h filer.h help.h helptext.h help_lua.h lang.h \
  luacmds.h luafuncs.h macro.h prefdlg.h prefs.h readtags.h recorder.h scidoc.h scisrch.h search.h \
  shmenu.h doctabs.h tagread.h toolmgr.h tooltree.h langlist.h backup.h fxasq.h fxasq_c.h \
  fxasq_lua.h luasci.h menuspec.h luafx.h appname.h \
  filedlg.h theme.h prefdlg_ext.h prefdlg_sntx.h prefdlg_tbar.h toolbar.h outpane.h statusbar.h\
  mainmenu.h appwin_pub.h scidoc_util.h foreachtab.h cmd_utils.h appwin_base.h




EXTRA_DIST=fxite.xpm icon32x32.ico icon32x32.rc icon32x32.coff \
  helptext.txt help_lua.txt fxite.desktop


$(srcdir)/help.cpp: $(srcdir)/helptext.h $(srcdir)/help_lua.h

$(srcdir)/helptext.h: $(srcdir)/helptext.txt
	awk '{if (/\\$$/) {sub(/\\$$/,""); printf("%s",$$0)} else {print}}' $< > $<.tmp
	$(RESWRAP) -o $@ $<.tmp
	$(RM) $<.tmp

$(srcdir)/help_lua.h: $(srcdir)/help_lua.txt
	awk '{if (/\\$$/) {sub(/\\$$/,""); printf("%s",$$0)} else {print}}' $< > $<.tmp
	$(RESWRAP) -o $@ $<.tmp
	$(RM) $<.tmp

# Inlining does not work for sl library!
sl.o: $(srcdir)/sl.c $(srcdir)/sl.h
	$(CC) $(CFLAGS) -c -fno-inline -o $@ $<


$(srcdir)/icon32x32.ico: $(srcdir)/fxite.xpm
	convert $< $@


$(srcdir)/icon32x32.rc: $(srcdir)/icon32x32.ico


$(srcdir)/icon32x32.coff: $(srcdir)/icon32x32.rc
	windres -i $< -o $@

win32: $(srcdir)/icon32x32.coff all



