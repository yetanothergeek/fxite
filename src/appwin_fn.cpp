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

#include "lang.h"
#include "scidoc.h"
#include "doctabs.h"
#include "prefs.h"
#include "prefdlg.h"
#include "theme.h"
#include "statusbar.h"
#include "filer.h"
#include "backup.h"
#include "menuspec.h"
#include "search.h"
#include "toolbar.h"
#include "mainmenu.h"
#include "tagread.h"
#include "toolmgr.h"
#include "compat.h"
#include "scidoc_util.h"
#include "foreachtab.h"

#include "intl.h"
#include "appwin.h"



// Tell the window manager we want the focus back after dialogs close...
void TopWindow::ClosedDialog()
{
  setFocus();
  FocusedDoc()->setFocus();
}



void TopWindow::Cut()
{
  SciDocUtils::Cut(FocusedDoc());
}



void TopWindow::Copy()
{
  SciDocUtils::Copy(FocusedDoc());
}



void TopWindow::Paste()
{
  SciDocUtils::Paste(FocusedDoc());
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
  menubar->SetReadOnly(rdonly);
  need_status=32;
  return true;
}



void TopWindow::SetWordWrap(SciDoc*sci, bool wrapped)
{
  sci->SetWordWrap(wrapped);
  menubar->SetCheck(ID_WORDWRAP,wrapped);
}



void TopWindow::EnableUserFilters(bool enabled)
{
  toolbar->EnableFilterBtn(enabled);
  menubar->EnableFilterMenu(enabled);
}



void TopWindow::RemoveTBarBtnData(void*p)
{
  toolbar->NullifyButtonData(p);
}



void TopWindow::UpdateToolbar()
{
  toolbar->CreateButtons(this);
  switch (prefs->DocTabPosition) {
    case 'T': MenuMgr::RadioUpdate(ID_TABS_TOP,    ID_TABS_TOP, ID_TABS_RIGHT); break;
    case 'B': MenuMgr::RadioUpdate(ID_TABS_BOTTOM, ID_TABS_TOP, ID_TABS_RIGHT); break;
    case 'L': MenuMgr::RadioUpdate(ID_TABS_LEFT,   ID_TABS_TOP, ID_TABS_RIGHT); break;
    case 'R': MenuMgr::RadioUpdate(ID_TABS_RIGHT,  ID_TABS_TOP, ID_TABS_RIGHT); break;
  }
  MenuMgr::RadioUpdate(prefs->DocTabsPacked?ID_TABS_COMPACT:ID_TABS_UNIFORM,ID_TABS_UNIFORM,ID_TABS_COMPACT);
  menubar->Recording(recording,recorder);
  toolbar->EnableFilterBtn(FocusedDoc()&&(FocusedDoc()->GetSelLength()>0));
}



bool TopWindow::ShowSaveAsDlg(SciDoc*sci)
{
  FXString orig=sci->Filename();
  if (filedlgs->SaveFileAs(sci)) {
    if (!orig.empty()) { menubar->PrependRecentFile(orig); }
    menubar->RemoveRecentFile(sci->Filename());
    return true;
  }
  return false;
}



/*
  This chore updates a document's settings, and adds a chore to do the same
  for the next document that needs updating. (See StyleNextDocCB() for more.)
*/
long TopWindow::CheckStyle(FXObject*o, FXSelector sel, void*p)
{
  SciDoc*sci=(SciDoc*)p;
  if (sci && sci->NeedStyled()) {
    SciDocUtils::SetSciDocPrefs(sci, prefs);
    sci->UpdateStyle();
    sci->NeedStyled(false);
  }
  tabbook->ForEachTab(TabCallbacks::StyleNextDocCB,this,false);
  return 1;
}



// Set or unset the '*' or '#' prefix of a tab's title...
void TopWindow::SetTabTag(SciDoc*sci, char mark, bool set)
{
  DocTab*tab=(DocTab*)sci->getParent()->getPrev();
  if (set) {
    if (tab->getText()[0]!=mark) { tab->setText(tab->getText().prepend(mark)); }
  } else {
    if (tab->getText()[0]==mark) { tab->setText(tab->getText().erase(0,1)); }
  }
}



// Add '*' prefix to tab's title to denote file has unsaved changes
void TopWindow::SetTabDirty(SciDoc*sci, bool dirty)
{
  SetTabTag(sci,'*', dirty);
  sci->Dirty(dirty);
}



// Add '#' prefix to tab's title to denote document is marked read-only
void TopWindow::SetTabLocked(SciDoc*sci, bool locked)
{
  SetTabTag(sci,'#', locked);
  sci->sendMessage(SCI_SETREADONLY,locked?1:0,0);
}



static const char* dont_freeze_me = "don't taze me, bro!";


void TopWindow::Freeze(FXWindow*win, bool frozen)
{
  FXWindow*w;
  for (w=win->getFirst(); w; w=w->getNext()) {
    if (w->getUserData()==dont_freeze_me) { continue; }
    if (frozen) {
      w->disable();
      w->repaint();
    } else {w->enable();}
    Freeze(w,frozen);
  }
}



const char* TopWindow::DontFreezeMe()
{
  return dont_freeze_me;
}



bool TopWindow::IsCommandReady()
{
  if (command_busy) {
    FXMessageBox::error(this, MBOX_OK, _("Command error"),
      _("Multiple commands cannot be executed at the same time."));
    return false;
  }
  if (!temp_accels) {
    FXMessageBox::error(this, MBOX_OK, _("Command support disabled"),
      _("Support for running macros and external commands has been\n"
        "disabled, because the interrupt key sequence is invalid.\n\n"
        "To fix this, go to:\n"
        "  Edit->Preferences->Keybindings\n"
        "and enter a valid setting for \"%s\""),
      MenuMgr::LookupMenu(TopWindow::ID_KILL_COMMAND)->pref
    );
    return false;
  }
  return true;
}



/* Return true if the document is still open */
bool TopWindow::IsDocValid(SciDoc*sci)
{
  if (sci) {
    SciDoc*closed=sci;
    tabbook->ForEachTab(TabCallbacks::FileStillOpenCB,&closed);
    return (!closed);
  } else {
    return false;
  }
}



// Exposes "userland" search behavior to scripting engine.
bool TopWindow::FindText(const char*searchstring, FXuint searchmode, bool forward)
{
  return srchdlgs->FindPhrase(ControlDoc(),searchstring,searchmode,forward);
}



long TopWindow::CheckStale(FXObject*o, FXSelector sel, void*p)
{
  static bool CheckingStale=false;
  SciDoc*sci=ControlDoc();
  if (!sci) { return 1; }
  switch (sci->Stale()) {
    case 0: { break; }
    case 1: {
      StaleTicks=0;
      if ( !CheckingStale ) {
        if (!IsDesktopCurrent(this)) { return 1; }
        CheckingStale=true;
        FXPopup *popup=getApp()->getPopupWindow();
        if (popup) { popup->popdown(); }
        if ( FXMessageBox::question(this, MBOX_YES_NO, _("File changed"),
             "%s\n%s\n\n%s",
             sci->Filename().text(),
             _("was modified externally."),
             _("Reload from disk?")
             )==MBOX_CLICKED_YES )
        {
          if (filedlgs->AskReload(sci)) {
            if (sci->GetReadOnly()) { SetTabLocked(sci,true); }
          }
        } else {
          sci->DoStaleTest(false);
        }
        CheckingStale=false;
      }
      break;
    }
    case 2: {
      StaleTicks=0;
      if ( !CheckingStale ) {
        if (!IsDesktopCurrent(this)) { return 1; }
        CheckingStale=true;
        FXPopup *popup=getApp()->getPopupWindow();
        if (popup) { popup->popdown(); }
        if ( FXMessageBox::question(this, MBOX_YES_NO, _("File status error"),
             "%s:\n%s\n(%s)\n\n%s",
             _("Error checking the status of"),
             sci->Filename().text(), sci->GetLastError().text(),
             _("Save to disk now?")
             )==MBOX_CLICKED_YES )
        {
          if (!filedlgs->SaveFile(sci,sci->Filename())) {
            ShowSaveAsDlg(sci);
          }
        } else {
          sci->DoStaleTest(false);
        }
        CheckingStale=false;
      }
      break;
    }
  }
  return 1;
}


/*
  When in split-view mode, ControlDoc() refers to the view at the top (or left) of
  the split,and FocusedDoc() refers to whichever view has the focus; In single-view
  mode there is no difference between the ControlDoc and the FocusedDoc.
*/
SciDoc*TopWindow::ControlDoc()
{
  FXWindow *page=tabbook->ActivePage();
  if (page) {
    return (SciDoc*)page->getFirst();
  } else {
    return NULL;
  }
}



SciDoc*TopWindow::FocusedDoc()
{
  return (SciDoc*)tabbook->ActiveView();
}



UserMenu**TopWindow::UserMenus() const
{
  return menubar->UserMenus();
}



void TopWindow::AdjustIndent(SciDoc*sci, char ch)
{
  getApp()->runWhileEvents();
  long pos=sci->sendMessage(SCI_GETCURRENTPOS,0,0);
  long line=sci->sendMessage(SCI_LINEFROMPOSITION,pos,0);
  CharAdded(sci,line,pos,ch);
}



void TopWindow::CharAdded(SciDoc*sci, long line, long pos, int ch)
{
  if ( (line<=0) || (prefs->AutoIndent==AUTO_INDENT_NONE)) { return; }
  if (recording) { recording->sendMessage(SCI_STOPRECORD,0,0); }
  SciDocUtils::CharAdded(sci, line, pos, ch, prefs->AutoIndent==AUTO_INDENT_SMART, prefs->IndentWidth);
  if (recording) { recording->sendMessage(SCI_STARTRECORD,0,0); }
}



void TopWindow::OpenSelected()
{
  SciDocUtils::OpenSelected(this, FocusedDoc());
}



void TopWindow::InvertColors(bool inverted)
{
  prefs->InvertColors=inverted;
  toolbar->SetToolbarColors();
  tabbook->ForEachTab(TabCallbacks::PrefsCB,NULL);
  CheckStyle(NULL,0,ControlDoc());
  menubar->SetCheck(ID_INVERT_COLORS,prefs->InvertColors); 
}



void TopWindow::RunUserCmd(FXMenuCommand*mc,FXSelector sel,FXuval b)
{
  FXString script=(char*)(mc->getUserData());
  if ( b==2 ) { // Right-clicked, open file instead of executing
    OpenFile(script.text(), NULL, false, true);
    return;
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
          if (!filedlgs->SaveFile(sci,sci->Filename())) { return; }
          break;
        }
        case MBOX_CLICKED_NO: { break; }
        default: { return; }
      }
    }
  }
  FXString input="";
  SciDoc *sci=FocusedDoc();
  switch (FXSELID(sel)) {
    case ID_USER_COMMAND: {
      if (PathMatch("*.save.*", FXPath::name(script), FILEMATCH_CASEFOLD)) {
        if (!SaveAll(true)) { return; }
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
}



void TopWindow::FindTag()
{
  FXString filename;
  FXString locn;
  FXString pattern;
  if ( !TagHandler::FindTag(FocusedDoc(), TagFiles(), filename, locn, pattern) ) { return; }
  if ( filename.empty()) { return; }
  if (OpenFile(filename.text(), locn.text(),false,true)) {
    if (locn.empty()) { TagHandler::GoToTag(ControlDoc(),pattern); }
  }
}



void TopWindow::SetFileFormat(FXSelector sel)
{
  int EolMode=SC_EOL_LF;
  switch (sel) {
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
  MenuMgr::RadioUpdate(sel,ID_FMT_DOS,ID_FMT_UNIX);
}



void TopWindow::SetTabOrientation(FXSelector sel)
{
  switch(sel){
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
  MenuMgr::RadioUpdate(sel, ID_TABS_TOP, ID_TABS_RIGHT);
}



void TopWindow::ShowPrefsDialog()
{
  if (!prefdlg) { prefdlg=new PrefsDialog(this, prefs); }
  prefdlg->execute(PLACEMENT_DEFAULT);
  delete prefdlg;
  prefdlg=NULL;
  ClosedDialog();
  SetSrchDlgsPrefs();
  tabbook->MaxTabWidth(prefs->TabTitleMaxWidth);
  tabbook->ForEachTab(TabCallbacks::PrefsCB, NULL);
  CheckStyle(NULL,0,ControlDoc());
  if ((prefs->WatchExternChanges||prefs->Autosave) && !getApp()->hasTimeout(this,ID_TIMER)) {
    getApp()->addTimeout(this,ID_TIMER, ONE_SECOND, NULL);
  }
  getApp()->setWheelLines(prefs->WheelLines);
  if ( PrefsDialog::ChangedToolbar() & ToolbarChangedLayout ) {
    UpdateToolbar();
  }
  if ( PrefsDialog::ChangedToolbar() & ToolbarChangedWrap ) {
    toolbar->handle(toolbar,FXSEL(SEL_CONFIGURE,0),NULL);
  }
  if ( PrefsDialog::ChangedToolbar() & ToolbarChangedFont ) {
    toolbar->SetTBarFont();
  }
  filedlgs->patterns(prefs->FileFilters);
  if (Theme::changed() & ThemeChangedColors) {
    Theme::apply(this);
    Theme::apply(srchdlgs->FindDialog());
    tips->setBackColor(getApp()->getTipbackColor());
    tips->setTextColor(getApp()->getTipforeColor());
    statusbar->Colorize();
  }
  tabbook->ActivateTab(tabbook->ActiveTab());
  toolbar->SetToolbarColors();
  EnableUserFilters(FocusedDoc()->GetSelLength());  
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



void TopWindow::ShowFilterDialog(bool is_filter)
{
  SciDoc *sci=FocusedDoc();
  HistBox *dlg;
  bool save_first;
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
}



void TopWindow::ShowCommandDialog()
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
}



void TopWindow::Indent(FXSelector sel)
{
  SciDoc*sci=FocusedDoc();
  long msg=((ID_INDENT_STEP==sel)||(ID_INDENT_FULL==sel))?SCI_TAB:SCI_BACKTAB;
  int tab_width=sci->TabWidth();
  if ((ID_INDENT_STEP==sel)||(ID_UNINDENT_STEP==sel))
  {
    FXbool use_tabs=sci->UseTabs();
    sci->UseTabs(false);
    sci->sendMessage(SCI_SETTABWIDTH,1,0);
    sci->sendMessage(msg,0,0);
    sci->TabWidth(tab_width);
    sci->UseTabs(use_tabs);
  } else {
    sci->TabWidth(sci->UseTabs()?tab_width:prefs->IndentWidth);
    sci->sendMessage(msg,0,0);
    sci->TabWidth(tab_width);
  }
}



bool TopWindow::FoundBookmarkedTab(DocTab*tab)
{
  if (bookmarked_tab == tab) {
    tabbook->ActivateTab(tab);
    FocusedDoc()->GoToPos(bookmarked_pos);
    return true;
  }
  return false;
}



void TopWindow::SetBookmark()
{
  SciDoc*sci=ControlDoc();
  bookmarked_file=sci->Filename();
  bookmarked_tab=tabbook->ActiveTab();
  bookmarked_pos=sci->GetCaretPos();
}



void TopWindow::GoToBookmark()
{
  if (!bookmarked_file.empty()) {
    if (OpenFile(bookmarked_file.text(),NULL,false,false)) {
      FocusedDoc()->GoToPos(bookmarked_pos);
    }
  } else {
    tabbook->ForEachTab(TabCallbacks::BookmarkCB,this);
  }
}



void TopWindow::ShowToolManagerDialog()
{
  ToolsDialog tooldlg(this,UserMenus());
  tooldlg.execute(PLACEMENT_SCREEN);
  RescanUserMenu();
  ClosedDialog();
}



void TopWindow::RescanUserMenu()
{
  menubar->RescanUserMenus();
  MenuMgr::PurgeTBarCmds();
  UpdateToolbar();
}

