INCLUDES = $(FXSCINTILLA_CFLAGS) $(FOX_CFLAGS) $(LUA_CFLAGS) $(X11_CFLAGS)
LDADD = $(FXSCINTILLA_LIBS) $(FOX_LIBS) $(LUA_LIBS) $(X11_LIBS) $(LIBINTL)
DEFS += -DLOCALEDIR=\"$(localedir)\"


bin_PROGRAMS=fxite

fxite_LDFLAGS=$(FXSCINTILLA_LIBS) $(LUA_LIBS) $(LDFLAGS)

fxite_SOURCES=\
  appmain.cpp appwin_ev.cpp appwin_mk.cpp appwin_mnu.cpp appwin_fn.cpp runcmd.cpp filer.cpp \
  histbox.cpp lang.cpp luacmds.cpp luafuncs.cpp macro.cpp prefdlg.cpp prefs.cpp readtags.c \
  recorder.cpp scidoc.cpp search.cpp shmenu.cpp sl.c doctabs.cpp tagread.cpp scisrch.cpp \
  export.cpp help.cpp toolmgr.cpp tooltree.cpp backup.cpp fxasq.cpp fxasq_c.cpp fxasq_lua.c \
  run_w32.cpp run_unx.cpp menuspec.cpp isutf8.c winshcut.c desclistdlg.cpp listdlls.c luafx.cpp \
  migration.cpp compat.cpp interproc.cpp ini_sort.c filedlg.cpp theme.cpp prefdlg_ext.cpp

noinst_HEADERS=\
  appmain.h appwin.h runcmd.h export.h filer.h help.h helptext.h help_lua.h histbox.h lang.h \
  luacmds.h luafuncs.h macro.h prefdlg.h prefs.h readtags.h recorder.h scidoc.h scisrch.h search.h \
  shmenu.h sl.h doctabs.h tagread.h toolmgr.h tooltree.h langlist.h backup.h fxasq.h fxasq_c.h \
  fxasq_lua.h luasci.h intl.h compat.h menuspec.h desclistdlg.h listdlls.h luafx.h appname.h \
  interproc.h filedlg.h theme.h prefdlg_ext.h


EXTRA_DIST=icon32x32.xpm icon32x32.ico icon32x32.rc icon32x32.coff helptext.txt help_lua.txt


$(srcdir)/help.cpp: $(srcdir)/helptext.h $(srcdir)/help_lua.h

$(srcdir)/helptext.h: $(srcdir)/helptext.txt
	awk '{if (/\\$$/) {sub(/\\$$/,""); printf("%s",$$0)} else {print}}' $< > $<.tmp
	reswrap -o $@ $<.tmp
	$(RM) $<.tmp

$(srcdir)/help_lua.h: $(srcdir)/help_lua.txt
	awk '{if (/\\$$/) {sub(/\\$$/,""); printf("%s",$$0)} else {print}}' $< > $<.tmp
	reswrap -o $@ $<.tmp
	$(RM) $<.tmp

# Inlining does not work for sl library!
sl.o: $(srcdir)/sl.c $(srcdir)/sl.h
	$(CC) $(CFLAGS) -c -fno-inline -o $@ $<


$(srcdir)/icon32x32.ico: $(srcdir)/icon32x32.xpm
	convert $< $@


$(srcdir)/icon32x32.rc: $(srcdir)/icon32x32.ico


$(srcdir)/icon32x32.coff: $(srcdir)/icon32x32.rc
	windres -i $< -o $@

win32: $(srcdir)/icon32x32.coff all



