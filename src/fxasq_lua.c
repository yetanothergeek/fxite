/*
  fxasq_lua.c - Lua bindings for fxasq.cpp
  Copyright (c) 2007-2012 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

*/


#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "fxasq_c.h"

#include "intl.h"
#include "fxasq_lua.h"

#if LUA_VERSION_NUM<502
# define lua_rawlen lua_objlen
#endif

#define DLG_MODULE_NAME "dialog"
#define MetaName "_fxasq_metatable"


static const char*DialogBoxType="DialogBox";

typedef struct _DialogBox {
  const char*id;
  FxAsqDlg *dlg;
} DialogBox;



static DialogBox* todialog(lua_State *L, int argnum)
{
  DialogBox*rv=(DialogBox*)lua_touserdata(L,argnum);
  return (rv && (DialogBoxType==rv->id))?rv:NULL;
}



#define DLG_REQUIRE \
  DialogBox*D=todialog(L,1); \
  luaL_argcheck(L,D,1,_("expected DialogBox"))




static int asq_checkbox(lua_State *L)
{
  DLG_REQUIRE;
  const char*label=luaL_checkstring(L,4);
  luaL_argcheck(L,lua_isboolean(L,3),3,_("expected boolean"));
  int value=lua_toboolean(L,3);
  const char*key=luaL_checkstring(L,2);
  fxasq_checkbox(D->dlg, key,value,label);
  return 0;
}



static int asq_hr(lua_State *L)
{
  DLG_REQUIRE;
  fxasq_hr(D->dlg);
  return 0;
}



static int asq_text(lua_State *L)
{
  DLG_REQUIRE;
  const char*label=luaL_checkstring(L,4);
  const char*value=lua_isnil(L,3)?NULL:luaL_checkstring(L,3);
  const char*key=luaL_checkstring(L,2);
  fxasq_text(D->dlg, key,value,label);
  return 0;
}



static int asq_password(lua_State *L)
{
  DLG_REQUIRE;
  const char*label=luaL_checkstring(L,4);
  const char*value=lua_isnil(L,3)?NULL:luaL_checkstring(L,3);
  const char*key=luaL_checkstring(L,2);
  fxasq_password(D->dlg, key,value,label);
  return 0;
}



static int asq_file(lua_State *L)
{
  DLG_REQUIRE;
  const char*label=luaL_checkstring(L,4);
  const char*value=lua_isnil(L,3)?NULL:luaL_checkstring(L,3);
  const char*key=luaL_checkstring(L,2);
  fxasq_file(D->dlg, key,value,label);
  return 0;
}



static int asq_color(lua_State *L)
{
  DLG_REQUIRE;
  const char*label=lua_isnil(L,4)?NULL:luaL_checkstring(L,4);
  const char*value=lua_isnil(L,3)?NULL:luaL_checkstring(L,3);
  const char*key=luaL_checkstring(L,2);
  fxasq_color(D->dlg, key,value,label);
  return 0;
}



static int asq_font(lua_State *L)
{
  DLG_REQUIRE;
  const char*label=lua_isnil(L,4)?NULL:luaL_checkstring(L,4);
  const char*value=lua_isnil(L,3)?NULL:luaL_checkstring(L,3);
  const char*key=luaL_checkstring(L,2);
  fxasq_font(D->dlg, key,value,label);
  return 0;
}



static int asq_textarea(lua_State *L)
{
  DLG_REQUIRE;
  const char*label=lua_isnil(L,4)?NULL:luaL_checkstring(L,4);
  const char*value=lua_isnil(L,3)?NULL:luaL_checkstring(L,3);
  const char*key=luaL_checkstring(L,2);
  fxasq_textarea(D->dlg, key,value,label);
  return 0;
}



static int asq_group(lua_State *L)
{
  DLG_REQUIRE;
  const char*label=lua_isnil(L,4)?NULL:luaL_checkstring(L,4);
  const char*value=lua_isnil(L,3)?NULL:luaL_checkstring(L,3);
  const char*key=luaL_checkstring(L,2);
  fxasq_group(D->dlg,key,value,label);
  return 0;
}



static int asq_select(lua_State *L)
{
  DLG_REQUIRE;
  const char*label=lua_isnil(L,4)?NULL:luaL_checkstring(L,4);
  const char*value=lua_isnil(L,3)?NULL:luaL_checkstring(L,3);
  const char*key=luaL_checkstring(L,2);
  fxasq_select(D->dlg, key,value,label);
  return 0;
}



static int asq_radio(lua_State *L)
{
  DLG_REQUIRE;
  const char*label=luaL_checkstring(L,4);
  const char*value=luaL_checkstring(L,3);
  const char*key=luaL_checkstring(L,2);
  fxasq_radio(D->dlg, key,value,label);
  return 0;
}



static int asq_option(lua_State *L)
{
  DLG_REQUIRE;
  const char*label=luaL_checkstring(L,4);
  const char*value=luaL_checkstring(L,3);
  const char*key=luaL_checkstring(L,2);
  fxasq_option(D->dlg,key,value,label);
  return 0;
}



static int asq_label(lua_State *L)
{
  DLG_REQUIRE;
  const char*text=luaL_checkstring(L,2);
  fxasq_label(D->dlg,text);
  return 0;
}



static int asq_heading(lua_State *L)
{
  DLG_REQUIRE;
  const char*text=luaL_checkstring(L,2);
  fxasq_heading(D->dlg,text);
  return 0;
}



static int asq_new(lua_State *L) {
  int argc=lua_gettop(L);
  FxAsqStr title=NULL;
  FxAsqStr *btns;
  int i,n;
  DialogBox*D;
  if (argc>=1) {
    title=luaL_checkstring(L,1);
  }
  if (argc>=2) {
    luaL_argcheck(L,lua_istable(L,2),2,_("expected table"));
  }
  n=lua_rawlen(L,2);
  for (i=1;i<=n; i++) {
    lua_rawgeti(L,2,i);
    char msg[64];
    memset(msg,0,sizeof(msg));
    snprintf(msg,sizeof(msg)-1, _("table element #%d is not a string"), i);
    luaL_argcheck(L,lua_isstring(L, -1),2,msg);
    lua_pop(L, 1);
  }
  btns=(const char**)calloc(sizeof(char*)*(n+1), 1);
  for (i=1;i<=n; i++) {
    lua_rawgeti(L,2,i);
    btns[i-1]=lua_tostring(L, -1);
    lua_pop(L, 1);
  }
  D=(DialogBox*)lua_newuserdata(L,sizeof(DialogBox));
  luaL_getmetatable(L, MetaName);
  lua_setmetatable(L, -2);
  D->id=DialogBoxType;
  D->dlg=fxasq_new(title,btns);
  free(btns);
  return 1;
}



static int asq_run(lua_State *L)
{
  int rv=-1;
  FxAsqItem *results;
  DLG_REQUIRE;
  results=fxasq_run(D->dlg, &rv, L);
  lua_pushnumber(L,(rv+1));
  if (results) {
    FxAsqItem*p;
    lua_newtable(L);
    for (p=results; p; p=p->next) {
      lua_pushstring(L, p->key);
      lua_pushstring(L, p->value);
      lua_rawset(L,-3);
    }
    fxasq_free_results(results);
    return 2;
  }
  return 1;
}



static int asq_done(lua_State *L)
{
  DialogBox*D;
  if (lua_isnil(L, 1)) {
    return 0;
  }
  D=(DialogBox*)lua_touserdata(L,1);
  if (D->id!=DialogBoxType) {

    return 1;
  }
  fxasq_free(D->dlg);
  return 1;
}




static const struct luaL_Reg asq_funcs[] = {
  {"new",      asq_new},
  {"run",      asq_run},
  {"label",    asq_label},
  {"text",     asq_text},
  {"select",   asq_select},
  {"option",   asq_option},
  {"group",    asq_group},
  {"radio",    asq_radio},
  {"password", asq_password},
  {"heading",  asq_heading},
  {"checkbox", asq_checkbox},
  {"hr",       asq_hr},
  {"textarea", asq_textarea},
  {"file",     asq_file},
  {"color",    asq_color},
  {"font",     asq_font},
  {0,0}
};



int luaopen_dialog(lua_State *L)
{
  luaL_newmetatable(L, MetaName);
  lua_pushstring(L, "__index");
  lua_pushvalue(L, -2);
  lua_settable(L, -3);

  lua_pushstring(L,"__gc");
  lua_pushcfunction(L,asq_done);
  lua_rawset(L,-3);

#if LUA_VERSION_NUM < 502
  luaL_register(L, NULL, &asq_funcs[1]);
  luaL_register(L, DLG_MODULE_NAME, asq_funcs);
#else
  luaL_setfuncs(L,asq_funcs,0);
#endif
  return 1;
}
