/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2009-2014 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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

#include <cerrno>
#include "compat.h"
#include "scidoc.h"
#include "appwin.h"
#include "appwin_pub.h"
#include "popmenu.h"
#include "shmenu.h"


#include "intl.h"

#include "menudefs.h"


#ifdef WIN32
# define QUIT_CMD "Alt+F4"
#else
# define QUIT_CMD "Ctrl+Q"
#endif



/*
To add a new menu command:
  1. Declare an event handler and an ID_* enum value in "appwin.h"
  2. Map the method to the enum value in the TopWindowMap[] in "appwin.cpp"
  3. Implement the event handler in "appwin.cpp"
  4. Add a new menu specification to the MnuSpecs[] list in "menudefs.cpp".
  5. Add a new menu item in CreateMenus() in "appwin_mnu.cpp" ( using MkMnuCmd() MkMnuChk() or MkMnuRad() )
*/


#define TW TopWindow
static MenuSpec MnuSpecs[] = {
  {TW::ID_NEW,              "FileNew",         _("&New"),               _("new file"),     "Ctrl+N",       'm', {NULL}},
  {TW::ID_OPEN_FILES,       "FileOpenFile",    _("&Open Files..."),     _("open files"),   "Ctrl+O",       'm', {NULL}},
  {TW::ID_OPEN_SELECTED,    "FileOpenSel",     _("Open Selecte&d"),     _("open sel"),     "Ctrl+Y",       'm', {NULL}},
  {TW::ID_SAVE,             "FileSave",        _("&Save"),              _("save file"),    "Ctrl+S",       'm', {NULL}},
  {TW::ID_SAVEAS,           "FileSaveAs",      _("Save &As..."),        _("save as"),      "Ctrl+Shift+S", 'm', {NULL}},
  {TW::ID_SAVEALL,          "FileSaveAll",     _("Save All"),           _("save all"),     "",             'm', {NULL}},
  {TW::ID_SAVECOPY,         "FileSaveCopy",    _("Save a co&py..."),    _("save copy"),    "",             'm', {NULL}},
  {TW::ID_EXPORT_PDF,       "FileExportPdf",   _("As &PDF..."),         _("exp pdf"),      "",             'm', {NULL}},
  {TW::ID_EXPORT_HTML,      "FileExportHtml",  _("As &HTML..."),        _("exp html"),     "",             'm', {NULL}},
  {TW::ID_RELOAD,           "FileReload",      _("&Reload"),            _("reload"),       "Ctrl+F5",      'm', {NULL}},
  {TW::ID_CLOSE,            "FileClose",       _("&Close"),             _("close file"),   "Ctrl+W",       'm', {NULL}},
  {TW::ID_CLOSEALL,         "FileCloseAll",    _("Close All"),          _("close all"),    "Ctrl+Shift+W", 'm', {NULL}},
  {TW::ID_INSERT_FILE,      "FileInsert",      _("&Insert file..."),    _("insert file"),  "",             'm', {NULL}},
  {TW::ID_LOAD_TAGS,        "FileLoadTags",    _("Load ta&gs file..."), _("load tags"),    "",             'm', {NULL}},
  {TW::ID_SELECT_DIR,       "FileSelectDir",   _("&Working Directory..."),_("work dir"),   "",             'm', {NULL}},
  {TW::ID_QUIT,             "FileQuit",        _("&Quit"),              _("quit"),         QUIT_CMD,       'm', {NULL}},
  {TW::ID_UNDO,             "EditUndo",        _("&Undo"),              _("undo"),         "Ctrl+Z",       'm', {NULL}},
  {TW::ID_REDO,             "EditRedo",        _("&Redo"),              _("redo"),         "Ctrl+Shift+Z", 'm', {NULL}},
  {TW::ID_CUT,              "EditCut",         _("Cu&t"),               _("cut"),          "Ctrl+X",       'm', {NULL}},
  {TW::ID_COPY,             "EditCopy",        _("&Copy"),              _("copy"),         "Ctrl+C",       'm', {NULL}},
  {TW::ID_PASTE,            "EditPaste",       _("&Paste"),             _("paste"),        "Ctrl+V",       'm', {NULL}},
  {TW::ID_DEL_WORD_LEFT,    "EditDelWordLeft", _("Word &Left"),         _("word bksp"),    "Ctrl+Back",    'm', {NULL}},
  {TW::ID_DEL_WORD_RIGHT,   "EditDelWordRight",_("Word &Right"),        _("word del"),     "Ctrl+Del",     'm', {NULL}},
  {TW::ID_DEL_LINE_LEFT,    "EditDelLineStart",_("To Line &Start"),     _("line bksp"),    "Ctrl+Shift+Back",'m',{NULL}},
  {TW::ID_DEL_LINE_RIGHT,   "EditDelLineEnd",  _("To Line &End"),       _("line trunc"),   "Ctrl+Shift+Del",'m',{NULL}},
  {TW::ID_TOLOWER,          "EditLower",       _("Lo&wer Case"),        _("lower"),        "Ctrl+Shift+^", 'm', {NULL}},
  {TW::ID_TOUPPER,          "EditUpper",       _("Upp&er Case"),        _("upper"),        "Ctrl+6",       'm', {NULL}},
  {TW::ID_INDENT_STEP,      "EditSpaceIn",     _("&Space right"),       _("space right"),  "Ctrl+0",       'm', {NULL}},
  {TW::ID_UNINDENT_STEP,    "EditUnspace",     _("Spa&ce left"),        _("space left"),   "Ctrl+9",       'm', {NULL}},
  {TW::ID_INDENT_FULL,      "EditTabIn",       _("&Indent right  "),    _("shift right"),  "Ctrl+Shift+)", 'm', {NULL}},
  {TW::ID_UNINDENT_FULL,    "EditUntab",       _("In&dent left   "),    _("shift left"),   "Ctrl+Shift+(", 'm', {NULL}},
  {TW::ID_READONLY,         "EditReadOnly",    _("Read Onl&y"),         _("read only"),    "",             'm', {NULL}},
  {TW::ID_WORDWRAP,         "EditWordWrap",    _("Word Wr&ap"),         _("word wrap"),    "",             'm', {NULL}},
  {TW::ID_FMT_DOS,          "EditFmtDos",      _("&DOS [CR/LF]"),       _("dos"),          "",             'm', {NULL}},
  {TW::ID_FMT_MAC,          "EditFmtMac",      _("&Mac [CR]"),          _("mac"),          "",             'm', {NULL}},
  {TW::ID_FMT_UNIX,         "EditFmtUnix",     _("&Unix [LF]"),         _("unix"),         "",             'm', {NULL}},
  {TW::ID_PREFS_DIALOG,     "EditPrefs",       _("Prefere&nces..."),    _("prefs"),        "Ctrl+P",       'm', {NULL}},
  {TW::ID_FIND,             "SearchFind",      _("&Find..."),           _("find"),         "Ctrl+F",       'm', {NULL}},
  {TW::ID_FINDNEXT,         "SearchFindNext",  _("Find &Next   "),      _("next"),         "Ctrl+G",       'm', {NULL}},
  {TW::ID_FINDPREV,         "SearchFindPrev",  _("Find &Prev   "),      _("prev"),         "Ctrl+Shift+G", 'm', {NULL}},
  {TW::ID_NEXT_SELECTED,    "SearchNextSel",   _("&Next"),              _("next sel"),     "Ctrl+H",       'm', {NULL}},
  {TW::ID_PREV_SELECTED,    "SearchPrevSel",   _("&Prev   "),           _("prev sel"),     "Ctrl+Shift+H", 'm', {NULL}},
  {TW::ID_REPLACE_IN_DOC,   "SearchReplace",   _("&Replace... "),       _("repl"),         "Ctrl+R",       'm', {NULL}},
  {TW::ID_GOTO,             "SearchGoTo",      _("&Go to..."),          _("goto"),         "Ctrl+L",       'm', {NULL}},
  {TW::ID_GOTO_SELECTED,    "SearchGoToSel",   _("Go to selecte&d"),    _("goto sel"),     "Ctrl+E",       'm', {NULL}},
  {TW::ID_GOTO_ERROR,       "SearchGoToError", _("Go to &error"),       _("goto err"),     "Ctrl+Shift+E", 'm', {NULL}},
  {TW::ID_BOOKMARK_SET,     "SearchMarkSet",   _("&Set"),               _("set mark"),     "Ctrl+M",       'm', {NULL}},
  {TW::ID_BOOKMARK_RETURN,  "SearchMarkReturn",_("&Return"),            _("goto mark"),    "Ctrl+Shift+M", 'm', {NULL}},
  {TW::ID_FIND_TAG,         "SearchFindDef",   _("Find &Definition"),   _("find tag"),     "Ctrl+D",       'm', {NULL}},
  {TW::ID_SHOW_CALLTIP,     "SearchShowTip",   _("Show &Calltip"),      _("show tip"),     "Ctrl+'",       'm', {NULL}},
  {TW::ID_AUTO_COMPLETE,    "SearchShowComp",  _("Show Comple&tions"),  _("auto comp"),    "Alt+'",        'm', {NULL}},
  {TW::ID_SHOW_STATUSBAR,   "ViewStatusBar",   _("&Status bar"),        _("stats bar"),    "",             'm', {NULL}},
  {TW::ID_SHOW_LINENUMS,    "ViewLineNumbers", _("Line &numbers"),      _("line nums"),    "",             'm', {NULL}},
  {TW::ID_SHOW_TOOLBAR,     "ViewToolbar",     _("Tool&bar"),           _("hide"),         "",             'm', {NULL}},
  {TW::ID_SHOW_WHITESPACE,  "ViewWhiteSpace",  _("&White space"),       _("white space"),  "",             'm', {NULL}},
  {TW::ID_SHOW_MARGIN,      "ViewRightMargin", _("Right &Margin"),      _("show margin"),  "",             'm', {NULL}},
  {TW::ID_SHOW_INDENT,      "ViewIndent",      _("Indent &Guides"),     _("show guides"),  "",             'm', {NULL}},
  {TW::ID_SHOW_CARET_LINE,  "ViewCaretLine",   _("&Caret Line"),        _("caret line"),   "",             'm', {NULL}},
  {TW::ID_SHOW_OUTLIST,     "ViewOutputPane",  _("&Output pane"),       _("show panel"),   "",             'm', {NULL}},
  {TW::ID_INVERT_COLORS,    "ViewInvertColors",_("&Inverted colors"),   _("invert color"), "",             'm', {NULL}},
  {TW::ID_TABS_TOP,         "ViewTabsTop",     _("&Top"),               _("tabs top"),     "",             'm', {NULL}},
  {TW::ID_TABS_BOTTOM,      "ViewTabsBottom",  _("&Bottom"),            _("tabs btm"),     "",             'm', {NULL}},
  {TW::ID_TABS_LEFT,        "ViewTabsLeft",    _("&Left"),              _("tabs lft"),     "",             'm', {NULL}},
  {TW::ID_TABS_RIGHT,       "ViewTabsRight",   _("&Right"),             _("tabs rgt"),     "",             'm', {NULL}},
  {TW::ID_TABS_UNIFORM,     "ViewTabsUniform", _("&Uniform"),           _("tabs equal"),   "",             'm', {NULL}},
  {TW::ID_TABS_COMPACT,     "ViewTabsPacked",  _("&Packed"),            _("tabs pack"),    "",             'm', {NULL}},
  {TW::ID_TABS_BY_POS,      "ViewTabsAuto",    _("&Automatic"),         _("tabs auto"),    "",             'm', {NULL}},
  {TW::ID_ZOOM_IN,          "ViewZoomIn",      _("&In "),               _("zoom in"),      "Ctrl+=",       'm', {NULL}},
  {TW::ID_ZOOM_OUT,         "ViewZoomOut",     _("&Out "),              _("zoom out"),     "Ctrl+-",       'm', {NULL}},
  {TW::ID_ZOOM_NONE,        "ViewZoomDef",     _("&Default "),          _("zoom none"),    "Ctrl+1",       'm', {NULL}},
  {TW::ID_ZOOM_NEAR,        "ViewZoomNear",    _("&Closest "),          _("zoom near"),    "Ctrl+Shift++", 'm', {NULL}},
  {TW::ID_ZOOM_FAR,         "ViewZoomFar",     _("&Furthest "),         _("zoom far"),     "Ctrl+Shift+_", 'm', {NULL}},
  {TW::ID_CYCLE_SPLITTER,   "ViewSplitView",   _("Split &View"),        _("split view"),   "Ctrl+2",       'm', {NULL}},
  {TW::ID_CLEAR_OUTPUT,     "ViewClearOutput", _("&Clear Output"),      _("clear pane"),   "Ctrl+Shift+L", 'm', {NULL}},
  {TW::ID_FILTER_SEL,       "ToolsFilterSel",  _("&Filter selection"),  _("filter sel"),   "Alt+R",        'm', {NULL}},
  {TW::ID_INSERT_CMD_OUT,   "ToolsInsert",     _("&Insert command"),    _("insert cmd"),   "Alt+I",        'm', {NULL}},
  {TW::ID_RUN_COMMAND,      "ToolsExecute",    _("&Execute command"),   _("exec cmd"),     "Alt+X",        'm', {NULL}},
  {TW::ID_MACRO_RECORD,     "ToolsRecMacro",   _("Re&cord macro"),      _("record macro"), "Alt+K",        'm', {NULL}},
  {TW::ID_MACRO_PLAYBACK,   "ToolsPlayMacro",  _("&Play macro"),        _("play macro"),   "Ctrl+K",       'm', {NULL}},
  {TW::ID_MACRO_TRANSLATE,  "ToolsShowMacro",  _("&Show macro"),        _("show macro"),   "Ctrl+Shift+K", 'm', {NULL}},
  {TW::ID_CONFIGURE_TOOLS,  "ToolsCustomize",  _("Customi&ze menu..."), _("edit tools"),   "",             'm', {NULL}},
  {TW::ID_RESCAN_USER_MENU, "ToolsRebuild",    _("Re&build menu"),      _("update tools"), "",             'm', {NULL}},
  {TW::ID_TAB_NEXT,         "DocsFocusNext",   _("&Next"),              _("next doc"),     "Ctrl+PgDn",    'm', {NULL}},
  {TW::ID_TAB_PREV,         "DocsFocusPrev",   _("&Prev"),              _("prev doc"),     "Ctrl+PgUp",    'm', {NULL}},
  {TW::ID_TAB_TOFIRST,      "DocsMoveToFirst", _("&First"),             _("move first"),   "",             'm', {NULL}},
  {TW::ID_TAB_TOLAST,       "DocsMoveToLast",  _("&Last"),              _("move last"),    "",             'm', {NULL}},
  {TW::ID_TAB_UP,           "DocsMoveUp",      _("&Up (Left)"),         _("move left"),    "",             'm', {NULL}},
  {TW::ID_TAB_DOWN,         "DocsMoveDown",    _("&Down (Right)"),      _("move right"),   "",             'm', {NULL}},
  {TW::ID_FOCUS_OUTLIST,    "DocsFocusPane",   _("&Output Pane"),       _("focus panel"),  "F4",           'm', {NULL}},
  {TW::ID_SHOW_HELP,        "HelpHelp",        _("&Help..."),           _("help"),         "F1",           'm', {NULL}},
  {TW::ID_SHOW_LUA_HELP,    "HelpMacro",       _("&Macro help..."),     _("macro help"),   "",             'm', {NULL}},
  {TW::ID_HELP_ABOUT,       "HelpAbout",       _("&About..."),          _("about"),        "",             'm', {NULL}},
  {TW::ID_POPUP_SELECT_ALL, "PopupSelectAll",  _("Select &All"),        _("sel all"),      "",             'm', {NULL}},
  {TW::ID_POPUP_DELETE_SEL, "PopupDeleteSel",  _("&Delete"),            _("del sel"),      "",             'm', {NULL}},
  {TW::ID_KILL_COMMAND,     "KillCommand",     "\0",                    NULL,              "Ctrl+.",       'm', {NULL}},
  {TW::ID_LAST,             "\0",              "\0",                    NULL,              "\0",           'm', {NULL}}
};



/*
  This array is used to create the buttons on the toolbar -
  If an element contains a positive integer, it will be passed directly
  to the button's constructor method as the the FXSelector "sel" argument.
  Th special case of ID_LAST is used as semamphore, it means the remaining buttons are unused.
  If the value is negative, it contains a unique identifier that should match
  the "sel" field for one of the MenuSpec elements in the custom_commands[] array,
  and a button will be created that calls the handler for the FXMenuCommand
  referenced by that MenuSpec's ms_mc field.
  The initial values here are simply reasonable defaults for a "typical" toolbar.
*/
static FXint ToolbarButtons[TBAR_MAX_BTNS+1]= {
  TW::ID_NEW,
  TW::ID_OPEN_FILES,
  TW::ID_SAVE,
  TW::ID_SAVEAS,
  TW::ID_CUT,
  TW::ID_COPY,
  TW::ID_PASTE,
  TW::ID_UNDO,
  TW::ID_REDO,
  TW::ID_FIND,
  TW::ID_NEXT_SELECTED,
  TW::ID_PREV_SELECTED,
  TW::ID_BOOKMARK_SET,
  TW::ID_BOOKMARK_RETURN,
  TW::ID_INDENT_FULL,
  TW::ID_UNINDENT_FULL,
  TW::ID_LAST,
  TW::ID_LAST,
  TW::ID_LAST,
  TW::ID_LAST,
  TW::ID_LAST,
  TW::ID_LAST,
  TW::ID_LAST,
  TW::ID_LAST,
  TW::ID_LAST,
};


FXint* GetToolbarButtons() {
  return ToolbarButtons;
}


void MyMenuMgr::UpdateEolMenu(SciDoc*sci)
{
  switch (sci->sendMessage(SCI_GETEOLMODE,0,0)) {
    case SC_EOL_CRLF: { RadioUpdate(TW::ID_FMT_DOS,  TW::ID_FMT_DOS, TW::ID_FMT_UNIX);  break; }
    case SC_EOL_CR:   { RadioUpdate(TW::ID_FMT_MAC,  TW::ID_FMT_DOS, TW::ID_FMT_UNIX);  break; }
    case SC_EOL_LF:   { RadioUpdate(TW::ID_FMT_UNIX, TW::ID_FMT_DOS, TW::ID_FMT_UNIX); break; }
  }
}



void MyMenuMgr::SetFileFormat(SciDoc*sci, FXSelector sel)
{
  long EolMode=SC_EOL_LF;
  switch (sel) {
    case TW::ID_FMT_DOS:{
      EolMode=SC_EOL_CRLF;
      break;
    }
    case TW::ID_FMT_MAC:{
      EolMode=SC_EOL_CR;
      break;
    }
    case TW::ID_FMT_UNIX:{
      EolMode=SC_EOL_LF;
      break;
    }
  }
  sci->sendMessage(SCI_SETEOLMODE,EolMode,0);
  sci->sendMessage(SCI_CONVERTEOLS,EolMode,0);
  RadioUpdate(sel,TW::ID_FMT_DOS,TW::ID_FMT_UNIX);
}



char MyMenuMgr::SetTabOrientation(FXSelector sel)
{
  RadioUpdate(sel, TW::ID_TABS_TOP, TW::ID_TABS_RIGHT);
  switch(sel){
    case TW::ID_TABS_TOP:    return 'T';
    case TW::ID_TABS_BOTTOM: return 'B';
    case TW::ID_TABS_LEFT:   return 'L';
    case TW::ID_TABS_RIGHT:  return 'R';
    default: return 'T';
  }
}


static const char* default_popup_commands[] = {
  "EditUndo",
  "EditRedo",
  "",
  "EditCut",
  "EditCopy",
  "EditPaste",
  "PopupDeleteSel",
  "",
  "PopupSelectAll",
  NULL
};


static char* popup_commands[POPUP_MAX_CMDS];


MyMenuMgr::MyMenuMgr():MenuMgr(MnuSpecs,ToolbarButtons,TW::ID_LAST, default_popup_commands,popup_commands,TW::instance()->ConfigDir().text())
{
  
}


void MyMenuMgr::ShowPopupMenu(FXPoint*pt)
{
  TopWindowBase*tw=TopWindowBase::instance();
  SciDoc*sci=tw->FocusedDoc();
  FXMenuPane *mnu=new FXMenuPane(tw);
  static FXint toolpathlen=tw->ConfigDir().length()+6;
  for (char**pref=popup_commands; *pref; pref++) {
    if ((*pref)[0]) {
      if (strchr(*pref,PATHSEP)) {
        if (FXStat::isFile(*pref)) {
          FXString label;
          FXSelector sel=0;
          const FXchar *subdir=(*pref)+toolpathlen;
          switch (subdir[0]) {
            case 'c': {
              if ((strncmp(subdir,"commands",8)==0)&&(subdir[8]==PATHSEP)) { sel=TopWindow::ID_USER_COMMAND; }
              break;
            }
            case 'f': {
              if ((strncmp(subdir,"filters",7)==0)&&(subdir[7]==PATHSEP)) { sel=TopWindow::ID_USER_FILTER; }
              break;
            }
            case 'm': {
              if ((strncmp(subdir,"macros",6)==0)&&(subdir[6]==PATHSEP)) { sel=TopWindow::ID_USER_MACRO; }
              break;
            }
            case 's': {
              if ((strncmp(subdir,"snippets",8)==0&&(subdir[8]==PATHSEP))) { sel=TopWindow::ID_USER_SNIPPET; }
              break;
            }
          }
          if (sel && UserMenu::MakeLabelFromPath(*pref, label)) {
            FXMenuCommand*mc = new PopUpMnuCmd(mnu,label,NULL,tw,sel);
            mc->setUserData((void*)(*pref));
            if ((sel==TopWindow::ID_USER_FILTER)&&(!sci->GetSelLength())) { mc->disable(); }
          }
        }
      } else {
        MenuSpec* spec=LookupMenuByPref(*pref);
        if (spec) {
          FXMenuCommand*mc = new PopUpMnuCmd(mnu,spec->mnu_txt,NULL,tw,spec->sel);
          switch (spec->sel) {
            case TopWindow::ID_UNDO:{
              if (!sci->sendMessage(SCI_CANUNDO,0,0)) { mc->disable(); }
              break;
            }
            case TopWindow::ID_REDO:{
              if (!sci->sendMessage(SCI_CANREDO,0,0)) { mc->disable(); }
              break;
            }
            case TopWindow::ID_PASTE:{
              if (!sci->sendMessage(SCI_CANPASTE,0,0)) { mc->disable(); }
              break;
            }
            case TopWindow::ID_CUT:
            case TopWindow::ID_COPY:
            case TopWindow::ID_POPUP_DELETE_SEL:{
              if (!sci->GetSelLength()) { mc->disable(); }
              break;
            }
          }
        }
      }
    } else {
      new FXMenuSeparator(mnu);
    }
  }
  mnu->create();
  mnu->show();
  mnu->popup(NULL,pt->x,pt->y);
  mnu->grabKeyboard();
  sci->getApp()->runModalWhileShown(mnu);
  delete mnu;
}



bool MyToolsDialog::CanRename(const FXString &oldpath, const FXString &newpath)
{
  bool isdir=FXStat::isDirectory(oldpath);
  bool dirty=false;
  filelist=FXString::null;
  if (isdir) {
    FXString oldprefix=oldpath+PATHSEPSTRING;
    FXString newprefix=newpath+PATHSEPSTRING;
    FXString *openfiles=TopWinPub::NamedFiles();
    for (FXString*p=openfiles; !p->empty(); p++) {
      if (FX::compare(oldprefix,*p,oldpath.length())==0) {
        if (TopWinPub::IsFileOpen(*p,false)==2) {
          dirty=true;
          break;
        }
        FXString filename=p->text()+oldprefix.length();
        filelist.append(newprefix+filename);
        filelist.append('\n');
        TopWinPub::IsFileOpen(*p,true);
        TopWinPub::CloseFile(false,false);
      }
    }
    delete[] openfiles;
  }
  switch (TopWinPub::IsFileOpen(oldpath,false)) {
    case 0: { break; }
    case 1: {
      TopWinPub::IsFileOpen(oldpath,true);
      TopWinPub::CloseFile(false,false);
      filelist.append(newpath);
      filelist.append('\n');
      break;
    }
    case 2: {
      dirty=true;
      break;
    }
  }
  if (dirty) {
    FXMessageBox::error(this, MBOX_OK, _("Active unsaved changes"), "%s\n%s",
    _("Cannot proceed because a file to be renamed is currently"),
    _("open in the editor, and has unsaved changes.")
    );
    return false;
  }
  return true;
}



bool MyToolsDialog::RenameItem(const FXString &oldpath, const FXString &newpath)
{
  if (ToolsDialog::RenameItem(oldpath,newpath)) {
    for (FXint i=0; i<filelist.contains('\n'); i++) { TopWinPub::OpenFile(filelist.section('\n',i).text(),NULL,false,false); }
    return true;
  } else {
    return false;
  }
}



void MyToolsDialog::EditFile()
{
  TopWinPub::OpenFile(
    (const char*)(((FXMenuCommand*)(PrevItem()->getData()))->getUserData()),NULL,false,false
  );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

FXDEFMAP(MyToolBarFrame) MyToolBarFrameMap[] = {
  FXMAPFUNC(SEL_LEFTBUTTONRELEASE,0,MyToolBarFrame::onLeftBtnRelease),
};

FXIMPLEMENT(MyToolBarFrame,ToolBarFrame,MyToolBarFrameMap,ARRAYNUMBER(MyToolBarFrameMap))



void MyToolBarFrame::EnableFilterBtnCB(FXButton*btn, void*user_data)
{
  MenuSpec*spec=(MenuSpec*)btn->getUserData();
  if (spec && (spec->type=='u') && spec->ms_mc && (spec->ms_mc->getSelector()==TopWindow::ID_USER_FILTER)) {
    if ((bool)user_data) { btn->enable(); } else { btn->disable(); }
  }
}



void MyToolBarFrame::EnableFilterBtn(bool enabled)
{
  ForEachToolbarButton(EnableFilterBtnCB,(void*)enabled);
}

