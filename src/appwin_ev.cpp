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


#include <fx.h>

#include "compat.h"
#include "scidoc.h"
#include "doctabs.h"
#include "search.h"
#include "filer.h"
#include "prefs.h"
#include "tagread.h"
#include "recorder.h"
#include "help.h"
#include "backup.h"
#include "menuspec.h"
#include "appname.h"
#include "outpane.h"
#include "statusbar.h"
#include "mainmenu.h"
#include "scidoc_util.h"
#include "foreachtab.h"

#include "intl.h"
#include "appwin.h"

#define STALECHECK 5 /* seconds between checks for external changes. */



FXDEFMAP(TopWindow) TopWindowMap[]={
  FXMAPFUNC(SEL_TIMEOUT,   TopWindow::ID_TIMER,            TopWindow::onTimer),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_SCINTILLA,        TopWindow::onScintilla),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_TAB_SWITCHED,     TopWindow::onSwitchTabs),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_TAB_ACTIVATE,     TopWindow::onTabActivate),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_OPEN_FILES,       TopWindow::onFileOpen),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_CLOSE,            TopWindow::onCloseTab),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_CLOSEALL,         TopWindow::onCloseAll),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_SAVE,             TopWindow::onFileSave),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_SAVEAS,           TopWindow::onFileSaveAs),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_SAVEALL,          TopWindow::onFileSaveAll),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_SAVECOPY,         TopWindow::onFileSaveACopy),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_NEW,              TopWindow::onFileNew),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_SELECT_DIR,       TopWindow::onSelectDir),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_QUIT,             TopWindow::onQuit),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_UNDO,             TopWindow::onUndo),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_REDO,             TopWindow::onRedo),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_CUT,              TopWindow::onCut),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_COPY,             TopWindow::onCopy),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_PASTE,            TopWindow::onPaste),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_FIND,             TopWindow::onFind),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_FINDNEXT,         TopWindow::onFindNext),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_FINDPREV,         TopWindow::onFindPrev),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_REPLACE_IN_DOC,   TopWindow::onReplace),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_GOTO,             TopWindow::onGoto),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_GOTO_SELECTED,    TopWindow::onGotoSelected),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_GOTO_ERROR,       TopWindow::onGoToError),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_SHOW_LINENUMS,    TopWindow::onShowLineNums),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_SHOW_TOOLBAR,     TopWindow::onShowToolbar),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_SHOW_WHITESPACE,  TopWindow::onShowWhiteSpace),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_SHOW_STATUSBAR,   TopWindow::onShowStatusBar),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_SHOW_MARGIN,      TopWindow::onShowMargin),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_SHOW_INDENT,      TopWindow::onShowIndent),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_SHOW_CARET_LINE,  TopWindow::onShowCaretLine),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_SHOW_OUTLIST,     TopWindow::onShowOutputPane),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_INVERT_COLORS,    TopWindow::onInvertColors),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_PREFS_DIALOG,     TopWindow::onPrefsDialog),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_CTRL_TAB,         TopWindow::onCtrlTab),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_KILL_COMMAND,     TopWindow::onKillCommand),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_RUN_COMMAND,      TopWindow::onRunCommand),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_SPLIT_CHANGED,    TopWindow::onSplitChanged),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_FOCUS_OUTLIST,    TopWindow::onOutlistFocus),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_SET_LANGUAGE,     TopWindow::onSetLanguage),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_RELOAD,           TopWindow::onReload),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_READONLY,         TopWindow::onReadOnly),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_WORDWRAP,         TopWindow::onWordWrap),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_LOAD_TAGS,        TopWindow::onLoadTags),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_UNLOAD_TAGS,      TopWindow::onUnloadTags),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_FIND_TAG,         TopWindow::onFindTag),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_SHOW_CALLTIP,     TopWindow::onShowCallTip),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_AUTO_COMPLETE,    TopWindow::onAutoComplete),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_MACRO_RECORD,     TopWindow::onMacroRecord),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_MACRO_PLAYBACK,   TopWindow::onMacroPlayback),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_MACRO_TRANSLATE,  TopWindow::onMacroShow),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_RESCAN_USER_MENU, TopWindow::onRescanUserMenu),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_CONFIGURE_TOOLS,  TopWindow::onConfigureTools),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_INSERT_FILE,      TopWindow::onInsertFile),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_HELP_ABOUT,       TopWindow::onHelpAbout),
  FXMAPFUNC(SEL_TIMEOUT,   TopWindow::ID_CLOSEWAIT,        TopWindow::onCloseWait),
  FXMAPFUNC(SEL_CHORE,     TopWindow::ID_CLOSEWAIT,        TopWindow::onCloseWait),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_FILE_SAVED,       TopWindow::onFileSaved),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_OPEN_PREVIOUS,    TopWindow::onOpenPrevious),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_OPEN_SELECTED,    TopWindow::onOpenSelected),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_CYCLE_SPLITTER,   TopWindow::onCycleSplitter),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_CLEAR_OUTPUT,     TopWindow::onClearOutput),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_SHOW_HELP,        TopWindow::onShowHelp),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_SHOW_LUA_HELP,    TopWindow::onShowHelp),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_TBAR_CUSTOM_CMD,  TopWindow::onTBarCustomCmd),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_POPUP_SELECT_ALL, TopWindow::onPopupSelectAll),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_POPUP_DELETE_SEL, TopWindow::onPopupDeleteSel),
  FXMAPFUNC(SEL_FOCUSIN,   TopWindow::ID_SCINTILLA,        TopWindow::onScintilla),
  FXMAPFUNC(SEL_PICKED,    TopWindow::ID_SCINTILLA,        TopWindow::onScintilla),
  FXMAPFUNC(SEL_CHORE,     TopWindow::ID_CHECK_STALE,      TopWindow::CheckStale),
  FXMAPFUNC(SEL_CHORE,     TopWindow::ID_CHECK_STYLE,      TopWindow::CheckStyle),
  FXMAPFUNC(SEL_CHORE,     TopWindow::ID_FOCUS_DOC,        TopWindow::onFocusDoc),
  FXMAPFUNC(SEL_FOCUSIN,   0,                              TopWindow::onFocusIn),
  FXMAPFUNC(SEL_COMMAND,   TopWindow::ID_TEST_SOMETHING,   TopWindow::onTestSomething),
  FXMAPFUNC(SEL_CHORE,     TopWindow::ID_TEST_SOMETHING,   TopWindow::onTestSomething),
  FXMAPFUNC(SEL_IO_WRITE,  TopWindow::ID_CMDIO,            TopWindow::onCmdIO),
  FXMAPFUNC(SEL_IO_EXCEPT, TopWindow::ID_CMDIO,            TopWindow::onCmdIO),
  FXMAPFUNCS(SEL_COMMAND,  TopWindow::ID_TABS_TOP,         TopWindow::ID_TABS_RIGHT,      TopWindow::onTabOrient),
  FXMAPFUNCS(SEL_COMMAND,  TopWindow::ID_TABS_UNIFORM,     TopWindow::ID_TABS_COMPACT,    TopWindow::onPackTabWidth),
  FXMAPFUNCS(SEL_COMMAND,  TopWindow::ID_TAB_TOFIRST,      TopWindow::ID_TAB_DOWN,        TopWindow::onMoveTab),
  FXMAPFUNCS(SEL_COMMAND,  TopWindow::ID_TAB_NEXT,         TopWindow::ID_TAB_PREV,        TopWindow::onNextTab),
  FXMAPFUNCS(SEL_COMMAND,  TopWindow::ID_DEL_WORD_LEFT,    TopWindow::ID_DEL_LINE_RIGHT,  TopWindow::onDeleteChunk),
  FXMAPFUNCS(SEL_COMMAND,  TopWindow::ID_NEXT_SELECTED,    TopWindow::ID_PREV_SELECTED,   TopWindow::onFindSelected),
  FXMAPFUNCS(SEL_COMMAND,  TopWindow::ID_BOOKMARK_SET,     TopWindow::ID_BOOKMARK_RETURN, TopWindow::onBookmark),
  FXMAPFUNCS(SEL_COMMAND,  TopWindow::ID_ZOOM_IN,          TopWindow::ID_ZOOM_NONE,       TopWindow::onZoom),
  FXMAPFUNCS(SEL_COMMAND,  TopWindow::ID_INDENT_STEP,      TopWindow::ID_UNINDENT_FULL,   TopWindow::onIndent),
  FXMAPFUNCS(SEL_COMMAND,  TopWindow::ID_FILTER_SEL,       TopWindow::ID_INSERT_CMD_OUT,  TopWindow::onFilterSel),
  FXMAPFUNCS(SEL_COMMAND,  TopWindow::ID_TOUPPER,          TopWindow::ID_TOLOWER,         TopWindow::onChangeCase),
  FXMAPFUNCS(SEL_COMMAND,  TopWindow::ID_USER_COMMAND,     TopWindow::ID_USER_MACRO,      TopWindow::onUserCmd),
  FXMAPFUNCS(SEL_COMMAND,  TopWindow::ID_EXPORT_PDF,       TopWindow::ID_EXPORT_HTML,     TopWindow::onFileExport),
  FXMAPFUNCS(SEL_COMMAND,  TopWindow::ID_FMT_DOS,          TopWindow::ID_FMT_UNIX,        TopWindow::onFileFormat),
};


FXIMPLEMENT(TopWindow,MainWinWithClipBrd,TopWindowMap,ARRAYNUMBER(TopWindowMap))



/* Utility function, only used for testing new stuff */
long TopWindow::onTestSomething(FXObject*o, FXSelector sel, void*p)
{
  if (FXSELTYPE(sel)==SEL_CHORE) {
 /*do something*/
  } else {
    getApp()->addChore(this, ID_TEST_SOMETHING);
  }
  return 1;
}



long TopWindow::onCmdIO(FXObject*o, FXSelector sel, void*p)
{
  outlist->fillItems(*((FXString*)p));
  return 1;
}



long TopWindow::onInvertColors(FXObject*o, FXSelector sel, void*p)
{
  InvertColors((bool)(FXival)p);
  return 1;
}



long TopWindow::onFileFormat(FXObject*o, FXSelector sel, void*p)
{
  SetFileFormat(FXSELID(sel));
  return 1;
}



long TopWindow::onBookmark(FXObject*o, FXSelector sel, void*p)
{
  switch (FXSELID(sel)) {
    case ID_BOOKMARK_SET: {
      SetBookmark();
      break;
    }
    case ID_BOOKMARK_RETURN: {
      GoToBookmark();
      break;
    }
  }
  return 1;
}



long TopWindow::onFocusIn(FXObject*o, FXSelector sel, void*p)
{
  long rv=FXMainWindow::onFocusIn(o,sel,p);
  if (active_widget==outlist) {
    outlist->setFocus();
  } else if (FocusedDoc()) {
    FocusedDoc()->setFocus();
  }
  return rv;
}



long TopWindow::onConfigureTools( FXObject*o, FXSelector sel, void*p )
{
  ShowToolManagerDialog();
  return 1;
}



long TopWindow::onShowHelp(FXObject*o, FXSelector sel, void*p)
{
  // If user has editor background set to dark color, show help with inverted colors
  bool dark=( (((FXuint)(FocusedDoc()->sendMessage(SCI_STYLEGETBACK,STYLE_DEFAULT,0)))<FXRGB(128,128,128)) );
  show_help(this, (FXSELID(sel)==ID_SHOW_HELP)?0:1, dark);
  return 1;
}



long TopWindow::onFileExport(FXObject*o, FXSelector sel, void*p)
{
  switch (FXSELID(sel)) {
    case ID_EXPORT_PDF: {
      filedlgs->ExportPdf(ControlDoc());
      break;
    }
    case ID_EXPORT_HTML: {
      filedlgs->ExportHtml(ControlDoc());
      break;
    }
  }
  return 1;
}



long TopWindow::onCycleSplitter(FXObject*o, FXSelector sel, void*p)
{
  SciDocUtils::CycleSplitter(ControlDoc(), prefs);
  return 1;
}



long TopWindow::onClearOutput(FXObject*o, FXSelector sel, void*p)
{
  outlist->clearItems();
  return 1;
}



long TopWindow::onOpenSelected(FXObject*o, FXSelector sel, void*p)
{
  OpenSelected();
  return 1;
}



long TopWindow::onOpenPrevious(FXObject*o, FXSelector sel, void*p)
{
  OpenFile((char*)p, NULL, false, false);
  return 1;
}



long TopWindow::onHelpAbout(FXObject*o, FXSelector sel, void*p)
{
  AppAbout::AboutBox();
  return 1;
}



long TopWindow::onMacroPlayback(FXObject*o, FXSelector sel, void*p)
{
  if (recorder&&!recording) { RunMacro(recorder->translate(), false); }
  return 1;
}



long TopWindow::onMacroShow(FXObject*o, FXSelector sel, void*p)
{
  if (recorder&&!recording) {
    NewFile(false);
    ControlDoc()->setLanguage("lua");
    ControlDoc()->SetText(recorder->translate().text());
  }
  return 1;
}



long TopWindow::onMacroRecord(FXObject*o, FXSelector sel, void*p)
{
  if (recording) {
    recording->sendMessage(SCI_STOPRECORD, 0, 0);
    recording=NULL;
  } else {
    if (!recorder) {recorder=new MacroRecorder(); }
    recorder->clear();
    recording=ControlDoc();
    recording->sendMessage(SCI_STARTRECORD, 0, 0);
  }
  statusbar->Recording(recording);
  menubar->Recording(recording,recorder);
  ControlDoc()->setFocus();
  return 1;
}



long TopWindow::onInsertFile(FXObject*o, FXSelector sel, void*p )
{
  FXString* filename=NULL;
  SciDoc*sci=FocusedDoc();
  if (!sci->GetReadOnly()) {
    if (filedlgs->GetOpenFilenames(sci,filename,false)&&!filename->empty()) {
      InsertFile(sci,*filename);
      delete[] filename;
    }
  }
  sci->setFocus();
  return 1;
}



long TopWindow::onRescanUserMenu(FXObject*o, FXSelector sel, void*p)
{
  RescanUserMenu();
  return 1;
}



long TopWindow::onFileSaved(FXObject*o, FXSelector sel, void*p)
{
  SciDoc *active=ControlDoc();
  SciDoc* saved=(SciDoc*)p;
  FXStat st;
  if ( FXStat::stat(save_hook,st) && (st.size()>0) && st.isFile() ) {
    if (active!=saved) {
      tabbook->ActivateTab((DocTab*)(saved->getParent()->getPrev()));
    }
    RunHookScript("saved");
    if ( IsDocValid(active) && (active!=saved) ) {
      tabbook->ActivateTab((DocTab*)(active->getParent()->getPrev()));
    }
  }
  return 1;
}



long TopWindow::onUserCmd(FXObject*o, FXSelector sel, void*p)
{
  RunUserCmd((FXMenuCommand*)o,sel,(FXuval)p);
  return 1;
}



long TopWindow::onTBarCustomCmd(FXObject*o, FXSelector sel, void*p)
{
  MenuSpec*spec=(MenuSpec*)(((FXButton*)o)->getUserData());
  RunUserCmd(spec->ms_mc, FXSEL(SEL_COMMAND,spec->ms_mc->getSelector()),0);
  return 1;
}



long TopWindow::onFindTag(FXObject*o, FXSelector sel, void*p)
{
  FindTag();
  return 1;
}



long TopWindow::onShowCallTip(FXObject*o, FXSelector sel, void*p )
{
  TagHandler::ShowCallTip(FocusedDoc(), menubar->TagsMenu());
  return 1;
}



long TopWindow::onAutoComplete(FXObject*o, FXSelector sel, void*p)
{
  completions->Start(FocusedDoc(), TagFiles());
  return 1;
}



long TopWindow::onLoadTags(FXObject*o, FXSelector sel, void*p)
{
  FXString filename="";
  if ( filedlgs->GetOpenTagFilename(ControlDoc(), filename) ) {
    AddFileToTagsMenu(filename);
  }
  return 1;
}



long TopWindow::onUnloadTags(FXObject*o, FXSelector sel, void*p)
{
  menubar->UnloadTagFile((FXMenuCommand*)o);
  return 1;
}



long TopWindow::onReadOnly(FXObject*o, FXSelector sel, void*p)
{
  SciDoc*sci=ControlDoc();
  SetReadOnly(sci, !sci->GetReadOnly());
  sci->setFocus();
  return 1;
}



long TopWindow::onWordWrap(FXObject*o, FXSelector sel, void*p)
{
  SciDoc*sci=ControlDoc();
  SetWordWrap(sci, !sci->GetWordWrap());
  sci->setFocus();
  return 1;
}



long TopWindow::onNextTab(FXObject*o,FXSelector sel,void* p)
{
  tabbook->FocusNextTab(FXSELID(sel) == ID_TAB_NEXT);
  need_status=32;
  return 1;
}



// Active document switched
long TopWindow::onSwitchTabs(FXObject*o, FXSelector sel, void*p)
{
  need_status=32;
  return 1;
}



long TopWindow::onTimer(FXObject*o, FXSelector sel, void*p)
{
  if (prefs->WatchExternChanges) {
    if (StaleTicks < STALECHECK)  {
      StaleTicks++;
    } else {
      if (getApp()->getActiveWindow() && (getApp()->getActiveWindow()->getShell()==this)) {
        CheckStale(NULL,0,NULL);
      }
    }
  }
  if (prefs->Autosave) {
    if (SaveTicks < prefs->AutosaveInterval) {
      SaveTicks++;
    } else {
      SaveTicks=0;
      tabbook->ForEachTab(TabCallbacks::AutoSaveCB,backups);
    }
  }
  if (prefs->Autosave||prefs->WatchExternChanges) { getApp()->addTimeout(this,ID_TIMER, ONE_SECOND, NULL); }
  return 1;
}



long TopWindow::onReload(FXObject*o, FXSelector sel, void*p)
{
  SciDoc*sci=ControlDoc();
  if (sci->Filename().empty()) {
    FXMessageBox::error(this,MBOX_OK,_("Unamed file"), _("File has no name, can't reload."));
    return 1;
  }
  if (!sci->Dirty()) {
    if ( FXMessageBox::question(this, MBOX_YES_NO, _("Reload file"),
    _("Reload current document?"))!=MBOX_CLICKED_YES ) { return 1; }
  }
  filedlgs->AskReload(sci);
  return 1;
}



long TopWindow::onSetLanguage(FXObject*o, FXSelector sel, void*p)
{
  SetLanguage((FXMenuRadio*)o);
  return 1;
}



long TopWindow::onSplitChanged(FXObject*o, FXSelector sel, void*p)
{
  if (hsplit->getSplit(1)>0) { prefs->OutputPaneHeight=hsplit->getSplit(1); }
  return 1;
}



long TopWindow::onScintilla(FXObject*o,FXSelector s,void*p)
{

  SciDoc* sci=(SciDoc*)o;
  switch (FXSELTYPE(s)) {
    case SEL_COMMAND: {
      SCNotification* scn = static_cast<SCNotification*>(p);
      if ((!need_status)&&(scn->nmhdr.code==SCN_PAINTED)) { return 1; }
      if (need_status) {need_status--;}
      if (sci->Loading()) { return 1; }
      long pos=sci->sendMessage(SCI_GETCURRENTPOS,0,0);
      long line=sci->sendMessage(SCI_LINEFROMPOSITION,pos,0);
      long col=sci->sendMessage(SCI_GETCOLUMN,pos,0);
      switch (scn->nmhdr.code) {
        case SCN_UPDATEUI: {
          if ( prefs->BraceMatch ) { sci->MatchBrace(); }
          break;
        }
        case SCN_SAVEPOINTLEFT: {
          if (!sci->Loading()) {
            SetTabDirty(sci,true);
          }
          break;
        }
        case SCN_SAVEPOINTREACHED: {
          if (!sci->Loading()) {
            SetTabDirty(sci,false);
            backups->RemoveBackup(sci);
          }
          break;
        }
        case SCN_PAINTED: {
          break;
        }
        case SCN_MODIFIED: {
          if (!sci->Loading()) {
            if (scn->modificationType & (SC_MOD_INSERTTEXT|SC_MOD_DELETETEXT)) {
              sci->NeedBackup(true);
            }
          }
          break;
        }
        case SCN_CHARADDED: {
          if (!completions->Continue(sci))  { CharAdded(sci,line,pos,scn->ch); }
          break;
        }
        case SCN_DOUBLECLICK: {
          break;
        }
        case SCN_MACRORECORD: {
          recorder->record(scn->message,scn->wParam, scn->lParam);
          break;
        }
        default: {
        }
      }
      UpdateTitle(line,col);
      if ( sci->GetSelLength() ) {
        EnableUserFilters(true);
      } else {
        EnableUserFilters(false);
      }
      return 1;
    }
    case SEL_FOCUSIN: {
      getApp()->addChore(this, ID_CHECK_STALE);
      if (sci->NeedStyled()) { getApp()->addChore(this, ID_CHECK_STYLE,sci); }
      active_widget=sci;
      return 1;
    }
    case SEL_PICKED: {
      MenuMgr::ShowPopupMenu((FXPoint*)p);
      return 1;
    }
    default: { return 1; }
  }
}



// Switch tab orientations
long TopWindow::onTabOrient(FXObject*o,FXSelector sel,void*p)
{
  SetTabOrientation(FXSELID(sel));
  FocusedDoc()->setFocus();
  return 1;
}



long TopWindow::onPackTabWidth(FXObject*o,FXSelector sel,void*p)
{
  prefs->DocTabsPacked=(FXSELID(sel) == ID_TABS_COMPACT);
  tabbook->setTabsCompact(prefs->DocTabsPacked);
  MenuMgr::RadioUpdate(FXSELID(sel),ID_TABS_UNIFORM,ID_TABS_COMPACT);
  return 1;
}



long TopWindow::onMoveTab(FXObject*o, FXSelector sel, void*p)
{
  switch (FXSELID(sel)) {
    case ID_TAB_TOLAST: {
      tabbook->MoveTabLast();
      break;
    }
    case ID_TAB_TOFIRST: {
      tabbook->MoveTabFirst();
      break;
    }
    case ID_TAB_UP: {
      tabbook->MoveTabUp();
      break;
    }
    case ID_TAB_DOWN: {
      tabbook->MoveTabDown();
      break;
    }
  }
  return 1;
}



long TopWindow::onTabActivate(FXObject*o, FXSelector sel, void*p)
{
  tabbook->ActivateTab((DocTab*)(((FXMenuCommand*)o)->getUserData()));
  return 1;
}



long TopWindow::onFileOpen(FXObject*o,FXSelector sel,void*p)
{
  FXString* filenames=NULL;
  if (filedlgs->GetOpenFilenames(FocusedDoc(),filenames,true)) {
    FXString* filename;
    for (filename=filenames; !filename->empty(); filename++) { OpenFile(filename->text(),NULL,false,true); }
    delete[] filenames;
  }
  return 1;
}



long TopWindow::onCloseTab(FXObject*o,FXSelector sel,void*p)
{
  CloseFile(false,true);
  return 1;
}



long TopWindow::onCloseAll(FXObject*o,FXSelector sel,void*p)
{
  if (prefs->PromptCloseMultiMenu && (tabbook->Count()>1)) {
    if (FXMessageBox::question(this, MBOX_OK_CANCEL, _("Multiple files"),
          _("Editing %d files - \nclose all tabs?"), tabbook->Count() ) != MBOX_CLICKED_OK ) {
      return 1;
    }
  }
  CloseAll(false);
  return 1;
}



long TopWindow::onFileSaveACopy(FXObject*o, FXSelector sel, void*p)
{
  SciDoc*sci=ControlDoc();
  filedlgs->SaveFileAs(sci,false,sci->Filename().empty()?"untitled.bak":FXPath::name(sci->Filename()+".bak"));
  return 1;
}



long TopWindow::onFileSaveAll(FXObject*o, FXSelector sel, void*p)
{
  SaveAll(false);
  return 1;
}



long TopWindow::onFileSave(FXObject*o, FXSelector sel, void*p)
{
  filedlgs->SaveFile(ControlDoc(),ControlDoc()->Filename());
  return 1;
}



long TopWindow::onFileSaveAs(FXObject*o, FXSelector sel, void*p)
{
  ShowSaveAsDlg(ControlDoc());
  return 1;
}



long TopWindow::onFileNew(FXObject*o, FXSelector sel, void*p )
{
  NewFile(true);
  return 1;
}



long TopWindow::onSelectDir(FXObject*o, FXSelector sel, void*p)
{
  filedlgs->SetWorkingDirectory(this);
  return 1;
}



long TopWindow::onQuit(FXObject*o, FXSelector sel, void*p)
{
  skipfocus=true;
  close(true);
  return 1;
}



#define DOC_SEND(msg) \
  SciDoc*sci=FocusedDoc(); \
  if (sci) { \
    sci->sendMessage(msg,0,0); \
    sci->setFocus(); \
  }


long TopWindow::onUndo(FXObject*o, FXSelector sel, void*p)
{
  DOC_SEND(SCI_UNDO);
  sci->SetEolModeFromContent();
  UpdateEolMenu(sci);
  return 1;
}



long TopWindow::onRedo(FXObject*o, FXSelector sel, void*p)
{
  DOC_SEND(SCI_REDO);
  sci->SetEolModeFromContent();
  UpdateEolMenu(sci);
  return 1;
}



long TopWindow::onCut(FXObject*o, FXSelector sel, void*p)
{
  Cut();
  return 1;
}



long TopWindow::onCopy(FXObject*o, FXSelector sel, void*p)
{
  Copy();
  return 1;
}



long TopWindow::onPaste(FXObject*o, FXSelector sel, void*p)
{
  Paste();
  return 1;
}



long TopWindow::onDeleteChunk(FXObject*o, FXSelector sel, void*p)
{
  FXint cmd=0;
  switch (FXSELID(sel)) {
    case ID_DEL_WORD_LEFT:  { cmd=SCI_DELWORDLEFT;  break; }
    case ID_DEL_WORD_RIGHT: { cmd=SCI_DELWORDRIGHT; break; }
    case ID_DEL_LINE_LEFT:  { cmd=SCI_DELLINELEFT;  break; }
    case ID_DEL_LINE_RIGHT: { cmd=SCI_DELLINERIGHT; break; }
    default: return 1;
  }
  DOC_SEND(cmd);
  return 1;
}



long TopWindow::onPopupSelectAll( FXObject*o, FXSelector sel, void*p )
{
  SciDoc*sci=FocusedDoc();
  sci->sendMessage(SCI_SELECTALL, 0, 0);
  return 1;
}



long TopWindow::onPopupDeleteSel( FXObject*o, FXSelector sel, void*p )
{
  SciDoc*sci=ControlDoc();
  sci->sendString(SCI_REPLACESEL, 0, "");
  return 1;
}



#define macro_record_search() \
  if (recording==FocusedDoc()) { \
    recorder->record( \
      srchdlgs->searchdirn, \
      srchdlgs->searchmode, \
      reinterpret_cast<long>(srchdlgs->searchstring.text()) \
    ); \
  }


long TopWindow::onFind(FXObject*o, FXSelector sel, void*p)
{
  if (srchdlgs->ShowFindDialog(FocusedDoc())) { macro_record_search(); }
  ClosedDialog();
  return 1;
}



long TopWindow::onFindNext(FXObject*o, FXSelector sel, void*p)
{
  if ( srchdlgs->FindNext(FocusedDoc()) ) { macro_record_search(); }
  return 1;
}



long TopWindow::onFindPrev(FXObject*o, FXSelector sel, void*p)
{
  if ( srchdlgs->FindPrev(FocusedDoc()) ) { macro_record_search(); }
  return 1;
}



long TopWindow::onReplace(FXObject*o, FXSelector sel, void*p)
{
  srchdlgs->ShowReplaceDialog(FocusedDoc());
  ClosedDialog();
  return 1;
}


#define macro_record_goto() if (recording==sci) { \
    recorder->record(SCI_GOTOLINE,sci->GetLineNumber(),sci->GetColumnNumber()); \
  }

long TopWindow::onGoto(FXObject*o, FXSelector sel, void*p)
{
  SciDoc*sci=FocusedDoc();
  if ( srchdlgs->ShowGoToDialog(sci) ) { macro_record_goto(); }
  ClosedDialog();
  return 1;
}



long TopWindow::onGotoSelected(FXObject*o, FXSelector sel, void*p)
{
  SciDoc*sci=FocusedDoc();
  if ( srchdlgs->GoToSelected(sci) ) { macro_record_goto(); }
  return 1;
}



long TopWindow::onFindSelected(FXObject*o, FXSelector sel, void*p)
{
  srchdlgs->FindSelected(FocusedDoc(), FXSELID(sel)==ID_NEXT_SELECTED);
  macro_record_search();
  return 1;
}



long TopWindow::onZoom(FXObject*o, FXSelector sel, void*p)
{
  FXival z;
  if (sel) {
    switch (FXSELID(sel)) {
      case ID_ZOOM_IN:{
        z=1;
        break;
      }
      case ID_ZOOM_OUT:{
        z=-1;
        break;
      }
      case ID_ZOOM_FAR:{
        z=-2;
        break;
      }
      case ID_ZOOM_NEAR:{
        z=2;
        break;
      }
      case ID_ZOOM_NONE:{
        z=0;
        break;
      }
    }
    tabbook->ForEachTab(TabCallbacks::ZoomStepCB, &z);
  } else {
    z=(FXival)p;
   tabbook->ForEachTab(TabCallbacks::ZoomSpecCB, &z);
  }
  prefs->ZoomFactor=FocusedDoc()->GetZoom();
  return 1;
}



long TopWindow::onShowLineNums( FXObject*o, FXSelector sel, void*p )
{
  ShowLineNumbers((bool)((FXival)p));
  return 1;
}



long TopWindow::onShowToolbar( FXObject*o, FXSelector sel, void*p )
{
  ShowToolbar((bool)((FXival)p));
  return 1;
}



long TopWindow::onShowWhiteSpace(FXObject*o, FXSelector sel, void*p)
{
  ShowWhiteSpace((bool)((FXival)p));
  return 1;
}



long TopWindow::onShowMargin(FXObject*o, FXSelector sel, void*p)
{
  ShowMargin((bool)((FXival)p));
  return 1;
}



long TopWindow::onShowIndent(FXObject*o, FXSelector sel, void*p)
{
  ShowIndent((bool)((FXival)p));
  return 1;
}



long TopWindow::onShowCaretLine(FXObject*o, FXSelector sel, void*p)
{
  ShowCaretLine((bool)((FXival)p));
  return 1;
}



long TopWindow::onShowOutputPane(FXObject*o, FXSelector sel, void*p)
{
  ShowOutputPane((bool)((FXival)p));
  return 1;
}



long TopWindow::onShowStatusBar(FXObject*o, FXSelector sel, void*p)
{
  ShowStatusBar((bool)((FXival)p));
  return 1;
}



long TopWindow::onPrefsDialog(FXObject*o, FXSelector sel, void*p)
{
  ShowPrefsDialog();
  return 1;
}



long TopWindow::onCtrlTab(FXObject*o, FXSelector sel, void*p)
{
  if (!FocusedDoc()->UseTabs()) {
    FocusedDoc()->sendString(SCI_REPLACESEL,0,"\t");
    return 1;
  }
  return 0;
}



long TopWindow::onIndent(FXObject*o, FXSelector sel, void*p)
{
  Indent(FXSELID(sel));
  return 1;
}



long TopWindow::onFilterSel(FXObject*o, FXSelector sel, void*p)
{
  ShowFilterDialog(FXSELID(sel)==ID_FILTER_SEL);
  return 1;
}



long TopWindow::onKillCommand(FXObject*o, FXSelector sel, void*p)
{
  command_timeout=true;
  return 1;
}



long TopWindow::onRunCommand(FXObject*o, FXSelector sel, void*p)
{
  ShowCommandDialog();
  return 1;
}



long TopWindow::onOutlistFocus(FXObject*o, FXSelector sel, void*p)
{
  if (outlist->Focus()) { skipfocus=true; }
  return 1;
}



long TopWindow::onGoToError(FXObject*o, FXSelector sel, void*p)
{
  outlist->GoToError();
  return 1;
}



long TopWindow::onChangeCase(FXObject*o, FXSelector sel, void*p)
{
  SciDoc*sci=FocusedDoc();
  if (sci->GetSelLength()>0) {
    switch (FXSELID(sel)) {
      case ID_TOUPPER: {
        sci->SelectionToUpper();
        if (recording==sci) { recorder->record(SCI_UPPERCASE,0,0); }
        break;
      }
      case ID_TOLOWER: {
        sci->SelectionToLower();
        if (recording==sci) { recorder->record(SCI_LOWERCASE,0,0); }
        break;
      }
    }
  }
  return 1;
}



long TopWindow::onFocusDoc(FXObject*o, FXSelector sel, void*p ) {
  if (skipfocus) {
    skipfocus=false;
  } else {
    if (getApp()->getActiveWindow()==this) {
      SciDoc*sci=FocusedDoc();
      if (sci) { sci->setFocus(); }
    }
  }
  return 0;
}

