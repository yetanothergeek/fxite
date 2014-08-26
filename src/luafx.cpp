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



#include <cctype>
#include <cerrno>
#include <sys/stat.h>

#ifdef WIN32
# define lstat stat
# include <io.h>
#else
# include <unistd.h>
#endif

#include <lua.hpp>
#include <fx.h>
#include <fxkeys.h>

#include "intl.h"
#include "luafx.h"

#if LUA_VERSION_NUM<502
# define lua_rawlen lua_objlen
#endif

static FXWindow*main_window=NULL;

static const char*default_title;

#ifdef WIN32

#include <windows.h>

static const char* SystemErrorStr(DWORD *e=NULL)
{
  DWORD code=e?*e:GetLastError();
  static TCHAR lpMsgBuf[512];
  lpMsgBuf[0]=0;
  FormatMessage(
      FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL, code, MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT), lpMsgBuf, sizeof(lpMsgBuf), NULL);
  lpMsgBuf[sizeof(lpMsgBuf)-1]=0;
  for (char*p=(char*)lpMsgBuf; *p; p++) { if (*p=='\r') { *p=' '; } }
  return (const char*)lpMsgBuf;
}

#else

static const char* SystemErrorStr(int *e=NULL)
{
  return strerror(e?*e:errno);
}

#endif

static int ArgErrFmt(lua_State *L, int numarg, const char *fmt, ...)
{
  FXString s;
  va_list args;
  va_start(args,fmt);
  s.vformat(fmt,args);
  va_end(args);
  return luaL_argerror(L,numarg,s.text());
}



// Limit dialog message text to 32 lines, maximum of 96 chars per line.
static void fixup_message(FXString &dst, const char*src)
{
#define msg_max_width 96
#define msg_max_height 32
  char line[msg_max_width+1];
  const char*p1=src;
  const char*p2;
  int numlines=0;
  do {
    p2=strchr(p1, '\n');
    int width = p2 ? (p2-p1) : strlen(p1);
    memset(line,0,sizeof(line));
    strncpy(line, p1, width>msg_max_width?msg_max_width:width);
    dst.append(line);
    if (p2) { dst.append('\n'); } else { return; }
    if (++numlines >= msg_max_height) { return; }
    p1=p2+1;
  } while (1);
}



// void message([title,] message [,icon])
static int message(lua_State*L)
{
  const char* title=default_title;
  const char* msg;
  FXString fmsg;
  const char* type="I";

  switch (lua_gettop(L)) {
    case 0:
    case 1: {
      msg=luaL_checkstring(L,1);
      break;
    }
    default: {
      title=luaL_checkstring(L,1);
      msg=luaL_checkstring(L,2);
      type=luaL_optstring(L,3,"I");
    }
  }
  fixup_message(fmsg, msg);
  switch (toupper(type[0])) {
    case 'W' : { FXMessageBox::warning(     main_window, MBOX_OK, title, "%s", fmsg.text()); break; }
    case 'E' : { FXMessageBox::error(       main_window, MBOX_OK, title, "%s", fmsg.text()); break; }
    default  : { FXMessageBox::information( main_window, MBOX_OK, title, "%s", fmsg.text()); break; }
  }
  return 0;
}



// bool confirm(title, question,default)
static int confirm(lua_State*L)
{
  luaL_argcheck(L,(lua_gettop(L)>=3)&&lua_isboolean(L,3),3,_("expected boolean"));
  const char* title=lua_isnil(L,1)?default_title:luaL_checkstring(L,1);
  const char* msg=luaL_checkstring(L,2);
  FXString fmsg;
  bool deflt=lua_toboolean(L,3);
  fixup_message(fmsg, msg);
  FXuint rv=FXMessageBox::question(main_window, MBOX_YES_NO, title, "%s", fmsg.text());
  lua_pushboolean(L, rv==MBOX_CLICKED_YES?true:rv==MBOX_CLICKED_NO?false:deflt);
  return 1;
}



// string input([title,] message [,default [,style]])
static int input(lua_State*L)
{
  const char* title=default_title;
  const char* msg="";
  FXString fmsg;
  FXString txt="";
  const char* type="S";
  FXint opt=INPUTDIALOG_STRING;
  switch (lua_gettop(L)) {
    case 0:
    case 1: {
      msg=luaL_checkstring(L,1);
      break;
    }
    default: {
      title=luaL_checkstring(L,1);
      msg=luaL_checkstring(L,2);
      txt=luaL_optstring(L,3,"");
      type=luaL_optstring(L,4,"S");
    }
  }
  switch (toupper(type[0])) {
    case 'I' : {
      if (!lua_isnil(L,3)) {
        txt.format("%ld", (long)luaL_checkinteger(L,3));
      }
      opt=INPUTDIALOG_INTEGER;
      break;
    }
    case 'R' : {
      if (!lua_isnil(L,3)) { txt.format("%g", luaL_checknumber(L,3)); }
      opt=INPUTDIALOG_REAL;
      break;
    }
    case 'P' : {
      opt=INPUTDIALOG_PASSWORD;
      break;
    }
    default  : {
      break;
    }
  }
  fixup_message(fmsg, msg);
  FXInputDialog dlg(main_window,title,fmsg.text(),NULL,opt);
  dlg.setText(txt);
  if (dlg.execute(PLACEMENT_OWNER)) {
    lua_pushstring(L,dlg.getText().text());
  } else {
    lua_pushnil(L);
  }
  return 1;
}



class PickList:public FXList {
  FXDECLARE(PickList);
  PickList(){}
public:
  long onPicked(FXObject*o, FXSelector sel, void*p) {
    switch (FXSELTYPE(sel)) {
      case SEL_DOUBLECLICKED: { break; }
      case SEL_KEYPRESS: {
        FXint code=((FXEvent*)p)->code;
        if ((code==KEY_Return)||(code==KEY_KP_Enter)) { break; }
      }
      default: return 0;
    }
    ((FXDialogBox*)getParent())->handle(this, FXSEL(SEL_COMMAND,FXMessageBox::ID_ACCEPT),NULL);
    return 1;
  }
  PickList(FXComposite*p):FXList(p, this,FXList::ID_LAST,LAYOUT_FILL_X|LIST_BROWSESELECT){}
};



FXDEFMAP(PickList) ClickListMap[] = {
  FXMAPFUNC(SEL_DOUBLECLICKED, FXList::ID_LAST, PickList::onPicked),
  FXMAPFUNC(SEL_KEYPRESS, FXList::ID_LAST, PickList::onPicked),
};

FXIMPLEMENT(PickList, FXList, ClickListMap, ARRAYNUMBER(ClickListMap))



// string choose([title,] message, {items})
static int choose(lua_State*L) {
  int argmsg=1;
  int argtbl=2;
  FXString title=default_title;
  if (lua_gettop(L)>2) {
    title=luaL_checkstring(L,1);
    argmsg++;
    argtbl++;
  }
  const char*msg=luaL_checkstring(L,argmsg);
  FXString fmsg;
  int i,n;
  luaL_argcheck(L, lua_istable(L,argtbl), argtbl, _("table expected") );
  n=lua_rawlen(L,argtbl);
  luaL_argcheck(L, n>0, argtbl, _("table can't be empty"));
  for (i=1;i<=n; i++) {
    lua_rawgeti(L,argtbl,i);
    if (!lua_isstring(L, -1)) {
      ArgErrFmt(L,argtbl,_("table element #%d is not a string"), i);
    }
    lua_pop(L, 1);
  }
  FXDialogBox dlg(main_window, title);
  fixup_message(fmsg, msg);
  new FXLabel(&dlg, fmsg.text());
  FXList*list=new PickList(&dlg);
  list->setNumVisible(n<12?n:12);
  for (i=1;i<=n; i++) {
    lua_rawgeti(L,argtbl,i);
    list->appendItem(lua_tostring(L, -1));
    lua_pop(L, 1);
  }
  FXHorizontalFrame*btns=new FXHorizontalFrame(&dlg,FRAME_NONE|LAYOUT_FILL|PACK_UNIFORM_WIDTH);
  FXButton*ok=new FXButton(btns, _("&OK"), NULL, &dlg,FXDialogBox::ID_ACCEPT);
  new FXButton(btns, _(" &Cancel "), NULL, &dlg,FXDialogBox::ID_CANCEL);
  ok->setDefault(true);
  list->setFocus();
  dlg.create();
  int want_width=list->getContentWidth()+list->verticalScrollBar()->getWidth()+dlg.getPadLeft()*2;
  if (want_width<dlg.getDefaultWidth()) { want_width=dlg.getDefaultWidth(); }
  int max_width=main_window->getApp()->getRootWindow()->getWidth()*0.75;
  dlg.setWidth(want_width>max_width?max_width:want_width);
  if ( dlg.execute(PLACEMENT_OWNER) ) {
    lua_pushstring(L,list->getItemText(list->getCurrentItem()).text());
  } else {
    lua_pushnil(L);
  }
  return 1;
}


//pickfile(mode[[,path][,filter]])
static int pickfile(lua_State*L)
{
  const char*modes[]={"open", "save", "dir", NULL};
  int mode=luaL_checkoption(L,1,modes[0], modes);
  FXString pathstr=FXSystem::getCurrentDirectory()+PATHSEPSTRING;
  const char*path=luaL_optstring(L,2,pathstr.text());
  const char*patt=luaL_optstring(L,3,_("All files (*)"));
  if (path && !*path) { path=pathstr.text(); }
  if (patt) {
    FXFileDialog dlg(main_window, "");
    if (mode!=2) {
      if (FXStat::isDirectory(path)) {
        dlg.setDirectory(path);
      } else {
        pathstr=FXPath::simplify(FXPath::absolute(path));
        if (FXStat::isDirectory(pathstr)) {
          dlg.setDirectory(pathstr);
        } else {
          if (FXStat::isDirectory(FXPath::directory(pathstr))) {
            dlg.setFilename(FXPath::name(pathstr));
            dlg.setDirectory(FXPath::directory(pathstr));
          } else {
            FXMessageBox::warning( main_window, MBOX_OK, _("No such directory"), "%s:\n%s",
               _("Specified path does not exist"),
               (FXPath::directory(pathstr)+PATHSEPSTRING).text()
            );
            dlg.setDirectory(FXSystem::getCurrentDirectory());
          }
        }
      }
    }

    dlg.setPatternList(patt);
    bool rv=false;
    switch (mode) {
      case 0:{
        dlg.setSelectMode(SELECTFILE_EXISTING);
        dlg.setTitle(_("Open file"));
        rv=dlg.execute(PLACEMENT_OWNER);
        break;
      }
      case 1:{
        dlg.setSelectMode(SELECTFILE_ANY);
        dlg.setTitle(_("Save file as"));
        while (dlg.execute(PLACEMENT_OWNER)) {
          if (FXStat::exists(dlg.getFilename())) {
            if (FXMessageBox::question(main_window, MBOX_YES_NO, _("Overwrite?"),
                 "%s:\n%s\n\n%s", _("File exists"), dlg.getFilename().text(),
                 _("Do you want to replace it?")
                 )==MBOX_CLICKED_YES
               )
            {
              rv=true;
              break;
            }
          } else {
            rv=true;
            break;
          }
        }
        break;
      }
      case 2: {
        FXDirDialog dir(main_window, _("Select Directory"));
        dir.setHeight(420);
        if ( FXStat::exists(path) && !FXStat::isDirectory(path) ) {
          dir.setDirectory(FXPath::directory(path));
        } else {
          dir.setDirectory(FXSystem::getCurrentDirectory()+PATHSEP);
        }
        rv=dir.execute(PLACEMENT_OWNER);
        if (rv) { dlg.setFilename(dir.getDirectory()); } // Pass result to file dialog.
        break;
      }
    }
    if (rv) { lua_pushstring(L, dlg.getFilename().text()); } else { lua_pushnil(L); }
    return 1;
  } else {
    return 0;
  }
}


// function basename( pathstr ) -- Extract the filename portion of a path string.
static int basename(lua_State*L)
{
  const char*s=luaL_checkstring(L,1);
  lua_pushstring(L, FXPath::name(s).text());
  return 1;
}



// function dirname( pathstr ) -- Get the directory portion of a file's path.
static int dirname(lua_State*L)
{
  const char*s=luaL_checkstring(L,1);
  lua_pushstring(L, FXPath::directory(s).text());
  return 1;
}



// function fullpath( filename ) -- Get the full path to a file.
static int fullpath(lua_State*L)
{
  if (lua_gettop(L)>=2) {
    const char*dn=luaL_checkstring(L,1);
    const char*fn=luaL_checkstring(L,2);
    lua_pushstring(L, FXPath::simplify(FXPath::absolute(dn,fn)).text());
  } else {
    const char*fn=luaL_checkstring(L,1);
    lua_pushstring(L, FXPath::simplify(FXPath::absolute(fn)).text());
  }
  return 1;
}



static int dirsep(lua_State*L)
{
  lua_pushstring(L, PATHSEPSTRING);
  return 1;
}



#define SetTableValue(name,value,pusher) \
  lua_pushstring(L, name); \
  pusher(L, value); \
  lua_rawset(L,-3);

#define SetTableStr(name,value) SetTableValue(name,value,lua_pushstring)
#define SetTableBool(name,value) SetTableValue(name,value,lua_pushboolean)
#define SetTableNum(name,value) SetTableValue(name,(lua_Number)value,lua_pushnumber)



typedef int (*statfunc) (const char *fn, struct stat *st);

static int _stat(lua_State* L)
{
  statfunc sf=stat;
  const char*fn=NULL;
  struct stat st;
  if (lua_gettop(L)>=2) {
    luaL_argcheck(L, lua_isboolean(L,2), 2, _("expected boolean"));
    sf=lua_toboolean(L,2)?lstat:stat;
  }
  fn=luaL_checkstring(L,1);
  if (sf(fn,&st)==0) {
    const char *ft=NULL;
    switch ( st.st_mode & S_IFMT) {
      case S_IFBLK:ft="b"; break;
      case S_IFCHR:ft="c"; break;
      case S_IFDIR:ft="d"; break;
      case S_IFIFO:ft="p"; break;
      case S_IFREG:ft="f"; break;
      #ifndef WIN32
      case S_IFLNK:ft="l"; break;
      case S_IFSOCK:ft="s"; break;
      #endif
    }
    lua_newtable(L);
    SetTableNum("size",st.st_size);
    SetTableNum("time",st.st_mtime);
    SetTableStr("type",ft);
    SetTableBool("read", (access(fn,R_OK)==0));
    SetTableBool("write", (access(fn,W_OK)==0));
    SetTableBool("exec", (access(fn,X_OK)==0));
    return 1;
  }
  lua_pushnil(L);
  lua_pushstring(L, SystemErrorStr());
  return 2;
}



// function wkdir ( [folder] ) -- Get or set the current working directory.
static int wkdir(lua_State* L)
{
  if (lua_gettop(L)==0) {
    lua_pushstring(L, FXSystem::getCurrentDirectory().text());
    return 1;
  } else {
    const char*dn=luaL_checkstring(L,1);
    if (FXSystem::setCurrentDirectory(dn)) {
      lua_pushboolean(L, true);
      return 1;
    } else {
      lua_pushboolean(L, false);
      lua_pushstring(L, SystemErrorStr());
      return 2;
    }
  }
}



// function mkdir(path,parented) -- create a directory, create full path if parented
static int mkdir(lua_State* L)
{
  const char*path=luaL_checkstring(L,1);
  int parented = (lua_gettop(L)>=2) ? lua_toboolean(L,2) : false;
  if (parented) {
    FXString parts=FXPath::absolute(path);
    FXString dirs=FXPath::root(parts);
    parts.append(PATHSEP);
    FXint nseps=parts.contains(PATHSEP);
    FXint i;
    for (i=1;i<nseps;i++) {
      dirs.append(parts.section(PATHSEP,i));
      if (!(FXStat::isDirectory(dirs)||FXDir::create(dirs,FXIO::OwnerFull))) {
        lua_pushboolean(L, false);
        lua_pushstring(L, SystemErrorStr());
        return 2;
      }
      dirs.append(PATHSEP);
    }
    lua_pushboolean(L, true);
    return 1;
  } else {
    if (FXDir::create(path)) {
      lua_pushboolean(L, true);
      return 1;
    } else {
      lua_pushboolean(L, false);
      lua_pushstring(L, SystemErrorStr());
      return 2;
    }
  }
}



#ifdef FOX_1_6
static int dirlist_closure(lua_State *L)
{
  FXDir*dir=(FXDir*)lua_touserdata(L,lua_upvalueindex(1));
  if (dir->next()) {
    lua_pushstring(L,dir->name().text());
    return 1;
  } else {
    dir->close();
    delete dir;
    return 0;
  }
}
#else
static int dirlist_closure(lua_State *L)
{
  FXDir*dir=(FXDir*)lua_touserdata(L,lua_upvalueindex(1));
  FXString name;
  if (dir->next(name)) {
    lua_pushstring(L,name.text());
    return 1;
  } else {
    dir->close();
    delete dir;
    return 0;
  }
}
#endif



//  function dirlist( path ) -- List the contents of a folder.
static int dirlist(lua_State* L)
{
  const char*dn=luaL_optstring(L,1, ".");
  FXDir *dir=new FXDir(dn);
  if (!dir->isOpen()) {
#ifdef WIN32
    DWORD e=GetLastError();
#else
    int e=errno;
#endif
    delete dir;
    return luaL_argerror(L,1,SystemErrorStr(&e));
  }
  lua_pushlightuserdata(L,dir);
  lua_pushcclosure(L,&dirlist_closure,1);
  return 1;
}



static int window(lua_State* L)
{
  lua_pushnumber(L,(lua_Number)((FXuval)main_window->id()));
  return(1);
}



static int pid(lua_State* L)
{
#if (FOX_MAJOR>1) || \
    ( (FOX_MAJOR==1) && (FOX_MINOR>7) ) || \
    ( (FOX_MAJOR==1) && (FOX_MINOR==7) && (FOX_LEVEL>26) )
  lua_pushnumber(L,FXProcess::current());
#else
  lua_pushnumber(L,fxgetpid());
#endif

  return(1);
}




static const struct luaL_Reg fx_util_funcs[] = {
  {"message", message},
  {"confirm", confirm},
  {"input", input},
  {"choose", choose},
  {"pickfile", pickfile},
  {"basename", basename},
  {"dirname", dirname},
  {"fullpath", fullpath},
  {"dirsep", dirsep},
  {"stat", _stat},
  {"wkdir", wkdir},
  {"mkdir", mkdir},
  {"dirlist", dirlist},
  {"window", window},
  {"pid", pid},
  {NULL,NULL}
};



const luaL_Reg* LuaFxUtils(FXWindow*topwin, const char*exe_name)
{
  default_title=exe_name;
  if (!main_window) { main_window=topwin; }
  return fx_util_funcs;
}

