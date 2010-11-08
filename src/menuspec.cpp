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

#include <cctype>

#include <fx.h>
#include "appmain.h"
#include "appwin.h"
#include "lang.h"

#include "intl.h"
#include "menuspec.h"

#ifdef WIN32
# define QUIT_CMD "Alt+F4"
#else
# define QUIT_CMD "Ctrl+Q"
#endif



/*
To add a new menu command:
  1. Declare an event handler and an ID_* enum value in "appwin.h"
  2. Map the method to the enum value in the TopWindowMap[] in "appwin_ev.cpp"
  3. Implement the event handler in "appwin_ev.cpp"
  4. Add a new menu specification to the menu_specs[] list in "menuspec.cpp".
  5. Add a new menu item in CreateMenus() in "appwin_mnu.cpp" ( using MkMnuCmd() MkMnuChk() or MkMnuRad() )
*/


#define TW TopWindow
static MenuSpec menu_specs[] = {
  {TW::ID_NEW,              "FileNew",         _("&New"),               _("new file"),     "Ctrl+N",       'm', {NULL}},
  {TW::ID_OPEN_FILE,        "FileOpenFile",    _("&Open File..."),      _("open file"),    "Ctrl+O",       'm', {NULL}},
  {TW::ID_OPEN_FILES,       "FileOpenFiles",   _("Open &Files..."),     _("open files"),   "Ctrl+Shift+O", 'm', {NULL}},
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
  {TW::ID_QUIT,             "FileQuit",        _("&Quit"),              _("quit"),         QUIT_CMD,       'm', {NULL}},
  {TW::ID_UNDO,             "EditUndo",        _("&Undo"),              _("undo"),         "Ctrl+Z",       'm', {NULL}},
  {TW::ID_REDO,             "EditRedo",        _("&Redo"),              _("redo"),         "Ctrl+Shift+Z", 'm', {NULL}},
  {TW::ID_CUT,              "EditCut",         _("Cu&t"),               _("cut"),          "Ctrl+X",       'm', {NULL}},
  {TW::ID_COPY,             "EditCopy",        _("&Copy"),              _("copy"),         "Ctrl+C",       'm', {NULL}},
  {TW::ID_PASTE,            "EditPaste",       _("&Paste"),             _("paste"),        "Ctrl+V",       'm', {NULL}},
  {TW::ID_TOLOWER,          "EditLower",       _("Lo&wer Case"),        _("lower"),        "Ctrl+Shift+^", 'm', {NULL}},
  {TW::ID_TOUPPER,          "EditUpper",       _("Upp&er Case"),        _("upper"),        "Ctrl+6",       'm', {NULL}},
  {TW::ID_INDENT_STEP,      "EditSpaceIn",     _("&Space right"),       _("space right"),  "Ctrl+0",       'm', {NULL}},
  {TW::ID_UNINDENT_STEP,    "EditUnspace",     _("Spa&ce left"),        _("space left"),   "Ctrl+9",       'm', {NULL}},
  {TW::ID_INDENT_FULL,      "EditTabIn",       _("&Indent right  "),    _("shift right"),  "Ctrl+Shift+)", 'm', {NULL}},
  {TW::ID_UNINDENT_FULL,    "EditUntab",       _("In&dent left   "),    _("shift left"),   "Ctrl+Shift+(", 'm', {NULL}},
  {TW::ID_READONLY,         "EditReadOnly",    _("Read Onl&y"),         _("read only"),    "",             'm', {NULL}},
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
  {TW::ID_SHOW_STATUSBAR,   "ViewStatusBar",   _("&Status bar"),        _("stats bar"),    "",             'm', {NULL}},
  {TW::ID_SHOW_LINENUMS,    "ViewLineNumbers", _("Line &numbers"),      _("line nums"),    "",             'm', {NULL}},
  {TW::ID_SHOW_TOOLBAR,     "ViewToolbar",     _("Tool&bar"),           _("hide"),         "",             'm', {NULL}},
  {TW::ID_SHOW_WHITESPACE,  "ViewWhiteSpace",  _("&White space"),       _("white space"),  "",             'm', {NULL}},
  {TW::ID_SHOW_MARGIN,      "ViewRightMargin", _("Right &Margin"),      _("show margin"),  "",             'm', {NULL}},
  {TW::ID_SHOW_INDENT,      "ViewIndent",      _("Indent &Guides"),     _("show guides"),  "",             'm', {NULL}},
  {TW::ID_SHOW_OUTLIST,     "ViewOutputPane",  _("&Output pane"),       _("show panel"),   "",             'm', {NULL}},
  {TW::ID_INVERT_COLORS,    "ViewInvertColors",_("&Inverted colors"),   _("invert color"), "",             'm', {NULL}},
  {TW::ID_TABS_TOP,         "ViewTabsTop",     _("&Top"),               _("tabs top"),     "",             'm', {NULL}},
  {TW::ID_TABS_BOTTOM,      "ViewTabsBottom",  _("&Bottom"),            _("tabs btm"),     "",             'm', {NULL}},
  {TW::ID_TABS_LEFT,        "ViewTabsLeft",    _("&Left"),              _("tabs lft"),     "",             'm', {NULL}},
  {TW::ID_TABS_RIGHT,       "ViewTabsRight",   _("&Right"),             _("tabs rgt"),     "",             'm', {NULL}},
  {TW::ID_TABS_UNIFORM,     "ViewTabsUniform", _("&Uniform"),           _("tabs equal"),   "",             'm', {NULL}},
  {TW::ID_TABS_COMPACT,     "ViewTabsPacked",  _("&Packed"),            _("tabs pack"),    "",             'm', {NULL}},
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
  {TW::ID_KILL_COMMAND,     "KillCommand",     "\0",                    NULL,              "Ctrl+.",       'm', {NULL}},
  {TW::ID_LAST,             "\0",              "\0",                    NULL,              "\0",           'm', {NULL}}
};


/*
MenuSpec types:
  m: FXMenuCommand (default)
  r: FXMenuRadio
  k: FXMenuCheck
  u: User-defined menu item from Tools menu item, used to create a toolbar button.
  x: User-defined menu item is in a "transitional" state:
      The mc field is overloaded and points to a filename, instead of a menu item.
*/


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
static FXint toolbar_buttons[TBAR_MAX_BTNS+1]= {
  TW::ID_NEW,
  TW::ID_OPEN_FILE,
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

// This array holds information about toolbar buttons that the user created from
// his own user-defined menu items from the Tools menu.
static MenuSpec* custom_commands[TBAR_MAX_BTNS+1] = {
  NULL,NULL,NULL,NULL,
  NULL,NULL,NULL,NULL,
  NULL,NULL,NULL,NULL,
  NULL,NULL,NULL,NULL,
  NULL,NULL,NULL,NULL,
  NULL,NULL,NULL,NULL,
  NULL };



// We aren't using any fancy icons, but we can at least make each button a different color...
static ColorName tbar_colors[TBAR_MAX_BTNS] = {
  "#BBFFFF",
  "#FFEECC",
  "#CCEEFF",
  "#CCFFEE",
  "#DDDDFF",
  "#DDEEEE",
  "#FFBBFF",
  "#DDFFDD",
  "#ECECEC",
  "#EECCFF",
  "#EEEEDD",
  "#EEDDEE",
  "#EEFFCC",
  "#FFCCEE",
  "#FFFFBB",
  "#FFDDDD",
  "#BBFFFF",
  "#FFEECC",
  "#CCEEFF",
  "#CCFFEE",
  "#DDDDFF",
  "#DDEEEE",
  "#FFBBFF",
  "#DDFFDD",
};



const char*MenuMgr::TBarColors(FXint i) { return (const char*)(tbar_colors[i]); }


static void RemoveToolbarButton(FXint index)
{
  for (FXint i=index; i<TBAR_MAX_BTNS; i++) {
    toolbar_buttons[i]=toolbar_buttons[i+1];
    if (toolbar_buttons[i]==TW::ID_LAST) { break; }
  }
}



// The text for a button created from a user-defined menu uses
// the first two words from the menu item's label. Each of the
// two words is then truncated to a maximum of five characters.
static void SetSpecTBarBtnText(MenuSpec*spec)
{
  FXString btn_txt=spec->ms_mc->getText();
  if (btn_txt.contains(' ')) {
    FXString btn_txt2=btn_txt.section(' ',1);
    btn_txt=btn_txt.section(' ',0);
    btn_txt.trunc(5);
    btn_txt2.trunc(5);
    btn_txt.append(" ");
    btn_txt.append(btn_txt2);
  } else {
    btn_txt.trunc(5);
  }
  btn_txt.lower();
  spec->btn_txt=strdup(btn_txt.text());
}



//  When UserMenu objects are re-scanned, the menu commands they contain become invalid.
//  If any of our toolbar buttons reference these menu commands, we need to invalidate the
//  menu commands, while at the same time saving the filename that the item pointed to.
//  That way, when the menu is rebuilt, we can compare the filenames referenced by the new
//  menu item to our saved filenames and point the toolbar button to the new menu command.
// ( This method is called by the menu item's destructor. )
void MenuMgr::InvalidateUsrTBarCmd(FXMenuCommand*mc)
{
  for (FXint i=0; i<TBAR_MAX_BTNS; i++) {
    MenuSpec*spec=custom_commands[i];
    if (spec && (spec->type=='u') && (spec->ms_mc==mc)) {
      spec->type='x';
      char*tmp=strdup((const char*)(spec->ms_mc->getUserData()));
      spec->ms_fn=tmp;
    }
  }
}



//  When UserMenu objects are re-scanned, a completely new set of menu items is created.
//  Most of these simply replace existing items, so any of the custom toolbar button specs
//  that point to the old copy of the menu item will need to be updated so they will point
//  to the new instance of the item.
// ( This method is called by the menu item's constructor. )
void MenuMgr::ValidateUsrTBarCmd(FXMenuCommand *mc)
{
  if (mc) {
    const char*tmp=(const char*)(mc->getUserData());
    if (tmp) {
      for (FXint i=0; i<TBAR_MAX_BTNS; i++) {
        MenuSpec*spec=custom_commands[i];
        if (spec && spec->ms_fn && (spec->type=='x') && (strcmp(spec->ms_fn,tmp)==0)) {
          free(spec->ms_fn);
          spec->ms_mc=mc;
          spec->type='u';
          if (spec->btn_txt==NULL) { SetSpecTBarBtnText(spec); }
          return;
        }
      }
    }
  }
}



static void RemoveTBarBtnDataCB(FXButton*btn, void*user_data)
{
  if (btn->getUserData()==user_data) { btn->setUserData(NULL); }
}



// After we have completed re-scanning the UserMenu object, there might
// still be some items that were deleted and never re-created. This procedure
// cleans up any remaining "orphaned" items.
void MenuMgr::PurgeTBarCmds()
{
  for (FXint i=0; i<TBAR_MAX_BTNS; i++) {
    MenuSpec*spec=custom_commands[i];
    if (spec && (spec->type=='x')) {
      if (spec->ms_fn) { free(spec->ms_fn); }
      if (spec->btn_txt) { free((char*)(spec->btn_txt)); }
      for (FXint j=0; j<TBAR_MAX_BTNS; j++) {
        if (toolbar_buttons[j]==spec->sel) { toolbar_buttons[j]=0; }
      }
      // If any toolbar button holds a reference to this spec, make sure we set it to NULL...
      ((AppClass*)(FXApp::instance()))->MainWin()->ForEachToolbarButton(RemoveTBarBtnDataCB,spec);
      delete spec;
      custom_commands[i]=NULL;
    }
  }
  bool found;
  do {
    found=false;
    for (FXint i=0; i<TBAR_MAX_BTNS; i++) {
      if (toolbar_buttons[i]==0) {
        RemoveToolbarButton(i);
        found=true;
        break;
      }
    }
  } while (found);
  // Somewhat unrelated, but now is also a good time to clear any old
  // button references from the builtin commands...
  for (MenuSpec*spec=menu_specs; spec && spec->ms_mc; spec++) {
    spec->ms_mc->setUserData(NULL);
  }
}



#define _GetCaption(p) ((FXMenuCaption*)((p)->getParent()->getUserData()))

// Construct a tooltip string based on a menu item's path.
void MenuMgr::GetTBarBtnTip(MenuSpec*spec, FXString &tip)
{
  if (spec&&spec->ms_mc) {
    tip=spec->ms_mc->getText();
    if (spec->type=='u') {
      tip=(const char*)spec->ms_mc->getUserData();
      tip.erase(0,((AppClass*)(FXApp::instance()))->ConfigDir().length());
      FXString path=FXPath::directory(tip);
#ifdef WIN32
      path.substitute(PATHSEP,'/');
#endif
      tip=FXPath::title(tip);
      tip.erase(0,3);
      if (tip.find('.')>=0) { tip=FXPath::title(tip); }
      tip=tip.section('@',0);
      tip.prepend(path+"/");
      tip.substitute("_","");
      tip.substitute('-',' ');
      FXint n=0;
      do {
        n=tip.find('/',n);
        if (n>=0) {
          if (isdigit(tip[n+1])&&isdigit(tip[n+2])&&(tip[n+3]=='.')) { tip.erase(n+1,3); }
          n++;
        } else {
          break;
        }
      } while (1);
      tip.substitute("/"," -> ");
      tip.at(0)=toupper(tip.text()[0]);
      for (char*c=&(tip.at(0)); *c; c++) { if (c[0]==' ') { c[1]=toupper(c[1]); } }
    } else {
      for (FXMenuCaption*cpn=_GetCaption(spec->ms_mc); cpn; cpn=_GetCaption(cpn)) {
        tip.prepend(cpn->getText()+" -> ");
      }
    }
  }
}



// Returns a unique negative ID number for a new user-defined toolbar button.
static FXint GetUniqueID()
{
  FXint unique=0;
  bool exists=true;
  do {
    unique--;
    exists=false;
    for (FXint i=0; i<TBAR_MAX_BTNS; i++) {
      if (custom_commands[i] && (custom_commands[i]->sel==unique)) {
        exists=true;
        break;
      }
    }
  } while (exists);
  return unique;
}



//  Create a new MenuSpec for a toolbar button from a user-defined menu item.
MenuSpec* MenuMgr::AddTBarUsrCmd(FXMenuCommand*mc)
{
  FXint unique=GetUniqueID();
  for (FXint i=0; i<TBAR_MAX_BTNS; i++) {
    if (custom_commands[i]==NULL) {
      MenuSpec*spec=new MenuSpec;
      spec->type='u';
      spec->ms_mc=mc;
      snprintf(spec->pref,sizeof(spec->pref)-1,"Custom_%d",abs(unique));
      SetSpecTBarBtnText(spec);
      spec->sel=GetUniqueID();
      custom_commands[i]=spec;
      return spec;
    }
  }
  return NULL;
}



//  When we read a user-defined toolbar item from the registry, its menu item has not
//  yet been created, so we set up a "transitional" menu spec containing the path to
//  the script file. This information will be used later to create a toolbar button,
//  after the user-defined menus are in place.
static MenuSpec* RegTBarUsrCmd(FXint index, const char*pref, const char*filename)
{
  FXint unique=0;
  for (FXint i=0; i<TBAR_MAX_BTNS; i++) {
    if (custom_commands[i]==NULL) {
      MenuSpec*spec=new MenuSpec;
      spec->type='x';
      spec->ms_fn=strdup(filename);
      sscanf(pref, "Custom_%d",&unique);
      spec->sel = -(unique);
      strncpy(spec->pref,pref,sizeof(spec->pref)-1);
      spec->btn_txt=NULL;
      custom_commands[i]=spec;
      toolbar_buttons[index]=spec->sel;
      return spec;
    }
  }
  return NULL;
}


// The user has decided to remove a toolbar button for a user-defined menu item.
void MenuMgr::RemoveTBarUsrCmd(MenuSpec*spec)
{
 if (spec==NULL) { return; }
 for (FXint i=0; i<TBAR_MAX_BTNS; i++) {
    if (custom_commands[i]==spec) {
      custom_commands[i]=NULL;
      // If any toolbar button holds a reference to this spec, make sure we set it to NULL...
      ((AppClass*)(FXApp::instance()))->MainWin()->ForEachToolbarButton(RemoveTBarBtnDataCB,spec);
    }
  }
  if (spec->btn_txt) free((char*)spec->btn_txt);
  delete(spec);
}



// Free up memory allocated to the custom_commands[] array when the program exits.
static void FreeTBarUsrCmds() {
  for (FXint i=0; i<TBAR_MAX_BTNS; i++) {
   MenuSpec*spec=custom_commands[i];
   if (spec) {
     if (spec->btn_txt) free((char*)spec->btn_txt);
     delete spec;
   }
  }
}



MenuSpec* MenuMgr::LookupMenu(FXint sel)
{
  if (sel>0) {
    MenuSpec*spec;
    for (spec=menu_specs; spec->sel!=TW::ID_LAST; spec++) {
      if (spec->sel==sel) { return spec; }
    }
  } else {
    for (FXint i=0; i<TBAR_MAX_BTNS; i++) {
      if (custom_commands[i] && (custom_commands[i]->sel==sel)) { return custom_commands[i]; }
    }
  }
  return NULL;
}



static MenuSpec* LookupMenuByPref(const char* pref)
{
  if (pref) {
    MenuSpec*spec;
    for (spec=menu_specs; spec->sel!=TW::ID_LAST; spec++) {
      if (strcmp(spec->pref, pref)==0) { return spec; }
    }
  }
  return NULL;
}



FXint*MenuMgr::TBarBtns() { return toolbar_buttons; }



MenuSpec*MenuMgr::MenuSpecs() { return menu_specs; }



const char* MenuMgr::GetUsrCmdPath(MenuSpec*spec) {
  if (spec) {
    switch (spec->type) {
      case 'x': { return spec->ms_fn; }
      case 'u': { return spec->ms_mc?((const char*)(spec->ms_mc->getUserData())):NULL; }
    }
  }
  return NULL;
}



FXMenuCommand*MenuMgr::MakeMenuCommand(FXComposite*p, FXObject*tgt, FXSelector sel, char type, bool checked)
{
  MenuSpec*spec=MenuMgr::LookupMenu(sel);

  if (spec) {
    FXWindow*own;
    FXAccelTable *table;
    switch(type) {
      case 'm': {
        spec->ms_mc = new FXMenuCommand(p,spec->mnu_txt,NULL,tgt,sel);
        break;
      }
      case 'k': {
        spec->ms_mc = (FXMenuCommand*) new FXMenuCheck(p,spec->mnu_txt,tgt,sel);
        ((FXMenuCheck*)spec->ms_mc)->setCheck(checked);
        break;
      }
      case 'r': {
        spec->ms_mc = (FXMenuCommand*) new FXMenuRadio(p,spec->mnu_txt,tgt,sel);
        break;
      }
      default: {
        fxwarning(_("%s: Warning: unknown menu type: '%c'.\n"), EXE_NAME, type);
        spec->ms_mc = new FXMenuCommand(p,spec->mnu_txt,NULL,tgt,sel);
      }
    }
    FXHotKey acckey=parseAccel(spec->accel);
    if (acckey) {
      spec->ms_mc->setAccelText(spec->accel);
      own=p->getShell()->getOwner();
      if (own) {
        table=own->getAccelTable();
        if (table) {
          if (table->hasAccel(acckey)) {
            fxwarning(_("%s: Warning: action \"%s\" overrides existing accelerator.\n"), EXE_NAME, spec->pref);
          }
          table->addAccel(acckey,tgt,FXSEL(SEL_COMMAND,sel));
        }
      }
    } else {
      if (spec->accel[0]) {
        fxwarning(_("%s: Warning: Failed to parse accelerator for \"%s\"\n"), EXE_NAME, spec->pref);
      }
    }
    spec->type=type;
    return spec->ms_mc;
  } else {
    fxwarning(_("%s: Warning: Could not build menu for selector #%d\n"), EXE_NAME, sel);
    return NULL;
  }
}



void MenuMgr::ReadMenuSpecs(FXRegistry*reg, const char* keys_sect)
{
  for (MenuSpec*spec=menu_specs; spec->sel!=TopWindow::ID_LAST; spec++) {
    FXString acc=reg->readStringEntry(keys_sect,spec->pref,spec->accel);
    if (strcmp(acc.text(),spec->accel)!=0) {
      memset(spec->accel,0,sizeof(spec->accel));
      strncpy(spec->accel,acc.text(),sizeof(spec->accel)-1);
    }
  }
}



void MenuMgr::WriteMenuSpecs(FXRegistry*reg, const char* keys_sect)
{
  for (MenuSpec*spec=menu_specs; spec->sel!=TopWindow::ID_LAST; spec++) {
    reg->writeStringEntry(keys_sect,spec->pref,spec->accel);
  }
}



void MenuMgr::ReadToolbarButtons(FXRegistry*reg, const char* tbar_sect)
{
  for (FXint i=0; i<TBAR_MAX_BTNS; i++) {
    if (toolbar_buttons[i]==TopWindow::ID_LAST) { break; }
    char keyname[32];
    memset(keyname,0, sizeof(keyname));
    snprintf(keyname,sizeof(keyname)-1,"Button_%d", i+1);
    MenuSpec*spec=MenuMgr::LookupMenu(toolbar_buttons[i]);
    const FXchar *keyval=reg->readStringEntry(tbar_sect,keyname,spec->pref);
    spec=LookupMenuByPref(keyval);
    if (spec) {
      toolbar_buttons[i]=spec->sel;
    } else {
      toolbar_buttons[i]=TopWindow::ID_LAST;
      if ((strncmp(keyval,"Custom_",7)==0)&&isdigit(keyval[7])) {
        const FXchar *filename=reg->readStringEntry(tbar_sect,keyval,NULL);
        if (filename && FXStat::isFile(filename)) {
           RegTBarUsrCmd(i,keyval,filename);
        }
      }
    }
  }
}



void MenuMgr::WriteToolbarButtons(FXRegistry*reg, const char* tbar_section)
{
  reg->deleteSection(tbar_section);
  for (FXint i=0; i<TBAR_MAX_BTNS; i++) {
    char keyname[32];
    memset(keyname,0, sizeof(keyname));
    snprintf(keyname,sizeof(keyname)-1,"Button_%d", i+1);
    MenuSpec*spec=MenuMgr::LookupMenu(toolbar_buttons[i]);
    if (spec) {
      reg->writeStringEntry(tbar_section,keyname,spec->pref);
      if (spec->type=='u') {
        reg->writeStringEntry(tbar_section,spec->pref,(const char*)(spec->ms_mc->getUserData()));
      }
    } else {
      reg->writeStringEntry(tbar_section,keyname,"");
    }
  }
  FreeTBarUsrCmds();
}

