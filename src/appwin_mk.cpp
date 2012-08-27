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
#include "appname.h"
#include "appmain.h"
#include "scidoc.h"
#include "filer.h"
#include "doctabs.h"
#include "prefs.h"
#include "backup.h"
#include "menuspec.h"
#include "toolbar.h"
#include "outpane.h"
#include "statusbar.h"
#include "mainmenu.h"
#include "scidoc_util.h"
#include "foreachtab.h"

#include "intl.h"
#include "appwin.h"



TopWindow::TopWindow(FXApp *a):TopWindowBase(a)
{
  hsplit->setSelector(ID_SPLIT_CHANGED);
  tabbook->setSelector(ID_TAB_SWITCHED);
  statusbar->SetKillID(ID_KILL_COMMAND);
  filedlgs->setSelector(ID_FILE_SAVED);
  SciDocUtils::SetScintillaSelector(ID_SCINTILLA);
  SciDocUtils::SetMacroRecordSelector(ID_MACRO_RECORD);
  ShowOutputPane(prefs->ShowOutputPane);
  ShowStatusBar(prefs->ShowStatusBar);
}



void TopWindow::ShowLineNumbers(bool showit)
{
  prefs->ShowLineNumbers=showit;
  tabbook->ForEachTab(TabCallbacks::LineNumsCB, (void*)(FXival)showit);
  menubar->SetCheck(ID_SHOW_LINENUMS,showit);
}



void TopWindow::ShowStatusBar(bool showit)
{
  prefs->ShowStatusBar=showit;
  statusbar->Show(showit);
  menubar->SetCheck(ID_SHOW_STATUSBAR,showit);
}



void TopWindow::ShowOutputPane(bool showit)
{
  prefs->ShowOutputPane=showit;
  if (showit) {
    if (prefs->OutputPaneHeight<16) { prefs->OutputPaneHeight=16; }
    hsplit->setSplit(1, prefs->OutputPaneHeight);
    outlist->show();
  } else {
    outlist->hide();
    hsplit->setSplit(1,0);
  }
  menubar->SetCheck(ID_SHOW_OUTLIST,showit);
}



void TopWindow::ShowWhiteSpace(bool showit)
{
  prefs->ShowWhiteSpace=showit;
  tabbook->ForEachTab(TabCallbacks::WhiteSpaceCB, (void*)(FXival)showit);
  menubar->SetCheck(ID_SHOW_WHITESPACE,prefs->ShowWhiteSpace);
}



void TopWindow::ShowToolbar(bool showit)
{
  prefs->ShowToolbar=showit;
  if (showit) { toolbar->show(); } else { toolbar->hide(); }
  menubar->SetCheck(ID_SHOW_TOOLBAR,showit);
}



void TopWindow::ShowMargin(bool showit)
{
  prefs->ShowRightEdge = showit;
  tabbook->ForEachTab(TabCallbacks::ShowMarginCB, (void*)(FXival)showit);
  menubar->SetCheck(ID_SHOW_MARGIN, showit);
}



void TopWindow::ShowIndent(bool showit)
{
  prefs->ShowIndentGuides = showit;
  tabbook->ForEachTab(TabCallbacks::ShowIndentCB, (void*)(FXival)showit);
  menubar->SetCheck(ID_SHOW_INDENT,showit);
}



void TopWindow::ShowCaretLine(bool showit)
{
  prefs->ShowCaretLine = showit;
  tabbook->ForEachTab(TabCallbacks::ShowCaretLineCB, (void*)prefs);
  menubar->SetCheck(ID_SHOW_CARET_LINE,showit);
}



void TopWindow::SetWordWrap(SciDoc*sci, bool wrapped)
{
  sci->SetWordWrap(wrapped);
  menubar->SetCheck(ID_WORDWRAP,wrapped);
}



void TopWindow::InvertColors(bool inverted)
{
  prefs->InvertColors=inverted;
  toolbar->SetToolbarColors();
  tabbook->ForEachTab(TabCallbacks::PrefsCB,NULL);
  CheckStyle(NULL,0,ControlDoc());
  menubar->SetCheck(ID_INVERT_COLORS,prefs->InvertColors); 
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
      prefs->DocTabPosition='T';
      break;
    case ID_TABS_BOTTOM:
      prefs->DocTabPosition='B';
      break;
    case ID_TABS_LEFT:
      prefs->DocTabPosition='L';
      break;
    case ID_TABS_RIGHT:
      prefs->DocTabPosition='R';
      break;
  }
  tabbook->setTabStyleByChar(prefs->DocTabPosition);
  MenuMgr::RadioUpdate(sel, ID_TABS_TOP, ID_TABS_RIGHT);
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
    if (!filedlgs->AskSaveModifiedCommand((SciDoc*)page->getFirst(), script)) { return; }
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
        SciDocUtils::InsertFile(sci,script);
      }
      break;
    }
    case ID_USER_MACRO: {
      RunMacro(script, true);
      break;
    }
  }  
}



void TopWindow::UpdateTitle(long line, long col)
{
  SciDoc*sci=ControlDoc();
  if (sci) {
    DocTab *tab=tabbook->ActiveTab();
    FXString s;
    s.format("%s  %s - %s", tab->getText().text(), FXPath::directory(sci->Filename()).text(), EXE_NAME);
    setTitle(s);
    menubar->SetLanguageCheckmark(sci->getLanguage());
    menubar->SetReadOnly(sci->sendMessage(SCI_GETREADONLY,0,0));
    menubar->SetCheck(ID_WORDWRAP,sci->GetWordWrap());
    statusbar->FileInfo(sci->Filename(),sci->GetUTF8(),line,col);
    MenuMgr::UpdateEolMenu(sci);
  } else {
    setTitle(EXE_NAME);
    statusbar->Clear();
  }
}

