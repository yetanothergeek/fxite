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


#include <fx.h>
#include <lua.hpp>


#include "appwin.h"
#include "filer.h"
#include "prefs_base.h"

#include "intl.h"

static TopWindow*tw=NULL;
static SettingsBase*prefs=NULL;


static int edit_indent(lua_State* L){
  const char*size=luaL_checkstring(L,1);
  const char*dirn=luaL_checkstring(L,2);
  bool bytab=false;
  bool toright=true;
  if (strcasecmp(size,"space")==0) {
    bytab=false;
  } else {
    if ((strcasecmp(size,"tab")==0)) {
     bytab=true;
    } else {
      luaL_argerror(L,1, _("expected keyword \"space\" or \"tab\""));
    }
  }
  if (strcasecmp(dirn,"right")==0) {
    toright=true;
  } else {
    if ((strcasecmp(dirn,"left")==0)) {
      toright=false;
    } else {
      luaL_argerror(L,2, _("expected keyword \"right\" or \"left\""));
    }
  }
  FXint id;
  if (toright) {
    if (bytab) {
      id=TopWindow::ID_INDENT_FULL;
    } else {
      id=TopWindow::ID_INDENT_STEP;
    }
  } else {
    if (bytab) {
      id=TopWindow::ID_UNINDENT_FULL;
    } else {
      id=TopWindow::ID_UNINDENT_STEP;
    }
  }
  tw->onIndent(NULL, FXSEL(SEL_COMMAND,id), NULL);
  return 0;
}



static int edit_format(lua_State* L)
{
  const char*opts[]={"dos", "mac", "unix", NULL};
  FXSelector cmd=0;
  int opt=luaL_checkoption(L,1,NULL,opts);
  switch (opt) {
    case 0: { cmd=TopWindow::ID_FMT_DOS; break; }
    case 1: { cmd=TopWindow::ID_FMT_MAC; break; }
    case 2: { cmd=TopWindow::ID_FMT_UNIX; break; }
  }
  tw->onFileFormat(NULL,FXSEL(SEL_COMMAND, cmd),NULL);
  return 0;
}



static int edit_preferences(lua_State* L)
{
  tw->onPrefsDialog(NULL,0,NULL);
  return 0;
}


static int edit_redo(lua_State* L)
{
  tw->onRedo(NULL,0,NULL);
  return 0;
}



static int edit_undo(lua_State* L){
  tw->onUndo(NULL,0,NULL);
  return 0;
}


static int file_new(lua_State* L)
{
  tw->onFileNew(NULL,0,NULL);
  return 0;
}



static int file_insert_file(lua_State* L)
{
  tw->onInsertFile(NULL,0,NULL);
  return 0;
}



static int file_load_tags_file(lua_State* L)
{
  if (lua_gettop(L)==0) {
    tw->onLoadTags(NULL,0,NULL);
  } else {
    const char*fn=luaL_checkstring(L,1);
    if (fn) {
      FXStat *info= new FXStat;
      FXString *filename=new FXString();
      *filename=FXPath::simplify(FXPath::absolute(fn));
      if (FXStat::statFile(*filename,*info)&&info->isFile()&&info->isReadable()) {
        tw->AddFileToTagsMenu(*filename);
        delete info;
        delete filename;
      } else {
        delete info;
        delete filename;
        luaL_argerror(L,1,_("failed to read file"));
      }
    }
  }
  return 0;
}

static int file_unload_tags_file(lua_State* L)
{
  const char* filename=luaL_optstring(L,1,"");
  if (!tw->RemoveFileFromTagsMenu(filename)) {
    luaL_argerror(L,1,_("file not loaded."));
  }
  return 0;
}

static int file_open(lua_State* L)
{
  tw->onFileOpen(NULL,0,NULL);
  return 0;
}



static int file_save(lua_State* L)
{
  tw->onFileSave(NULL,0,NULL);
  return 0;
}



static int file_save_as(lua_State* L)
{
  tw->onFileSaveAs(NULL,0,NULL);
  return 0;
}



static int file_save_all(lua_State* L)
{
  tw->onFileSaveAll(NULL,0,NULL);
  return 0;
}



static int file_save_copy(lua_State* L)
{
  tw->onFileSaveACopy(NULL,0,NULL);
  return 0;
}



static int file_close(lua_State* L)
{
  tw->onCloseTab(NULL,0,NULL);
  return 0;
}


static int file_close_all(lua_State* L)
{
  tw->onCloseAll(NULL,0,NULL);
  return 0;
}

static int file_reload(lua_State* L){
  tw->onReload(NULL,0,NULL);
  return 0;
}



static int search_find(lua_State* L)
{
  tw->onFind(NULL,0,NULL);
  return 0;
}



static int search_find_next(lua_State* L)
{
  tw->onFindNext(NULL,0,NULL);
  return 0;
}



static int search_find_prev(lua_State* L)
{
  tw->onFindPrev(NULL,0,NULL);
  return 0;
}



static int search_find_selected(lua_State* L)
{
  const char*size=luaL_checkstring(L,1);
  if (strcasecmp(size,"next")==0) {
    tw->onFindSelected(NULL,FXSEL(SEL_COMMAND,TopWindow::ID_NEXT_SELECTED),NULL);
  } else {
    if ((strcasecmp(size,"prev")==0)) {
      tw->onFindSelected(NULL,FXSEL(SEL_COMMAND,TopWindow::ID_PREV_SELECTED),NULL);
    } else {
      luaL_argerror(L,1, _("expected keyword \"next\" or \"prev\""));
    }
  }
  return 0;
}


static int search_go_to(lua_State* L)
{
  tw->onGoto(NULL,0,NULL);
  return 0;
}


static int search_go_to_selected(lua_State* L)
{
  tw->onGotoSelected(NULL,0,NULL);
  return 0;
}



static int search_replace(lua_State* L)
{
  tw->onReplace(NULL,0,NULL);
  return 0;
}



static int search_find_definition(lua_State* L)
{
  tw->onFindTag(NULL,0,NULL);
  return 0;
}



static int search_show_calltip(lua_State* L)
{
  tw->onShowCallTip(NULL,0,NULL);
  return 0;
}



static int search_show_completions(lua_State* L)
{
  tw->onAutoComplete(NULL,0,NULL);
  return 0;
}



static int tools_rebuild_menu(lua_State* L)
{
 tw->onRescanUserMenu(NULL,0,NULL);
  return 0;
}



static int tools_customize_menu(lua_State* L)
{
  tw->onConfigureTools(NULL,0,NULL);
  return 0;
}



static int view_language(lua_State* L)
{
  const char*name=luaL_checkstring(L,1);
  if (!tw->SetLanguage(name)) {
    luaL_argerror(L,1,_("invalid language name"));
  }
  return 0;
}



static int view_line_numbers(lua_State* L)
{
  bool show;
  if (lua_gettop(L)==0) {
    show=!prefs->ShowLineNumbers;
  } else {
    luaL_argcheck(L, lua_isboolean(L,1), 1, _("expected boolean"));
    show=lua_toboolean(L,1);
  }
  tw->ShowLineNumbers(show);
  return 0;
}



static int view_status(lua_State* L)
{
  bool show;
  if (lua_gettop(L)==0) {
    show=!prefs->ShowStatusBar;
  } else {
    luaL_argcheck(L, lua_isboolean(L,1), 1, _("expected boolean"));
    show=lua_toboolean(L,1);
  }
  tw->ShowStatusBar(show);
  return 0;
}



static int view_output_pane(lua_State* L)
{
  bool show;
  if (lua_gettop(L)==0) {
    show=!prefs->ShowOutputPane;
  } else {
    luaL_argcheck(L, lua_isboolean(L,1), 1, _("expected boolean"));
    show=lua_toboolean(L,1);
  }
  tw->ShowOutputPane(show);
  return 0;
}



static int view_white_space(lua_State* L)
{
  bool show;
  if (lua_gettop(L)==0) {
    show=!prefs->ShowWhiteSpace;
  } else {
    luaL_argcheck(L, lua_isboolean(L,1), 1, _("expected boolean"));
    show=lua_toboolean(L,1);
  }
  tw->ShowWhiteSpace(show);
  return 0;
}



static int view_toolbar(lua_State* L)
{
  bool show;
  if (lua_gettop(L)==0) {
    show=!prefs->ShowToolbar;
  } else {
    luaL_argcheck(L, lua_isboolean(L,1), 1, _("expected boolean"));
    show=lua_toboolean(L,1);
  }
  tw->ShowToolbar(show);
  return 0;
}


static int view_tabs(lua_State* L)
{
  const char*opts[]={"position","width",NULL};
  int opt=luaL_checkoption(L,1,NULL,opts);
  switch (opt) {
    case 0: {
      const char*cmds[]={"top", "bottom", "left", "right",NULL};
      int cmd=luaL_checkoption(L,2,NULL,cmds);
      switch (cmd) {
        case 0:{cmd=TopWindow::ID_TABS_TOP;    break;}
        case 1:{cmd=TopWindow::ID_TABS_BOTTOM; break;}
        case 2:{cmd=TopWindow::ID_TABS_LEFT;   break;}
        case 3:{cmd=TopWindow::ID_TABS_RIGHT;  break;}
      }
      tw->onTabOrient(NULL, FXSEL(SEL_COMMAND, cmd), NULL);
      break;
    }
    case 1: {
      const char*cmds[]={"uniform", "packed",NULL};
      int cmd=luaL_checkoption(L,2,NULL,cmds);
      switch (cmd) {
        case 0:{cmd=TopWindow::ID_TABS_UNIFORM;    break;}
        case 1:{cmd=TopWindow::ID_TABS_COMPACT;    break;}
      }
      tw->onPackTabWidth(NULL,FXSEL(SEL_COMMAND, cmd),NULL);
      break;
    }
  }
  return 0;
}



static int view_zoom(lua_State* L)
{
  if ((lua_gettop(L)>0) && lua_isnumber(L,1)) {
    int z=lua_tointeger(L,1);//-10 to +20
    luaL_argcheck(L,(z>-11)&&(z<21), 1, _("zoom factor out of range (-10 to +20)"));
    tw->onZoom(NULL, 0, (void*)(FXival)z);
  } else {
    const char*cmds[]={"in", "out", "default", "closest", "furthest" , NULL};
    int cmd=luaL_checkoption(L,1,NULL,cmds);
    switch(cmd) {
      case 0:{ cmd=TopWindow::ID_ZOOM_IN; break; }
      case 1:{ cmd=TopWindow::ID_ZOOM_OUT; break; }
      case 2:{ cmd=TopWindow::ID_ZOOM_NONE; break; }
      case 3:{ cmd=TopWindow::ID_ZOOM_NEAR; break; }
      case 4:{ cmd=TopWindow::ID_ZOOM_FAR; break; }
    }
    tw->onZoom(NULL, FXSEL(SEL_COMMAND, cmd), NULL);
  }
  return 0;
}



static int documents_next(lua_State* L)
{
  tw->onNextTab(NULL, FXSEL(SEL_COMMAND,TopWindow::ID_TAB_NEXT), NULL);
  return 0;
}



static int documents_prev(lua_State* L)
{
  tw->onNextTab(NULL, FXSEL(SEL_COMMAND,TopWindow::ID_TAB_PREV), NULL);
  return 0;
}


static int documents_output_pane(lua_State* L)
{
  tw->onOutlistFocus(NULL,0,NULL);
  return 0;
}


static int documents_move(lua_State* L)
{
  const char*cmds[]={"first","last","up", "down", NULL};
  int cmd=luaL_checkoption(L,1,NULL,cmds);
  switch (cmd) {
    case 0: { cmd=TopWindow::ID_TAB_TOFIRST; break; }
    case 1: { cmd=TopWindow::ID_TAB_TOLAST; break; }
    case 2: { cmd=TopWindow::ID_TAB_UP; break; }
    case 3: { cmd=TopWindow::ID_TAB_DOWN; break; }
  }
  tw->onMoveTab(NULL, FXSEL(SEL_COMMAND, cmd), NULL);
  return 0;
}


static int file_export(lua_State* L)
{
  const char*cmds[]={"pdf","html", NULL};
  int cmd=luaL_checkoption(L,1,NULL,cmds);
  const char*filename=luaL_optstring(L,2,NULL);
  bool ok=false;
  switch (cmd) {
    case 0: {
      ok=tw->FileDlgs()->ExportPdf(tw->ControlDoc(), filename);
      break;
    }
    case 1: {
      ok=tw->FileDlgs()->ExportHtml(tw->ControlDoc(), filename);
      break;
    }
  }
  lua_pushboolean(L,ok);
  return 1;
}



static int view_clear_output(lua_State* L)
{
  tw->ClearOutput();
  return 0;
}


static const struct luaL_Reg fxte_commands[] = {
  {"file_new",                  file_new},
  {"file_open",                 file_open},
  {"file_reload",               file_reload},
  {"file_save",                 file_save},
  {"file_save_as",              file_save_as},
  {"file_save_all",             file_save_all},
  {"file_save_copy",            file_save_copy},
  {"file_export",               file_export},
  {"file_close",                file_close},
  {"file_close_all",            file_close_all},
  {"file_insert_file",          file_insert_file},
  {"file_load_tags_file",       file_load_tags_file},
  {"file_unload_tags_file",     file_unload_tags_file},


  {"edit_undo",                 edit_undo},
  {"edit_redo",                 edit_redo},
  {"edit_indent",               edit_indent},
  {"edit_format",               edit_format},
  {"edit_preferences",          edit_preferences},

  {"search_find",               search_find},
  {"search_find_next",          search_find_next},
  {"search_find_prev",          search_find_prev},
  {"search_find_selected",      search_find_selected},
  {"search_replace",            search_replace},
  {"search_go_to",              search_go_to},
  {"search_go_to_selected",     search_go_to_selected},
  {"search_find_definition",    search_find_definition},
  {"search_show_calltip",       search_show_calltip},
  {"search_show_completions",   search_show_completions},

  {"tools_rebuild_menu",        tools_rebuild_menu},
  {"tools_customize_menu",      tools_customize_menu},

  {"view_status",               view_status},
  {"view_line_numbers",         view_line_numbers},
  {"view_toolbar",              view_toolbar},
  {"view_white_space",          view_white_space},
  {"view_output_pane",          view_output_pane},
  {"view_zoom",                 view_zoom},
  {"view_tabs",                 view_tabs},
  {"view_language",             view_language},
  {"view_clear_output",         view_clear_output},

  {"documents_next",            documents_next},
  {"documents_prev",            documents_prev},
  {"documents_move",            documents_move},
  {"documents_output_pane",     documents_output_pane},
  {NULL,NULL}
};



const luaL_Reg* LuaCommands(FXMainWindow*topwin)
{
  if (!tw) {
    tw=(TopWindow*)topwin;
    prefs=(SettingsBase*)tw->Prefs();
  }
  return fxte_commands;
}

