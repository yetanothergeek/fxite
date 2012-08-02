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

#include <cctype>

#include <fx.h>
#include <fxkeys.h>

#include <SciLexer.h>
#include <Scintilla.h>
#include <FXScintilla.h>
#include <lua.h>

#include "intl.h"
#include "scidoc.h"
#include "prefs.h"
#include "shmenu.h"
#include "lang.h"
#include "menuspec.h"
#include "fxasq.h"
#include "compat.h"
#include "appname.h"

#include "appwin.h"

#define TEST_SOMETHING 0

// Create a new menu command, menu check box, or menu radio item...
#define MkMnuCmd(p,s) MenuMgr::MakeMenuCommand(p,this,s,'m')
#define MkMnuChk(p,s,c) ((FXMenuCheck*)MenuMgr::MakeMenuCommand(p,this,s,'k',c))
#define MkMnuRad(p,s) ((FXMenuRadio*)MenuMgr::MakeMenuCommand(p,this,s,'r'))


static const char*usercmdflags[]={
  "save", _("Save open files first."),
  NULL
};


static const char*usersnipflags[]={
  "read", _("Insert file's contents."),
  "exec", _("Insert command output."),
  NULL
};



/*
  Fox menus don't re-focus the previously active widget after they are dismissed.
  This sub-class helps to work around that.
*/
class TopMenuPane: public FXMenuPane {
  FXDECLARE(TopMenuPane)
  TopMenuPane(){}
public:
  TopMenuPane(FXComposite*o):FXMenuPane(o) {}
  long onCmdUnpost(FXObject*o, FXSelector sel, void*p ){
    long rv=FXMenuPane::onCmdUnpost(o,sel,p);
    getApp()->addChore(TopWindow::instance(),TopWindow::ID_FOCUS_DOC,NULL);
    return rv;
  }
};



FXDEFMAP(TopMenuPane) TopMenuPaneMap[] = {
  FXMAPFUNC(SEL_COMMAND,FXWindow::ID_UNPOST,TopMenuPane::onCmdUnpost),
};

FXIMPLEMENT(TopMenuPane,FXMenuPane,TopMenuPaneMap,ARRAYNUMBER(TopMenuPaneMap))

#define SubMenuPane TopMenuPane


void TopWindow::CreateLanguageMenu()
{
  langmenu=new SubMenuPane(this);
  langcasc=new FXMenuCascade(viewmenu,_("&Language"),NULL,langmenu);
  FXMenuRadio*mr;

  cpp_langmenu=new SubMenuPane(langmenu);
  cpp_langcasc=new FXMenuCascade(langmenu,_("C/C++"),NULL,cpp_langmenu);

  scr_langmenu=new SubMenuPane(langmenu);
  scr_langcasc=new FXMenuCascade(langmenu,_("Scripting"),NULL,scr_langmenu);

  cfg_langmenu=new SubMenuPane(langmenu);
  cfg_langcasc=new FXMenuCascade(langmenu,_("Config"),NULL,cfg_langmenu);

  html_langmenu=new SubMenuPane(langmenu);
  html_langcasc=new FXMenuCascade(langmenu,_("Web"),NULL,html_langmenu);

  inf_langmenu=new SubMenuPane(langmenu);
  inf_langcasc=new FXMenuCascade(langmenu,_("Info"),NULL,inf_langmenu);

  lgcy_langmenu=new SubMenuPane(langmenu);
  lgcy_langcasc=new FXMenuCascade(langmenu,_("Legacy"),NULL,lgcy_langmenu);

  tex_langmenu=new SubMenuPane(langmenu);
  tex_langcasc=new FXMenuCascade(langmenu,_("Typeset"),NULL,tex_langmenu);

  db_langmenu=new SubMenuPane(langmenu);
  db_langcasc=new FXMenuCascade(langmenu,_("Database"),NULL,db_langmenu);

  asm_langmenu=new SubMenuPane(langmenu);
  asm_langcasc=new FXMenuCascade(langmenu,_("Asm/HDL"),NULL,asm_langmenu);

  misc_langmenu=new SubMenuPane(langmenu);
  misc_langcasc=new FXMenuCascade(langmenu,_("Other"),NULL,misc_langmenu);

  mr=new FXMenuRadio(misc_langmenu,_("none"),this,ID_SET_LANGUAGE);
  mr->setUserData(NULL);

  for (LangStyle*ls=languages; ls->name; ls++) {
    FXMenuPane*mp;
    switch (ls->id) {
    case SCLEX_COBOL:
    case SCLEX_F77:
    case SCLEX_FORTRAN:
    case SCLEX_FREEBASIC:
    case SCLEX_PASCAL:
    case SCLEX_SMALLTALK:
    case SCLEX_ERLANG:
    case SCLEX_LISP:
    case SCLEX_HASKELL:
    case SCLEX_MODULA:
    case SCLEX_ADA: {
      mp=lgcy_langmenu;
      break;
    }
    case SCLEX_CSS:
    case SCLEX_HTML:
    case SCLEX_MARKDOWN:
    case SCLEX_TXT2TAGS: {
      mp=(compare(ls->name,"docbook")==0)?tex_langmenu:html_langmenu;
      break;
    }
    case SCLEX_CPP: {
      mp=(compare(ls->name,"javascript")==0)?html_langmenu:cpp_langmenu;
      break;
    }
    case SCLEX_BASH:
    case SCLEX_BATCH:
    case SCLEX_LUA:
    case SCLEX_PERL:
    case SCLEX_PYTHON:
    case SCLEX_RUBY:
    case SCLEX_R:
    case SCLEX_TCL: {
      mp=scr_langmenu;
      break;
    }
    case SCLEX_FLAGSHIP:
    case SCLEX_SQL: {
      mp=db_langmenu;
      break;
    }
    case SCLEX_DIFF:
    case SCLEX_ERRORLIST: {
      mp=inf_langmenu;
      break;
    }
    case SCLEX_CONF:
    case SCLEX_CMAKE:
    case SCLEX_MAKEFILE:
    case SCLEX_PROPERTIES:
    case SCLEX_XML: {
      mp=cfg_langmenu;
      break;
    }
    case SCLEX_ASM:
    case SCLEX_VHDL:
    case SCLEX_VERILOG:
    case SCLEX_A68K: {
      mp=asm_langmenu;
      break;
    }
    case SCLEX_LATEX:
    case SCLEX_TEX:
    case SCLEX_METAPOST:
    case SCLEX_PS: {
      mp=tex_langmenu;
      break;
    }
    default: { mp=misc_langmenu; }
    }
    mr=new FXMenuRadio(mp,ls->name,this,ID_SET_LANGUAGE);
    mr->setUserData((void*)ls);
  }
}



static FXMenuTitle*NewTitle(FXComposite*p, const FXString&text, FXIcon*ic=NULL, FXPopup*pup=NULL, FXuint opts=0)
{
  FXMenuTitle*rv=new FXMenuTitle(p, text, ic, pup, opts);
  if (pup) { pup->setUserData((void*)rv); }
  return rv;
}



static FXMenuCascade*NewCascade(FXComposite*p, const FXString&text, FXIcon*ic=NULL, FXPopup*pup=NULL, FXuint opts=0)
{
  FXMenuCascade*rv=new FXMenuCascade(p, text, ic, pup, opts);
  if (pup) { pup->setUserData((void*)rv); }
  return rv;
}



void TopWindow::CreateTabsMenu()
{
  tabmenu=new SubMenuPane(this);
  NewCascade(viewmenu,_("&Tabs"),NULL,tabmenu);

  tabsidemenu=new SubMenuPane(this);
  NewCascade(tabmenu,_("&Position"),NULL,tabsidemenu);

  FXMenuRadio*tT=MkMnuRad(tabsidemenu,ID_TABS_TOP);
  FXMenuRadio*tB=MkMnuRad(tabsidemenu,ID_TABS_BOTTOM);
  FXMenuRadio*tL=MkMnuRad(tabsidemenu,ID_TABS_LEFT);
  FXMenuRadio*tR=MkMnuRad(tabsidemenu,ID_TABS_RIGHT);

  switch (prefs->DocTabPosition) {
    case 'B': { tB->setCheck(); break; }
    case 'L': { tL->setCheck(); break; }
    case 'R': { tR->setCheck(); break; }
    default:  { tT->setCheck(); break; }
  }

  tabwidthmenu=new SubMenuPane(this);
  NewCascade(tabmenu,_("&Width"),NULL,tabwidthmenu);

  FXMenuRadio*tU=MkMnuRad(tabwidthmenu,ID_TABS_UNIFORM);
  FXMenuRadio*tP=MkMnuRad(tabwidthmenu,ID_TABS_COMPACT);

  (prefs->DocTabsPacked?tP:tU)->setCheck();
}



void TopWindow::CreateZoomMenu()
{
  viewzoommenu=new SubMenuPane(this);
  NewCascade(viewmenu,_("&Zoom"),NULL,viewzoommenu);
  MkMnuCmd(viewzoommenu,ID_ZOOM_IN);
  MkMnuCmd(viewzoommenu,ID_ZOOM_OUT);
  MkMnuCmd(viewzoommenu,ID_ZOOM_NONE);
  MkMnuCmd(viewzoommenu,ID_ZOOM_NEAR);
  MkMnuCmd(viewzoommenu,ID_ZOOM_FAR);
}



void TopWindow::CreateMenus()
{
  menubar=new FXMenuBar(this,LAYOUT_SIDE_TOP|LAYOUT_FILL_X);
  filemenu=new TopMenuPane(this);
  NewTitle(menubar,_("&File"),NULL,filemenu);
  MkMnuCmd(filemenu,ID_NEW);
  MkMnuCmd(filemenu,ID_OPEN_FILES);
  openselmenu=MkMnuCmd(filemenu,ID_OPEN_SELECTED);
  recent_files=new HistMenu(filemenu, _("Open pre&vious"), "RecentFiles", this, ID_OPEN_PREVIOUS);

  new FXMenuSeparator(filemenu, 0);
  MkMnuCmd(filemenu,ID_SAVE);
  MkMnuCmd(filemenu,ID_SAVEAS);
  MkMnuCmd(filemenu,ID_SAVEALL);
  MkMnuCmd(filemenu,ID_SAVECOPY);

  new FXMenuSeparator(filemenu, 0);
  MkMnuCmd(filemenu,ID_RELOAD);

  fileexportmenu=new SubMenuPane(this);
  NewCascade(filemenu,_("E&xport"),NULL,fileexportmenu);
  MkMnuCmd(fileexportmenu,ID_EXPORT_PDF);
  MkMnuCmd(fileexportmenu,ID_EXPORT_HTML);

  new FXMenuSeparator(filemenu, 0);
  MkMnuCmd(filemenu,ID_CLOSE);
  MkMnuCmd(filemenu,ID_CLOSEALL);
  new FXMenuSeparator(filemenu, 0);
  MkMnuCmd(filemenu,ID_INSERT_FILE);
  MkMnuCmd(filemenu,ID_LOAD_TAGS);
  unloadtagsmenu=new FXMenuCascade(filemenu,_("&Unload tags file"),NULL,(new SubMenuPane(filemenu)));
  unloadtagsmenu->disable();
  MkMnuCmd(filemenu,ID_SELECT_DIR);
  new FXMenuSeparator(filemenu, 0);
  MkMnuCmd(filemenu,ID_QUIT);

  editmenu=new TopMenuPane(this);
  NewTitle(menubar,_("&Edit"),NULL,editmenu);

  MkMnuCmd(editmenu,ID_UNDO);
  MkMnuCmd(editmenu,ID_REDO);
  new FXMenuSeparator(editmenu, 0);
  MkMnuCmd(editmenu,ID_CUT);
  MkMnuCmd(editmenu,ID_COPY);
  MkMnuCmd(editmenu,ID_PASTE);

  editdeletemenu=new SubMenuPane(this);
  NewCascade(editmenu,_("&Delete"),NULL,editdeletemenu);

  MkMnuCmd(editdeletemenu,ID_DEL_WORD_LEFT);
  MkMnuCmd(editdeletemenu,ID_DEL_WORD_RIGHT);
  MkMnuCmd(editdeletemenu,ID_DEL_LINE_LEFT);
  MkMnuCmd(editdeletemenu,ID_DEL_LINE_RIGHT);

  new FXMenuSeparator(editmenu, 0);
  MkMnuCmd(editmenu,ID_TOLOWER);
  MkMnuCmd(editmenu,ID_TOUPPER);

  editindentmenu=new SubMenuPane(this);
  NewCascade(editmenu,_("&Indent"),NULL,editindentmenu);

  MkMnuCmd(editindentmenu,ID_INDENT_STEP);
  MkMnuCmd(editindentmenu,ID_UNINDENT_STEP);
  MkMnuCmd(editindentmenu,ID_INDENT_FULL);
  MkMnuCmd(editindentmenu,ID_UNINDENT_FULL);

  new FXMenuSeparator(editmenu, 0);
  readonlymenu=MkMnuChk(editmenu,ID_READONLY,false);
  wordwrapmenu=MkMnuChk(editmenu,ID_WORDWRAP,false);

  fileformatmenu=new SubMenuPane(this);
  fileformatcasc=NewCascade(editmenu,_("File &Format"),NULL,fileformatmenu);

  fmt_dos_mnu=MkMnuRad(fileformatmenu,ID_FMT_DOS);
  fmt_mac_mnu=MkMnuRad(fileformatmenu,ID_FMT_MAC);
  fmt_unx_mnu=MkMnuRad(fileformatmenu,ID_FMT_UNIX);

  MkMnuCmd(editmenu,ID_PREFS_DIALOG);

  searchmenu=new TopMenuPane(this);
  NewTitle(menubar,_("&Search"),NULL,searchmenu);

  MkMnuCmd(searchmenu,ID_FIND);
  MkMnuCmd(searchmenu,ID_FINDNEXT);
  MkMnuCmd(searchmenu,ID_FINDPREV);

  searchselectmenu=new SubMenuPane(this);
  NewCascade(searchmenu,_("Find &Selected"),NULL,searchselectmenu);
  MkMnuCmd(searchselectmenu,ID_NEXT_SELECTED);
  MkMnuCmd(searchselectmenu,ID_PREV_SELECTED);
  MkMnuCmd(searchmenu,ID_REPLACE_IN_DOC);

  new FXMenuSeparator(searchmenu, 0);
  MkMnuCmd(searchmenu,ID_GOTO);
  MkMnuCmd(searchmenu,ID_GOTO_SELECTED);
  MkMnuCmd(searchmenu,ID_GOTO_ERROR);

  searchmarkmenu=new SubMenuPane(this);
  NewCascade(searchmenu,_("&Bookmark"),NULL,searchmarkmenu);
  MkMnuCmd(searchmarkmenu,ID_BOOKMARK_SET);
  MkMnuCmd(searchmarkmenu,ID_BOOKMARK_RETURN);

  new FXMenuSeparator(searchmenu, 0);

  findtagmenu=MkMnuCmd(searchmenu,ID_FIND_TAG);
  findtagmenu->disable();
  showtipmenu=MkMnuCmd(searchmenu,ID_SHOW_CALLTIP);
  showtipmenu->disable();
  autocompmenu=MkMnuCmd(searchmenu,ID_AUTO_COMPLETE);
  autocompmenu->disable();


  viewmenu=new TopMenuPane(this);
  NewTitle(menubar,_("&View"),NULL,viewmenu);

  CreateLanguageMenu();
  CreateTabsMenu();
  CreateZoomMenu();

  new FXMenuSeparator(viewmenu, 0);

  status_chk =   MkMnuChk(viewmenu, ID_SHOW_STATUSBAR,  prefs->ShowStatusBar);
  outpane_chk =  MkMnuChk(viewmenu, ID_SHOW_OUTLIST,    prefs->ShowOutputPane);
  linenums_chk = MkMnuChk(viewmenu, ID_SHOW_LINENUMS,   prefs->ShowLineNumbers);
  toolbar_chk =  MkMnuChk(viewmenu, ID_SHOW_TOOLBAR,   prefs->ShowToolbar);

  new FXMenuSeparator(viewmenu, 0);

  margin_chk =   MkMnuChk(viewmenu, ID_SHOW_MARGIN,     prefs->ShowRightEdge);
  guides_chk =   MkMnuChk(viewmenu, ID_SHOW_INDENT,     prefs->ShowIndentGuides);
  white_chk =    MkMnuChk(viewmenu, ID_SHOW_WHITESPACE, prefs->ShowWhiteSpace);
  invert_chk =   MkMnuChk(viewmenu, ID_INVERT_COLORS,   prefs->InvertColors);
  caretline_chk = MkMnuChk(viewmenu, ID_SHOW_CARET_LINE, prefs->ShowCaretLine);

  new FXMenuSeparator(viewmenu, 0);

  MkMnuCmd(viewmenu,ID_CYCLE_SPLITTER);
  MkMnuCmd(viewmenu,ID_CLEAR_OUTPUT);

  toolsmenu=new TopMenuPane(this);
  NewTitle(menubar,_("&Tools"),NULL,toolsmenu);

  filterselmenu=MkMnuCmd(toolsmenu,ID_FILTER_SEL);
  MkMnuCmd(toolsmenu,ID_INSERT_CMD_OUT);
  MkMnuCmd(toolsmenu,ID_RUN_COMMAND);

  recordermenu=new SubMenuPane(this);
  NewCascade(toolsmenu,_("Macro &recorder"),NULL,recordermenu);

  recorderstartmenu=MkMnuCmd(recordermenu,ID_MACRO_RECORD);
  playbackmenu=MkMnuCmd(recordermenu,ID_MACRO_PLAYBACK);
  playbackmenu->disable();
  showmacromenu=MkMnuCmd(recordermenu,ID_MACRO_TRANSLATE);
  showmacromenu->disable();
  new FXMenuSeparator(toolsmenu, 0);

  MkMnuCmd(toolsmenu,ID_RESCAN_USER_MENU);
  MkMnuCmd(toolsmenu,ID_CONFIGURE_TOOLS);
  new FXMenuSeparator(toolsmenu, 0);

  FXString scriptdir;
  scriptdir.format("%s%s", ConfigDir().text(), "tools");
  FXDir::create(scriptdir);

  scriptdir.format("%s%s%c%s", ConfigDir().text(), "tools", PATHSEP, "commands");
  FXDir::create(scriptdir);
  usercmdmenu=new UserMenu(toolsmenu, _("&Commands"), scriptdir, this, ID_USER_COMMAND, usercmdflags);
  usercmdmenu->helpText(_("Commands: Execute a command and send its output to the output pane."));

  scriptdir.format("%s%s%c%s", ConfigDir().text(), "tools", PATHSEP, "filters");
  FXDir::create(scriptdir);
  userfiltermenu=new UserMenu(toolsmenu, _("&Filters"), scriptdir, this, ID_USER_FILTER);
  userfiltermenu->helpText(_("Filters: Pipe selected text to a command and replace with its output."));

  scriptdir.format("%s%s%c%s", ConfigDir().text(), "tools", PATHSEP, "snippets");
  FXDir::create(scriptdir);
  usersnipmenu=new UserMenu(toolsmenu, _("&Snippets"), scriptdir, this, ID_USER_SNIPPET,usersnipflags);
  usersnipmenu->helpText(_("Snippets: Insert the text of a file or the output of a command."));

  scriptdir.format("%s%s%c%s", ConfigDir().text(), "tools", PATHSEP, "macros");
  FXDir::create(scriptdir);
  usermacromenu=new UserMenu(toolsmenu, _("&Macros"), scriptdir, this, ID_USER_MACRO);
  usermacromenu->helpText(_("Macros: Execute a Lua macro using the built-in interpreter."));

#if TEST_SOMETHING
  new FXMenuCommand(toolsmenu, _("Test something\tF2"), NULL, this, ID_TEST_SOMETHING);
#endif

  docmenu=new TopMenuPane(this);
  NewTitle(menubar,_("&Documents"),NULL,docmenu);

  MkMnuCmd(docmenu,ID_TAB_NEXT);
  MkMnuCmd(docmenu,ID_TAB_PREV);

  tabordermenu=new SubMenuPane(this);
  NewCascade(docmenu,_("&Move"),NULL,tabordermenu);

  MkMnuCmd(tabordermenu,ID_TAB_TOFIRST);
  MkMnuCmd(tabordermenu,ID_TAB_TOLAST);
  MkMnuCmd(tabordermenu,ID_TAB_UP);
  MkMnuCmd(tabordermenu,ID_TAB_DOWN);

  FXMenuCommand*tmpcmd=new FXMenuCommand(tabordermenu,"foo",NULL,NULL,0);
  FXint dh=tmpcmd->getDefaultHeight();
  delete tmpcmd;
  getApp()->getRootWindow()->getDefaultHeight();
  doclistmenu=new FXScrollPane(docmenu,24);
  doclistmenu->setNumVisible((FXint)((getApp()->getRootWindow()->getDefaultHeight()/dh)*0.75));
  NewCascade(docmenu, _("&Select"), NULL, doclistmenu);

  new FXMenuSeparator(docmenu, 0);
  MkMnuCmd(docmenu,ID_FOCUS_OUTLIST);

  helpmenu=new TopMenuPane(this);
  NewTitle(menubar,_("&Help"),NULL,helpmenu /*,LAYOUT_RIGHT*/);

  MkMnuCmd(helpmenu,ID_SHOW_HELP);
  MkMnuCmd(helpmenu,ID_SHOW_LUA_HELP);
  new FXMenuSeparator(helpmenu, 0);
  MkMnuCmd(helpmenu,ID_HELP_ABOUT);
}



void TopWindow::DeleteMenus()
{
  delete usercmdmenu;
  delete userfiltermenu;
  delete usersnipmenu;
  delete usermacromenu;
  delete tabmenu;
  delete filemenu;
  delete doclistmenu;
  delete docmenu;
  delete tabordermenu;
  delete tabsidemenu;
  delete tabwidthmenu;
  delete editdeletemenu;
  delete editindentmenu;
  delete editmenu;
  delete searchselectmenu;
  delete searchmarkmenu;
  delete searchmenu;
  delete recordermenu;
  delete toolsmenu;
  delete helpmenu;
  delete fileexportmenu;
  delete langmenu;
  delete viewzoommenu;
  delete viewmenu;
}

