/*
  lua_ctokens.c - Lua bindings to a simple parser for "C" style languages.
  Copyright (C) 2015 Jeff Pohlmeyer <yetanothergeek@gmail.com>
  
  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the
  "Software"), to deal in the Software without restriction, including
  without limitation the rights to use, copy, modify, merge, publish,
  distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to
  the following conditions:
  
  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdlib.h>
#include <string.h>
#include <lauxlib.h>
#include "lua_ctokens.h"
#include "ctokens.h"

#define CTOKENS_META_NAME "ctokens"

#define SetTableValue(name,value,pusher) \
  lua_pushstring(L, name); \
  pusher(L, value); \
  lua_rawset(L,-3);

#define SetTableStr(name,value) SetTableValue(name,value,lua_pushstring)
#define SetTableInt(name,value) SetTableValue(name,(lua_Number)value,lua_pushinteger)


#if LUA_VERSION_NUM>501
# define TableLength lua_rawlen
#else
# define TableLength lua_objlen
#endif


typedef struct _cbdata {
  lua_State*L;
  int func_idx;
  int data_idx;
} cbdata;



const char* kinds[] = {
  "",
  "name",
  "keyword",
  "comment",
  "comment",
  "char",
  "string",
  "symbol",
  "number",
  "space",
  "preproc",
  "eol",
  ""
};




int ctokens_callback(const TokenInfo*info)
{
  cbdata*c=info->user_data;
  lua_State*L=c->L;
  lua_rawgeti(L, LUA_REGISTRYINDEX, c->func_idx);
  lua_newtable(L);
  lua_pushstring(L, "token");
  lua_pushlstring(L,info->token,info->len);
  lua_rawset(L,-3);
  SetTableStr("kind",kinds[info->type]);
  SetTableInt("line", info->line);
  SetTableInt("column", info->col);
  if (c->data_idx!=LUA_REFNIL) {
    lua_pushstring(L, "data");
    lua_rawgeti(L, LUA_REGISTRYINDEX, c->data_idx);
    lua_rawset(L,-3);
  }
  lua_call(c->L, 1, 1);
  return lua_toboolean(c->L,-1);
}


/*
parse(src,func,[udata[,kwords[,utf8]]])
*/
static int ctokens_parse(lua_State*L)
{ 
  cbdata c;
  const char**kwords=NULL;
  const char*src=luaL_checkstring(L,1);
  int utf8=1;
  luaL_argcheck(L,lua_isfunction(L,2),2,"expected function");
  while (lua_gettop(L)>5) { lua_pop(L,1); }
  if (lua_gettop(L)==5) {
    utf8=lua_toboolean(L,5);
    lua_pop(L,1);
  }
  if (lua_gettop(L)==4) {
    int n=0;
    int i=1;
    luaL_argcheck(L, lua_istable(L,4), 4, "expected table");
    n=TableLength(L, 4);
    i=1;
    kwords=(const char**)calloc(n+1,sizeof(const char*));
    for (i=1; i<=n; i++) {
      lua_rawgeti(L, 4, i);
      if (lua_isstring(L,-1)) {
        kwords[i-1]=lua_tostring(L, -1);
      } else {
        free(kwords);
        luaL_argerror(L,4,"table element is not a string");
      }
      lua_pop(L,1);
    }
    lua_pop(L,1);
  }
  c.L=L;
  c.data_idx=LUA_REFNIL;
  if ((lua_gettop(L)>2)) {
    if (lua_isnil(L,3)) {
      lua_pop(L,1);
    } else {
      c.data_idx=luaL_ref(L,LUA_REGISTRYINDEX);
    }
  }
  c.func_idx=luaL_ref(L,LUA_REGISTRYINDEX);
  tokenize(src, ctokens_callback, (void*)&c, kwords, utf8);
  luaL_unref(L,LUA_REGISTRYINDEX,c.func_idx);
  luaL_unref(L,LUA_REGISTRYINDEX,c.data_idx);
  if (kwords) { free(kwords); }
  return 0;
}



static const struct luaL_Reg ctokens_funcs[] = {
  {"parse", ctokens_parse},
  {NULL,NULL}
};



int luaopen_ctokens(lua_State*L)
{
  luaL_newmetatable(L, CTOKENS_META_NAME);
  lua_pushstring(L, "__index");
  lua_pushvalue(L, -2);
  lua_settable(L, -3);
#if LUA_VERSION_NUM < 502
  luaL_register(L, NULL, &ctokens_funcs[1]);
  luaL_register(L, CTOKENS_META_NAME, ctokens_funcs);
#else
  luaL_setfuncs(L,ctokens_funcs,0);
#endif
  return 1;
}

