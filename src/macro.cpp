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

#include <unistd.h>
#include <lua.hpp>
#include <fx.h>

#include "compat.h"
#include "appname.h"
#include "appwin_pub.h"
#include "luacmds.h"
#include "luafuncs.h"
#include "luafx.h"
#include "fxasq_lua.h"

#include "intl.h"
#include "macro.h"



MacroRunner::MacroRunner():FXObject(){}


MacroRunner::~MacroRunner()
{
  LuaFuncsCleanup();
  ClearKeepers();
}



/* Catch and report script errors */
static FXint traceback(lua_State *L)
{
  lua_getglobal(L, "debug");
  if (!lua_istable(L, -1)) {
    lua_pop(L, 1);
    return 1;
  }
  lua_getfield(L, -1, "traceback");
  if (!lua_isfunction(L, -1)) {
    lua_pop(L, 2);
    return 1;
  }
  lua_pushvalue(L, 1);
  lua_pushinteger(L, 2);
  lua_call(L, 2, 1);
  return 1;
}



/* Class to record current script name and file position */
class StateInfo: public FXObject {
public:
  lua_State *L;
  FXString source;
  FXString script;
  FXint line;
  FXint counter;
  bool optimized;
  StateInfo(lua_State *aL);
};



StateInfo::StateInfo(lua_State *aL)
{
  L=aL;
  source="";
  script="";
  line=-1;
  counter=0;
  optimized=false;
}


/*
  It's probaly rare that we'll have more than one interpreter
  running at a time, but we let them each carry their own info
  just in case.
*/
static FXObjectListOf<StateInfo> states;



/*
  Find the StateInfo associated with this Lua state
*/
static StateInfo* LookupState(lua_State *L)
{
  for (FXint i=0; i<states.no(); i++) {
    if ( states[i]->L == L ) { return states[i]; }
  }
  return NULL;
}


/*
  The default Lua error messages are intended to be printed to the console.
  Here we doctor them up so they look better when displayed in a dialog box.
  In particular, we need to convert tabs to spaces, and replace the long
  path names with an ellipsis (.../)
  The current hack used here is to parse the lines that contain a slash
  and chop out everything before the last slash. Of course this will
  cause problems if the error message contains slashes beyond the filename,
  but it should be OK in most cases.
*/
static void format_message(const char *rawmsg, FXString &outmsg)
{
    FXString s=rawmsg;
    outmsg="";
    s.substitute("\t", "  ", true);
    s.substitute("\r\n", "\n");
    s.append("\n");
    FXint n=s.contains("\n");
    for (FXint i=0; i<n; i++) {
      FXString sect=s.section('\n',i);
      if (sect.contains(PATHSEP)) {
        outmsg.append(".../"+FXPath::name(sect)+"\n");
      } else {
        outmsg.append(sect+"\n");
      }
    }
    outmsg.substitute(": ", ":\n\n  ", false);
    outmsg.substitute("stack traceback:", "\nstack traceback:", false);
}



/*
  Pop up a message dialog if we encounter a script error.
  If we know the exact location of the error, give the user
  a chance to open the offending file at the point of error.
*/
static void script_error(const FXString &filename, const char *msg, bool need_name, FXint line)
{
  if ((!TopWinPub::instance()) || TopWinPub::Destroying() || TopWinPub::Closing()) {
   fprintf(stderr, "%s\n", msg);
   return;
 }
  if (need_name||filename.empty()) {
    FXMessageBox::error(TopWinPub::instance(),MBOX_OK,_("Macro Error"),"%s\n%s\n", filename.text(),msg);
  } else {
    FXString m;
    format_message(msg,m);
    if ( FXMessageBox::error(TopWinPub::instance(), MBOX_YES_NO, _("Macro Error"), "%s\n\n\n%s",
           m.text(), _("Edit script?"))==MBOX_CLICKED_YES) {
      char linenum[8]="\0\0\0\0\0\0\0";
      snprintf(linenum,sizeof(linenum)-1, "%d", line);
      TopWinPub::OpenFile(filename.text(), linenum, false, false);
    }
  }
}



/*
  Retrieve an error message from the Lua state if possible,
  else make up our own message.
*/
static void show_error(lua_State *L, const FXString &filename)
{
  StateInfo*si=LookupState(L);
  if (!lua_isnil(L, -1)) {
   const char *msg;
   msg = lua_tostring(L, -1);
   if (msg == NULL) {
    msg = _("(error object is not a string)");
   }
   if (strncmp(msg,LuaQuitMessage(), strlen(LuaQuitMessage()))!=0) {
     script_error(si->source.empty()?filename:si->source, msg, false, si->line);
   }
   lua_pop(L, 1);
  } else {
   script_error(si->source.empty()?filename:si->source, "Unknown error inside script.", false, si->line);
  }
}



/* Provide the user with a function to return the script's filename */
static int scriptname(lua_State *L)
{
  lua_pushstring(L, LookupState(L)->script.text());
  return 1;
}


/* Override Lua's standard os.exit() function, so we can have a clean shutdown. */
static int osexit(lua_State *L)
{
  TopWinPub::CloseWait();
  lua_pushstring(L,LuaQuitMessage());
  lua_error(L);
  return 0;
}


/* Override Lua's standard print() function, so we can send the data to the output pane. */
static int print(lua_State *L)
{
  int argc=lua_gettop(L);
  FXString data="";
  for (int i=1; i<=argc; i++) {
    const void* p=NULL;
    char buf[128];
    memset(buf,0,sizeof(buf));
    switch (lua_type(L,i)) {
      case LUA_TNONE: {
        data.append(_("<out-of-bounds>"));
        break;
      }
      case LUA_TNIL: {
        data.append("nil");
        break;
      }
      case LUA_TNUMBER: {
        snprintf(buf, sizeof(buf)-1, "%g", lua_tonumber(L,i));
        data.append(buf);
        break;
      }
      case LUA_TBOOLEAN: {
        data.append(lua_toboolean(L,i)?"true":"false");
        break;
      }
      case LUA_TSTRING: {
        data.append(lua_tostring(L,i));
        break;
      }
      case LUA_TTABLE: {
        p=lua_topointer(L,i);
        data.append("table: ");
        break;
      }
      case LUA_TFUNCTION: {
        p=lua_topointer(L,i);
        data.append("function: ");
        break;
      }
      case LUA_TLIGHTUSERDATA:
      case LUA_TUSERDATA: {
        p=lua_topointer(L,i);
        data.append("userdata: ");
        break;
      }
      case LUA_TTHREAD: {
        p=lua_topointer(L,i);
        data.append("userdata: ");
        break;
      }
    }
    if (p) {
      snprintf(buf, sizeof(buf)-1, "%p", p);
      data.append(buf);
    }
    if (i!=argc) {
      data.append("    ");
    }
  }
  data.substitute('\t', ' ');
  data.substitute("\r\n", "\n");
  FXint sects=data.contains('\n');
  for (FXint sect=0; sect<sects; sect++) {
    TopWinPub::AddOutput(data.section('\n', sect));
  }
  if (!data.section('\n', sects).empty()) { TopWinPub::AddOutput(data.section('\n', sects)); }
  return 0;
}


/* Scripts that call this function will bypass most of the debug hook's functionality. */
static int optimize(lua_State *L)
{
  LookupState(L)->optimized=true;
  return 0;
}


/*
  This hook gets called each time the interpreter executes a line of code.
  It stores the current script file name and line number, checks if the
  user is trying to cancel the script, and occasionally refreshes the GUI.
*/
static void debug_hook(lua_State *L, lua_Debug *ar)
{
  StateInfo*si=LookupState(L);
  if (!si->optimized) {
    if (lua_getinfo(L,"Sl",ar)) {
      if (ar->source && (ar->source[0]=='@') && (strcmp(si->source.text(), ar->source+1)!=0)) {
        si->source=ar->source+1;
      }
      si->line=ar->currentline;
    }
    if ( (!TopWinPub::instance()) || TopWinPub::IsMacroCancelled()) {
      lua_pushstring(L, _("Macro cancelled by user."));
      lua_error(L);
      return;
    }
    if (si->counter > 100000) {
      TopWinPub::update();
      FXApp::instance()->runWhileEvents();
      si->counter=0;
    } else si->counter++;
  }
}


/* Override a builtin Lua function, or add a new one if it doesn't exist */
static void override(lua_State *L, const char*module, const char* funcname, lua_CFunction newfunc)
{
  lua_getglobal(L,module);
  if (lua_istable(L,-1)) {
    lua_pushstring(L,funcname);
    if (newfunc) {
      lua_pushcfunction(L,newfunc);
    } else {
      lua_pushnil(L);
    }
    lua_rawset(L,-3);
  } else {
    lua_pop(L, 1);
  }
}


/* Don't let scripts try to read from stdin, as this would block indefinitely */
static void close_stdin(lua_State *L)
{
  lua_getglobal(L, "io");
  if (!lua_istable(L, -1)) {
    lua_pop(L, 1);
    return;
  }
  lua_getfield(L, -1, "input");
  if (!lua_isfunction(L, -1)) {
    lua_pop(L, 2);
    return;
  }
  lua_pushstring(L, NULL_FILE);
  lua_call(L, 1, 1);
}


/* Push a name=value variable into the application module's namespace */
static void set_string_token(lua_State *L, const char*name, const char*value)
{
  lua_getglobal(L, LUA_MODULE_NAME);
  if (lua_istable(L, -1)) {
    lua_pushstring(L,name);
    lua_pushstring(L,value);
    lua_settable(L, -3);
  } else {
    fxwarning(_("*** %s: Failed to set value for %s\n"), LUA_MODULE_NAME, name);
  }
}



typedef struct {
  int t;
  union {
    char*s;
    lua_Number n;
    bool b;
  };
} PersistRecord;


void MacroRunner::ClearKeepers()
{
  if (UsedSlotsInDict(&keepers)>0) for (FXint i=0; i<TotalSlotsInDict(&keepers); ++i) {
    PersistRecord*pr=(PersistRecord*)keepers.data(i);
    if (pr) {
      if (pr->t==LUA_TSTRING) {
        free(pr->s);
        delete pr;
      }
    }
  }
  keepers.clear();
}


#define PERSIST_TABLE_NAME "keep"

void MacroRunner::PushKeepers(lua_State *L)
{
  lua_getglobal(L, LUA_MODULE_NAME);
  if (lua_istable(L, -1)) {
    lua_pushstring(L,PERSIST_TABLE_NAME);
    lua_newtable(L);
    lua_settable(L, -3);
    if (UsedSlotsInDict(&keepers)>0) for (FXint i=0; i<TotalSlotsInDict(&keepers); ++i) {
      PersistRecord*pr=(PersistRecord*)keepers.data(i);
      if (!pr) { continue; }
      lua_getglobal(L, LUA_MODULE_NAME);
      lua_getfield(L,-1,PERSIST_TABLE_NAME);
      lua_pushstring(L,DictKeyName(keepers,i));
      switch (pr->t) {
        case LUA_TNUMBER: { lua_pushnumber(L, pr->n);  break;}
        case LUA_TBOOLEAN:{ lua_pushboolean(L, pr->b); break;}
        case LUA_TSTRING: { lua_pushstring(L,pr->s);   break;}
      }
      lua_rawset(L,-3);
    }
  } else {
    fxwarning(_("*** %s: Failed to set value for %s\n"), LUA_MODULE_NAME, "keep");
  }
}



void MacroRunner::PopKeepers(lua_State *L)
{
  lua_getglobal(L, LUA_MODULE_NAME);
  if ( lua_istable(L, -1) ) {
    lua_pushstring(L, PERSIST_TABLE_NAME);
    lua_gettable(L, -2);
    if (lua_istable(L, -1)) {
      ClearKeepers();
      lua_pushnil(L);  // make room for first key
      while (lua_next(L, -2) != 0) { // walk the table
        if (lua_type(L, -2)==LUA_TSTRING) { // 'key' is at index -2
          PersistRecord*pr=NULL;
          switch (lua_type(L, -1)) { // 'value' is at index -1
            case LUA_TBOOLEAN: {
              pr=new PersistRecord;
              pr->t=LUA_TBOOLEAN;
              pr->b=lua_toboolean(L,-1);
              break;
            }
            case LUA_TNUMBER: {
              pr=new PersistRecord;
              pr->t=LUA_TNUMBER;
              pr->n=lua_tonumber(L,-1);
              break;
            }
            case LUA_TSTRING: {
              pr=new PersistRecord;
              pr->t=LUA_TSTRING;
              pr->s=strdup(lua_tostring(L,-1));
              break;
            }
          }
          if (pr) {
            ReplaceInDict(&keepers,lua_tostring(L,-2),pr);
          }
        }
        lua_pop(L, 1);
      }
    } else {
      if (lua_isnil(L, -1)) {
        ClearKeepers();
      }
    }
  }
}


/*
  Create a new Lua state and execute the source as
  a string of code or a script filename
*/
bool MacroRunner::RunMacro(const FXString &source, bool isfilename)
{
  FXint status;
  lua_State *L=luaL_newstate();
  luaL_openlibs(L);
  luaopen_dialog(L);
#if LUA_VERSION_NUM>=502
  lua_setglobal(L, "dialog");
#endif
  override(L,"os","exit", osexit);
  override(L,"io","stdin", NULL);
  override(L,"_G","print", print);
  close_stdin(L);
  StateInfo*si=new StateInfo(L);
  si->script=isfilename?source.text():NULL;
  states.append(si);
  lua_sethook(L,debug_hook,LUA_MASKLINE,1);
#if LUA_VERSION_NUM<502
  luaL_register(L, LUA_MODULE_NAME, LuaFuncs());
  luaL_register(L, LUA_MODULE_NAME, LuaFxUtils(TopWinPub::instance(), EXE_NAME));
  luaL_register(L, LUA_MODULE_NAME, LuaCommands(TopWinPub::instance()));
#else
  int n=0;
  const luaL_Reg*p;
  const luaL_Reg*funcs = LuaFuncs();
  const luaL_Reg*utils = LuaFxUtils(TopWinPub::instance(), EXE_NAME);
  const luaL_Reg*cmds  = LuaCommands(TopWinPub::instance());
  for (p=funcs; p->name; p++) { n++; }
  for (p=utils; p->name; p++) { n++; }
  for (p=cmds;  p->name; p++) { n++; }
  lua_createtable(L, 0, n);
  luaL_setfuncs(L, funcs, 0);
  luaL_setfuncs(L, utils, 0);
  luaL_setfuncs(L, cmds,  0);
  lua_setglobal(L, LUA_MODULE_NAME);
#endif
  override(L,LUA_MODULE_NAME,"script", scriptname);
  override(L,LUA_MODULE_NAME,"optimize", optimize);
  set_string_token(L, "_VERSION", VERSION);
  PushKeepers(L);
  if (isfilename) {
    status = luaL_loadfile(L, source.text());
  } else {
    status = luaL_loadstring(L, source.text());
  }
  switch (status) {
    case 0: {
      FXint base = lua_gettop(L); /* function index */
      lua_pushcfunction(L, traceback); /* push traceback function */
      lua_insert(L, base); /* put it under chunk and args */
      status = lua_pcall(L, 0, 0, base);
      lua_remove(L, base); /* remove traceback function */
      if (0 == status) {
        PopKeepers(L);
      } else {
        lua_gc(L, LUA_GCCOLLECT, 0); /* force garbage collection if error */
        show_error(L, si->script);
      }
      break;
    }
    case LUA_ERRSYNTAX:
      show_error(L, si->script);
      break;
    case LUA_ERRMEM:
      script_error(si->script,_("Out of memory."),true,-1);
      break;
    case LUA_ERRFILE:
      script_error(si->script,_("Failed to open script file."),true,-1);
      break;
    default:
      script_error(si->script,_("Unknown error while loading script file."),true,-1);
  }
  lua_close(L);
  states.erase(states.find(si));
  delete si;
  return (status==0);
}



/*
  Execute the specified script file, called by
  the user-made commands from the "Tools" menu.
*/
bool MacroRunner::DoFile(const FXString &filename)
{
  return RunMacro(filename, true);
}



/*
  Execute the specified source code directly,
  used by the macro playback function and the
  -e command line option.
*/
bool MacroRunner::DoString(const FXString &sourcecode)
{
  return RunMacro(sourcecode, false);
}

