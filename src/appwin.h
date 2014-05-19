/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2009-2012 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License version 3 as
  published by the Free Software Foundation.

  This software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef FXITE_APPWIN_H
#define FXITE_APPWIN_H

#include "appwin_base.h"


class TopWindow: public TopWindowBase {
private:
  FXDECLARE(TopWindow)
  TopWindow(){}
public:
  long onTabOrient(      FXObject* o, FXSelector sel, void* p );
  long onPackTabWidth(   FXObject* o, FXSelector sel, void* p );
  long onSwitchTabs(     FXObject* o, FXSelector sel, void* p );
  long onNextTab(        FXObject* o, FXSelector sel, void* p );
  long onTabActivate(    FXObject* o, FXSelector sel, void* p );
  long onMoveTab(        FXObject* o, FXSelector sel, void* p );
  long onFileOpen(       FXObject* o, FXSelector sel, void* p );
  long onFileSave(       FXObject* o, FXSelector sel, void* p );
  long onFileSaveAs(     FXObject* o, FXSelector sel, void* p );
  long onFileSaveAll(    FXObject* o, FXSelector sel, void* p );
  long onFileSaveACopy(  FXObject* o, FXSelector sel, void* p );
  long onFileNew(        FXObject* o, FXSelector sel, void* p );
  long onSelectDir(      FXObject* o, FXSelector sel, void* p );
  long onQuit(           FXObject* o, FXSelector sel, void* p );
  long onScintillaCmd(   FXObject* o, FXSelector sel, void* p );
  long onScintillaPick(  FXObject* o, FXSelector sel, void* p );
  long onScintillaKey(   FXObject* o, FXSelector sel, void* p );
  long onCloseTab(       FXObject* o, FXSelector sel, void* p );
  long onCloseAll(       FXObject* o, FXSelector sel, void* p );
  long onUndo(           FXObject* o, FXSelector sel, void* p );
  long onRedo(           FXObject* o, FXSelector sel, void* p );
  long onCut(            FXObject* o, FXSelector sel, void* p );
  long onCopy(           FXObject* o, FXSelector sel, void* p );
  long onPaste(          FXObject* o, FXSelector sel, void* p );
  long onDeleteChunk(    FXObject* o, FXSelector sel, void* p );
  long onFind(           FXObject* o, FXSelector sel, void* p );
  long onFindNext(       FXObject* o, FXSelector sel, void* p );
  long onFindPrev(       FXObject* o, FXSelector sel, void* p );
  long onReplace(        FXObject* o, FXSelector sel, void* p );
  long onGoto(           FXObject* o, FXSelector sel, void* p );
  long onGotoSelected(   FXObject* o, FXSelector sel, void* p );
  long onFindSelected(   FXObject* o, FXSelector sel, void* p );
  long onZoom(           FXObject* o, FXSelector sel, void* p );
  long onShowStatusBar(  FXObject* o, FXSelector sel, void* p );
  long onShowMargin(     FXObject* o, FXSelector sel, void* p );
  long onShowIndent(     FXObject* o, FXSelector sel, void* p );
  long onShowCaretLine(  FXObject* o, FXSelector sel, void* p );
  long onShowLineNums(   FXObject* o, FXSelector sel, void* p );
  long onShowToolbar(    FXObject* o, FXSelector sel, void* p );
  long onShowWhiteSpace( FXObject* o, FXSelector sel, void* p );
  long onShowOutputPane( FXObject* o, FXSelector sel, void* p );
  long onInvertColors(   FXObject* o, FXSelector sel, void* p );
  long onPrefsDialog(    FXObject* o, FXSelector sel, void* p );
  long onCtrlTab(        FXObject* o, FXSelector sel, void* p );
  long onIndent(         FXObject* o, FXSelector sel, void* p );
  long onFilterSel(      FXObject* o, FXSelector sel, void* p );
  long onKillCommand(    FXObject* o, FXSelector sel, void* p );
  long onRunCommand(     FXObject* o, FXSelector sel, void* p );
  long onGoToError(      FXObject* o, FXSelector sel, void* p );
  long onBookmark(       FXObject* o, FXSelector sel, void* p );
  long onSplitChanged(   FXObject* o, FXSelector sel, void* p );
  long onOutlistFocus(   FXObject* o, FXSelector sel, void* p );
  long onChangeCase(     FXObject* o, FXSelector sel, void* p );
  long onSetLanguage(    FXObject* o, FXSelector sel, void* p );
  long onReload(         FXObject* o, FXSelector sel, void* p );
  long onReadOnly(       FXObject* o, FXSelector sel, void* p );
  long onWordWrap(       FXObject* o, FXSelector sel, void* p );
  long onLoadTags(       FXObject* o, FXSelector sel, void* p );
  long onUnloadTags(     FXObject* o, FXSelector sel, void* p );
  long onFindTag(        FXObject* o, FXSelector sel, void* p );
  long onShowCallTip(    FXObject* o, FXSelector sel, void* p );
  long onAutoComplete(   FXObject* o, FXSelector sel, void* p );
  long onUserCmd(        FXObject* o, FXSelector sel, void* p );
  long onRescanUserMenu( FXObject* o, FXSelector sel, void* p );
  long onConfigureTools( FXObject* o, FXSelector sel, void* p );
  long onInsertFile(     FXObject* o, FXSelector sel, void* p );
  long onMacroRecord(    FXObject* o, FXSelector sel, void* p );
  long onMacroPlayback(  FXObject* o, FXSelector sel, void* p );
  long onMacroShow(      FXObject* o, FXSelector sel, void* p );
  long onHelpAbout(      FXObject* o, FXSelector sel, void* p );
  long onFileSaved(      FXObject* o, FXSelector sel, void* p );
  long onOpenPrevious(   FXObject* o, FXSelector sel, void* p );
  long onOpenSelected(   FXObject* o, FXSelector sel, void* p );
  long onCycleSplitter(  FXObject* o, FXSelector sel, void* p );
  long onClearOutput(    FXObject* o, FXSelector sel, void* p );
  long onFileFormat(     FXObject* o, FXSelector sel, void* p );
  long onFileExport(     FXObject* o, FXSelector sel, void* p );
  long onShowHelp(       FXObject* o, FXSelector sel, void* p );
  long onTBarCustomCmd(  FXObject* o, FXSelector sel, void* p );
  long onPopupSelectAll( FXObject* o, FXSelector sel, void* p );
  long onPopupDeleteSel( FXObject* o, FXSelector sel, void* p );
  long onFoundSearch(    FXObject* o, FXSelector sel, void* p );
  long onTestSomething(  FXObject* o, FXSelector sel, void* p );
  enum {
    ID_TABS_TOP=TopWindowBase::ID_LAST,
    ID_TABS_BOTTOM,
    ID_TABS_LEFT,
    ID_TABS_RIGHT,
    ID_TABS_UNIFORM,
    ID_TABS_COMPACT,
    ID_TABS_BY_POS,
    ID_TAB_NEXT,
    ID_TAB_PREV,
    ID_TAB_SWITCHED,
    ID_TAB_TOFIRST,
    ID_TAB_TOLAST,
    ID_TAB_UP,
    ID_TAB_DOWN,
    ID_TAB_ACTIVATE,
    ID_OPEN_FILES,
    ID_OPEN_PREVIOUS,
    ID_OPEN_SELECTED,
    ID_CLOSE,
    ID_CLOSEALL,
    ID_NEW,
    ID_SAVE,
    ID_SAVEAS,
    ID_SAVEALL,
    ID_SAVECOPY,
    ID_SELECT_DIR,
    ID_QUIT,
    ID_UNDO,
    ID_REDO,
    ID_CUT,
    ID_COPY,
    ID_PASTE,
    ID_DEL_WORD_LEFT,
    ID_DEL_WORD_RIGHT,
    ID_DEL_LINE_LEFT,
    ID_DEL_LINE_RIGHT,
    ID_SHOW_STATUSBAR,
    ID_FIND,
    ID_FINDNEXT,
    ID_FINDPREV,
    ID_REPLACE_IN_DOC,
    ID_GOTO,
    ID_NEXT_SELECTED,
    ID_PREV_SELECTED,
    ID_GOTO_SELECTED,
    ID_GOTO_ERROR,
    ID_BOOKMARK_SET,
    ID_BOOKMARK_RETURN,
    ID_ZOOM_IN,
    ID_ZOOM_OUT,
    ID_ZOOM_FAR,
    ID_ZOOM_NEAR,
    ID_ZOOM_NONE,
    ID_SHOW_LINENUMS,
    ID_SHOW_TOOLBAR,
    ID_SHOW_WHITESPACE,
    ID_SHOW_OUTLIST,
    ID_INVERT_COLORS,
    ID_SHOW_MARGIN,
    ID_SHOW_INDENT,
    ID_SHOW_CARET_LINE,
    ID_PREFS_DIALOG,
    ID_CTRL_TAB,
    ID_INDENT_STEP,
    ID_INDENT_FULL,
    ID_UNINDENT_STEP,
    ID_UNINDENT_FULL,
    ID_KILL_COMMAND,
    ID_FILTER_SEL,
    ID_INSERT_CMD_OUT,
    ID_RUN_COMMAND,
    ID_SPLIT_CHANGED,
    ID_FOCUS_OUTLIST,
    ID_TOUPPER,
    ID_TOLOWER,
    ID_SET_LANGUAGE,
    ID_RELOAD,
    ID_READONLY,
    ID_WORDWRAP,
    ID_INSERT_FILE,
    ID_LOAD_TAGS,
    ID_UNLOAD_TAGS,
    ID_FIND_TAG,
    ID_SHOW_CALLTIP,
    ID_AUTO_COMPLETE,
    ID_USER_COMMAND,
    ID_USER_FILTER,
    ID_USER_SNIPPET,
    ID_USER_MACRO,
    ID_RESCAN_USER_MENU,
    ID_CONFIGURE_TOOLS,
    ID_MACRO_RECORD,
    ID_MACRO_PLAYBACK,
    ID_MACRO_TRANSLATE,
    ID_HELP_ABOUT,
    ID_FILE_SAVED,
    ID_CYCLE_SPLITTER,
    ID_CLEAR_OUTPUT,
    ID_EXPORT_PDF,
    ID_EXPORT_HTML,
    ID_SHOW_HELP,
    ID_SHOW_LUA_HELP,
    ID_FMT_DOS,
    ID_FMT_MAC,
    ID_FMT_UNIX,
    ID_TBAR_CUSTOM_CMD,
    ID_POPUP_SELECT_ALL,
    ID_POPUP_DELETE_SEL,
    ID_FOUND_SEARCH,
    ID_TEST_SOMETHING,
    ID_LAST
  };
  TopWindow(FXApp* a);
};

#endif

