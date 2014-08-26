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


#include <lua.hpp>
#include <fx.h>
#include <FX88591Codec.h>
#include <FXUTF16Codec.h>

#include "compat.h"
#include "macro.h"
#include "appwin_pub.h"
#include "scidoc.h"
#include "doctabs.h"
#include "filer.h"
#include "prefs_base.h"
#include "scisrch.h"
#include "luasci.h"

#include "intl.h"
#include "luafuncs.h"

#if LUA_VERSION_NUM<502
# define lua_rawlen lua_objlen
#endif


/*
  Not impemented:
  function keygrab ( [prompt] ) -- Intercept a keystroke from the editor.
  var wordchars : string -- The characters that are considered part of a word.
*/

static const char *LUA_QUIT_MESSAGE="just ignore me, i'm quitting now";

const char *LuaQuitMessage()
{
  return LUA_QUIT_MESSAGE;
}


class SelSaver;

static SelSaver* selsaver=NULL;

#define DOC_REQD \
SciDoc*sci=TopWinPub::FocusedDoc(); \
if (!sci) { \
  luaL_error(L,_("No active document!")); \
  return 0; \
}

#define CheckReadOnly() { if (sci->GetReadOnly()) { luaL_error(L, _("Attempt to modify a read-only document")); } }

#define sendPointer sendString


static int ArgErrFmt(lua_State *L, int numarg, const char *fmt, ...)
{
  FXString s;
  va_list args;
  va_start(args,fmt);
  s.vformat(fmt,args);
  va_end(args);
  return luaL_argerror(L,numarg,s.text());
}



static bool check_find_flags(lua_State* L, int pos, int &flags)
{
  flags=0;
  luaL_argcheck(L, lua_istable(L,pos), pos, _("expected table"));
  int n=lua_rawlen(L, pos);
  for (int i=1; i<=n; i++) {
    lua_rawgeti(L, pos, i);
    if (lua_isstring(L,-1)) {
      const char*flagname = lua_tostring(L, -1);
      if (strcasecmp(flagname, "matchcase") == 0){
        flags+= SCFIND_MATCHCASE;
      } else if (strncasecmp(flagname, "wholeword", 5) == 0) {
        flags+= SCFIND_WHOLEWORD;
      } else if (strncasecmp(flagname, "regexp", 5) == 0) {
        flags+= SCFIND_REGEXP;
      } else {
        ArgErrFmt(L, pos, _("unknown flag at table element #%d"), i);
      }
    } else {
      ArgErrFmt(L, pos, _("table element #%d is not a string"), i);
    }
    lua_pop(L,1);
  }
  if ( (flags&SCFIND_WHOLEWORD) && (flags&SCFIND_REGEXP) ) {
    luaL_argerror(L, pos, _("Cannot specify \"wholeword\" and \"regexp\" together."));
  }
  return true;
}



// find(phrase, start, stop, {options})
static int find(lua_State* L)
{
  int flags=0;
  DOC_REQD
  long cpMin=luaL_checklong(L,2);
  long cpMax=luaL_checklong(L,3);
  if (!check_find_flags(L,4,flags)) { return 0; }
  const char*lpstrText=luaL_checkstring(L,1);
  int rv=sci->search->FindTextNoSel(lpstrText,flags,cpMin,cpMax);
  switch(rv) {
    case 0: { break; }
    case 1: {
      lua_pushnumber(L,cpMin);
      lua_pushnumber(L,cpMax);
      return 2;
    }
    default: {
      ArgErrFmt(L,1,_("Invalid regular expression: %s"),FXRex::getError((FXRexError)(-rv)) );
      break;
    }
  }
  return 0;
}



// gofind(phrase, {options} [, forward])
static int gofind(lua_State* L)
{
  DOC_REQD;
  int flags=0;
  const char*lpstrText=luaL_checkstring(L,1);
  if (!check_find_flags(L,2,flags)) { return 0; }
  bool forward=(lua_gettop(L)>2)?lua_toboolean(L,3):true;
  TopWinPub::FindText(lpstrText,flags,forward);
  return 0;
}


// replace(search_for, replace_with, {options}[, scope] ) -- scope is one of "next" "prev" "sel" "all"
static int replace(lua_State* L)
{
  DOC_REQD
  int flags=0;
  const char* search_for=luaL_checkstring(L,1);
  const char* replace_with=luaL_checkstring(L,2);
  if (!check_find_flags(L,3,flags)) { return 0; }
  const char* scopes[]={"next","prev","sel","all",NULL};
  int scope=luaL_checkoption(L,4,"next",scopes);
  switch (scope) {
    case 0:
    case 1: {
      TopWinPub::FindAndReplace(search_for,replace_with,flags,scope==0?true:false);
      break;
    }
    case 2: {
      TopWinPub::ReplaceAllInSelection(search_for,replace_with,flags);
      break;
    }
    case 3: {
      TopWinPub::ReplaceAllInDocument(search_for,replace_with,flags);
      break;
    }
  }
  return 0;
}





static int match(lua_State* L)
{
  long pos;
  DOC_REQD
  if (lua_gettop(L)==0) {
    pos=sci->GetCaretPos();
  } else {
    pos=luaL_checklong(L,1);
  }
  lua_pushnumber(L,sci->sendMessage(SCI_BRACEMATCH, pos, 0));
  return 1;
}



//  function word ( [position] ) -- Get the word at the specified location.
static int word(lua_State* L)
{
  DOC_REQD
  long pos=luaL_optlong(L,1,-1);
  FXString s="";
  sci->WordAtPos(s,pos);
  lua_pushstring(L, s.text());
  return 1;
}




//  Translate between rectangular (line/column) and linear (position) locations.
static int rowcol(lua_State* L)
{
  int argc=lua_gettop(L);
  int line,col,pos,len,cnt;
  DOC_REQD
  switch (argc) {
    case 0:
    case 1: {
      if (argc==0) {
        pos=sci->GetCaretPos();
      } else {
        pos=luaL_checkinteger(L,1);
        if ( pos < 0 ) {
          pos=0;
        } else {
          len=sci->GetTextLength();
          if ( pos >= len ) { pos=len-1; }
        }
      }
      line=sci->sendMessage(SCI_LINEFROMPOSITION,pos,0);
      col=sci->sendMessage(SCI_GETCOLUMN,pos,0);
      lua_pushnumber(L,line+1);
      lua_pushnumber(L,col);
      return 2;
    }
    default: {
      line=luaL_checkinteger(L,1);
      if ( line < 1 ) {
        line=1;
      } else {
        cnt=sci->GetLineCount();
        if ( line > cnt ) { line=cnt; }
      }
      col=luaL_checkinteger(L,2);
      if ( col < 0 ) {
        col=0;
      } else {
        len=sci->GetLineLength(line);
        if (col>=len) {col=len-1;}
      }
      pos=sci->sendMessage(SCI_POSITIONFROMLINE,line-1,0)+col;
      lua_pushnumber(L,pos);
      return 1;
    }
  }
}


// Set or get the endpoints of the scintilla selection
static int select(lua_State* L)
{
  int argc=lua_gettop(L);
  int sel_start, sel_end;
  bool rectsel=false;
  DOC_REQD

  if (0==argc) {
    rectsel=sci->sendMessage(SCI_SELECTIONISRECTANGLE, 0, 0);
  } else {
    if (3==argc) { rectsel=lua_toboolean(L,3);  }
  }
  if (0==argc) {
    sel_end=sci->sendMessage(SCI_GETCURRENTPOS,0,0);
    sel_start=sci->sendMessage(SCI_GETANCHOR, 0, 0);
    lua_pushnumber(L, sel_start);
    lua_pushnumber(L, sel_end);
    lua_pushboolean(L, rectsel);
    return 3;
  } else {
    sel_start=luaL_checkinteger(L,1);
    sel_end=(1==argc)?sel_start:luaL_checkinteger(L,2);
    sci->sendMessage(SCI_SETSELECTIONMODE, rectsel?SC_SEL_RECTANGLE:SC_SEL_STREAM, 0);
    sci->sendMessage(SCI_SETCURRENTPOS, sel_end, false);
    sci->sendMessage(SCI_SETANCHOR, sel_start, 0);
    sci->ScrollCaret();
    sci->sendMessage(SCI_SETSELECTIONMODE, rectsel?SC_SEL_RECTANGLE:SC_SEL_STREAM, 0);
    return 0;
  }
}



static int GetSciNavCmd(const char*str, bool fwd, bool sel, bool rect, bool smarthome)
{
  if (strncasecmp(str, "char", 4) == 0) {
    if (fwd) {
      return sel?(rect?SCI_CHARRIGHTRECTEXTEND:SCI_CHARRIGHTEXTEND):SCI_CHARRIGHT;
    } else {
      return sel?(rect?SCI_CHARLEFTRECTEXTEND:SCI_CHARLEFTEXTEND):SCI_CHARLEFT;
    }
  } else if (strncasecmp(str, "word", 4) == 0) {
    if (fwd) {
      return sel?SCI_WORDRIGHTEXTEND:SCI_WORDRIGHT;
    } else {
      return sel?SCI_WORDLEFTEXTEND:SCI_WORDLEFT;
    }
  } else if (strncasecmp(str, "part", 4) == 0) {
    if (fwd) {
      return sel?SCI_WORDPARTRIGHTEXTEND:SCI_WORDPARTRIGHT;
    } else {
      return sel?SCI_WORDPARTLEFTEXTEND:SCI_WORDPARTLEFT;
    }
  } else if (strncasecmp(str, "edge", 4) == 0) {
    if (fwd) {
      return sel?(rect?SCI_LINEENDRECTEXTEND:SCI_LINEENDEXTEND):SCI_LINEEND;
    } else {
      if (smarthome) {
        return sel?(rect?SCI_VCHOMERECTEXTEND:SCI_VCHOMEEXTEND):SCI_VCHOME;
      } else {
        return sel?(rect?SCI_HOMERECTEXTEND:SCI_HOMEEXTEND):SCI_HOME;
      }
    }
  } else if (strncasecmp(str, "line", 4) == 0) {
    if (fwd) {
      return sel?(rect?SCI_LINEDOWNRECTEXTEND:SCI_LINEDOWNEXTEND):SCI_LINEDOWN;
    } else {
      return sel?(rect?SCI_LINEUPRECTEXTEND:SCI_LINEUPEXTEND):SCI_LINEUP;
    }
  } else if (strncasecmp(str, "para", 4) == 0) {
    if (fwd) {
      return sel?SCI_PARADOWNEXTEND:SCI_PARADOWN;
    } else {
      return sel?SCI_PARAUPEXTEND:SCI_PARAUP;
    }
  } else if (strncasecmp(str, "page", 4) == 0) {
    if (fwd) {
      return sel?(rect?SCI_PAGEDOWNRECTEXTEND:SCI_PAGEDOWNEXTEND):SCI_PAGEDOWN;
    } else {
      return sel?(rect?SCI_PAGEUPRECTEXTEND:SCI_PAGEUPEXTEND):SCI_PAGEUP;
    }
  } else if (strncasecmp(str, "body", 4) == 0) {
    if (fwd) {
      return sel?SCI_DOCUMENTENDEXTEND:SCI_DOCUMENTEND;
    } else {
      return sel?SCI_DOCUMENTSTARTEXTEND:SCI_DOCUMENTSTART;
    }
  } else return SCI_NULL;
}



// go(mode,count [,extend [,rect]])
static int go(lua_State* L)
{
  int scicmd;
  const char *strcmd="char";
  bool sel=false;
  bool fwd=true;
  bool rect=false;
  int count=1;
  int i;
  DOC_REQD
  switch (lua_gettop(L)) {
    case 0: { break; }
    case 4: {
      luaL_argcheck(L,lua_isboolean(L,4),4,_("boolean expected"));
      rect=lua_toboolean(L,4);
    }
    case 3: {
      luaL_argcheck(L,lua_isboolean(L,3),3,_("boolean expected"));
      sel=lua_toboolean(L,3);
    }
    case 2: {
      count=luaL_checkinteger(L,2);
      if (count<0) {
        fwd=false;
        count=0-count;
      }
    }
    case 1:{ strcmd=luaL_checkstring(L,1); }
  }
  scicmd=GetSciNavCmd(strcmd,fwd,sel,rect,SettingsBase::instance()->SmartHome);
  luaL_argcheck(L,SCI_NULL != scicmd, 1, _("invalid mode"));
  for (i=0; i<count; i++) { sci->sendMessage(scicmd, 0, 0); }
  return 0;
}



static int _byte(lua_State* L)
{
  long pos;
  DOC_REQD
  if (lua_gettop(L)==0) {
    pos=sci->GetCaretPos();
  } else {
    pos=luaL_checklong(L,1);
  }
  lua_pushnumber(L,sci->sendMessage(SCI_GETCHARAT,pos,0));
  return 1;
}



static int caret(lua_State* L)
{
  DOC_REQD
  switch (lua_gettop(L)) {
    case 0: {
      lua_pushnumber(L,sci->GetCaretPos());
      return 1;
    }
    case 1: {
      long pos=luaL_checklong(L,1);
      sci->GoToPos(pos);
      return 0;
    }
    default: {
     long row=luaL_checklong(L,1);
     long col=luaL_checklong(L,2);
     sci->GoToCoords(row,col);
     return 0;
    }
  }
}



static void swap(int &a, int &b)
{
  int tmp=a;
  a=b;
  b=tmp;
}



static int _delete(lua_State* L)
{
  DOC_REQD
  int argc=lua_gettop(L);
  int count=1;
  int mode=0;
  const char* modes[]={"char","word","edge",NULL};
  switch (argc) {
    case 0: { break; }
    case 1: {
      if (lua_isnumber(L,1)) {
        count=luaL_checkint(L,1);
      } else {
        mode=luaL_checkoption(L,1,"char",modes);
      }
      break;
    }
    default: {
      mode=luaL_checkoption(L,1,"char",modes);
      count=luaL_checkint(L,2);
    }
  }
  luaL_argcheck(L, count!=0, 1, _("count must be non-zero"));
  int cmd=SCI_CLEAR;
  switch (mode) {
    case 0: {
      cmd=count>0?SCI_CLEAR:SCI_DELETEBACK;
      break;
    }
    case 1: {
      cmd=count>0?SCI_DELWORDRIGHT:SCI_DELWORDLEFT;
      break;
    }
    case 2: {
      cmd=count>0?SCI_DELLINERIGHT:SCI_DELLINELEFT;
      count=1;
      break;
    }
  }
  count=abs(count);
  for (int i=0; i<count; i++) {
    sci->sendMessage(cmd, 0,0);
  }
  return 0;
}



static int indent(lua_State* L)
{

  DOC_REQD
  int count=luaL_optint(L,1,1);
  int i;
  int cmd=SCI_TAB;
  luaL_argcheck(L, count!=0, 1, _("count must be non-zero"));
  if (count<0) {
    cmd=SCI_BACKTAB;
    count=abs(count);
  }
  for (i=0; i<count; i++) {
    sci->sendMessage(cmd, 0,0);
  }
  return 0;
}



static int copy(lua_State* L)
{
  const char *content=NULL;
  int len=0;
  DOC_REQD

  switch (lua_gettop(L)) {
    case 0: {
      len=sci->GetSelLength();
      if (len) { sci->sendMessage(SCI_COPY,0,0); }
      lua_pushnumber(L, len);
      return 1;
    }
    case 1: {
     content=luaL_checkstring(L,1);
     len=strlen(content);
     if (len) { sci->sendString(SCI_COPYTEXT,len,content); }
     lua_pushnumber(L, len);
     return 1;
    }
    default: {
      int start=luaL_checkinteger(L,1);
      int stop=luaL_checkinteger(L,2);
      if (start<0) { luaL_argerror(L,1,_("expected unsigned integer")); }
      if (stop<0) { luaL_argerror(L,2,_("expected unsigned integer")); }
      if (start>stop) { swap(start,stop); }
      if (start!=stop) sci->sendMessage(SCI_COPYRANGE,start,stop);
      lua_pushnumber(L, stop-start);
      return 1;
    }
  }
}



static int cut(lua_State* L)
{
  int start,stop,len;
  DOC_REQD
  start=sci->sendMessage(SCI_GETSELECTIONSTART,0,0);
  stop=sci->sendMessage(SCI_GETSELECTIONEND,0,0);
  len=sci->GetTextLength();
  if (start!=stop) {
    CheckReadOnly();
    sci->sendMessage(SCI_CUT,0,0);
  }
  lua_pushnumber(L, (len - sci->GetTextLength()) );
  return 1;
}



static int paste(lua_State* L)
{
  DOC_REQD
  CheckReadOnly();
  if (sci->sendMessage(SCI_CANPASTE,0,0)) {
    int len=sci->GetTextLength();
    TopWinPub::Paste();
    lua_pushnumber(L, (len - sci->GetTextLength()) );
  } else {
    lua_pushnil(L);
  }
  return 1;
}



// seltext()
static int seltext(lua_State*L)
{
  DOC_REQD
  long beg=sci->sendMessage(SCI_GETSELECTIONSTART,0,0);
  long end=sci->sendMessage(SCI_GETSELECTIONEND,0,0);
  if (end>beg) {
    const char *txt=(const char*)(sci->sendMessage(SCI_GETCHARACTERPOINTER,0,0));
    lua_pushlstring(L,txt+beg, end-beg);
  } else {
    lua_pushstring(L,"");
  }
  return 1;
}



// insert(text [,adjust])
static int insert(lua_State*L)
{
  DOC_REQD
  const char* txt=luaL_checkstring(L,1);
  bool adjust=((lua_gettop(L)>1) && lua_isboolean(L,2))?lua_toboolean(L,2):false;
  CheckReadOnly();
  if (adjust && *txt && SettingsBase::instance()->AutoIndent) { // Fix up indents for macro recorder
    if (*txt=='}') {  // if text starts with a closing brace, align it first
      sci->sendString(SCI_REPLACESEL,0,"}");
      TopWinPub::AdjustIndent(sci,*txt);
      sci->sendString(SCI_REPLACESEL,0,txt+1);
    } else {
      sci->sendString(SCI_REPLACESEL,0,txt);
    }
    const char*eos=strchr(txt,'\0')-1;
    if (('\n'==*eos)||('\r'==*eos)) { // indent if new line
      TopWinPub::AdjustIndent(sci,*eos);
    }
  } else {  // ordinary insertion without auto-indent
    sci->sendString(SCI_REPLACESEL,0,txt);
  }
  return 0;
}



// text([text])
static int text(lua_State*L)
{
  DOC_REQD
  if (lua_gettop(L)==0) {
    const char *txt=(const char*)(sci->sendMessage(SCI_GETCHARACTERPOINTER,0,0));
    lua_pushstring(L,txt);
    return 1;
  } else {
    const char*txt=luaL_checkstring(L,1);
    CheckReadOnly();
    sci->SetText(txt);
    return 0;
  }
}



static int filename(lua_State*L)
{
  DOC_REQD
  lua_pushstring(L, sci->Filename().text());
  return 1;
}



typedef struct _DocCBData {
  const char *name;
  int index;
} DocCBData;



static bool FindIndexFromNameCB(FXint index, DocTab*tab, FXWindow*page, void*p)
{
  DocCBData* data=(DocCBData*)p;
  SciDoc*sci=(SciDoc*)page->getFirst();
  if (strcmp(data->name, sci->Filename().text())==0) {
   data->index=index;
   return false;
  } else {
    return true;
  }
}



static bool doc_idx_to_filename(int n, FXString &fn)
{
  FXWindow*w=TopWinPub::Tabs()->PageAt(n);
  if (w) {
    SciDoc*sci=(SciDoc*)w->getFirst();
    if (sci) {
      fn=sci->Filename().text();
      return true;
    }
  }
  fn="";
  return false;
}



static int documents_closure(lua_State *L)
{
  int idx=lua_tointeger(L, lua_upvalueindex(1));
  int max=TopWinPub::Tabs()->Count();
  idx++;
  if ( idx < max ){
    lua_pushnumber(L, idx);
    lua_pushvalue(L, -1);
    lua_replace(L, lua_upvalueindex(1));
    FXString fn="";
    doc_idx_to_filename(idx,fn);
    lua_pushstring(L,fn.text());
    return 1;
  } else {
    return 0;
  }
}



static int FindIndexFromName(const char*filename)
{
  DocCBData data;
  data.name=filename;
  data.index=-1;
  TopWinPub::Tabs()->ForEachTab(FindIndexFromNameCB,&data);
  return data.index;
}



// iterator|string|integer documents([doc_id])
static int documents(lua_State*L)
{
  if (lua_gettop(L)==0) {
    lua_pushnumber(L,-1);
    lua_pushcclosure(L, &documents_closure, 1);
    return 1;
  } else {
    if (lua_isnumber(L,1)) {
      int n=lua_tointeger(L,1);
      FXString fn;
      if (n==-1) {
        DOC_REQD
        lua_pushstring(L, sci->Filename().text());
        return 1;
      }
      if (doc_idx_to_filename(n-1, fn)) {
        lua_pushstring(L,fn.text());
        return 1;
      }
    } else {
      const char *name=luaL_checkstring(L,1);
      int idx=FindIndexFromName(name);
      if (idx!=-1) {
        lua_pushnumber(L,idx+1);
        return 1;
      }
    }
    lua_pushnil(L);
    return 1;
  }
}



// bool activate(doc_id) -- argument can be a filename or a numeric tab index.
static int activate(lua_State *L)
{
  int idx;
  bool isnum=lua_isnumber(L,1);
  luaL_argcheck(L,isnum||lua_isstring(L,1),1,_("string or number expected"));
  if (isnum) {
    idx=lua_tointeger(L,1)-1;
  } else {
    const char *name=luaL_checkstring(L,1);
    idx=FindIndexFromName(name);
  }
  lua_pushboolean(L, TopWinPub::Tabs()->ActivateTab(idx));
  return 1;
}



static int lines_closure(lua_State *L)
{
  int idx=lua_tointeger(L, lua_upvalueindex(1))+1;
  SciDoc*sci=(SciDoc*)lua_touserdata(L,lua_upvalueindex(2));
  if (sci!=TopWinPub::FocusedDoc()) { return 0; }
  FXString text="";
  if ( sci->GetLineText(idx-1,text)>=0 ) {
    lua_pushnumber(L, idx);
    lua_pushvalue(L, -1);
    lua_replace(L, lua_upvalueindex(1));
    lua_pushstring(L,text.text());
    return 2;
  } else {
    return 0;
  }
}



/* Access the individual lines in the current document */
// iterator|string lines(index)
static int lines(lua_State* L)
{
  DOC_REQD
  if (lua_gettop(L)==0) {
    lua_pushnumber(L,0);
    lua_pushlightuserdata(L,sci); /* Pass the doc pointer to our iterator */
    lua_pushcclosure(L, &lines_closure, 2);
    return 1;
  } else {
    int idx = luaL_checkinteger(L,1);
    FXString text;
    if ( sci->GetLineText(idx,text) >=0 ) {
      lua_pushstring(L,text.text());
      return 1;
    } else {
      return 0;
    }
  }
}



static int ndocs(lua_State*L)
{
  lua_pushnumber(L,TopWinPub::Tabs()->Count());
  return 1;
}



static int nchars(lua_State*L)
{
  DOC_REQD
  lua_pushnumber(L,sci->GetTextLength());
  return 1;
}



static int nlines(lua_State*L)
{
  DOC_REQD
  lua_pushnumber(L, sci->GetLineCount());
  return 1;
}



// bool open(filename [,line[:column] [,readonly]])
static int open(lua_State*L)
{
  bool rdonly=false;
  const char*rowcol=NULL;
  const char*name=NULL;
  int nargs=lua_gettop(L);
  if (nargs>3) { nargs=3; }
  switch (nargs) {
    case 3: { rdonly=lua_toboolean(L,3); }
    case 2: { rowcol=luaL_checkstring(L,2); }
    default: { name=luaL_checkstring(L,1); }
  }
  lua_pushboolean(L, TopWinPub::OpenFile(name[0]?name:NULL,rowcol,rdonly,false));
  return 1;
}



// bool close(["prompt"|"save"|"nosave"])
static int close(lua_State*L)
{
  DOC_REQD
  const char* opt=luaL_optstring(L,1,"prompt");
  bool dirty=sci->Dirty();
  if (strcmp(opt,"nosave")==0) {
    if (dirty) { sci->Dirty(false); }
  } else {
    if (strcmp(opt,"save")==0) {
      if ( TopWinPub::FileDlgs()->SaveFile(sci,sci->Filename()) ) {
        dirty=false;
      } else {
        lua_pushboolean(L,false);
        return 1;
      }
    } else {
      if (strcmp(opt,"prompt")!=0) {
        luaL_error(L,_("expected string value of 'prompt', 'save' or 'nosave'"));
      }
    }
  }
  if (TopWinPub::CloseFile(false,false)) {
    lua_pushboolean(L, true);
  } else {
    if (dirty!=sci->Dirty()) { sci->Dirty(dirty); }
    lua_pushboolean(L, false);
  }
  return 1;
}



// bool save([filename [,overwrite]])
static int save(lua_State*L)
{
  DOC_REQD
  int nargs=lua_gettop(L);
  if (nargs==0) {
    char*fn=strdup(sci->Filename().text());
    lua_pushboolean(L,TopWinPub::FileDlgs()->SaveFile(sci,fn));
    free(fn);
  } else {
    const char *fn=luaL_checkstring(L,1);
    if (FXStat::exists(fn)) {
      bool overwrite=((nargs>1) && lua_toboolean(L,2));
      if ( !overwrite ) {
        lua_pushboolean(L,false);
        return 1;
      }
    }
    lua_pushboolean(L,TopWinPub::FileDlgs()->SaveFile(sci,fn));
  }
  return 1;
}



static bool breathe(lua_State*L)
{
  FXApp*a=FXApp::instance();
  TopWinPub::update();
  a->refresh();
  a->forceRefresh();
  a->flush();
  a->repaint();
  a->runWhileEvents();
  if ( (!TopWinPub::instance()) || TopWinPub::IsMacroCancelled() || TopWinPub::Closing() ) {
    lua_pushstring(L, _("Macro cancelled by user."));
    lua_error(L);
    return false;
  }
  return true;
}



static int sleep(lua_State*L)
{
  int ms=luaL_checkint(L,1);
  int secs=ms/1000;
  ms=ms%1000;
  for (int i=0; i<secs; i++) { // Just sleep for one second at a time, to give user a chance to cancel
    if (!breathe(L)) { return 0; }
    FXThread::sleep(1000000);
  }
  if (!breathe(L)) { return 0; }
  FXThread::sleep(ms*1000);
  return 0;
}


static int yield(lua_State*L)
{
  breathe(L);
  return 0;
}



// Only works if all documents are already closed!
static int quit(lua_State*L)
{
  DOC_REQD
  if ((TopWinPub::Tabs()->Count()==1)&&(sci->Filename().empty())&&(!sci->Dirty()))
  {
    TopWinPub::CloseWait();
    lua_pushstring(L,LuaQuitMessage());
    lua_error(L);
  }
  return 0;
}



// bool readonly([readonly])
static int readonly(lua_State*L)
{
  DOC_REQD
  if (lua_gettop(L)==0) {
    lua_pushboolean(L,sci->GetReadOnly());
  } else {
    bool rdonly=lua_toboolean(L,1);
    lua_pushboolean(L,TopWinPub::SetReadOnly(sci, rdonly));
  }
  return 1;
}



// bool wordwrap()  -- get
// void wordwrap([wrapped])  -- set
static int wordwrap(lua_State*L)
{
  DOC_REQD
  if (lua_gettop(L)==0) {
    lua_pushboolean(L,sci->GetWordWrap());
    return 1;
  } else {
    bool wrapped=lua_toboolean(L,1);
    TopWinPub::SetWordWrap(sci, wrapped);
    return 0;
  }
}



static int batch(lua_State*L)
{
  DOC_REQD
  luaL_argcheck(L, lua_gettop(L)>0, 1, _("expected boolean"));
  if (lua_toboolean(L,1)) {
    sci->SetUserUndoLevel(+1);
  } else {
    sci->SetUserUndoLevel(-1);
  }
  return 0;
}


class SelSaver: public FXWindow
{
  private:
    FXDECLARE(SelSaver)
    SelSaver(){}
    FXString string;
    FXuchar*data;
    FXuint len;
  public:
    long onSelectionRequest(FXObject*o, FXSelector sel, void*p) {
      FXDragType dragtype=((FXEvent*)p)->target;
      if ( string.empty() ) { return 0; }
      if (dragtype==stringType || dragtype==textType || dragtype==utf8Type || dragtype==utf16Type) {
        if (dragtype==utf8Type){
          setDNDData(FROM_SELECTION,dragtype,string);
          return 1;
        }
        if (dragtype==stringType || dragtype==textType) {
          FX88591Codec ascii;
          setDNDData(FROM_SELECTION,dragtype,ascii.utf2mb(string));
          return 1;
        }
        if (dragtype==utf16Type) {
          FXUTF16LECodec unicode;
          setDNDData(FROM_SELECTION,dragtype,unicode.utf2mb(string));
          return 1;
        }
      }
      return 0;
    }
    long onSelectionLost(FXObject*o, FXSelector sel, void*p) {
      string="";
      return 1;
    }
    bool SetSel(const FXchar*s) {
      if (!string.empty()) { releaseSelection(); }
      string=s;
      return ( s && s[0] && acquireSelection(&FXWindow::stringType,1) );
    }
    const char*GetSel() {
      data=NULL;
      len=0;
      FXDragType types[] = {textType,utf8Type,stringType,0};
      for ( FXDragType*type=types; *type; type++ ) {
        if (getDNDData(FROM_SELECTION,*type, data, len)&&data&&*data) {
          return (const char*) data;
        }
      }
      return NULL;
    }
    SelSaver(FXComposite *p):FXWindow(p) {
      create();
      if (!textType) { textType = getApp()->registerDragType(textTypeName); }
      if (!utf8Type) { utf8Type = getApp()->registerDragType(utf8TypeName); }
    }
};



FXDEFMAP(SelSaver) SelSaverMap[] = {
  FXMAPFUNC(SEL_SELECTION_REQUEST,0,SelSaver::onSelectionRequest),
  FXMAPFUNC(SEL_SELECTION_LOST,0,SelSaver::onSelectionLost)
};

FXIMPLEMENT(SelSaver, FXWindow, SelSaverMap, ARRAYNUMBER(SelSaverMap))



static int xsel(lua_State* L)
{
  DOC_REQD
  if (!selsaver) { selsaver=new SelSaver(TopWinPub::instance()); }
  if (lua_gettop(L)==0) {
    lua_pushstring(L,selsaver->GetSel());
    return 1;
  } else {
    const char*s=luaL_checkstring(L,1);
    lua_pushboolean(L, selsaver->SetSel(s));
    return 1;
  }
}



static int uppercase(lua_State* L)
{
  DOC_REQD
  sci->SelectionToUpper();
  return 0;
}



static int lowercase(lua_State* L)
{
  DOC_REQD
  sci->SelectionToLower();
  return 0;
}



static SciCmdDesc* lookup_cmd_id(int cmd)
{
  for (int i=0; scintilla_commands[i].name; i++) {
    if (scintilla_commands[i].msgid==cmd) { return &scintilla_commands[i];}
  }
  return NULL;
}



static SciCmdDesc* lookup_cmd_str(const char*cmd)
{
  if (cmd) {
    for (int i=0; scintilla_commands[i].name; i++) {
      if (strcmp(scintilla_commands[i].name,cmd)==0) { return &scintilla_commands[i];}
    }
  }
  return NULL;
}



static int scintilla(lua_State* L)
{
  static const char*unsupported=_("Unsupported scintilla type");
  uptr_t wparam=0;
  sptr_t lparam=0;
  SciCmdDesc*desc=NULL;
  int argc=lua_gettop(L);
  char*resultbuf=NULL;
  int bufsize=0;
  DOC_REQD
  luaL_checkany (L, 1);
  if (lua_isnumber(L,1)) {
    desc=lookup_cmd_id((int)lua_tonumber(L,1));
  } else {
      const char *arg=luaL_checkstring(L,1);
      char cmdbuf[64];
      int i;
      memset(cmdbuf,'\0', sizeof(cmdbuf));
      strncpy(cmdbuf,arg,sizeof(cmdbuf)-1);
      for (i=0;cmdbuf[i];i++) {cmdbuf[i]=toupper(cmdbuf[i]);}
      desc=lookup_cmd_str((strncmp(cmdbuf,"SCI_",4)==0)?&cmdbuf[4]:cmdbuf);
  }
  if ( !desc ) {
    return luaL_argerror(L,1,_("Unknown message ID"));
  }

  if (((desc->wparam==SLT_INT)&&(desc->lparam==SLT_STRINGRESULT))) {
    // We can allow missing wparam (length) for some string result types
  } else {
    if (desc->wparam!=SLT_VOID) { luaL_checkany(L,2); }
    if (desc->lparam!=SLT_VOID) { luaL_checkany(L,3); }
  }
  switch (desc->wparam) {
    case SLT_VOID:
    break;
    case SLT_INT:
      if (argc>=2) {
        wparam=luaL_checklong(L,2);
      }
      break;
    case SLT_STRING: {
      const char*arg=luaL_checkstring(L,2);
      wparam=(uptr_t)arg;
    break;
    }
    case SLT_CELLS: return luaL_argerror(L,1,unsupported);
    case SLT_BOOL:
      if (!lua_isboolean(L,2)) { return luaL_argerror(L,2,_("expected boolean")); };
      wparam=lua_toboolean(L,2);
    break;
    case SLT_TEXTRANGE: return luaL_argerror(L,1,unsupported);
    case SLT_STRINGRESULT: return luaL_argerror(L,1,unsupported);
    case SLT_FINDTEXT: return luaL_argerror(L,1,unsupported);
    case SLT_FORMATRANGE: return luaL_argerror(L,1,unsupported);
    default: return luaL_argerror(L,1,unsupported);
  }
  switch (desc->lparam) {
    case SLT_VOID:
    break;
    case SLT_INT:
      lparam=luaL_checklong(L,3);
    break;
    case SLT_STRING: {
      const char*arg=luaL_checkstring(L,3);
      lparam=(sptr_t)(arg);
      break;
    }
    case SLT_CELLS: return luaL_argerror(L,1,unsupported);;
    case SLT_BOOL:
      if (!lua_isboolean(L,3)) { return luaL_argerror(L,3,_("expected boolean")); }
      lparam=lua_toboolean(L,3);
      break;
    case SLT_TEXTRANGE: return luaL_argerror(L,3,_("expected boolean"));
    case SLT_STRINGRESULT:
      if ((desc->msgid==SCI_GETTEXT)&&(wparam==0)) {
        wparam=sci->sendMessage(SCI_GETLENGTH,0,0);
      }
      switch (desc->msgid) {
        case SCI_GETTEXT:
          if (wparam==0){
            wparam=sci->sendMessage(SCI_GETLENGTH,0,0);
          } else { wparam++; }
          break;
        case SCI_GETCURLINE:
          if (wparam>0) { wparam++; }
          break;
      }
      bufsize=sci->sendMessage(desc->msgid, wparam, 0);
      if (bufsize) {
        resultbuf=(char*)(calloc(bufsize+1,1));
        lparam=(sptr_t)resultbuf;
      } else {
        lua_pushnil(L);
        return 1;
      }
      break;
    case SLT_FINDTEXT: return luaL_argerror(L,1,unsupported);
    case SLT_FORMATRANGE: return luaL_argerror(L,1,unsupported);
    default: return luaL_argerror(L,1,unsupported);
  }
  switch (desc->result) {
    case SLT_VOID:
      sci->sendMessage(desc->msgid, wparam, lparam);
      lua_pushnil(L);
      return 1;
    case SLT_INT:
      if (desc->lparam==SLT_STRINGRESULT) {
        sci->sendMessage(desc->msgid, wparam, lparam);
        lua_pushstring(L,resultbuf);
        free(resultbuf);
        resultbuf=NULL;
      } else {
        lua_pushnumber(L, sci->sendMessage(desc->msgid, wparam, lparam));
      }
      return 1;
    case SLT_BOOL:
      lua_pushboolean(L, sci->sendMessage(desc->msgid, wparam, lparam));
      return 1;
    default: {
      if (resultbuf) {
        free(resultbuf);
        resultbuf=NULL;
      }
      return luaL_argerror(L,1,unsupported);
    }
  }
}



static int scrollpos(lua_State* L)
{
  DOC_REQD
  FXint x,y;
  if (lua_gettop(L)==0) {
    sci->getPosition(x,y);
    lua_pushinteger(L, abs(x));
    lua_pushinteger(L, abs(y));
    return 2;
  } else {
    x=luaL_checkinteger(L,1);
    y=luaL_checkinteger(L,2);
    sci->setPosition(-x,-y);
    return 0;
  }
}



static int lexer(lua_State* L)
{
  char*buf=NULL;
  DOC_REQD
  sptr_t len=sci->sendString(SCI_GETLEXERLANGUAGE, 0, buf);
  if (len) {
    buf=(char*)calloc(len+1, 1);
    sci->sendString(SCI_GETLEXERLANGUAGE, 0, buf);
    lua_pushstring(L,buf);
    free(buf);
    return 1;
  }
  return 0;
}



static int configdir(lua_State* L)
{
  lua_pushstring(L,TopWinPub::ConfigDir().text());
  return 1;
}


#ifdef WIN32
# include <windows.h>
#endif

#include "interproc.h"


static int ipc(lua_State* L)
{
  if (lua_gettop(L)==0) {
    lua_pushstring(L,TopWinPub::Connector().text());
  } else {
    size_t len=0;
    const char*cmd=luaL_checklstring(L,1,&len);
    const char*conn=luaL_checkstring(L,2);
    const char*topic=luaL_optstring(L,3,NULL);
    FXString cmdstr(cmd,len);
    InterProc ip(FXApp::instance(),conn,topic);
    lua_pushboolean(L,ip.ClientSend(NULL,cmdstr));
  }
  return 1;
}



static int tagfiles(lua_State* L)
{
  lua_newtable(L);
  FXint i=1;
  for (FXWindow*w=TopWinPub::TagFiles(); w; w=w->getNext(), i++) {
    lua_pushinteger(L,i);
    lua_pushstring(L,((FXMenuCaption*)w)->getText().text());
    lua_rawset(L,-3);
  }
  return 1;
}



static const struct luaL_Reg fxte_funcs[] = {
  {"seltext", seltext},
  {"text", text},
  {"documents", documents},
  {"ndocs", ndocs},
  {"nchars", nchars},
  {"nlines", nlines},
  {"lines", lines},
  {"activate", activate},
  {"open", open},
  {"close", close},
  {"readonly", readonly},
  {"wordwrap", wordwrap},
  {"sleep", sleep},
  {"yield", yield},
  {"quit", quit},
  {"save", save},
  {"filename", filename},
  {"batch", batch},
  {"copy", copy},
  {"cut", cut},
  {"paste", paste},
  {"delete", _delete},
  {"indent", indent},
  {"caret", caret},
  {"byte", _byte},
  {"go", go},
  {"select", select},
  {"insert", insert},
  {"rowcol", rowcol},
  {"find", find},
  {"gofind", gofind},
  {"replace", replace},
  {"match", match},
  {"word", word},
  {"xsel", xsel},
  {"uppercase", uppercase},
  {"lowercase", lowercase},
  {"scintilla", scintilla},
  {"scrollpos", scrollpos},
  {"lexer", lexer},
  {"configdir", configdir},
  {"ipc", ipc},
  {"tagfiles", tagfiles},
  {NULL, NULL}
};



const luaL_Reg* LuaFuncs()
{
  return fxte_funcs;
}



void LuaFuncsCleanup()
{
  if (selsaver) {
    delete selsaver;
    selsaver=NULL;
  }
}

