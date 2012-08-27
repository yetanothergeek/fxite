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
appmain.cpp \
appname.cpp \
appwin_base.cpp \
appwin.cpp \
appwin_pub.cpp \
backup.cpp \
cmd_utils.cpp \
doctabs.cpp \
export.cpp \
filedlg.cpp \
filer.cpp \
foreachtab.cpp \
fxasq.cpp \
fxasq_c.cpp \
fxasq_lua.c \
help.cpp \
isutf8.c \
lang.cpp \
luacmds.cpp \
luafuncs.cpp \
luafx.cpp \
macro.cpp \
mainmenu.cpp \
menuspec.cpp \
migration.cpp \
outpane.cpp \
prefdlg.cpp \
prefdlg_ext.cpp \
prefdlg_sntx.cpp \
prefdlg_tbar.cpp \
prefs.cpp \
readtags.c \
recorder.cpp \
scidoc.cpp \
scidoc_util.cpp \
scisrch.cpp \
search.cpp \
shmenu.cpp \
statusbar.cpp \
tagread.cpp \
theme.cpp \
toolbar.cpp \
toolmgr.cpp \
tooltree.cpp \
winshcut.c


noinst_HEADERS=\
appmain.h \
appname.h \
appwin.h \
appwin_base.h \
appwin_pub.h \
backup.h \
cmd_utils.h \
doctabs.h \
export.h \
filedlg.h \
filer.h \
foreachtab.h \
fxasq.h \
fxasq_c.h \
fxasq_lua.h \
help.h \
help_lua.h \
helptext.h \
lang.h \
langlist.h \
luacmds.h \
luafuncs.h \
luafx.h \
luasci.h \
macro.h \
mainmenu.h \
menuspec.h \
outpane.h \
prefdlg.h \
prefdlg_ext.h \
prefdlg_sntx.h \
prefdlg_tbar.h \
prefs.h \
readtags.h \
recorder.h \
scidoc.h \
scidoc_util.h \
scisrch.h \
search.h \
shmenu.h \
statusbar.h \
tagread.h \
theme.h \
toolbar.h \
toolmgr.h \
tooltree.h


EXTRA_DIST=\
fxite.desktop \
fxite.xpm \
help_lua.txt \
helptext.txt \
icon32x32.coff \
icon32x32.ico \
icon32x32.rc


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



