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


#include <cerrno>
#include <fx.h>
#include <fxkeys.h>

#include "compat.h"
#include "appname.h"
#include "appmain.h"
#include "scidoc.h"
#include "filer.h"
#include "search.h"
#include "doctabs.h"
#include "prefs.h"
#include "lang.h"
#include "runcmd.h"
#include "macro.h"
#include "recorder.h"
#include "backup.h"
#include "menuspec.h"
#include "toolbar.h"
#include "outpane.h"
#include "statusbar.h"
#include "mainmenu.h"
#include "tagread.h"
#include "scidoc_util.h"
#include "foreachtab.h"
#include "cmd_utils.h"

#include "intl.h"
#include "appwin.h"


#define PACK_UNIFORM ( PACK_UNIFORM_WIDTH | PACK_UNIFORM_HEIGHT )


static bool topwin_closing = false;
static TopWindow* global_top_window_instance=NULL;


bool TopWindow::Closing() { return topwin_closing; }

TopWindow* TopWindow::instance() { return global_top_window_instance; }

const FXString& TopWindow::ConfigDir() { return ((AppClass*)(FXApp::instance()))->ConfigDir(); }

const FXString& TopWindow::Connector() { return ((AppClass*)(FXApp::instance()))->Connector(); }

#define SessionFile() (((AppClass*)getApp())->SessionFile())



TopWindow::TopWindow(FXApp *a):MainWinWithClipBrd(a,EXE_NAME,NULL,NULL,DECOR_ALL,0,0,600,400)
{
  FXASSERT(!global_top_window_instance);
  global_top_window_instance=this;
  active_widget=NULL;
  recorder=NULL;
  recording=NULL;
  need_status=0;
  skipfocus=false;
  destroying=false;
  close_all_confirmed=false;
  kill_commands_confirmed=false;
  topwin_closing=false;
  StyleDef*sd=GetStyleFromId(Settings::globalStyle(), STYLE_CALLTIP);
  tips=new FXToolTip(getApp(),0);
  RgbToHex(getApp()->getTipbackColor(), sd->bg);
  RgbToHex(getApp()->getTipforeColor(), sd->fg);
  prefs=new Settings(this, ConfigDir());
  SciDoc::DefaultStyles(prefs->Styles());
  menubar=new MainMenu(this);
  FXVerticalFrame*vbox=new FXVerticalFrame(this,FRAME_NONE|LAYOUT_FILL,0,0,0,0,4,4,4,4);
  toolbar=new ToolBarFrame(vbox);
  hsplit=new FXSplitter(vbox,this, ID_SPLIT_CHANGED, SPLITTER_VERTICAL|SPLITTER_REVERSED|LAYOUT_FILL|SPLITTER_TRACKING);
  tabbook=new DocTabs(hsplit,this,ID_TAB_SWITCHED,FRAME_NONE|PACK_UNIFORM|LAYOUT_FILL);
  tabbook->setTabStyleByChar(prefs->DocTabPosition);
  tabbook->setTabsCompact(prefs->DocTabsPacked);
  tabbook->MaxTabWidth(prefs->TabTitleMaxWidth);
  outlist=new OutputList(hsplit,NULL,0,LAYOUT_SIDE_TOP|LAYOUT_FILL);
  statusbar=new StatusBar(this,ID_KILL_COMMAND,(void*)CommandUtils::DontFreezeMe());
  ShowOutputPane(prefs->ShowOutputPane);
  ShowStatusBar(prefs->ShowStatusBar);
  backups=new BackupMgr(this, ConfigDir());
  completions=new AutoCompleter();
  filedlgs=new FileDialogs(this,ID_FILE_SAVED,prefs->FileFilters);
  srchdlgs=new SearchDialogs(this);
  srchdlgs->SetPrefs(prefs->SearchOptions,prefs->SearchWrap,prefs->SearchVerbose);
  cmdutils=new CommandUtils(this);
}



void TopWindow::SetKillCommandAccelKey(FXHotKey acckey)
{
  cmdutils->SetKillCommandAccelKey(acckey);
}


TopWindow::~TopWindow()
{
  destroying=true;
  delete filedlgs;
  delete backups;
  delete cmdutils;
  delete recorder;
  delete getIcon();
  delete getMiniIcon();
  delete completions;
  global_top_window_instance=NULL;
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
#ifdef WIN32
     FileDialogs::ReadShortcut(this,fn);
#endif
    tabbook->ForEachTab(TabCallbacks::FileAlreadyOpenCB,&fn);
    if (fn.empty()) {
      if (rowcol && *rowcol) { ControlDoc()->GoToStringCoords(rowcol); }
      if (hooked) { RunHookScript("opened"); }
      return true;
    }
  }
  if (!FileDialogs::FileExistsOrConfirmCreate(this,fn)) { return false; }
  DocTab*tab=tabbook->NewTab(fn.empty()?_("Untitled"):FXPath::name(fn));
  sci=SciDocUtils::NewSci((FXComposite*)tab->getNext(),this,ID_SCINTILLA,prefs);
  if (!fn.empty()) {
    if (!sci->LoadFromFile(fn.text())) {
      if (!sci->GetLastError().contains(SciDoc::BinaryFileMessage())) {
         FXMessageBox::error(this, MBOX_OK, _("Error opening file"), "%s:\n%s\n%s",
             _("Could not open file"),  fn.text(),  sci->GetLastError().text());
      }
      SciDocUtils::DoneSci(sci,recording,ID_MACRO_RECORD);
      return false;
    }
    if (!sci->SetLanguageForHeader(fn)) {
      if (!sci->setLanguageFromFileName(FXPath::name(fn).text())) {
        sci->setLanguageFromContent();
      }
    }
  } else {
    sci->SetUTF8(!prefs->DefaultToAscii);
    sci->UpdateStyle();
  }
  SetTabLocked(sci,readonly);
  tabbook->ActivateTab(tabbook->Count()-1);
  menubar->AppendDocList(sci->Filename(), tab);
  if (shown()) {
    sci->create();
    if (rowcol && *rowcol) { sci->GoToStringCoords(rowcol); }
  }

  //  If the only thing we had open prior to this file was a single,
  //  empty, untitled, unmodified document, then close it...
  if ((tabbook->Count()==2) && (!sci->Filename().empty())) {
    SciDoc*sc0=(SciDoc*)(tabbook->PageAt(0)->getFirst());
    if ( sc0->Filename().empty() && (sc0->GetTextLength()==0) && (sc0->Dirty()==false)  ) {
      tabbook->ActivateTab(0);
      CloseFile(false,false);
    }
  }
  sci->setFocus();
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



bool TopWindow::CloseFile(bool close_last, bool hooked)
{
  FXint i=tabbook->getCurrent();
  DocTab*tab=tabbook->ActiveTab();
  SciDoc*sci=ControlDoc();
  if (!filedlgs->TryClose(sci,tab->getText().text())) { return false; }
  menubar->PrependRecentFile(sci->Filename());
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
    SciDocUtils::DoneSci(sci,recording,ID_MACRO_RECORD);
  } else {
    SciDocUtils::DoneSci(sci,recording,ID_MACRO_RECORD);
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
  hook.format("%s%s%c%s%c%s.lua", ConfigDir().text(), "tools", PATHSEP, "hooks", PATHSEP, hookname);
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
  menubar->AddFileToTagsList(filename);
}



bool TopWindow::RemoveFileFromTagsMenu(const FXString &filename)
{
  return menubar->RemoveFileFromTagsList(filename);
}



bool TopWindow::SetLanguage(FXMenuRadio *mnu)
{
  if (mnu) {
    LangStyle*ls=(LangStyle*) mnu->getUserData();
    SciDoc*sci=ControlDoc();
    sci->setLanguage(ls);
    menubar->SetLanguageCheckmark(ls);
    return true;
  } else { return false; }
}



bool TopWindow::SetLanguage(const FXString &name)
{
  return SetLanguage(menubar->GetMenuForLanguage(name));
}



bool TopWindow::ShowLineNumbers() { return prefs->ShowLineNumbers; }
bool TopWindow::ShowStatusBar()   { return prefs->ShowStatusBar; }
bool TopWindow::ShowOutputPane()  { return prefs->ShowOutputPane; }
bool TopWindow::ShowWhiteSpace()  { return prefs->ShowWhiteSpace; }
bool TopWindow::ShowToolbar()     { return prefs->ShowToolbar; }
bool TopWindow::ShowMargin()      { return prefs->ShowRightEdge; }
bool TopWindow::ShowIndent()      { return prefs->ShowIndentGuides; }
bool TopWindow::ShowCaretLine()   { return prefs->ShowCaretLine; }



void TopWindow::ShowLineNumbers(bool showit)
{
  prefs->ShowLineNumbers=showit;
  tabbook->ForEachTab(TabCallbacks::LineNumsCB, (void*)(FXival)showit);
  menubar->SetCheck(ID_SHOW_LINENUMS,showit);
}



void TopWindow::ShowStatusBar(bool showit)
{
  prefs->ShowStatusBar=showit;
  menubar->SetCheck(ID_SHOW_STATUSBAR,showit);
  statusbar->Show(showit);
}



void TopWindow::ShowOutputPane(bool showit)
{
  prefs->ShowOutputPane=showit;
  menubar->SetCheck(ID_SHOW_OUTLIST,showit);
  if (showit) {
    if (prefs->OutputPaneHeight<16) {prefs->OutputPaneHeight=16; }
    hsplit->setSplit(1, prefs->OutputPaneHeight);
    outlist->show();
  } else {
    outlist->hide();
    hsplit->setSplit(1,0);
  }
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



void TopWindow::UpdateEolMenu(SciDoc*sci)
{
  switch (sci->sendMessage(SCI_GETEOLMODE,0,0)) {
    case SC_EOL_CRLF: { MenuMgr::RadioUpdate(ID_FMT_DOS,  ID_FMT_DOS, ID_FMT_UNIX);  break; }
    case SC_EOL_CR:   { MenuMgr::RadioUpdate(ID_FMT_MAC,  ID_FMT_DOS, ID_FMT_UNIX);  break; }
    case SC_EOL_LF:   { MenuMgr::RadioUpdate(ID_FMT_UNIX, ID_FMT_DOS, ID_FMT_UNIX); break; }
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
    UpdateEolMenu(sci);
  } else {
    setTitle(EXE_NAME);
    statusbar->Clear();
  }
}



class MyCmdIO: public CmdIO {
  virtual bool IsCancelled() { return ((TopWindow*)win)->IsMacroCancelled(); }
public:
  MyCmdIO(FXMainWindow *window, const char*shellcmd="/bin/sh -c"):CmdIO(window,shellcmd){}
};



bool TopWindow::FilterSelection(SciDoc *sci, const FXString &cmd, const FXString &input)
{
  if (!cmdutils->IsCommandReady()) { return false; }
  cmdutils->CommandBusy(true);
  cmdutils->SetShellEnv(sci->Filename().text(),sci->GetLineNumber());
  bool rv=false;
  if (!cmd.empty()) {
    MyCmdIO cmdio(this, prefs->ShellCommand.text());
    FXString output="";
    command_timeout=false;
    getApp()->beginWaitCursor();
    statusbar->Running(_("command"));
    cmdutils->DisableUI(true);
    if (cmdio.filter(cmd.text(), input, output, &command_timeout)) {
      sci->sendString(SCI_REPLACESEL, 0, output.text());
      sci->ScrollWrappedInsert();
      rv=true;
    }
    cmdutils->DisableUI(false);
    statusbar->Normal();
    getApp()->endWaitCursor();
  }
  sci->setFocus();
  need_status=1;
  cmdutils->CommandBusy(false);
  return rv;
}



bool TopWindow::RunCommand(SciDoc *sci, const FXString &cmd)
{
  if (!cmdutils->IsCommandReady()) { return false; }
  cmdutils->CommandBusy(true);
  cmdutils->SetShellEnv(sci->Filename().text(),sci->GetLineNumber());
  bool success=false;
  if (!cmd.empty()) {
    MyCmdIO cmdio(this, prefs->ShellCommand.text());
    command_timeout=false;
    outlist->clearItems(true);
    update();
    repaint();
    if (!prefs->ShowOutputPane) { ShowOutputPane(true); }
    getApp()->beginWaitCursor();
    statusbar->Running(_("command"));
    cmdutils->DisableUI(true);
    getApp()->runWhileEvents();
    success=cmdio.lines(cmd.text(), this, ID_CMDIO, &command_timeout, true);
    cmdutils->DisableUI(false);
    statusbar->Normal();
    getApp()->endWaitCursor();
    if (success) {
      outlist->appendItem(_("Command succeeded."));
    } else {
      if (command_timeout) {
        outlist->appendItem(_("Command cancelled."));
      } else {
        outlist->appendItem(_("Command failed."));
        outlist->SelectFirstError();
      }
    }
  }
  if (FocusedDoc() && (GetNetActiveWindow()==id())) { FocusedDoc()->setFocus(); }
  need_status=1;
  cmdutils->CommandBusy(false);
  return success;
}



bool TopWindow::IsMacroCancelled()
{
  return cmdutils->IsMacroCancelled(command_timeout);
}



bool TopWindow::RunMacro(const FXString &script, bool isfilename)
{
  if (!cmdutils->IsCommandReady()) { return false; }
  cmdutils->CommandBusy(true);
  MacroRunner macros;
  command_timeout=false;
  statusbar->Running(_("macro"));
  update();
  cmdutils->DisableUI(true);
  getApp()->runWhileEvents();
  bool rv=isfilename?macros.DoFile(script):macros.DoString(script);
  if (!destroying) {
    tabbook->ForEachTab(TabCallbacks::ResetUndoLevelCB,NULL);
    cmdutils->DisableUI(false);
    statusbar->Normal();
    if (FocusedDoc() && (GetNetActiveWindow()==id())) { FocusedDoc()->setFocus(); }
    need_status=1;
  }
  cmdutils->CommandBusy(false);
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



#define confirm(hdr,fmt,arg) \
  ( FXMessageBox::question(this,MBOX_OK_CANCEL,hdr,fmt,arg) == MBOX_CLICKED_OK )

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
  if (!RunHookScript("shutdown")) { return false; }
  prefs->LastFocused=FocusedDoc()->Filename();
  if (!CloseAll(true)) {
    not_confirmed();
    prefs->LastFocused="";
    FocusedDoc()->setFocus();
    return false;
  }

  if (!session_data.empty()) {
     FXFile fh(SessionFile(), FXIO::Writing);
     if (fh.isOpen()) {
       FXival wrote=0;
       wrote=fh.writeBlock(session_data.text(),session_data.length());
       if (!(fh.close() && (wrote==session_data.length()))) {
         fxwarning(_(EXE_NAME": Could not save %s (%s)"), SessionFile().text(), SystemErrorStr());
       }
     } else {
       fxwarning(_(EXE_NAME": Could not open %s for writing (%s)"), SessionFile().text(), SystemErrorStr());
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
  delete menubar;
  return FXMainWindow::close(notify);
}



void TopWindow::ParseCommands(FXString &commands)
{
  FXString sect="";
  FXString rowcol="";
  FXRex rx_rowcol(":\\d+$",REX_CAPTURE);
  FXchar rw='w';
  bool tagopt=false;
  bool macopt=false;
  bool session_restored=false;
  int i=0;
  if (!((compare(commands,"-q\n",2)==0) || (commands.contains("\n-q\n")))) {
    hide();
    show();
    getApp()->runWhileEvents();
  }
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
              FXFile fh(SessionFile(), FXIO::Reading);
              if (fh.isOpen()) {
                session_data.length(fh.size());
                fh.readBlock((void*)(session_data.text()),fh.size());
                fh.close();
                ParseCommands(session_data);
                if (!prefs->LastFocused.empty()) {
                  if (FXStat::isFile(prefs->LastFocused)) {
                    tabbook->ForEachTab(TabCallbacks::FileAlreadyOpenCB,&prefs->LastFocused);
                  }
                }
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
  commands=FXString::null;
}


const
#include "fxite.xpm"


void TopWindow::create()
{
  AppClass* a=(AppClass*)getApp();
  a->setWheelLines(prefs->WheelLines);
  position(prefs->Left, prefs->Top,prefs->Width,prefs->Height);
  UpdateToolbar();

  FXMainWindow::create();

  tips->create();
  show(prefs->placement);

  FXIcon*ico=NULL;
  FXString icon_file=ConfigDir()+"icon.xpm";
  if (FXStat::isFile(icon_file)) {
    FXFileStream*icon_strm=new FXFileStream();
    if (icon_strm->open(icon_file.text(),FXStreamLoad)) {
      ico=new FXXPMIcon(a,NULL,0,IMAGE_KEEP);
      if ( ((FXXPMIcon*)ico)->loadPixels(*icon_strm) ) {
        ico->scale(32,32);
      } else {
        delete ico;
        ico=NULL;
      }
      icon_strm->close();
    }
    delete icon_strm;
    if (!ico) { fxwarning(_("NOTE: Failed to load custom icon.\n")); }
  }
  if (!ico) { ico=new FXXPMIcon(a,icon32x32_xpm,0,IMAGE_KEEP); }
  ico->create();
  setIcon(ico);
  SetupXAtoms(this, a->ServerName().text(), APP_NAME);
  if (prefs->Maximize) { maximize(); }
  FXFont fnt(a, prefs->FontName, prefs->FontSize/10);
  GetFontDescription(prefs->fontdesc,&fnt);

  save_hook.format("%s%s%c%s%c%s.lua", ConfigDir().text(), "tools", PATHSEP, "hooks", PATHSEP, "saved");
  RunHookScript("startup");
  ParseCommands(a->Commands());
  if (tabbook->numChildren()==0) { NewFile(false); }
  StaleTicks=0;
  SaveTicks=0;
  a->addTimeout(this,ID_TIMER,ONE_SECOND,NULL);
#ifndef FOX_1_6
  if (!a->migration_errors.empty()) {
    NewFile(false);
    SciDoc*sci=ControlDoc();
    sci->SetText(a->migration_errors.text());
    a->migration_errors="";
  }
#endif
}


void TopWindow::AddOutput(const FXString&line)
{
  outlist->appendItem(line.text());
}



void TopWindow::ClearOutput()
{
  outlist->clearItems();
}



FXMenuCaption*TopWindow::TagFiles() {
  return (FXMenuCaption*)(menubar->TagsMenu()->getMenu()->getFirst());
}

