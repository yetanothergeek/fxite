/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2009-2010 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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
#include <Scintilla.h>
#include <SciLexer.h>
#include <FXScintilla.h>

#include "lang.h"
#include "scidoc.h"
#include "doctabs.h"
#include "prefs.h"
#include "filer.h"
#include "backup.h"
#include "menuspec.h"
#include "shmenu.h"
#include "search.h"

#include "intl.h"
#include "appwin.h"



//  Used by the macro recorder to translate "machine language" into a Lua macro.
void TopWindow::TranslatorCB(const char*text, void*user_data)
{
  FXString*all=(FXString*)user_data;
  all->append(text);
  all->append('\n');
}


// Update a set of radio buttons
void TopWindow::RadioUpdate(FXSelector curr, FXSelector min, FXSelector max)
{
  for (FXSelector i=min; i<=max; i++) {
    MenuSpec*spec=MenuMgr::LookupMenu(i);
    if (spec && spec->ms_mc) {
      ((FXMenuRadio*)(spec->ms_mc))->setCheck(curr==i);
      FXButton*btn=(FXButton*)spec->ms_mc->getUserData();
      if (btn) {
        if (curr==i) {
          btn->setState(btn->getState()|STATE_ENGAGED);
        } else {
          btn->setState(btn->getState()&~STATE_ENGAGED);
        }
      }
    }
  }
}



void TopWindow::SyncToggleBtn(FXObject*o, FXSelector sel)
{
  MenuSpec*spec=MenuMgr::LookupMenu(FXSELID(sel));
  if (spec && (spec->ms_mc==o) && (spec->type=='k')) {
    FXToggleButton*btn=(FXToggleButton*)(spec->ms_mc->getUserData());
    if (btn) {
      btn->setState(((FXMenuCheck*)(spec->ms_mc))->getCheck());
    }
  }
}



void TopWindow::SetMenuEnabled(FXMenuCommand*mnu, bool enabled)
{
  FXLabel*btn=(FXLabel*)(mnu->getUserData());
  if (enabled) {
    mnu->enable();
    if (btn) { btn->enable(); }
  } else {
    mnu->disable();
    if (btn) { btn->disable(); }
  }
}



void TopWindow::EnableFilterBtnCB(FXButton*btn, void*user_data)
{
  MenuSpec*spec=(MenuSpec*)btn->getUserData();
  if (spec && (spec->type=='u') && spec->ms_mc && (spec->ms_mc->getSelector()==ID_USER_FILTER)) {
    if ((bool)user_data) { btn->enable(); } else { btn->disable(); }
  }
}



void TopWindow::EnableUserFilters(bool enabled)
{
  ForEachToolbarButton(EnableFilterBtnCB,(void*)enabled);
  if (enabled) { userfiltermenu->enable(); } else { userfiltermenu->disable(); }
}




// Check each document to see if any of them should be auto-saved.
bool TopWindow::AutoSaveCB(FXint index, DocTab*tab, FXWindow*page, void*user_data)
{
  SciDoc*sci=(SciDoc*)page->getFirst();
  if (sci->NeedBackup()) {
    ((BackupMgr*)user_data)->SaveBackup(sci);
  }
  return true;
}



/* Zoom each document In/Out/Close/Far/Default */
bool TopWindow::ZoomStepCB(FXint index, DocTab*tab, FXWindow*page, void*user_data)
{
  SciDoc*sci=(SciDoc*)page->getFirst();
  sci->Zoom(*((int*)user_data));
  return true;
}



/* Zoom each document to a specified amount (Used by Lua) */
bool TopWindow::ZoomSpecCB(FXint index, DocTab*tab, FXWindow*page, void*user_data)
{
  SciDoc*sci=(SciDoc*)page->getFirst();
  sci->sendMessage(SCI_SETZOOM,*((int*)user_data),0);
  return true;
}



bool TopWindow::LineNumsCB(FXint index, DocTab*tab, FXWindow*page, void*user_data)
{
  SciDoc*sci=(SciDoc*)page->getFirst();
  sci->ShowLineNumbers((bool)user_data);
  return true;
}



bool TopWindow::WhiteSpaceCB(FXint index, DocTab*tab, FXWindow*page, void*user_data)
{
  SciDoc*sci=(SciDoc*)page->getFirst();
  sci->ShowWhiteSpace((bool)user_data);
  return true;
}



bool TopWindow::ShowMarginCB(FXint index, DocTab*tab, FXWindow*page, void*user_data)
{
  for (SciDoc*sci=(SciDoc*)page->getFirst(); sci; sci=(SciDoc*)sci->getNext()) {
    sci->SetShowEdge((bool)user_data);
  }
  return true;
}



bool TopWindow::ShowIndentCB(FXint index, DocTab*tab, FXWindow*page, void*user_data)
{
  for (SciDoc*sci=(SciDoc*)page->getFirst(); sci; sci=(SciDoc*)sci->getNext()) {
    sci->SetShowIndent((bool)user_data);
  }
  return true;
}



/* Mark all open documents as needing thier settings updated. */
bool TopWindow::PrefsCB(FXint index, DocTab*tab, FXWindow*page, void*user_data)
{
  ((SciDoc*)page->getFirst())->NeedStyled(true);
  return true;
}



void TopWindow::SetSciDocPrefs(SciDoc*sci, Settings*prefs)
{
  sci->setFont(prefs->fontdesc.face, prefs->fontdesc.size / 10);
  sci->CaretLineBG(prefs->caretlineStyle()->bg);
  sci->RightMarginBG(prefs->rightmarginStyle()->bg);
  sci->CaretWidth(prefs->CaretWidth);
  sci->SmartHome(prefs->SmartHome);
  sci->SmoothScroll(prefs->SmoothScroll);
  sci->TabWidth(prefs->TabWidth);
  sci->UseTabs(prefs->UseTabs);
  sci->WhiteSpaceBG(prefs->whitespaceStyle()->bg);
  sci->WhiteSpaceFG(prefs->whitespaceStyle()->fg);
  sci->SetEdgeColumn(prefs->RightEdgeColumn);
  sci->SetShowIndent(prefs->ShowIndentGuides);

  sci->CaretFG(prefs->caretStyle()->bg);
  sci->SelectionBG(prefs->selectionStyle()->bg);
  if (prefs->ShowWhiteSpace) { sci->ShowWhiteSpace(true); }
  if (prefs->ShowLineNumbers) { sci->ShowLineNumbers(true); }

  sci->sendMessage(SCI_SETVIRTUALSPACEOPTIONS,
    (SCVS_RECTANGULARSELECTION|(prefs->CaretPastEOL?SCVS_USERACCESSIBLE:0)), 0);

}



/*
  For each tab callback to add a chore to update the settings of the first
  document it finds that needs updating -
  CheckStyle() invokes this callback, and in turn, this callback adds a chore
  that invokes CheckStyle(). This exchange continues until all documents have
  had their settings updated.
*/
bool TopWindow::StyleNextDocCB(FXint index, DocTab*tab, FXWindow*page, void*user_data)
{
  SciDoc*sci=(SciDoc*)page->getFirst();
  if (sci->NeedStyled()) {
    TopWindow*tw=(TopWindow*)user_data;
    tw->getApp()->addChore(tw, ID_CHECK_STYLE, sci);
    return false;
  }
  return true;
}


/*
  This chore updates a document's settings, and adds a chore to do the same
  for the next document that needs updating. (See StyleNextDocCB() for more.)
*/
long TopWindow::CheckStyle(FXObject*o, FXSelector sel, void*p)
{
  SciDoc*sci=(SciDoc*)p;
  if (sci && sci->NeedStyled()) {
    SetSciDocPrefs(sci, prefs);
    sci->UpdateStyle();
    sci->NeedStyled(false);
  }
  tabbook->ForEachTab(StyleNextDocCB,this,false);
  return 1;
}




/*
  ForEachTab callback that checks to see if a file is already open
  If we find the document, then we activate that tab and set the
  user_data string to empty as a sign that we found it.
*/
bool TopWindow::FileAlreadyOpenCB(FXint index, DocTab*tab, FXWindow*page, void*user_data)
{
  FXString*fn=(FXString*)user_data;
  SciDoc*sci=(SciDoc*)page->getFirst();
  if (strcmp(sci->Filename().text(),fn->text())==0) {
    DocTabs*tabbook=(DocTabs*)tab->getParent();
    tabbook->ActivateTab(index);
    fn->clear();
    return false;
  }
  return true;
}



// Set or unset the '*' or '#' prefix of a tab's title...
void TopWindow::SetTabTag(SciDoc*sci, char mark, bool set)
{
  FXTabItem*tab=(FXTabItem*)sci->getParent()->getPrev();
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



bool TopWindow::CheckKillCommand(FXWindow*w, FXAccelTable*tbl)
{
  if (tbl) { return true; }
  FXMessageBox::error(w, MBOX_OK, _("Command support disabled"),
    _("Support for running macros and external commands has been\n"
      "disabled, because the interrupt key sequence is invalid.\n\n"
      "To fix this, go to:\n"
      "  Edit->Preferences->Keybindings\n"
      "and enter a valid setting for \"%s\""),
    MenuMgr::LookupMenu(TopWindow::ID_KILL_COMMAND)->pref
  );
  return false;
}



/*
  ForEachTab callback that checks to see if a file is still open
  If we find the document, then we set the SciDoc** pointed to
  by the user_data to NULL as a sign that we found it.
*/
bool TopWindow::FileStillOpenCB(FXint index, DocTab*tab, FXWindow*page, void*user_data)
{
  SciDoc**sci=(SciDoc**)user_data;
  if (*sci==page->getFirst()) {
    *sci=NULL;
    return false;
  }
  return true;
}



/* Return true if the document is still open */
bool TopWindow::IsDocValid(SciDoc*sci)
{
  if (sci) {
    SciDoc*closed=sci;
    tabbook->ForEachTab(FileStillOpenCB,&closed);
    return (!closed);
  } else {
    return false;
  }
}



// In case a Lua script left the SCI_*UNDOACTION level in an unbalanced state.
bool TopWindow::ResetUndoLevelCB(FXint index, DocTab*tab, FXWindow*page, void*user_data)
{
  ((SciDoc*)(page->getFirst()))->SetUserUndoLevel(0);
  return true;
}



// Exposes "userland" search behavior to scripting engine.
bool TopWindow::FindText(const char*searchstring, FXuint searchmode, bool forward)
{
  return srchdlgs->FindPhrase(ControlDoc(),searchstring,searchmode,forward);
}



#ifdef WIN32
# define IsDesktopCurrent(tw) (true)
#else

/*
  We don't want to pop up the notification dialog for external changes unless our application
  currently has the focus, and that's easy enough to do with the Fox API - But - we also don't
  want to pop up the dialog unless our application is on the current virtual desktop, and the
  only way I can see to do that is via the X11 API. I guess this stuff is fairly expensive,
  so we wait to call it until we are sure the dialog would otherwise need to be displayed.
*/
# include <X11/Xlib.h>
# include <X11/Xatom.h>

static int GetXIntProp(FXWindow *win, Atom prop)
{
  int num = 0;
  Atom type_ret;
  int format_ret;
  unsigned long items_ret;
  unsigned long after_ret;
  unsigned char *prop_ret=NULL;
  XGetWindowProperty( (Display*)win->getApp()->getDisplay(), win->id(), prop, 0, 0x7fffffff, False,
                        XA_CARDINAL, &type_ret, &format_ret, &items_ret, &after_ret, &prop_ret);
  if (prop_ret) {
    num = *((int*)prop_ret);
    XFree(prop_ret);
  }
  return num;
}



static bool IsDesktopCurrent(TopWindow*tw)
{
  static bool failed=false;
  static Atom NET_WM_DESKTOP=None;
  static Atom NET_CURRENT_DESKTOP=None;
  if (!failed) {
    if (NET_WM_DESKTOP==None) {
      NET_WM_DESKTOP=XInternAtom((Display*)tw->getApp()->getDisplay(), "_NET_WM_DESKTOP",true);
    }
    if (NET_CURRENT_DESKTOP==None) {
      NET_CURRENT_DESKTOP=XInternAtom((Display*)tw->getApp()->getDisplay(), "_NET_CURRENT_DESKTOP",true);
    }
    if ((NET_CURRENT_DESKTOP==None)||(NET_WM_DESKTOP==None)) {
      failed=true;
    } else {
      int this_desk=GetXIntProp(tw,NET_WM_DESKTOP);
      int that_desk=GetXIntProp(tw->getApp()->getRootWindow(),NET_CURRENT_DESKTOP);
      bool rv=(this_desk==that_desk);
      return rv;
    }
  }
  return true;
}
#endif



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
        if ( FXMessageBox::question(this, MBOX_YES_NO, _("File status error"),
             "%s:\n%s\n(%s)\n\n%s",
             _("Error checking the status of"),
             sci->Filename().text(), sci->GetLastError().text(),
             _("Save to disk now?")
             )==MBOX_CLICKED_YES )
        {
          if (!filedlgs->SaveFile(sci,sci->Filename())) {
            onFileSaveAs(NULL,0,NULL);
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



#ifdef WIN32
// Windows doesn't have this?
char*strndup(const char*src,int len)
{
  char*dst=(char*)calloc(len+1,1);
  strncpy(dst,src,len);
  return dst;
}
#endif



UserMenu**TopWindow::UserMenus() const
{
  static UserMenu* menus[]={usercmdmenu, userfiltermenu, usersnipmenu, usermacromenu,NULL};
  return menus;
}



// Prints the names of the compiled-in lexers to stdout,
// along with the number of word lists per lexer
// ( Only used during development, to help with the
//   syntax-highlighting interface. )

class WordList;
class Accessor;
#include <cctype>
#include <KeyWords.h>

void TopWindow::DumpLexers() {
  for (int i=0; i<=SCLEX_AUTOMATIC; i++) {
    const LexerModule*lex=LexerModule::Find(i);
    if (lex) {
      fprintf(stdout, "%d %s %d\n", i, lex->languageName, lex->GetNumWordLists());
    }
  }
}

