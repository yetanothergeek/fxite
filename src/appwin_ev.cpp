/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2009-2011 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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


#include <unistd.h>
#include <cerrno>
#include <cstdio>
#include <fx.h>
#include <fxkeys.h>
#include <FXAccelTable.h>
#include <Scintilla.h>
#include <SciLexer.h>
#include <FXScintilla.h>

#include "compat.h"
#include "scisrch.h"
#include "scidoc.h"
#include "doctabs.h"
#include "search.h"
#include "filer.h"
#include "prefs.h"
#include "prefdlg.h"
#include "histbox.h"
#include "tagread.h"
#include "shmenu.h"
#include "recorder.h"
#include "help.h"
#include "toolmgr.h"
#include "backup.h"
#include "menuspec.h"
#include "theme.h"

#include "intl.h"
#include "appwin.h"

#define STALECHECK 5 /* seconds between checks for external changes. */


#define TWMAPFUNC(id,func) FXMAPFUNC(SEL_COMMAND,TopWindow::id,TopWindow::func)
#define TWMAPFUNCS(id1,id2,func) FXMAPFUNCS(SEL_COMMAND,TopWindow::id1,TopWindow::id2,TopWindow::func)
#define TWMAPSEL(sel,id,func) FXMAPFUNC(sel,TopWindow::id,TopWindow::func)




FXDEFMAP(TopWindow) TopWindowMap[]={
  TWMAPSEL(SEL_TIMEOUT,ID_TIMER,onTimer),
  TWMAPFUNC(ID_SCINTILLA, onScintilla),
  TWMAPFUNCS(ID_TABS_TOP,ID_TABS_RIGHT,onTabOrient),
  TWMAPFUNCS(ID_TABS_UNIFORM,ID_TABS_COMPACT,onPackTabWidth),
  TWMAPFUNCS(ID_TAB_TOFIRST,ID_TAB_DOWN,onMoveTab),
  TWMAPFUNC(ID_TAB_SWITCHED,onSwitchTabs),
  TWMAPFUNC(ID_TAB_ACTIVATE,onTabActivate),
  TWMAPFUNCS(ID_TAB_NEXT,ID_TAB_PREV,onNextTab),
  TWMAPFUNC(ID_OPEN_FILES, onFileOpen),
  TWMAPFUNC(ID_CLOSE, onCloseTab),
  TWMAPFUNC(ID_CLOSEALL, onCloseAll),
  TWMAPFUNC(ID_SAVE,onFileSave),
  TWMAPFUNC(ID_SAVEAS,onFileSaveAs),
  TWMAPFUNC(ID_SAVEALL,onFileSaveAll),
  TWMAPFUNC(ID_SAVECOPY,onFileSaveACopy),
  TWMAPFUNC(ID_NEW,onFileNew),
  TWMAPFUNC(ID_SELECT_DIR,onSelectDir),
  TWMAPFUNC(ID_QUIT,onQuit),
  TWMAPFUNC(ID_UNDO,onUndo),
  TWMAPFUNC(ID_REDO,onRedo),
  TWMAPFUNC(ID_CUT,onCut),
  TWMAPFUNC(ID_COPY,onCopy),
  TWMAPFUNC(ID_PASTE,onPaste),
  TWMAPFUNCS(ID_DEL_WORD_LEFT,ID_DEL_LINE_RIGHT,onDeleteChunk),
  TWMAPFUNC(ID_FIND,onFind),
  TWMAPFUNC(ID_FINDNEXT,onFindNext),
  TWMAPFUNC(ID_FINDPREV,onFindPrev),
  TWMAPFUNC(ID_REPLACE_IN_DOC,onReplace),
  TWMAPFUNCS(ID_NEXT_SELECTED, ID_PREV_SELECTED,onFindSelected),
  TWMAPFUNC(ID_GOTO,onGoto),
  TWMAPFUNC(ID_GOTO_SELECTED,onGotoSelected),
  TWMAPFUNC(ID_GOTO_ERROR,onGoToError),
  TWMAPFUNCS(ID_BOOKMARK_SET, ID_BOOKMARK_RETURN, onBookmark),
  TWMAPFUNCS(ID_ZOOM_IN, ID_ZOOM_NONE, onZoom),
  TWMAPFUNC(ID_SHOW_LINENUMS,onShowLineNums),
  TWMAPFUNC(ID_SHOW_TOOLBAR,onShowToolbar),
  TWMAPFUNC(ID_SHOW_WHITESPACE,onShowWhiteSpace),
  TWMAPFUNC(ID_SHOW_STATUSBAR,onShowStatusBar),
  TWMAPFUNC(ID_SHOW_MARGIN,onShowMargin),
  TWMAPFUNC(ID_SHOW_INDENT,onShowIndent),
  TWMAPFUNC(ID_SHOW_OUTLIST,onShowOutputPane),
  TWMAPFUNC(ID_INVERT_COLORS,onInvertColors),
  TWMAPFUNC(ID_PREFS_DIALOG,onPrefsDialog),
  TWMAPFUNC(ID_CTRL_TAB,onCtrlTab),
  TWMAPFUNCS(ID_INDENT_STEP,ID_UNINDENT_FULL,onIndent),
  TWMAPFUNCS(ID_FILTER_SEL,ID_INSERT_CMD_OUT, onFilterSel),
  TWMAPFUNC(ID_KILL_COMMAND,onKillCommand),
  TWMAPFUNC(ID_RUN_COMMAND, onRunCommand),
  TWMAPFUNC(ID_SPLIT_CHANGED, onSplitChanged),
  TWMAPFUNC(ID_FOCUS_OUTLIST,onOutlistFocus),
  TWMAPFUNC(ID_OUTLIST_ASEL,onOutlistPopup),
  TWMAPFUNC(ID_OUTLIST_COPY,onOutlistPopup),
  TWMAPFUNCS(ID_TOUPPER,ID_TOLOWER,onChangeCase),
  TWMAPFUNC(ID_SET_LANGUAGE,onSetLanguage),
  TWMAPFUNC(ID_RELOAD,onReload),
  TWMAPFUNC(ID_READONLY,onReadOnly),
  TWMAPFUNC(ID_WORDWRAP,onWordWrap),
  TWMAPFUNC(ID_LOAD_TAGS,onLoadTags),
  TWMAPFUNC(ID_UNLOAD_TAGS,onUnloadTags),
  TWMAPFUNC(ID_FIND_TAG,onFindTag),
  TWMAPFUNC(ID_SHOW_CALLTIP,onShowCallTip),
  TWMAPFUNCS(ID_USER_COMMAND,ID_USER_MACRO,onUserCmd),
  TWMAPFUNC(ID_MACRO_RECORD,onMacroRecord),
  TWMAPFUNC(ID_MACRO_PLAYBACK,onMacroPlayback),
  TWMAPFUNC(ID_MACRO_TRANSLATE,onMacroShow),
  TWMAPFUNC(ID_RESCAN_USER_MENU,onRescanUserMenu),
  TWMAPFUNC(ID_CONFIGURE_TOOLS,onConfigureTools),
  TWMAPFUNC(ID_INSERT_FILE, onInsertFile),
  TWMAPFUNC(ID_HELP_ABOUT, onHelpAbout),
  TWMAPSEL(SEL_TIMEOUT,ID_CLOSEWAIT,onCloseWait),
  TWMAPSEL(SEL_CHORE,ID_CLOSEWAIT,onCloseWait),
  TWMAPSEL(SEL_DOUBLECLICKED,     ID_OUTLIST_CLICK, onOutlistClick),
  TWMAPSEL(SEL_KEYPRESS,          ID_OUTLIST_CLICK, onOutlistClick),
  TWMAPSEL(SEL_RIGHTBUTTONRELEASE,ID_OUTLIST_CLICK, onOutlistClick),
  TWMAPSEL(SEL_FOCUSIN,           ID_OUTLIST_CLICK, onOutlistClick),
  TWMAPFUNC(ID_FILE_SAVED,onFileSaved),
  TWMAPFUNC(ID_OPEN_PREVIOUS, onOpenPrevious),
  TWMAPFUNC(ID_OPEN_SELECTED, onOpenSelected),
  TWMAPFUNC(ID_CYCLE_SPLITTER, onCycleSplitter),
  TWMAPFUNC(ID_CLEAR_OUTPUT,onClearOutput),
  TWMAPFUNCS(ID_EXPORT_PDF, ID_EXPORT_HTML,onFileExport),
  TWMAPFUNC(ID_SHOW_HELP, onShowHelp),
  TWMAPFUNC(ID_SHOW_LUA_HELP, onShowHelp),
  TWMAPFUNCS(ID_FMT_DOS,ID_FMT_UNIX,onFileFormat),
  TWMAPFUNC(ID_TBAR_CUSTOM_CMD,onTBarCustomCmd),
  TWMAPSEL(SEL_FOCUSIN,ID_SCINTILLA, onScintilla),
  TWMAPSEL(SEL_CHORE,  ID_CHECK_STALE,CheckStale),
  TWMAPSEL(SEL_CHORE,  ID_CHECK_STYLE,CheckStyle),
  TWMAPSEL(SEL_CHORE,  ID_FOCUS_DOC,onFocusDoc),
  FXMAPFUNC(SEL_FOCUSIN,0,TopWindow::onFocusIn),
  TWMAPSEL(SEL_COMMAND,ID_TEST_SOMETHING,onTestSomething),
  TWMAPSEL(SEL_CHORE,  ID_TEST_SOMETHING,onTestSomething),
  FXMAPFUNC(SEL_IO_WRITE,TopWindow::ID_CMDIO,TopWindow::onCmdIO),
  FXMAPFUNC(SEL_IO_EXCEPT,TopWindow::ID_CMDIO,TopWindow::onCmdIO),
};


FXIMPLEMENT(TopWindow,FXMainWindow,TopWindowMap,ARRAYNUMBER(TopWindowMap))



// Tell the window manager we want the focus back after dialogs close...
void TopWindow::ClosedDialog()
{
  setFocus();
  FocusedDoc()->setFocus();
}



long TopWindow::onCmdIO(FXObject*o, FXSelector sel, void*p)
{
  FXString*s=(FXString*)p;
  s->substitute('\t', ' ');
  s->substitute('\r', ' ');
  outlist->fillItems(*s);
  outlist->makeItemVisible(outlist->getNumItems()-1);
  return 1;
}



long TopWindow::onInvertColors(FXObject*o, FXSelector sel, void*p)
{
  prefs->InvertColors=!prefs->InvertColors;
  SetToolbarColors();
  SciDoc*sci=ControlDoc();
  tabbook->ForEachTab(PrefsCB,NULL);
  CheckStyle(NULL,0,sci);
  SyncToggleBtn(o,sel);
  return 1;
}



long TopWindow::onFileFormat(FXObject*o, FXSelector sel, void*p)
{
  int EolMode=SC_EOL_LF;
  switch (FXSELID(sel)) {
    case ID_FMT_DOS:{
      EolMode=SC_EOL_CRLF;
      break;
    }
    case ID_FMT_MAC:{
      EolMode=SC_EOL_CR;
      break;
    }
    case ID_FMT_UNIX:{
      EolMode=SC_EOL_LF;
      break;
    }
  }
  FocusedDoc()->sendMessage(SCI_SETEOLMODE,EolMode,0);
  FocusedDoc()->sendMessage(SCI_CONVERTEOLS,EolMode,0);
  RadioUpdate(FXSELID(sel),ID_FMT_DOS,ID_FMT_UNIX);
  return 1;
}



bool TopWindow::BookmarkCB(FXint index, DocTab*tab, FXWindow*page, void*user_data)
{
  TopWindow*tw=(TopWindow*)user_data;
  if (tw->bookmarked_tab == tab) {
    tw->tabbook->ActivateTab(tab);
    SciDoc*sci=tw->FocusedDoc();
    sci->GoToPos(tw->bookmarked_pos);
    return false;
  }
  return true;
}



long TopWindow::onBookmark(FXObject*o, FXSelector sel, void*p)
{
  SciDoc*sci=ControlDoc();
  switch (FXSELID(sel)) {
    case ID_BOOKMARK_SET: {
      bookmarked_file=sci->Filename();
      bookmarked_tab=tabbook->ActiveTab();
      bookmarked_pos=sci->GetCaretPos();
      return 1;
    }
    case ID_BOOKMARK_RETURN: {
      if (!bookmarked_file.empty()) {
        if (OpenFile(bookmarked_file.text(),NULL,false,false)) {
          FocusedDoc()->GoToPos(bookmarked_pos);
        }
      } else {
        tabbook->ForEachTab(BookmarkCB,this);
      }
      return 1;
    }
    default:return 0;
  }
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



long TopWindow::onConfigureTools( FXObject*o, FXSelector sel, void*p )
{
  tooldlg=new ToolsDialog(this,UserMenus());
  tooldlg->execute(PLACEMENT_SCREEN);
  delete tooldlg;
  tooldlg=NULL;
  onRescanUserMenu(NULL,0,NULL);
  ClosedDialog();
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
  SciDoc*sci=ControlDoc();
  switch (prefs->SplitView) {
    case SPLIT_NONE: {
      switch (sci->GetSplit()) {
        case SPLIT_NONE: {
          sci->SetSplit(SPLIT_BELOW);
          break;
        }
        case SPLIT_BELOW: {
          sci->SetSplit(SPLIT_BESIDE);
          break;
        }
        case SPLIT_BESIDE: {
          sci->SetSplit(SPLIT_NONE);
          break;
        }
      }
      break;
    }
    case SPLIT_BELOW:
    case SPLIT_BESIDE: {
      switch (sci->GetSplit()) {
        case SPLIT_NONE: {
          sci->SetSplit(prefs->SplitView);
          break;
        }
        case SPLIT_BELOW:
        case SPLIT_BESIDE: {
          sci->SetSplit(SPLIT_NONE);
          break;
        }
      }
      break;
    }
  }
  sci=(SciDoc*)sci->getNext();
  if (sci) {
    SetSciDocPrefs(sci,prefs);
    sci->setFocus();
  } else {
    ControlDoc()->setFocus();
  }
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
  About();
  return 1;
}



long TopWindow::onMacroPlayback(FXObject*o, FXSelector sel, void*p)
{
  if (recorder&&!recording) {
    FXString src;
    recorder->translate(TranslatorCB,&src);
    RunMacro(src, false);
  }
  return 1;
}



long TopWindow::onMacroShow(FXObject*o, FXSelector sel, void*p)
{
  if (recorder&&!recording) {
    FXString src;
    recorder->translate(TranslatorCB,&src);
    NewFile(false);
    SciDoc*sci=ControlDoc();
    sci->setLanguage("lua");
    sci->SetText(src.text());
  }
  return 1;
}



long TopWindow::onMacroRecord(FXObject*o, FXSelector sel, void*p)
{
  FXToggleButton*tbar_rec_btn=(FXToggleButton*)(recorderstartmenu->getUserData());
  if (recording) {
    recorderstartmenu->setText(_("Re&cord macro"));
    recording->sendMessage(SCI_STOPRECORD, 0, 0);
    recording=NULL;
  } else {
    if (!recorder) {recorder=new MacroRecorder(); }
    recorder->clear();
    recorderstartmenu->setText(_("Stop re&cording"));
    recording=ControlDoc();
    recording->sendMessage(SCI_STARTRECORD, 0, 0);
  }
  SetMenuEnabled(playbackmenu,recording==NULL);
  SetMenuEnabled(showmacromenu,recording==NULL);
  if (tbar_rec_btn) { tbar_rec_btn->setState(recording!=NULL); }

  SetInfo(recording?_("(recording)"):"");
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
  usercmdmenu->rescan();
  userfiltermenu->rescan();
  usersnipmenu->rescan();
  usermacromenu->rescan();
  MenuMgr::PurgeTBarCmds();
  UpdateToolbar();
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
  FXMenuCommand* mc=(FXMenuCommand*)o;
  FXString script=(char*)(mc->getUserData());
  if ( ((FXuval)p)==2 ) { // Right-clicked, open file instead of executing
    OpenFile(script.text(), NULL, false, true);
    return 1;
  }
  //  If this file is currently open in the editor, and has
  //  unsaved changes, prompt the user to save the changes.
  FXWindow*tab,*page;
  for (tab=tabbook->getFirst(); tab && (page=tab->getNext()); tab=page->getNext()) {
    SciDoc*sci=(SciDoc*)page->getFirst();
    if (sci->Dirty() && (sci->Filename()==script)) {
      FXuint answer=FXMessageBox::warning(this,
        MBOX_YES_NO_CANCEL,_("Unsaved changes"),
        _("The disk file for the \"%s\" command is currently\n"
          " open in the editor, and has unsaved changes.\n\n"
          "  Save the file before continuing?"), mc->getText().text());
      switch (answer) {
        case MBOX_CLICKED_YES: {
          if (!filedlgs->SaveFile(sci,sci->Filename())) { return 1; }
          break;
        }
        case MBOX_CLICKED_NO: { break; }
        default: { return 1; }
      }
    }
  }
  FXString input="";
  SciDoc *sci=FocusedDoc();
  switch (FXSELID(sel)) {
    case ID_USER_COMMAND: {
      if (PathMatch("*.save.*", FXPath::name(script), FILEMATCH_CASEFOLD)) {
        if (!SaveAll(true)) { return 1; }
      }
#ifdef WIN32
     script.prepend('"');
     script.append('"');
#endif
      RunCommand(sci, script);
      break;
    }
    case ID_USER_FILTER: {
      if (sci->GetSelLength()>0) {
        sci->GetSelText(input);
        FilterSelection(sci, script, input);
      }
      break;
    }
    case ID_USER_SNIPPET: {
      if (PathMatch("*.exec.*", FXPath::name(script), FILEMATCH_CASEFOLD)) {
        FilterSelection(sci, script, input);
      } else {
        InsertFile(sci,script);
      }
      break;
    }
    case ID_USER_MACRO: {
      RunMacro(script, true);
      break;
    }
  }
  return 1;
}



long TopWindow::onFindTag(FXObject*o, FXSelector sel, void*p)
{
  FXString filename;
  FXString locn;
  FXString pattern;
  if ( FindTag(FocusedDoc(), unloadtagsmenu, filename, locn, pattern) ) {
    if (!filename.empty()) {
       OpenFile(filename.text(), locn.empty()?NULL:locn.text(),false,true);
       if (locn.empty() &&!pattern.empty()) {
         SciDoc*sci=ControlDoc();
         sci->sendMessage(SCI_SETTARGETSTART,0,0);
         sci->sendMessage(SCI_SETTARGETEND,sci->sendMessage(SCI_GETTEXTLENGTH,0,0),0);
         long oldflags=sci->sendMessage(SCI_GETSEARCHFLAGS,0,0);
         sci->sendMessage(SCI_SETSEARCHFLAGS, SCFIND_REGEXP|SCFIND_POSIX|SCFIND_MATCHCASE, 0);
         pattern.erase(0,1);
         pattern.trunc(pattern.length()-1);
         const char *esc_chars="*()";
         const char *c;
         for (c=esc_chars; *c; c++) {
           char esc[3]="\\ ";
           char orig[2]=" ";
           esc[1]=*c;
           orig[0]=*c;
           if ( (pattern.find(*c)>=0) && (pattern.find(esc)==-1)) {
             pattern.substitute(orig,esc,true);
           }
         }
         long found=sci->sendString(SCI_SEARCHINTARGET,pattern.length(),pattern.text());
         if (found>=0) {
           sci->GoToPos(found);
         }
         sci->sendMessage(SCI_SETSEARCHFLAGS,oldflags,0);
       }
    }
  }
  return 1;
}



long TopWindow::onShowCallTip(FXObject*o, FXSelector sel, void*p )
{
  ShowCallTip(FocusedDoc(), unloadtagsmenu);
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
  FXMenuCommand*mc=(FXMenuCommand*)o;
  mc->hide();
  mc->destroy();
  delete mc;
  if (unloadtagsmenu->getMenu()->numChildren()==0) {
    unloadtagsmenu->disable();
    SetMenuEnabled(findtagmenu,false);
    SetMenuEnabled(showtipmenu,false);
  }
  return 1;
}



long TopWindow::onReadOnly(FXObject*o, FXSelector sel, void*p)
{
  SciDoc*sci=ControlDoc();
  SetReadOnly(sci, !sci->GetReadOnly());
  readonlymenu->setCheck(sci->GetReadOnly());
  SyncToggleBtn(o,sel);
  sci->setFocus();
  return 1;
}



bool TopWindow::SetReadOnly(SciDoc*sci, bool rdonly)
{
  if (!sci) { return false; }
  if (rdonly && sci->Dirty()) {
    FXMessageBox::error(this, MBOX_OK, _("Unsaved changes"),
      _("Cannot mark a modified document as read-only.\n"
        "You should save or undo your changes first.")
    );
    return false;
  }
  SetTabLocked(sci,rdonly);
  need_status=32;
  return true;
}



void TopWindow::SetWordWrap(SciDoc*sci, bool wrapped)
{
  sci->SetWordWrap(wrapped);
  wordwrapmenu->setCheck(wrapped);
  SyncToggleBtn(wordwrapmenu,FXSEL(SEL_COMMAND,ID_WORDWRAP));
  
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
      tabbook->ForEachTab(AutoSaveCB,backups);
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
  LangStyle*ls=(LangStyle*) ((FXWindow*)o)->getUserData();
  SciDoc*sci=ControlDoc();
  sci->setLanguage(ls);
  for (FXWindow*wmc=langmenu->getFirst(); wmc; wmc=wmc->getNext()) {
    FXPopup*pu=((FXMenuCascade*)wmc)->getMenu();
    for (FXWindow*wmr=pu->getFirst(); wmr; wmr=wmr->getNext()) {
      FXMenuRadio*mr=(FXMenuRadio*)wmr;
      mr->setCheck(mr==o);
    }
  }
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
          CharAdded(sci,line,pos,scn->ch);
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
        SetMenuEnabled(filterselmenu,true);
        EnableUserFilters(true);
      } else {
        SetMenuEnabled(filterselmenu,false);
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
    default: { return 1; }
  }
}



// Switch tab orientations
long TopWindow::onTabOrient(FXObject*o,FXSelector sel,void*p)
{
  switch(FXSELID(sel)){
    case ID_TABS_TOP:
      tabbook->setTabStyle(TABBOOK_TOPTABS);
      prefs->DocTabPosition='T';
      break;
    case ID_TABS_BOTTOM:
      tabbook->setTabStyle(TABBOOK_BOTTOMTABS);
      prefs->DocTabPosition='B';
      break;
    case ID_TABS_LEFT:
      tabbook->setTabStyle(TABBOOK_LEFTTABS);
      prefs->DocTabPosition='L';
      break;
    case ID_TABS_RIGHT:
      tabbook->setTabStyle(TABBOOK_RIGHTTABS);
      prefs->DocTabPosition='R';
      break;
  }
  RadioUpdate(FXSELID(sel), ID_TABS_TOP, ID_TABS_RIGHT);
  FocusedDoc()->setFocus();
  return 1;
}



long TopWindow::onPackTabWidth(FXObject*o,FXSelector sel,void*p)
{
  prefs->DocTabsPacked=(FXSELID(sel) == ID_TABS_COMPACT);
  tabbook->setTabsCompact(prefs->DocTabsPacked);
  RadioUpdate(FXSELID(sel),ID_TABS_UNIFORM,ID_TABS_COMPACT);
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
  filedlgs->SaveFileAs(ControlDoc(),false);
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
  SciDoc*sci=ControlDoc();
  FXString orig=sci->Filename();
  if (filedlgs->SaveFileAs(sci)) {
    if (!orig.empty()) { recent_files->prepend(orig); }
    recent_files->remove(sci->Filename());
  }
  return 1;
}



long TopWindow::onFileNew(FXObject*o, FXSelector sel, void*p )
{
  NewFile(true);
  return 1;
}


class WkDirDlg: public FXDirDialog {
private:
  class DirSel: public FXDirSelector {
    public:
    FXDirList* list()  { return dirbox; }
  };
public:
  WkDirDlg(FXWindow* win,const FXString& name):FXDirDialog(win,name) {}
  void setDirectory(const FXString& path) {
    FXDirDialog::setDirectory((FXPath::simplify(path)));
    if (FXPath::isTopDirectory(getDirectory())) {
      FXDirList*list=((DirSel*)dirbox)->list();
      list->expandTree(list->getFirstItem());
    }
  }
};


long TopWindow::onSelectDir(FXObject*o, FXSelector sel, void*p)
{
  WkDirDlg dlg(this, _("Set Working Directory"));
  dlg.setHeight(420);
  dlg.setDirectory(FXSystem::getCurrentDirectory()+PATHSEP);
  if (dlg.execute(PLACEMENT_OWNER)) {
    FXSystem::setCurrentDirectory(dlg.getDirectory());
  }
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
  SciDoc*sci=FocusedDoc();
  if (sci->GetSelLength()>0) { sci->sendMessage(SCI_CUT,0,0); }
  sci->setFocus();
  return 1;
}



long TopWindow::onCopy(FXObject*o, FXSelector sel, void*p)
{
  SciDoc*sci=FocusedDoc();
  // If any text is already selected, make sure the selection is "alive"
  long start=sci->sendMessage(SCI_GETSELECTIONSTART,0,0);
  long end=sci->sendMessage(SCI_GETSELECTIONEND,0,0);
  if (start!=end) {
    sci->sendMessage(SCI_SETSELECTIONSTART,start,0);
    sci->sendMessage(SCI_SETSELECTIONEND,end,0);
  }
  if (sci->GetSelLength()>0) { sci->sendMessage(SCI_COPY,0,0); }
  if (getApp()->getFocusWindow()!=outlist) { sci->setFocus(); }
  return 1;
}



void TopWindow::Paste()
{
  SciDoc*sci=FocusedDoc();
  sci->setFocus();
  if (sci->sendMessage(SCI_CANPASTE,0,0)) {
    // If any text is already selected, make sure the selection is "alive"
    long start=sci->sendMessage(SCI_GETSELECTIONSTART,0,0);
    long end=sci->sendMessage(SCI_GETSELECTIONEND,0,0);
    if (start!=end) {
      sci->sendMessage(SCI_SETSELECTIONSTART,start,0);
      sci->sendMessage(SCI_SETSELECTIONEND,end,0);
    }
    sci->sendMessage(SCI_PASTE,0,0);
    sci->sendMessage(SCI_CONVERTEOLS,sci->sendMessage(SCI_GETEOLMODE,0,0),0);
    if (sci->GetWordWrap()) {
      getApp()->runWhileEvents();
      sci->sendMessage(SCI_SCROLLCARET,0,0);
    }
  }
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
  if (srchdlgs->ShowFindDialog(FocusedDoc())) {
    macro_record_search();
  }
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
  SciDoc*sci=FocusedDoc();
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
    tabbook->ForEachTab(ZoomStepCB, &z);
  } else {
    z=(FXival)p;
   tabbook->ForEachTab(ZoomSpecCB, &z);
  }
  if (sci) { prefs->ZoomFactor=sci->GetZoom(); }
  return 1;
}



long TopWindow::onShowLineNums( FXObject*o, FXSelector sel, void*p )
{
  prefs->ShowLineNumbers=!prefs->ShowLineNumbers;
  tabbook->ForEachTab(LineNumsCB, (void*)(FXival)prefs->ShowLineNumbers);
  linenums_chk->setCheck(prefs->ShowLineNumbers);
  SyncToggleBtn(o,sel);
  return 1;
}



long TopWindow::onShowToolbar( FXObject*o, FXSelector sel, void*p )
{
  prefs->ShowToolbar=!prefs->ShowToolbar;
  if (prefs->ShowToolbar) {
    toolbar_frm->show();
  } else {
    toolbar_frm->hide();
  }
  toolbar_frm->getParent()->layout();
  SyncToggleBtn(o,sel);
  return 1;
}



long TopWindow::onShowWhiteSpace(FXObject*o, FXSelector sel, void*p)
{
  prefs->ShowWhiteSpace=!prefs->ShowWhiteSpace;
  tabbook->ForEachTab(WhiteSpaceCB, (void*)(FXival)prefs->ShowWhiteSpace);
  white_chk->setCheck(prefs->ShowWhiteSpace);
  SyncToggleBtn(o,sel);
  return 1;
}



long TopWindow::onShowMargin(FXObject*o, FXSelector sel, void*p)
{
  prefs->ShowRightEdge=!prefs->ShowRightEdge;
  tabbook->ForEachTab(ShowMarginCB, (void*)(FXival)prefs->ShowRightEdge);
  margin_chk->setCheck(prefs->ShowRightEdge);
  SyncToggleBtn(o,sel);
  return 1;
}



long TopWindow::onShowIndent(FXObject*o, FXSelector sel, void*p)
{
  prefs->ShowIndentGuides = !prefs->ShowIndentGuides;
  tabbook->ForEachTab(ShowIndentCB, (void*)(FXival)prefs->ShowIndentGuides);
  guides_chk->setCheck(prefs->ShowIndentGuides);
  SyncToggleBtn(o,sel);
  return 1;
}



long TopWindow::onShowOutputPane(FXObject*o, FXSelector sel, void*p)
{
  ShowOutputPane(!prefs->ShowOutputPane);
  SyncToggleBtn(o,sel);
  return 1;
}



long TopWindow::onShowStatusBar(FXObject*o, FXSelector sel, void*p)
{
  ShowStatusBar(!prefs->ShowStatusBar);
  SyncToggleBtn(o,sel);
  return 1;
}



long TopWindow::onPrefsDialog(FXObject*o, FXSelector sel, void*p)
{
  if (!prefdlg) { prefdlg=new PrefsDialog(this, prefs); }
  prefdlg->execute(PLACEMENT_DEFAULT);
  delete prefdlg;
  prefdlg=NULL;
  ClosedDialog();
  SetSrchDlgsPrefs();
  tabbook->MaxTabWidth(prefs->TabTitleMaxWidth);
  tabbook->ForEachTab(PrefsCB, NULL);
  CheckStyle(NULL,0,ControlDoc());
  if ((prefs->WatchExternChanges||prefs->Autosave) && !getApp()->hasTimeout(this,ID_TIMER)) {
    getApp()->addTimeout(this,ID_TIMER, ONE_SECOND, NULL);
  }
  getApp()->setWheelLines(prefs->WheelLines);
  if ( PrefsDialog::ChangedToolbar() & ToolbarChangedLayout ) { UpdateToolbar(); }
  if ( PrefsDialog::ChangedToolbar() & ToolbarChangedWrap ) { UpdateToolbarWrap(); }
  if ( PrefsDialog::ChangedToolbar() & ToolbarChangedFont ) {
    delete toolbar_font;
    toolbar_font=NULL;
    ForEachToolbarButton(SetTBarBtnFontCB,this);
    toolbar_frm->getParent()->layout();
  }
  filedlgs->patterns(prefs->FileFilters);
  if (Theme::changed() & ThemeChangedColors) {
    Theme::apply(this);
    Theme::apply(srchdlgs->FindDialog());
    tips->setBackColor(getApp()->getTipbackColor());
    tips->setTextColor(getApp()->getTipforeColor());
    SetStatusBarColors();
  }
  tabbook->ActivateTab(tabbook->ActiveTab());
  SetToolbarColors();
  EnableUserFilters(FocusedDoc()->GetSelLength());
  return 1;
}



long TopWindow::onCtrlTab(FXObject*o, FXSelector sel, void*p)
{
  if (!prefs->UseTabs) {
    SciDoc*sci=FocusedDoc();
    if (sci) {
      sci->sendString(SCI_REPLACESEL,0,"\t");
      return 1;
    }
  }
  return 0;
}



long TopWindow::onIndent(FXObject*o, FXSelector sel, void*p)
{
  SciDoc*sci=FocusedDoc();
  if (!sci) { return 0; }
  int selid=FXSELID(sel);
  long msg=((ID_INDENT_STEP==selid)||(ID_INDENT_FULL==selid))?SCI_TAB:SCI_BACKTAB;
  int tab_width=sci->TabWidth();
  if ((ID_INDENT_STEP==selid)||(ID_UNINDENT_STEP==selid))
  {
    FXbool use_tabs=sci->UseTabs();
    sci->UseTabs(false);
    sci->TabWidth(1);
    sci->sendMessage(msg,0,0);
    sci->TabWidth(tab_width);
    sci->UseTabs(use_tabs);
  } else {
    sci->TabWidth(sci->UseTabs()?tab_width:prefs->IndentWidth);
    sci->sendMessage(msg,0,0);
    sci->TabWidth(tab_width);
  }
  return 1;
}



static bool AvoidMultiLineCommand(TopWindow*w, const FXString &cmd)
{
  if (cmd.contains('\n')) {
    FXMessageBox::error(w, MBOX_OK, _("Command Error"),
      _("Multiline commands are not supported."));
    return false;
  } else {
    return true;
  }
}



long TopWindow::onFilterSel(FXObject*o, FXSelector sel, void*p)
{
  SciDoc *sci=FocusedDoc();
  HistBox *dlg;
  bool save_first;
  bool is_filter=FXSELID(sel)==ID_FILTER_SEL;
  if(is_filter) {
    dlg=new HistBox(this, _("Filter selection"), _("Command:"), "Filters");
    save_first=prefs->SaveBeforeFilterSel;
  } else {
    dlg=new HistBox(this, _("Insert output of command"), _("Command:"), "InsertOutput");
    save_first=prefs->SaveBeforeInsCmd;
  }
  dlg->setNumColumns(48);
  if ( dlg->execute(PLACEMENT_OWNER) ) {
    FXString cmd=dlg->getText();
    if (AvoidMultiLineCommand(this, cmd)) {
      if ( (!save_first) || SaveAll(true) ) {
        FXString input="";
        if (is_filter) { sci->GetSelText(input); }
        FilterSelection(sci, cmd, input);
      }
    }
  }
  delete dlg;
  ClosedDialog();
  return 1;
}



long TopWindow::onKillCommand(FXObject*o, FXSelector sel, void*p)
{
  command_timeout=true;
  return 1;
}



long TopWindow::onRunCommand(FXObject*o, FXSelector sel, void*p)
{
  SciDoc *sci=FocusedDoc();
  HistBox *dlg= new HistBox(this, _("Run command"), _("Command:"), "Commands");
  dlg->setNumColumns(48);
  if ( dlg->execute(PLACEMENT_OWNER) ) {
    FXString cmd=dlg->getText();
    if (AvoidMultiLineCommand(this,cmd)) {
      if ( (!prefs->SaveBeforeExecCmd) || SaveAll(true) ) {
        ClosedDialog();
        RunCommand(sci,cmd);
      }
    }
  } else {
    ClosedDialog();
  }
  delete dlg;
  return 1;
}



long TopWindow::onOutlistPopup(FXObject*o, FXSelector sel, void*p)
{
  FXint count=outlist->getNumItems();
  if (count==0) { return 1; }
  switch (FXSELID(sel))
  {
    case ID_OUTLIST_ASEL: {
      outlist->setAnchorItem(0);
      outlist->selectItem(0);
      outlist->extendSelection(count-1);
      break;
    }
    case ID_OUTLIST_COPY: {
      FXint i;
      FXString outclip="";
      FXString newline="\n";
      switch (prefs->DefaultFileFormat) {
        case 0: { newline="\r\n"; break; }
        case 1: { newline="\r";   break; }
        case 2: { newline="\n";   break; }
      }
      for (i=0; i<count; i++) {
        if (outlist->isItemSelected(i)) {
          outclip.append(outlist->getItemText(i));
          outclip.append(newline);
        }
      }
      ControlDoc()->sendString(SCI_COPYTEXT,outclip.length(), outclip.text());
    }
    default: { return 0; }
  }
  return 1;
}



long TopWindow::onOutlistFocus(FXObject*o, FXSelector sel, void*p)
{
  static bool isfocused=false;
  if (outlist->getNumItems()<=0) { return 1; }
  if (isfocused) {
    outlist->killFocus();
    ControlDoc()->setFocus();
  } else {
    skipfocus=true;
    if (!prefs->ShowOutputPane) { ShowOutputPane(true); }
    FocusedDoc()->killFocus();
    outlist->setFocus();
    if (outlist->getCurrentItem()<0) { outlist->setCurrentItem(0); }
    if (!outlist->isItemSelected(outlist->getCurrentItem())) {
      outlist->selectItem(outlist->getCurrentItem());
    }
    outlist->makeItemVisible(outlist->getCurrentItem());
  }
  isfocused=!isfocused;
  return 1;
}



long TopWindow::onGoToError(FXObject*o, FXSelector sel, void*p)
{
  FXint n=outlist->getCurrentItem();
  if (n>=0) {
    FXListItem*item=outlist->getItem(n);
    if (item) {
      FXString txt=item->getText();
      if (!txt.empty()) {
        ErrorPattern*pats=prefs->ErrorPatterns();
        for (FXint i=0; i<prefs->ErrorPatternCount(); i++) {
          FXint begs[4]={0,0,0,0};
          FXint ends[4]={0,0,0,0};
          FXRex rx(pats[i].pat, REX_CAPTURE);
          if (rx.match(txt,begs,ends,REX_FORWARD,3)) {
            FXString filename = txt.mid(begs[1],ends[1]-begs[1]);
            FXString linenum =  txt.mid(begs[2],ends[2]-begs[2]);
            if (FXStat::isFile(filename)) {
              OpenFile(filename.text(), linenum.text(),false,true);
              break;
            } else {
              SciDoc*sci=ControlDoc();
              if (sci && (!sci->Filename().empty()) && (!FXPath::isAbsolute(filename))) {
                filename=FXPath::name(filename);
                filename.prepend(PATHSEP);
                filename.prepend(FXPath::directory(sci->Filename()));
                if (FXStat::isFile(filename)) {
                  OpenFile(filename.text(), linenum.text(),false,true);
                  break;
                }
              }
            }
          }
        }
      }
    }
  }
  return 1;
}



long TopWindow::onOutlistClick(FXObject*o, FXSelector sel, void*p)
{
  if (o!=outlist) {return 0;}
  FXEvent* ev=(FXEvent*)p;
  switch (FXSELTYPE(sel)) {
    case SEL_RIGHTBUTTONRELEASE: {
      if(!ev->moved){
        outpop->popup(NULL,ev->root_x,ev->root_y);
        getApp()->runModalWhileShown(outpop);
      }
      return 1;
    }
    case SEL_DOUBLECLICKED: { break; }
    case SEL_KEYPRESS: {
      FXint code=ev->code;
      if ((code==KEY_Return)||(code==KEY_KP_Enter)) { break; } else {
        if (code==KEY_Tab) {
          outlist->killFocus();
          ControlDoc()->setFocus();
          return 1;
        } else {
          if (ev->state==CONTROLMASK) {
            switch (code) {
              case KEY_a: { return onOutlistPopup(this,FXSEL(SEL_COMMAND,ID_OUTLIST_ASEL),p); }
              case KEY_c: { return onOutlistPopup(this,FXSEL(SEL_COMMAND,ID_OUTLIST_COPY),p); }
            }
          }
        }
        return 0;
      }
    }
    case SEL_FOCUSIN: {
      active_widget=outlist;
      return 0;
    }
    default: { return 0; }
  }
  onGoToError(o,sel,p);
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



long TopWindow::onTBarCustomCmd(FXObject*o, FXSelector sel, void*p)
{
  MenuSpec*spec=(MenuSpec*)(((FXButton*)o)->getUserData());
  return onUserCmd(spec->ms_mc, FXSEL(SEL_COMMAND,spec->ms_mc->getSelector()),p);
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

