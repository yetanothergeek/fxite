/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2009 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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


#include <cerrno>
#include <fx.h>
#include <fxkeys.h>

#include <Scintilla.h>
#include <FXScintilla.h>

#include "compat.h"
#include "appmain.h"
#include "scidoc.h"
#include "filer.h"
#include "search.h"
#include "doctabs.h"
#include "prefs.h"
#include "prefdlg.h"
#include "lang.h"
#include "runcmd.h"
#include "shmenu.h"
#include "toolmgr.h"
#include "macro.h"
#include "recorder.h"
#include "backup.h"
#include "histbox.h"
#include "menuspec.h"


#include "intl.h"
#include "appwin.h"


#define PACK_UNIFORM ( PACK_UNIFORM_WIDTH | PACK_UNIFORM_HEIGHT )


static bool topwin_closing = false;

bool TopWindow::Closing() { return topwin_closing; }


TopWindow::TopWindow(FXApp *a):FXMainWindow(a,EXE_NAME,NULL,NULL,DECOR_ALL,0,0,600,400)
{
  ((AppClass*)a)->MainWin(this);
  StyleDef*sd=GetStyleFromId(Settings::globalStyle(), STYLE_CALLTIP);
  RgbToHex(getApp()->getTipbackColor(), sd->bg);
  RgbToHex(getApp()->getTipforeColor(), sd->fg);
  prefs=new Settings(this);

  need_status=0;
  SciDoc::DefaultStyles(prefs->Styles());
  CreateMenus();
  new FXHorizontalSeparator(this,LAYOUT_SIDE_TOP|LAYOUT_FILL_X|SEPARATOR_GROOVE);

  statusbar=new FXHorizontalFrame(this,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|FRAME_RAISED, 0,0,0,0, 3,3,3,3, 7,3);


  vbox=new FXVerticalFrame(this,FRAME_NONE|LAYOUT_FILL,0,0,0,0,4,4,4,4);
  CreateToolbar();


  hsplit=new FXSplitter(vbox,this, ID_SPLIT_CHANGED, SPLITTER_VERTICAL|SPLITTER_REVERSED|LAYOUT_FILL|SPLITTER_TRACKING);
  tabbook=new DocTabs(hsplit,this,ID_TAB_SWITCHED,FRAME_NONE|PACK_UNIFORM|LAYOUT_FILL);
  switch (prefs->DocTabPosition) {
    case 'B': { tabbook->setTabStyle(TABBOOK_BOTTOMTABS); break; }
    case 'L': { tabbook->setTabStyle(TABBOOK_LEFTTABS); break; }
    case 'R': { tabbook->setTabStyle(TABBOOK_RIGHTTABS); break; }
  }
  tabbook->setTabsCompact(prefs->DocTabsPacked);

  outputpane=new FXGroupBox(hsplit,"",LAYOUT_SIDE_TOP|LAYOUT_FILL_X|FRAME_SUNKEN|FRAME_THICK,0,0,0,0,0,0,0,0);
  outlist=new FXList(outputpane,this,ID_OUTLIST_CLICK,LAYOUT_SIDE_TOP|LAYOUT_FILL);
  hsplit->setSplit(1,prefs->OutputPaneHeight);
  ShowOutputPane(prefs->ShowOutputPane);

  FXColor clr=statusbar->getBaseColor();

  coords=new FXTextField(statusbar,12,NULL,FRAME_RAISED|FRAME_SUNKEN|TEXTFIELD_READONLY);
  coords->setShadowColor(clr);
  coords->setBackColor(clr);
  coords->setEditable(false);

  docname=new FXTextField(statusbar, 64, NULL, FRAME_RAISED|FRAME_SUNKEN|TEXTFIELD_READONLY);
  docname->setShadowColor(clr);
  docname->setBackColor(clr);
  docname->setEditable(false);

  encname=new FXTextField(statusbar, 6, NULL, FRAME_RAISED|FRAME_SUNKEN|TEXTFIELD_READONLY);
  encname->setShadowColor(clr);
  encname->setBackColor(clr);
  encname->setEditable(false);


  general_info=new FXLabel(statusbar, FXString::null, NULL,JUSTIFY_LEFT|LAYOUT_FIX_Y);
  general_info->setUserData((void*)DontFreezeMe());
  general_info->setBackColor(HexToRGB("#FFFFCC"));
  general_info->setTextColor(HexToRGB("#FF0000"));
  general_info->hide();

  srchdlgs=new SearchDialogs(this);
  srchdlgs->searchstring="";
  SetSrchDlgsPrefs();

  filedlgs=new FileDialogs(this,ID_FILE_SAVED);
  filedlgs->patterns(prefs->FileFilters);

  backups=new BackupMgr();

  outpop=new FXMenuPane(outlist);
  new FXMenuCommand(outpop,_("Select &All"),NULL,this,ID_OUTLIST_ASEL);
  new FXMenuCommand(outpop,_("&Copy"),NULL,this,ID_OUTLIST_COPY);
  macros=NULL;
  recorder=NULL;
  prefdlg=NULL;
  tooldlg=NULL;
  clipsci=NULL;
  recording=NULL;
  skipfocus=false;
  destroying=false;
  close_all_confirmed=false;
  kill_commands_confirmed=false;
  topwin_closing=false;
  MenuSpec*killcmd=MenuMgr::LookupMenu(ID_KILL_COMMAND);
  killkey=parseAccel(killcmd->accel);
  if (killkey && FXSELID(killkey)) {
    temp_accels=new FXAccelTable();
    temp_accels->addAccel(killkey,this,FXSEL(SEL_COMMAND,ID_KILL_COMMAND),0);
   } else {
    FXMessageBox::warning(getApp(), MBOX_OK, _("Configuration error"),
      "%s \"%s\"\n%s",
      _("Failed to parse accelerator for"),
      killcmd->pref,
      _("disabling support for macros and external commands.")
      );
    temp_accels=NULL;
  }
  saved_accels=NULL;
  ShowStatusBar(prefs->ShowStatusBar);
  ClipTextField::SetSaver(this);
}



void TopWindow::SetKillCommandAccelKey(FXHotKey acckey)
{
  killkey=acckey;
  if (temp_accels) { delete temp_accels; }
  temp_accels=new FXAccelTable();
  temp_accels->addAccel(acckey,this,FXSEL(SEL_COMMAND,ID_KILL_COMMAND),0);
}



TopWindow::~TopWindow()
{
  destroying=true;
  DeleteMenus();
  delete outpop;
  delete macros;
  delete filedlgs;
  delete backups;
  if (temp_accels) { delete temp_accels; }
  if (saved_accels)  { delete saved_accels; }
  if (recorder) { delete recorder; }
  if (prefdlg) { delete prefdlg; }
  if (tooldlg) { delete tooldlg; }
  delete getIcon();
  delete getMiniIcon();
  delete toolbar_font;
}



void TopWindow::SetSrchDlgsPrefs()
{
  srchdlgs->searchmode=prefs->SearchOptions;
  srchdlgs->defaultsearchmode=prefs->SearchOptions;
  srchdlgs->searchwrap=(SearchWrapPolicy)prefs->SearchWrap;
  srchdlgs->searchverbose=prefs->SearchVerbose;
}



void TopWindow::SaveClipboard()
{
  if (clipsci) {
    FXuchar* cb_data=NULL;
    FXuint len=0;
    if (getDNDData(FROM_CLIPBOARD, FXWindow::textType, cb_data, len) && cb_data && *cb_data) {
      clipsci->sendString(SCI_COPYTEXT,len,cb_data);
    }
  }
}



// Create a new tab and editor panel
bool TopWindow::OpenFile(const char*filename, const char*rowcol, bool readonly, bool hooked)
{
  if (topwin_closing) return false;
  FXString fn="";
  SciDoc*sci=NULL;
  if ( tabbook->Count() >= prefs->MaxFiles ) { return false; }

  if (filename) {
    fn=FXPath::simplify(FXPath::absolute(filename));
    tabbook->ForEachTab(FileAlreadyOpenCB,&fn);
    if (fn.empty()) {
      if (rowcol) {
        sci=ControlDoc();
        sci->GoToStringCoords(rowcol);
      }
      if (hooked) { RunHookScript("opened"); }
      return true;
    }
  }

  if ((!fn.empty())&&(!FXStat::exists(fn))) {
    if (FXMessageBox::question( this, MBOX_YES_NO, _("File not found"),
          "%s:\n%s\n %s",
          _("Can't find the file"),
          fn.text(),
          _("Would you like to create it?")
     )==MBOX_CLICKED_YES) {
       if (!FXStat::exists(fn)) { /* <-maybe someone created it while we were waiting for a response? */
         FXFile fh(fn, FXFile::Writing);
         if (!(fh.isOpen() && fh.close())) {
           FXMessageBox::error(this, MBOX_OK, _("File error"),
            "%s:\n%s\n%s",
             _("Failed to create the file"),
             fn.text(),
             strerror(errno));
           return false;
         }
       }
    } else { return false; }
  }

  DocTab*tab=tabbook->NewTab(fn.empty()?_("Untitled"):FXPath::name(fn));
  sci=new SciDoc((FXComposite*)tab->getNext(),this,ID_SCINTILLA);

  sci->sendMessage(SCI_SETMULTIPLESELECTION,false,0);
  sci->sendMessage(SCI_SETADDITIONALSELECTIONTYPING,true,0);

  sci->ShowLineNumbers(prefs->ShowLineNumbers);
  sci->ShowWhiteSpace(prefs->ShowWhiteSpace);
  sci->SetShowEdge(prefs->ShowRightEdge);
  SetSciDocPrefs(sci,prefs);

  sci->DoStaleTest(true);

  if (!fn.empty()) {
    if (!sci->LoadFromFile(fn.text())) {
      if (!sci->GetLastError().contains(SciDoc::BinaryFileMessage())) {
         FXMessageBox::error(this, MBOX_OK, _("Error opening file"), "%s:\n%s\n%s",
             _("Could not open file"),  fn.text(),  sci->GetLastError().text());
      }
      delete sci;
      delete tab->getNext();
      delete tab;
      return false;
    }
    if (FXPath::extension(fn)=="h") { // Is header C or C++ ?
      FXString fnbase=FXPath::stripExtension(fn);
      // Check for matching source file and set language accordingly if found...
      if (FXStat::exists(fnbase+".c")) {
        sci->setLanguage("c");
      } else if (FXStat::exists(fnbase+".cpp")||FXStat::exists(fnbase+".cxx")||FXStat::exists(fnbase+".cc")) {
        sci->setLanguage("cpp");
      } else {
        // Take a wild guess - if the file contains the word "class" it's probably  C++
        const char *content=(const char*)(sci->sendMessage(SCI_GETCHARACTERPOINTER,0,0));
        if (FXRex("\\<class\\>").match(content)) {
          sci->setLanguage("cpp");
        } else {
          sci->setLanguage("c");
        }
      }
    } else {
      if (!sci->setLanguageFromFileName(FXPath::name(fn).text())) {
        sci->setLanguageFromContent();
      }
    }
  } else {
    sci->SetUTF8(!prefs->DefaultToAscii);
    sci->UpdateStyle();
  }

  if (!clipsci) { clipsci=sci; }
  SetTabLocked(sci,readonly);
  sci->setFocus();
  tabbook->ActivateTab(tabbook->Count()-1);
  FXMenuCommand*cmd=new FXMenuCommand(doclistmenu, sci->Filename().empty()?tab->getText():sci->Filename(),
                              NULL,this,ID_TAB_ACTIVATE, 0);

  cmd->setUserData(tab);
  tab->setUserData(cmd);
  if (shown()) {
    sci->create();
    cmd->create();
    if (rowcol) { sci->GoToStringCoords(rowcol); }
  }
  recent_files->remove(sci->Filename());

  //  If the only thing we had open prior to this file was a single, 
  //  empty, untitled, unmodified document, then close it...
  if ((tabbook->Count()==2) && (!sci->Filename().empty())) {
    SciDoc*sc0=(SciDoc*)(tabbook->PageAt(0)->getFirst());
    if ( sc0->Filename().empty() && (sc0->GetTextLength()==0) && (sc0->Dirty()==false)  ) {
      tabbook->ActivateTab(0);
      CloseFile(false,false);
    }
  }

  if (hooked) { RunHookScript("opened"); }
  return true;
}



bool TopWindow::NewFile(bool hooked)
{
  return OpenFile(NULL,NULL,false,hooked);
}



bool TopWindow::SaveAll(bool break_on_fail)
{
  DocTab*curr=tabbook->ActiveTab();
  FXWindow*tab,*page;
  bool success=true;
  for (tab=tabbook->getFirst(); tab && (page=tab->getNext()); tab=page->getNext()) {
    SciDoc*sci=(SciDoc*)page->getFirst();
    if (sci->Dirty()) {
      if (sci->Filename().empty() ) {
        tabbook->ActivateTab((DocTab*)tab);
      }
      if (!filedlgs->SaveFile(sci,sci->Filename())) {
        if (break_on_fail) { return false; } else { success=false; }
      }
    }
  }
  if (curr!=tabbook->ActiveTab()) { tabbook->ActivateTab((curr)); }
  return success;
}



/*
  This is a bit messy, but...
  If a FOX widget owns the clipboard, the data is lost when the widget is destroyed.
  So we check to see if the SciDoc we are destroying owns the clipboard, and if it
  does, we pass the data off to some other SciDoc before we destroy this one.
*/
bool TopWindow::SetGlobalClipboardCB(FXint index, DocTab*tab, FXWindow*page, void*user_data)
{
  TopWindow*tw=(TopWindow*)user_data;
  SciDoc*sci=(SciDoc*)(page->getFirst());
  if (sci!=tw->clipsci) {
    tw->clipsci=sci;
    return false;
  }
  return true;
}



void TopWindow::DoneSci(SciDoc*sci)
{
  if (clipsci==sci) {
    tabbook->ForEachTab(SetGlobalClipboardCB,this);
    if (clipsci==sci) { clipsci=NULL; }
  }
  if (recording==sci) {
    onMacroRecord(NULL,0,NULL);
  }
  if (sci->hasClipboard()) { SaveClipboard(); }
  FXWindow*page=sci->getParent();
  FXWindow*tab=page->getPrev();
  delete sci;
  delete (FXMenuCommand*)tab->getUserData();
  delete tab;
  delete page;
}



bool TopWindow::CloseFile(bool close_last, bool hooked)
{
  FXint i=tabbook->getCurrent();
  FXTabItem*tab=tabbook->ActiveTab();
  SciDoc*sci=ControlDoc();
  if (!filedlgs->TryClose(sci,tab->getText().text())) { return false; }
  recent_files->prepend(sci->Filename());
  if (hooked) { RunHookScript("closing"); }
  if (close_last) {
    if (!sci->Filename().empty()) {
      FXString line;
      line.format("-%c\n+%ld,%ld\n%s\n", sci->GetReadOnly()?'r':'w',
        sci->GetLineNumber()+1, sci->GetColumnNumber(), sci->Filename().text());
      session_data.append(line);
    }
  }
  backups->RemoveBackup(sci);
  if (tabbook->numChildren()==2) {
    if (!close_last) { NewFile(false); }
    DoneSci(sci);
  } else {
    DoneSci(sci);
    FXWindow *w=tabbook->childAtIndex(i*2);
    if (!w) {
      i--;
      w=tabbook->childAtIndex(i*2);
    }
    tabbook->ActivateTab(i);
  }
  need_status=1;
  return true;
}



bool TopWindow::RunHookScript(const char*hookname)
{
  static bool running_hook_script=false;

  FXString hook;
  if (running_hook_script) { return false; }
  hook.format("%s%s%c%s%c%s.lua", GetApp()->ConfigDir().text(), "tools", PATHSEP, "hooks", PATHSEP, hookname);
  if (FXStat::isFile(hook)) {
   bool rv;
   running_hook_script=true;
   rv=RunMacro(hook, true);
   running_hook_script=false;
   return rv;
  } else {
    return true;
  }
}



bool TopWindow::CloseAll(bool close_last)
{
  bool rv;
  do {
    tabbook->ActivateTab(0);
  } while ((tabbook->numChildren()>0) && CloseFile(true,true));
  rv=tabbook->numChildren()==0;
  if (!close_last) { NewFile(false); }
  return rv;
}



void TopWindow::AddFileToTagsMenu(const FXString &filename)
{
  FXMenuPane*pane=(FXMenuPane*)unloadtagsmenu->getMenu();
  FXWindow*w;
  FXString fn=FXPath::simplify(FXPath::absolute(filename));
  for ( w=pane->getFirst(); w; w=w->getNext() ) {
    if (strcmp(fn.text(),((FXMenuCaption*)w)->getText().text())==0) {
      return;
    }
  }
  unloadtagsmenu->enable();
  SetMenuEnabled(findtagmenu,true);
  SetMenuEnabled(showtipmenu,true);
  FXMenuCommand*mc=new FXMenuCommand(pane,"",NULL,this,ID_UNLOAD_TAGS);
  mc->create();
  mc->setText(fn);
}



bool TopWindow::RemoveFileFromTagsMenu(const FXString &filename)
{
  FXMenuPane*pane=(FXMenuPane*)unloadtagsmenu->getMenu();
  if (filename.empty()) { /* remove all tag files */
    while (pane->numChildren()>0) {
      onUnloadTags(pane->getFirst(),0,NULL);
    }
    return true;
  } else {
    FXWindow*w;
    FXString fn=FXPath::simplify(FXPath::absolute(filename));
    for ( w=pane->getFirst(); w; w=w->getNext() ) {
      if (strcmp(fn.text(),((FXMenuCaption*)w)->getText().text())==0) {
        onUnloadTags(w,0,NULL);
        return true; /* we found it */
      }
    }
    return false; /* we did not find the filename in the menu */
  }
}



bool TopWindow::SetLanguage(const FXString &name)
{
  for (FXWindow*grp=langmenu->getFirst(); grp; grp=grp->getNext()) {
    for (FXWindow*mnu=((FXMenuCascade*)grp)->getMenu()->getFirst(); mnu; mnu=mnu->getNext()) {
      if (compare(((FXMenuCommand*)mnu)->getText(),name)==0) {
        onSetLanguage(mnu,0,NULL);
        return true;
      }
    }
  }
  return false;
}



void TopWindow::ShowLineNumbers(bool showit)
{
  if (linenums_chk->getCheck()!=showit) {
    linenums_chk->setCheck(showit);
    onShowLineNums(linenums_chk,FXSEL(SEL_COMMAND,ID_SHOW_LINENUMS),(void*)showit);
  }
}



bool TopWindow::ShowLineNumbers()
{
  return prefs->ShowLineNumbers;
}



void TopWindow::ShowStatusBar(bool showit)
{

  prefs->ShowStatusBar=showit;
  status_chk->setCheck(showit);
  SyncToggleBtn(status_chk,FXSEL(SEL_COMMAND,ID_SHOW_STATUSBAR));

  if (showit) {
    statusbar->show();
  } else {
    statusbar->hide();
  }
  // Layout doesn't work right without this...
  if (prefs->ShowOutputPane) {
    ShowOutputPane(false);
    ShowOutputPane(true);
  } else {
    ShowOutputPane(true);
    ShowOutputPane(false);
  }
}



bool TopWindow::ShowStatusBar()
{
  return prefs->ShowStatusBar;
}



void TopWindow::ShowOutputPane(bool showit)
{
  prefs->ShowOutputPane=showit;
  outpane_chk->setCheck(showit);
  SyncToggleBtn(outpane_chk,FXSEL(SEL_COMMAND,ID_SHOW_OUTLIST));
  if (showit) {
    if (prefs->OutputPaneHeight<16) {prefs->OutputPaneHeight=16; }
    hsplit->setSplit(1, prefs->OutputPaneHeight);
    outputpane->show();
    outlist->show();
    } else {
    outlist->hide();
    outputpane->hide();
    hsplit->setSplit(1,0);
  }
}



bool TopWindow::ShowOutputPane()
{
  return prefs->ShowOutputPane;
}



void TopWindow::ShowWhiteSpace(bool showit)
{
  if (white_chk->getCheck()!=showit) {
    white_chk->setCheck(showit);
    onShowWhiteSpace(white_chk,FXSEL(SEL_COMMAND,ID_SHOW_WHITESPACE),(void*)showit);
  }
}



bool TopWindow::ShowWhiteSpace()
{
  return prefs->ShowWhiteSpace;
}



void TopWindow::ShowToolbar(bool showit)
{
  if (toolbar_chk->getCheck()!=showit) {
    toolbar_chk->setCheck(showit);
    onShowToolbar(toolbar_chk,FXSEL(SEL_COMMAND,ID_SHOW_TOOLBAR),(void*)showit);
  }
}

bool TopWindow::ShowToolbar()
{
  return prefs->ShowToolbar;
}



void TopWindow::UpdateEolMenu(SciDoc*sci)
{
  switch (sci->sendMessage(SCI_GETEOLMODE,0,0)) {
    case SC_EOL_CRLF: {
      RadioUpdate(ID_FMT_DOS,ID_FMT_DOS,ID_FMT_UNIX);
      break;
    }
    case SC_EOL_CR: {
      RadioUpdate(ID_FMT_MAC,ID_FMT_DOS,ID_FMT_UNIX);
      break;
    }
    case SC_EOL_LF: {
      RadioUpdate(ID_FMT_UNIX,ID_FMT_DOS,ID_FMT_UNIX);
      break;
    }
  }
}



void TopWindow::UpdateTitle(long line, long col)
{
  SciDoc*sci=ControlDoc();
  if (sci) {
    FXTabItem *tab=tabbook->ActiveTab();
    LangStyle*ls=NULL;
    FXString s;
    s.format("%s  %s - %s", tab->getText().text(), FXPath::directory(sci->Filename()).text(), EXE_NAME);
    setTitle(s);
    ls=sci->getLanguage();
    for (FXWindow*wmc=langmenu->getFirst(); wmc; wmc=wmc->getNext()) {
      FXPopup*pu=((FXMenuCascade*)wmc)->getMenu();
      for (FXWindow*wmr=pu->getFirst(); wmr; wmr=wmr->getNext()) {
        FXMenuRadio*mr=(FXMenuRadio*)wmr;
        mr->setCheck(mr->getUserData()==ls);
      }
    }
    if (sci->sendMessage(SCI_GETREADONLY,0,0)) {
      readonlymenu->setCheck(true);
      fileformatcasc->disable();
    } else {
      readonlymenu->setCheck(false);
      fileformatcasc->enable();
    }
    SyncToggleBtn(readonlymenu,FXSEL(SEL_COMMAND,ID_READONLY));
    SetMenuEnabled(fmt_dos_mnu,!readonlymenu->getCheck());
    SetMenuEnabled(fmt_mac_mnu,!readonlymenu->getCheck());
    SetMenuEnabled(fmt_unx_mnu,!readonlymenu->getCheck());
    docname->setText(sci->Filename().empty()?"":sci->Filename().text());
    encname->setText(sci->GetUTF8()?"UTF-8":"ASCII");
    char rowcol[16];
    memset(rowcol,0,sizeof(rowcol));
    snprintf(rowcol,sizeof(rowcol)-1," %ld:%ld",line+1,col);
    coords->setText(rowcol);
    UpdateEolMenu(sci);
  } else {
    setTitle(EXE_NAME);
    docname->setText("");
    coords->setText("");
  }
}



void TopWindow::DisableUI(bool disabled)
{
  if (destroying) { return; }
  if (disabled) {
    saved_accels=getAccelTable();
    setAccelTable(temp_accels);
    temp_accels=NULL;
  } else {
    temp_accels=getAccelTable();
    setAccelTable(saved_accels);
    saved_accels=NULL;
  }
  Freeze(this,disabled);
}




void TopWindow::SetInfo(const char*msg, bool hide_docname)
{
  if (hide_docname) {
    docname->setNumColumns(24);
  } else {
    docname->setNumColumns(64);
  }
  general_info->setText(msg);
  if (msg&&*msg) {
    general_info->show();
  } else {
    general_info->hide();
  }
}



static void SetShellEnv(const char*file, long line)
{
  char linenum[8]="\0\0\0\0\0\0\0";
  snprintf(linenum,sizeof(linenum)-1, "%ld", line);
  FXSystem::setEnvironment("l",linenum);
  FXSystem::setEnvironment("f",file); 
}



bool TopWindow::FilterSelection(SciDoc *sci, const FXString &cmd, const FXString &input)
{
  if (!CheckKillCommand(this,temp_accels)) { return false; }
  SetShellEnv(sci->Filename().text(),sci->GetLineNumber());
  bool rv=false;
  if (!cmd.empty()) {
    CmdIO cmdio(this, prefs->ShellCommand.text());
    FXString output="";
    command_timeout=false;
    getApp()->beginWaitCursor();
    FXString status;
    status.format(_("Running command (press %s to cancel)"), MenuMgr::LookupMenu(ID_KILL_COMMAND)->accel);
    SetInfo(status.text(), true);
    DisableUI(true);
    if (cmdio.filter(cmd.text(), input, output, &command_timeout)) {
      sci->sendString(SCI_REPLACESEL, 0, output.text());
      rv=true;
    }
    DisableUI(false);
    SetInfo("");
    getApp()->endWaitCursor();
  }
  sci->setFocus();
  need_status=1;
  return rv;
}



bool TopWindow::RunCommand(SciDoc *sci, const FXString &cmd)
{
  if (!CheckKillCommand(this,temp_accels)) { return false; }
  SetShellEnv(sci->Filename().text(),sci->GetLineNumber());
  bool success=false;
  if (!cmd.empty()) {
    CmdIO cmdio(this, prefs->ShellCommand.text());
    command_timeout=false;
    outlist->clearItems(true);
    update();
    repaint();
    if (!prefs->ShowOutputPane) { ShowOutputPane(true); }
    getApp()->beginWaitCursor();
    FXString status;
    status.format(_("Running command (press %s to cancel)"), MenuMgr::LookupMenu(ID_KILL_COMMAND)->accel);
    SetInfo(status.text(), true);
    DisableUI(true);
    getApp()->runWhileEvents();
    success=cmdio.list(cmd.text(), outlist, &command_timeout);
    DisableUI(false);
    SetInfo("");
    getApp()->endWaitCursor();
    if (success) {
      outlist->appendItem(_("Command succeeded."));
      outlist->makeItemVisible(outlist->getNumItems()-1);
    } else {
      if (command_timeout) {
        outlist->appendItem(_("Command cancelled."));
        outlist->makeItemVisible(outlist->getNumItems()-1);
      } else {
        FXint i;
        outlist->appendItem(_("Command failed."));
        outlist->makeItemVisible(outlist->getNumItems()-1);
        FXRex rx(_(": [Ee]rror: "));
        for (i=0; i<outlist->getNumItems(); i++) {
          FXListItem *item=outlist->getItem(i);
          if (rx.match(item->getText())) {
            outlist->selectItem(i);
            outlist->setCurrentItem(i);
            outlist->makeItemVisible(i);
            break;
          }
        }
      }
    }
  }
  if (FocusedDoc() && (GetActiveWindow()==id())) { FocusedDoc()->setFocus(); }
  need_status=1;
  return success;
}


bool TopWindow::InsertFile(SciDoc *sci, const FXString &filename)
{
  if (sci->InsertFile(filename.text())) {
    return true;
  } else {
    FXMessageBox::error(this, MBOX_OK, _("Error opening file"), "%s:\n%s\n%s",
       _("Could not open file"), filename.text(), sci->GetLastError().text());
  }
  return false;
}


/*
  Usually, the application will catch the kill command key sequence by itself,
  but if the event queue gets really full e.g. when appending large amounts
  of data to the output window, the key event may get buried underneath
  everything else that's happening. This function "manually" checks for
  the key sequence and returns true if the user is trying to cancel.
*/
bool TopWindow::IsMacroCancelled()
{
  if (command_timeout) { return true; }
  FXApp*a=getApp();
  if (a->getKeyState(FXSELID(killkey))) {
    FXushort mods=FXSELTYPE(killkey);
    if (mods&CONTROLMASK) {
      if (!(a->getKeyState(KEY_Control_L) || a->getKeyState(KEY_Control_R))) { return false; }
    }
    if (mods&SHIFTMASK) {
      if (!(a->getKeyState(KEY_Shift_L) || a->getKeyState(KEY_Shift_R))) {  return false; }
    }
    if (mods&ALTMASK) {
      if (!(a->getKeyState(KEY_Alt_L) || a->getKeyState(KEY_Alt_R))) { return false; }
    }
    command_timeout=true;
  }
  return command_timeout;
}


bool TopWindow::RunMacro(const FXString &script, bool isfilename)
{
  if (!CheckKillCommand(this,temp_accels)) { return false; }
  if (!macros) { macros = new MacroRunner(this); }
  command_timeout=false;
  SetInfo(_("Running macro (press Ctrl+. to cancel)"), true);
  update();
  DisableUI(true);
  getApp()->runWhileEvents();
  bool rv=isfilename?macros->DoFile(script):macros->DoString(script);
  if (!destroying) {
    tabbook->ForEachTab(ResetUndoLevelCB,NULL);
    DisableUI(false);
    SetInfo("");
    if (FocusedDoc() && (GetActiveWindow()==id())) { FocusedDoc()->setFocus(); }
    need_status=1;
  }
  return rv;
}


long TopWindow::onCloseWait(FXObject*o, FXSelector sel, void*p)
{  
  if (FXSELTYPE(sel)==SEL_CHORE) {
    getApp()->addTimeout(this,ID_CLOSEWAIT, ONE_SECOND/10, NULL);
  } else {
    close();
  }
  return 1;
}



#define confirm(hdr,fmt,arg) ( FXMessageBox::question(this,MBOX_OK_CANCEL,hdr,fmt,arg) == MBOX_CLICKED_OK)

#define not_confirmed() {\
 close_all_confirmed=false; \
 kill_commands_confirmed=false; \
 topwin_closing=false; \
}

FXbool TopWindow::close(FXbool notify)
{
  if (topwin_closing) { return false; }
  if (!getApp()->getActiveWindow()) {
    hide();
    getApp()->runWhileEvents();
    if (topwin_closing) { return false; }
    show();
    getApp()->runWhileEvents();
    if (topwin_closing) { return false; }
  }
  if ((!tabbook->isEnabled()) && (!kill_commands_confirmed) ) {
    if (!confirm(_("Command in progress"), "%s", _("External command in progress - exit anyway?"))) {
      not_confirmed();
      return false;
    }
    kill_commands_confirmed=true;
    command_timeout=true;
    getApp()->addChore(this,ID_CLOSEWAIT, NULL);
    getApp()->runWhileEvents();
    return false;
  }

  if (prefs->PromptCloseMultiExit && (tabbook->Count()>1) && (!close_all_confirmed)) {
    if (!confirm(_("Multiple files"), _("Editing %d files - \nclose all tabs?"), tabbook->Count() ) ) {
      not_confirmed();
      FocusedDoc()->setFocus();
      return false;
    }
    close_all_confirmed=true;
  }

  session_data="";
  RunHookScript("shutdown");
  if (!CloseAll(true)) {
    not_confirmed();
    FocusedDoc()->setFocus();
    return false;
  }

  if (!session_data.empty()) {
     FXFile fh(GetApp()->SessionFile(), FXIO::Writing);
     if (fh.isOpen()) {
       FXival wrote=0;
       wrote=fh.writeBlock(session_data.text(),session_data.length());
       if (!(fh.close() && (wrote==session_data.length()))) {
         fxwarning(_(EXE_NAME": Could not save %s (%s)"), GetApp()->SessionFile().text(), strerror(errno));
       }
     } else {
       fxwarning(_(EXE_NAME": Could not open %s for writing (%s)"), GetApp()->SessionFile().text(), strerror(errno));
     }
     session_data="";
  }
  prefs->Maximize=isMaximized();
  if (prefs->Maximize) {
    restore();
    repaint();
    getApp()->runWhileEvents();
  }
  prefs->Left=getX();
  prefs->Top=getY();
  prefs->Width=getWidth();
  prefs->Height=getHeight();

  getApp()->runWhileEvents();
  if (topwin_closing) { return false; }
  topwin_closing=true;
  delete srchdlgs;
  delete prefs;
  delete recent_files;
  return FXMainWindow::close(notify);
}



void TopWindow::ParseCommands(const FXString &commands)
{
  FXString sect="";
  FXString rowcol="";
  FXRex rx_rowcol(":\\d+$",REX_CAPTURE);
  FXchar rw='w';
  bool tagopt=false;
  bool macopt=false;
  bool session_restored=false;
  int i=0;
  while (1) {
    sect=commands.section('\n',i++);
    if (sect.empty()) { break; }
    switch (sect.text()[0]) {
      case '+': {
        rowcol=sect.text();
        break;
      }
      case '-': {
        switch (sect.text()[1]) {
          case 'r':
          case 'w': {
            rw=sect.text()[1];
            break;
          }
          case 't': {
            tagopt=true;
            break;
          }
          case 'e': {
            macopt=true;
            break;
          }
          case 'p' : {
            if (!session_restored) {
              session_restored=true;
              FXString sessionname;
              FXival count;
              FXFile fh(GetApp()->SessionFile(), FXIO::Reading);
              if (fh.isOpen()) {
                session_data.length(fh.size());
                count=fh.readBlock((void*)(session_data.text()),fh.size());
                fh.close();
                ParseCommands(session_data);
                session_data="";
              }
            }
            break;
          }
          case 'q' : {
            break; // "quiet" option is invalid for a new instance, so just ignore it.
          }
          default: fprintf(stderr, _("Warning: unrecognized option: %s\n"), sect.text());
        }
        break;
      }
      default: {
        if (tagopt) {
          AddFileToTagsMenu(sect);
          tagopt=false;
        } else {
          if (macopt) {
            macopt=false;
            if (tabbook->numChildren()==0) { /* Be sure we have a document before running a macro */
              NewFile(false);
              RunMacro(sect,false);
              if (tabbook->numChildren()==2) {
                SciDoc*sci=ControlDoc();
                if ( (sci->Filename().empty()) && (sci->GetTextLength()==0) && (!sci->Dirty()) ) {
                  CloseFile(true,false); /* Forget this document if it wasn't changed */
                }
              }
            } else {
              RunMacro(sect,false);
            }
          } else {
            FXString filename=sect.text();
            if (rowcol.empty()) {
              FXint beg,end;
              if (rx_rowcol.match(filename,&beg,&end)) {
                if (!FXStat::isFile(filename)) {
                  rowcol=filename.mid(beg,end-beg).text();
                  filename=filename.left(beg).text();
                  if (!FXStat::isFile(filename)) {
                    rowcol="";
                    filename=sect.text();
                  }
                }
              }
            }
            OpenFile(filename.text(), rowcol.empty()?NULL:(rowcol.text())+1, rw=='r',true);
            rowcol="";
          }
        }
      }
    }
  }
}

#ifndef __WIN32__
#define NET_WM_ICON
#endif

#ifdef NET_WM_ICON
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include <X11/Xatom.h>
void SetupXAtoms(FXTopWindow*win, const char*class_name)
{
  FXIcon*ico=win->getIcon();
  Display*d=(Display*)(win->getApp()->getDisplay());
  Atom net_wm_icon = XInternAtom(d, "_NET_WM_ICON", 0);
  Atom cardinal = XInternAtom(d, "CARDINAL", False);
  if (net_wm_icon&&cardinal) {
    FXint w=ico->getWidth();
    FXint h=ico->getHeight();
    CARD32 *icon_buf=NULL;
    FXint icon_buf_size=(w*h)+2;
    icon_buf=(CARD32*)malloc(icon_buf_size*sizeof(CARD32));
    FXint j=0;
    icon_buf[j++]=w;
    icon_buf[j++]=h;
    FXint x,y;
    for (y=0; y<h; y++) {
      for (x=0; x<w; x++) {
        FXColor px=ico->getPixel(x,y);
        icon_buf[j++]=px?FXRGB(FXBLUEVAL(px),FXGREENVAL(px),FXREDVAL(px)):FXRGBA(255,255,255,0);
      }
    }
    XChangeProperty(d, win->id(), net_wm_icon, cardinal, 32,
      PropModeReplace, (const FXuchar*) icon_buf, icon_buf_size);
    free(icon_buf);
  }

  Atom net_wm_pid = XInternAtom(d, "_NET_WM_PID", 0);
  pid_t pid=fxgetpid();
  XChangeProperty(d, win->id(), net_wm_pid, cardinal, 32,
    PropModeReplace, (const FXuchar*) &pid, sizeof(pid));
  FXString hn=FXSystem::getHostName();
  if (!hn.empty()) {
    Atom wm_client_machine = XInternAtom(d, "WM_CLIENT_MACHINE", 0);
    XChangeProperty(d, win->id(), wm_client_machine, XA_STRING, 8,
      PropModeReplace, (const FXuchar*) hn.text(), hn.length());
  }
  FXString cn=class_name;
  cn.append(".");
  cn.append(APP_NAME);
  Atom wm_class = XInternAtom(d, "WM_CLASS", 0);
  XChangeProperty(d, win->id(), wm_class, XA_STRING, 8,
    PropModeReplace, (const FXuchar*) cn.text(), cn.length());
} 



FXID TopWindow::GetActiveWindow()
{
  FXID rv=0;
  Display*dpy=(Display*)getApp()->getDisplay();
  Window root=getApp()->getRootWindow()->id();
  static Atom xa=XInternAtom(dpy, "_NET_ACTIVE_WINDOW", False);
  Atom rtype;
  int fmt;
  ulong n;
  ulong rem;
  FXuchar *xw;
  if (XGetWindowProperty(dpy,root,xa,0,sizeof(Window),False,XA_WINDOW,&rtype,&fmt,&n,&rem,&xw)==Success) {
    rv=*((Window*)xw);
    XFree(xw);
  }
  return rv;
}


#else
#include <windows.h>
#define SetupXAtoms(win,class_name)
FXID TopWindow::GetActiveWindow()
{
  return GetForegroundWindow();
}
#endif


const
#include "icon32x32.xpm"


void TopWindow::create()
{
  AppClass* a=GetApp();
  a->setWheelLines(prefs->WheelLines);
  position(prefs->Left, prefs->Top,prefs->Width,prefs->Height);

  toolbar_font = NULL;
  UpdateToolbar();

  FXMainWindow::create();
  
  show(prefs->placement);

  FXIcon*ico=new FXXPMIcon(a,icon32x32_xpm,0,IMAGE_KEEP);
  ico->create();
  setIcon(ico);
  SetupXAtoms(this, ((AppClass*)(getApp()))->ServerName().text());
  if (prefs->Maximize) { maximize(); }
  FXFont fnt(a, prefs->FontName, prefs->FontSize/10);
  GetFontDescription(prefs->fontdesc,&fnt);
  outpop->create();
  recent_files->create();
  unloadtagsmenu->create();
  save_hook.format("%s%s%c%s%c%s.lua", GetApp()->ConfigDir().text(), "tools", PATHSEP, "hooks", PATHSEP, "saved");
  RunHookScript("startup");
  ParseCommands(a->Commands());
  if (tabbook->numChildren()==0) { NewFile(false); }
  StaleTicks=0;
  SaveTicks=0;
  a->addTimeout(this,ID_TIMER,ONE_SECOND,NULL);
  general_info->setY(6);
}

