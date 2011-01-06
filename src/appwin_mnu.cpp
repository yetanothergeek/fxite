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


static const char *Scintilla_Copyright =" \
The Scintilla editing control and the PDF/HTML export code are\n\
copyright 1998-2011 by Neil Hodgson <neilh@scintilla.org>\n\
and are released under the following license:\n\
\n\
All Rights Reserved\n\
Permission to use, copy, modify, and distribute this software and its\n\
documentation for any purpose and without fee is hereby granted,\n\
provided that the above copyright notice appear in all copies and that\n\
both that copyright notice and this permission notice appear in\n\
supporting documentation.\n\
Neil Hodgson disclaims all warranties with regard to this\n\
software, including all implied warranties of merchantability\n\
and fitness, in no event shall Neil Hodgson be liable for any\n\
special, indirect or consequential damages or any damages\n\
whatsoever resulting from loss of use, data or profits,\n\
whether in an action of contract, negligence or other\n\
tortious action, arising out of or in connection with the use\n\
or performance of this software.\n\
\n\
For more information, visit http://scintilla.org/\
";

#ifdef FOX_1_6
// Fox 1.6 message box won't accept strings longer than ~1000 chars,
// so split the license across two dialogs...
static const char* Lua_License ="\
The Lua scripting engine is released under the following license:\n\
\n\
Copyright (C) 1994-2008 Lua.org, PUC-Rio.\n\
Permission is hereby granted, free of charge, to any person obtaining a copy\n\
of this software and associated documentation files (the \"Software\"), to deal\n\
in the Software without restriction, including without limitation the rights\n\
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n\
copies of the Software, and to permit persons to whom the Software is\n\
furnished to do so, subject to the following conditions:\n\
";
static const char* Lua_License_2 ="\
The preceeding copyright notice and this permission notice shall be included in\n\
all copies or substantial portions of the Software.\n\
The software is provided \"AS IS\", without warranty of any kind, express or\n\
implied, including but not limited to the warranties of merchantability,\n\
fitness for a particular purpose and noninfringement.  In no event shall the\n\
authors or copyright holders be liable for any claim, damages or other\n\
liability, whether in an action of contract, tort or otherwise, arising from,\n\
out of or in connection with the software or the use or other dealings in\n\
the software.\n\
\n\
For more information, visit http://www.lua.org/license.html .\
";
#else
static const char* Lua_License ="\
The Lua scripting engine is released under the following license:\n\
\n\
Copyright (C) 1994-2008 Lua.org, PUC-Rio.\n\
Permission is hereby granted, free of charge, to any person obtaining a copy\n\
of this software and associated documentation files (the \"Software\"), to deal\n\
in the Software without restriction, including without limitation the rights\n\
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n\
copies of the Software, and to permit persons to whom the Software is\n\
furnished to do so, subject to the following conditions:\n\
The above copyright notice and this permission notice shall be included in\n\
all copies or substantial portions of the Software.\n\
The software is provided \"AS IS\", without warranty of any kind, express or\n\
implied, including but not limited to the warranties of merchantability,\n\
fitness for a particular purpose and noninfringement.  In no event shall the\n\
authors or copyright holders be liable for any claim, damages or other\n\
liability, whether in an action of contract, tort or otherwise, arising from,\n\
out of or in connection with the software or the use or other dealings in\n\
the software.\n\
\n\
For more information, visit http://www.lua.org/license.html .\
";
#endif

static const char* Fox_LGPL="\
   This software uses the FOX Toolkit Library, released   \n\
under the GNU Lesser General Public License and \n\
the FOX Library License addendum.\n\
\n\
For more details, visit http://www.fox-toolkit.org\
";

#define App_About "\
Free eXtensIble Text Editor (FXiTe) %s %s\n\n\
Copyright (c) 2009,2011 Jeffrey Pohlmeyer\n\
<%s>\n\n\
%s\
GNU GENERAL PUBLIC LICENSE Version 3\n\n\
%s FOX-%d.%d.%d; FXScintilla-%s; %s\n\
"

void TopWindow::VersionInfo()
{
  fxmessage(App_About, _("Version"), VERSION, "yetanothergeek@gmail.com",
    _("This program is free software, under the terms of the\n"),
    _("Running"),
    fxversion[0],fxversion[1],fxversion[2],
    FXScintilla::version().text(),
    LUA_RELEASE);
  
}

void TopWindow::About()
{
  int i=0;
  while (1) {
    const char *btns[]={
      _(" About &Scintilla "),
      _(" About &Lua "),
      _(" About &FOX "),
      _(" &Close   "),NULL};
    const char*btn[]={_("    &Close    "), NULL};
    FxAsqWin*dlg=NULL;
    FXString msg;
    int rv=-1;
    dlg=new FxAsqWin(_("About "EXE_NAME), btns, i++);
    msg.format(App_About, _("Version"), VERSION, "yetanothergeek@gmail.com",
      _("  This program is free software, under the terms of the  \n"),
      _("Running"),
      fxversion[0],fxversion[1],fxversion[2],
      FXScintilla::version().text(),
      LUA_RELEASE);
    dlg->Label(msg.text(), JUSTIFY_CENTER_X);
    rv=dlg->Run(NULL);
    delete dlg;
    switch (rv) {
      case 0: {
        dlg=new FxAsqWin(_("About Scintilla"), btn);
        dlg->Label(Scintilla_Copyright);
        dlg->Run();
        delete dlg;
        break;
      }
      case 1: { 
        dlg=new FxAsqWin(_("About Lua"), btn);
        dlg->Label(Lua_License);
  #ifdef FOX_1_6 
        dlg->Label(Lua_License_2);
  #endif
        dlg->Run();
        delete dlg;
        break;
      }
      case 2: {
        dlg=new FxAsqWin(_("About FOX Toolkit"), btn);
        dlg->Label(Fox_LGPL,JUSTIFY_CENTER_X);
        dlg->Run();
        delete dlg;
        break;
      }
      default: { return; }
    }
  }
}



#define TBarOpts (FRAME_NONE|LAYOUT_FILL_X|PACK_UNIFORM_HEIGHT)

#define UsedWidthOf(f) ((f&&f->getLast())?(f->getLast()->getX()+f->getLast()->getWidth()):0)

// This thing behaves something like an FXHorizontalFrame that can "wrap" its contents across two rows.
class ToolBarFrame: public FXVerticalFrame {
private:
  FXDECLARE(ToolBarFrame);
  ToolBarFrame(){}
  FXHorizontalFrame*rows[2];
  bool wraptoolbar;
public:
  long onConfigure(FXObject*o,FXSelector sel,void*p) {
    FXVerticalFrame::onConfigure(o,sel,p);
    reconf();
    return 1;
  }
  void reconf() { // wrap or unwrap buttons as needed.
    bool prefwrap=Settings::instance()->WrapToolbar;
    if ((!prefwrap)&&(!wraptoolbar)) { return; }
    FXint kids1=rows[0]->numChildren();
    FXint kids2=rows[1]->numChildren();
    if ((prefwrap)&&(kids1>0)&&(kids2==0)&&(width<UsedWidthOf(rows[0]))) {
      FXWindow *topright=rows[0]->childAtIndex((kids1/2));
      if (topright && (kids1%2==0)) { topright=topright->getPrev(); }
      while ( rows[0]->getLast() && (rows[0]->getLast() != topright) ) {
        rows[0]->getLast()->reparent(rows[1], rows[1]->getFirst());
      }
    } else if ( (kids2>0) && ((!prefwrap)||(width>(UsedWidthOf(rows[0])+UsedWidthOf(rows[1])))) ) {
      while (rows[1]->getFirst()) {
        rows[1]->getFirst()->reparent(rows[0],NULL);
      }
    }
    if (rows[1]->numChildren()) { rows[1]->show(); } else { rows[1]->hide(); }
    wraptoolbar=prefwrap;
    normalize();
  }
  void normalize() { // make all buttons the same width
    if (rows[0]->numChildren()==0) { return; }
    FXint wdt=0;
    for (FXWindow*row=getFirst(); row; row=row->getNext()) {
      for (FXWindow*btn=row->getFirst(); btn; btn=btn->getNext()) {
        btn->setLayoutHints(btn->getLayoutHints()&~LAYOUT_FIX_WIDTH);
      }
    }
    layout();
    for (FXWindow*row=getFirst(); row; row=row->getNext()) {
      for (FXWindow*btn=row->getFirst(); btn; btn=btn->getNext()) {
        FXint w=btn->getWidth();
        if (w>wdt) { wdt=w; }
      }
    }
    for (FXWindow*row=getFirst(); row; row=row->getNext()) {
      for (FXWindow*btn=row->getFirst(); btn; btn=btn->getNext()) {
        btn->setLayoutHints(btn->getLayoutHints()|LAYOUT_FIX_WIDTH);
        btn->setWidth(wdt);
      }
    }
    layout();
  }
  ToolBarFrame(FXComposite *o):FXVerticalFrame(o,TBarOpts,0,0,0,0,0,0,0,0,1,1) {
    wraptoolbar=Settings::instance()->WrapToolbar;
    rows[0]=new FXHorizontalFrame(this,TBarOpts,0,0,0,0,0,0,0,0,1,1);
    rows[1]=new FXHorizontalFrame(this,TBarOpts,0,0,0,0,0,0,0,0,1,1);
    rows[1]->hide();
  }
};



FXDEFMAP(ToolBarFrame)ToolBarFrameMap[]={
  FXMAPFUNC(SEL_CONFIGURE, 0, ToolBarFrame::onConfigure)
};

FXIMPLEMENT(ToolBarFrame,FXVerticalFrame,ToolBarFrameMap,ARRAYNUMBER(ToolBarFrameMap));


/* Class that makes tool bar buttons restore document focus after they are clicked */
class ToolBarBtn: public FXButton {
  FXDECLARE(ToolBarBtn)
  ToolBarBtn(){}
public:
  ToolBarBtn(FXComposite* p, const FXString& text, FXObject*tgt, FXSelector sel):
      FXButton(p,text,NULL,tgt,sel,FRAME_RAISED|JUSTIFY_NORMAL,0,0,24,24,2,2,0,0) {}
  long onLeftBtnRelease(FXObject*o, FXSelector sel, void*p ) {
    long rv=FXButton::onLeftBtnRelease(o,sel,p);
    getApp()->addChore(TopWindow::instance(),TopWindow::ID_FOCUS_DOC,NULL);
    return rv;
  }
};

FXDEFMAP(ToolBarBtn) ToolBarBtnMap[] = {
  FXMAPFUNC(SEL_LEFTBUTTONRELEASE,0,ToolBarBtn::onLeftBtnRelease),
};

FXIMPLEMENT(ToolBarBtn,FXButton,ToolBarBtnMap,ARRAYNUMBER(ToolBarBtnMap))



/* Class that makes tool bar toggle buttons restore document focus after they are clicked */
class ToolBarTogBtn: public FXToggleButton {
  FXDECLARE(ToolBarTogBtn)
  ToolBarTogBtn(){}
public:
  ToolBarTogBtn(FXComposite* p, const FXString& text, FXObject*tgt, FXSelector sel):
      FXToggleButton( p,text,text,NULL,NULL,tgt,sel,
                        FRAME_RAISED|JUSTIFY_NORMAL|TOGGLEBUTTON_KEEPSTATE,0,0,24,24,2,2,0,0) {}
  long onLeftBtnRelease(FXObject*o, FXSelector sel, void*p ) {
    long rv=FXToggleButton::onLeftBtnRelease(o,sel,p);
    getApp()->addChore(TopWindow::instance(),TopWindow::ID_FOCUS_DOC,NULL);
    return rv;
  }
};

FXDEFMAP(ToolBarTogBtn) ToolBarTogBtnMap[] = {
  FXMAPFUNC(SEL_LEFTBUTTONRELEASE,0,ToolBarTogBtn::onLeftBtnRelease),
};

FXIMPLEMENT(ToolBarTogBtn,FXToggleButton,ToolBarTogBtnMap,ARRAYNUMBER(ToolBarTogBtnMap))



void TopWindow::UpdateToolbarWrap()
{
  ((ToolBarFrame*)toolbar_frm)->reconf();
}



void TopWindow::SetTBarBtnFontCB(FXButton*btn, void*user_data)
{
  TopWindow*tw=(TopWindow*)user_data;
  if (!tw->toolbar_font) {
    FXFontDesc dsc;
    GetFontDescription(dsc, tw->getApp()->getNormalFont());
    FXfloat scale=1.0;
    switch (tw->prefs->ToolbarButtonSize) {
      case 0:{ scale=0.75; break; }
      case 1:{ scale=0.90; break; }
      case 2:{ scale=1.00; break; }
    }
    dsc.size=FXushort(((FXfloat)dsc.size)*scale);
    tw->toolbar_font=new FXFont(tw->getApp(),dsc);
  }
  btn->setFont(tw->toolbar_font);
  btn->destroy();
  if (tw->shown()) {
    btn->create();
  }
  if (btn->getNext()==NULL) {
    ((ToolBarFrame*)(tw->toolbar_frm))->normalize();
    ((ToolBarFrame*)(tw->toolbar_frm))->reconf();
  }
}



void TopWindow::ClearTBarBtnDataCB(FXButton*btn, void*user_data)
{
  MenuSpec*spec=(MenuSpec*)btn->getUserData();
  if (spec&&spec->ms_mc&&(spec->type!='u')) { spec->ms_mc->setUserData(NULL); }
}



#define EngageBtn(s,t) ((t*)btn)->setState(s?((t*)btn)->getState()|STATE_ENGAGED:((t*)btn)->getState()&~STATE_ENGAGED)

void TopWindow::UpdateToolbar()
{
  ForEachToolbarButton(ClearTBarBtnDataCB,NULL);
  for (FXWindow*hfrm=toolbar_frm->getFirst(); hfrm; hfrm=hfrm->getNext()) {
    while (hfrm->numChildren()) { delete hfrm->getFirst(); }
  }
  for (FXint i=0; i<TBAR_MAX_BTNS; i++) {
    MenuSpec* spec=MenuMgr::LookupMenu(MenuMgr::TBarBtns()[i]);
    if (spec && spec->sel<ID_LAST) {
      FXString txt=spec->btn_txt;
      txt.substitute(' ','\n',true);
      FXLabel*btn;
      if ((spec->type=='k')||(spec->sel==ID_MACRO_RECORD)) {
        btn=new ToolBarTogBtn( (FXComposite*)(toolbar_frm->getFirst()),txt,this,spec->sel);
        if (spec->sel==ID_MACRO_RECORD) {
          ((FXToggleButton*)btn)->setState(recording!=NULL);
        }
      } else {
        btn=new ToolBarBtn( (FXComposite*)(toolbar_frm->getFirst()),txt,this,
                       (spec->type=='u')?ID_TBAR_CUSTOM_CMD:spec->sel );
        switch (spec->sel) { // These menus might be in a disabled state...
          case ID_MACRO_PLAYBACK:
          case ID_MACRO_TRANSLATE:
          case ID_FILTER_SEL:
          case ID_FIND_TAG:
          case ID_SHOW_CALLTIP:
          case ID_FMT_DOS:
          case ID_FMT_MAC:
          case ID_FMT_UNIX: {
            SetMenuEnabled(spec->ms_mc, spec->ms_mc->isEnabled());
            break;
          }
        }
      }
      if (spec->ms_mc) {
        if (spec->type!='u') { spec->ms_mc->setUserData((void*)btn); }
        btn->setUserData(spec);
        switch(spec->type) {
          case 'k': { EngageBtn(((FXMenuCheck*)(spec->ms_mc))->getCheck(),FXToggleButton); break; }
          case 'r': { EngageBtn(((FXMenuRadio*)(spec->ms_mc))->getCheck(),FXButton); break; }
        }
      }
      btn->setBackColor(HexToRGB(MenuMgr::TBarColors(i)));
      btn->setTextColor(HexToRGB("#000000"));
      FXString tip;
      MenuMgr::GetTBarBtnTip(spec,tip);
      btn->setTipText(tip);
    } else {
      break;
    }
  }
  ForEachToolbarButton(SetTBarBtnFontCB,this);
  toolbar_frm->layout();
  if (FocusedDoc()) { EnableUserFilters(FocusedDoc()->GetSelLength()>0); }
}



void TopWindow::ForEachToolbarButton(void (*cb)(FXButton*btn, void*user_data), void*user_data) {
  for (FXWindow*hfrm=toolbar_frm->getFirst(); hfrm; hfrm=hfrm->getNext()) {
    for (FXWindow*btn=hfrm->getFirst(); btn; btn=btn->getNext()) {
      cb((FXButton*)btn, user_data);
    }
  }
}



void TopWindow::CreateToolbar()
{
  toolbar_frm=new ToolBarFrame(vbox);
  if (!prefs->ShowToolbar) { toolbar_frm->hide(); }
}

