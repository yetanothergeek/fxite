/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2009 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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
#include <Scintilla.h>

#include "macro.h"

#include "intl.h"
#include "recorder.h"


#define _LUAMOD_ LUA_MODULE_NAME

struct MacroMessage {
    int message;
    uptr_t wParam;
    sptr_t lParam;
    unsigned char allocd;
};

MacroRecorder::MacroRecorder():FXObject() {

}



#define NewMessage() \
MacroMessage* mm=new MacroMessage; \
mm->message=message; \
mm->wParam=wParam; \
mm->lParam=lParam; \
mm->allocd=0; \
list.prepend((FXObject*)mm);



#define AddString() { \
NewMessage(); \
mm->lParam=reinterpret_cast<long>(strdup((char*)lParam)); \
mm->allocd=1; \
}



#define AppendString() { \
  char *s=(char*)calloc(strlen((char*)prev->lParam)+strlen((char*)lParam)+1,1); \
  strcpy(s,(char*)prev->lParam); \
  strcat(s,(char*)lParam); \
  free((void*)prev->lParam); \
  prev->lParam=reinterpret_cast<long>(s); \
}


void MacroRecorder::record(int message, uptr_t wParam, sptr_t lParam)
{
  MacroMessage*prev=list[0];
  switch (message) {
    case SCI_CUT:
    case SCI_COPY:
    case SCI_CLEARALL:
    case SCI_SELECTALL:
    case SCI_HOME:
    case SCI_HOMEEXTEND:
    case SCI_LINEEND:
    case SCI_LINEENDEXTEND:
    case SCI_HOMEWRAP:
    case SCI_HOMEWRAPEXTEND:
    case SCI_LINEENDWRAP:
    case SCI_LINEENDWRAPEXTEND:
    case SCI_DOCUMENTSTART:
    case SCI_DOCUMENTSTARTEXTEND:
    case SCI_DOCUMENTEND:
    case SCI_DOCUMENTENDEXTEND:
    case SCI_LINECOPY:
    case SCI_LINECUT:
    case SCI_LINEDELETE:
    case SCI_LOWERCASE:
    case SCI_UPPERCASE:
    case SCI_COPYALLOWLINE:
    case SCI_HOMERECTEXTEND:
    case SCI_LINEENDRECTEXTEND:
    case SCI_SEARCHANCHOR:
    {
      if ((!prev)||(prev->message!=message)) {
        NewMessage();
      }
      return;
    }

    case SCI_GOTOLINE:
    case SCI_GOTOPOS:
    case SCI_SETSELECTIONMODE:
    {
      if ((!prev)||(prev->message!=message)||(prev->wParam!=wParam)||(prev->lParam!=lParam)) {
        NewMessage();
      }
      return;
    }

    case SCI_REPLACESEL:
    case SCI_ADDTEXT:
    case SCI_INSERTTEXT:
    case SCI_APPENDTEXT:
    {
      if ((prev) && (prev->message==message)) {
        AppendString();
      } else {
        AddString();
      }
      return;
    }


    case SCI_SEARCHNEXT:
    case SCI_SEARCHPREV:
    {
      if (!prev) {
        AddString();
      } else {
        if ((prev->message==SCI_SEARCHNEXT)||(prev->message==SCI_SEARCHPREV)) {
          NewMessage();
          if (strcmp((char*)prev->lParam,(char*)lParam)==0) {
            mm->lParam=prev->lParam;
          } else {
            mm->lParam=reinterpret_cast<long>(strdup((char*)lParam));
            mm->allocd=1;
          }
        } else {
          AddString();
        }
      }
      return;
    }

    case SCI_PASTE:
    case SCI_LINEDOWN:
    case SCI_LINEDOWNEXTEND:
    case SCI_PARADOWN:
    case SCI_PARADOWNEXTEND:
    case SCI_LINEUP:
    case SCI_LINEUPEXTEND:
    case SCI_PARAUP:
    case SCI_PARAUPEXTEND:
    case SCI_CHARLEFT:
    case SCI_CHARLEFTEXTEND:
    case SCI_CHARRIGHT:
    case SCI_CHARRIGHTEXTEND:
    case SCI_WORDLEFT:
    case SCI_WORDLEFTEXTEND:
    case SCI_WORDRIGHT:
    case SCI_WORDRIGHTEXTEND:
    case SCI_WORDPARTLEFT:
    case SCI_WORDPARTLEFTEXTEND:
    case SCI_WORDPARTRIGHT:
    case SCI_WORDPARTRIGHTEXTEND:
    case SCI_WORDLEFTEND:
    case SCI_WORDLEFTENDEXTEND:
    case SCI_WORDRIGHTEND:
    case SCI_WORDRIGHTENDEXTEND:
    case SCI_STUTTEREDPAGEUP:
    case SCI_STUTTEREDPAGEUPEXTEND:
    case SCI_STUTTEREDPAGEDOWN:
    case SCI_STUTTEREDPAGEDOWNEXTEND:
    case SCI_PAGEUP:
    case SCI_PAGEUPEXTEND:
    case SCI_PAGEDOWN:
    case SCI_PAGEDOWNEXTEND:
    case SCI_DELETEBACK:
    case SCI_TAB:
    case SCI_BACKTAB:
    case SCI_FORMFEED:
    case SCI_DELWORDLEFT:
    case SCI_DELWORDRIGHT:
    case SCI_EDITTOGGLEOVERTYPE:
    case SCI_CANCEL:
    case SCI_VCHOME:
    case SCI_VCHOMEEXTEND:
    case SCI_VCHOMEWRAP:
    case SCI_VCHOMEWRAPEXTEND:
    case SCI_DELWORDRIGHTEND:
    case SCI_DELLINELEFT:
    case SCI_DELLINERIGHT:
    case SCI_LINETRANSPOSE:
    case SCI_LINEDUPLICATE:
    case SCI_HOMEDISPLAY:
    case SCI_HOMEDISPLAYEXTEND:
    case SCI_LINEENDDISPLAY:
    case SCI_LINEENDDISPLAYEXTEND:
    case SCI_LINEDOWNRECTEXTEND:
    case SCI_LINEUPRECTEXTEND:
    case SCI_CHARLEFTRECTEXTEND:
    case SCI_CHARRIGHTRECTEXTEND:
    case SCI_VCHOMERECTEXTEND:
    case SCI_PAGEUPRECTEXTEND:
    case SCI_PAGEDOWNRECTEXTEND:
    case SCI_SELECTIONDUPLICATE:
    case SCI_LINESCROLLDOWN:
    case SCI_LINESCROLLUP:
    case SCI_DELETEBACKNOTLINE:
    case SCI_CLEAR:
    {
      if ( (!prev) || (prev->message!=message) ) {
        NewMessage();
      } else {
        prev->lParam++;
      }
      return;
    }
  }
}


void MacroRecorder::playback(PlaybackFunc callback, void*user_data)
{

  for (FXint i=list.no()-1; i>=0; i--) {
    MacroMessage* mm=list[i];
    callback(mm->message, mm->wParam, mm->lParam, user_data);
  }
}



void MacroRecorder::clear()
{

  for (FXint i=list.no()-1; i>=0; i--) {
    MacroMessage* mm=list[i];
    if (mm->allocd) { free((void*)mm->lParam); }
    delete mm;
  }
  list.clear();
}



MacroRecorder::~MacroRecorder() {
  clear();
}

void requote(FXString&s)
{
  s.substitute("\\", "\\\\", true);
  s.substitute("\n", "\\n", true);
  s.substitute("\r", "\\r", true);
  s.substitute("\"", "\\\"", true);
}

#define noimpl(msg) { text="-- message " msg " not implemented"; break; }

void MacroRecorder::translate(TranslateFunc callback, void* user_data)
{

  for (FXint i=list.no()-1; i>=0; i--) {
    FXString text;
    MacroMessage* mm=list[i];
    switch (mm->message) {
      case SCI_CUT: {
        text=_LUAMOD_".cut()";
        break;
      }
      case SCI_COPY: {
        text=_LUAMOD_".copy()";
        break;
      }
      case SCI_CLEARALL: {
        text=_LUAMOD_".text(\"\")";
        break;
      }
      case SCI_SELECTALL: {
        text=_LUAMOD_".select(0,"_LUAMOD_".nchars())";
        break;
      }


      case SCI_HOME:
      case SCI_VCHOME:{
        text=_LUAMOD_".go(\"edge\", -1)";
        break;
      }

      case SCI_HOMEEXTEND:
      case SCI_VCHOMEEXTEND:{
        text=_LUAMOD_".go(\"edge\", -1, true)";
        break;
      }


      case SCI_LINEEND: {
        text=_LUAMOD_".go(\"edge\", 1)";
        break;
      }
      case SCI_LINEENDEXTEND: {
        text=_LUAMOD_".go(\"edge\", 1, true)";
        break;
      }
      case SCI_DOCUMENTSTART: {
        text=_LUAMOD_".go(\"body\", -1)";
        break;
      }
      case SCI_DOCUMENTSTARTEXTEND: {
        text=_LUAMOD_".go(\"body\", -1, true)";
        break;
      }
      case SCI_DOCUMENTEND: {
        text=_LUAMOD_".go(\"body\", 1)";
        break;
      }
      case SCI_DOCUMENTENDEXTEND: {
        text=_LUAMOD_".go(\"body\", 1, true)";
        break;
      }
      case SCI_HOMERECTEXTEND:
      case SCI_VCHOMERECTEXTEND: {
        text=_LUAMOD_".go(\"edge\", -1, true, true)";
        break;
      }
      case SCI_LINEENDRECTEXTEND: {
        text=_LUAMOD_".go(\"edge\", 1, true, true)";
        break;
      }
      case SCI_GOTOLINE: {
        text.format(_LUAMOD_".caret(%ld,%ld)", mm->wParam+1, mm->lParam);
        break;
      }
      case SCI_GOTOPOS: {
        text.format(_LUAMOD_".caret(%ld)", mm->wParam);
        break;
      }
      case SCI_REPLACESEL: {
        FXString s=(char*)mm->lParam;
        FXint n=s.contains('\n');
        if ( (n>0) && (s.length()>1) ) { // If multiple lines,  split into lines for playback...
          int p;
          for (p=0; p<n; p++) {
            FXString part=s.section('\n', p);
            requote(part);
            text.format(_LUAMOD_".insert(\"%s\\n\")", part.text());
            callback(text.text(), user_data);
          }
          if (!s.section('\n', n).empty()) { // there may still be text beyond the last LF...
            FXString part=s.section('\n', n);
            requote(part);
            text.format(_LUAMOD_".insert(\"%s\")", part.text());
            callback(text.text(), user_data);
          }
          continue;
        }
        requote(s);
        text.format(_LUAMOD_".insert(\"%s\")", s.text());
        break;
      }

      case SCI_SEARCHNEXT:
      case SCI_SEARCHPREV: {
        FXString what=(char*)mm->lParam;
        requote(what);
        FXString flags="";
        if (mm->wParam & SCFIND_MATCHCASE) { flags.append("\"matchcase\""); }
        if (mm->wParam & SCFIND_WHOLEWORD) { flags.append(",\"wholeword\""); }
        if (mm->wParam & SCFIND_REGEXP)    { flags.append(",\"regexp\""); }
        if (flags[0]==',') { flags.erase(0,1); }
        text.format( _LUAMOD_".gofind(\"%s\", {%s}%s)",
                       what.text(), flags.text(), mm->message==SCI_SEARCHNEXT?"":", false"  );
        break;
      }
      case SCI_PASTE: {
        text=_LUAMOD_".paste()";
        break;
      }
      case SCI_LINEDOWN: {
        text.format(_LUAMOD_".go(\"line\", %ld)", mm->lParam+1);
        break;
      }
      case SCI_LINEDOWNEXTEND: {
        text.format(_LUAMOD_".go(\"line\", %ld, true)", mm->lParam+1);
        break;
      }
      case SCI_PARADOWN: {
        text.format(_LUAMOD_".go(\"para\", %ld)", mm->lParam+1);
        break;
      }
      case SCI_PARADOWNEXTEND: {
        text.format(_LUAMOD_".go(\"para\", %ld, true)", mm->lParam+1);
        break;
      }
      case SCI_LINEUP: {
        text.format(_LUAMOD_".go(\"line\", -%ld)", mm->lParam+1);
        break;
      }
      case SCI_LINEUPEXTEND: {
        text.format(_LUAMOD_".go(\"line\", -%ld, true)", mm->lParam+1);
        break;
      }
      case SCI_PARAUP: {
        text.format(_LUAMOD_".go(\"para\", -%ld)", mm->lParam+1);
        break;
      }
      case SCI_PARAUPEXTEND: {
        text.format(_LUAMOD_".go(\"para\", -%ld, true)", mm->lParam+1);
        break;
      }
      case SCI_CHARLEFT: {
        text.format(_LUAMOD_".go(\"char\", -%ld)", mm->lParam+1);
        break;
      }
      case SCI_CHARLEFTEXTEND: {
        text.format(_LUAMOD_".go(\"char\", -%ld, true)", mm->lParam+1);
        break;
      }
      case SCI_CHARRIGHT: {
        text.format(_LUAMOD_".go(\"char\", %ld)", mm->lParam+1);
        break;
      }
      case SCI_CHARRIGHTEXTEND: {
        text.format(_LUAMOD_".go(\"char\", %ld, true)", mm->lParam+1);
        break;
      }
      case SCI_WORDLEFT: {
        text.format(_LUAMOD_".go(\"word\", -%ld)", mm->lParam+1);
        break;
      }
      case SCI_WORDLEFTEXTEND: {
        text.format(_LUAMOD_".go(\"word\", -%ld, true)", mm->lParam+1);
        break;
      }
      case SCI_WORDRIGHT: {
        text.format(_LUAMOD_".go(\"word\", %ld)", mm->lParam+1);
        break;
      }
      case SCI_WORDRIGHTEXTEND: {
        text.format(_LUAMOD_".go(\"word\", %ld, true)", mm->lParam+1);
        break;
      }
      case SCI_WORDPARTLEFT: {
        text.format(_LUAMOD_".go(\"part\", -%ld)", mm->lParam+1);
        break;
      }
      case SCI_WORDPARTLEFTEXTEND: {
        text.format(_LUAMOD_".go(\"part\", -%ld, true)", mm->lParam+1);
        break;
      }
      case SCI_WORDPARTRIGHT: {
        text.format(_LUAMOD_".go(\"part\", %ld)", mm->lParam+1);
        break;
      }
      case SCI_WORDPARTRIGHTEXTEND: {
        text.format(_LUAMOD_".go(\"part\", %ld, true)", mm->lParam+1);
        break;
      }
      case SCI_PAGEUP: {
        text.format(_LUAMOD_".go(\"page\", -%ld)", mm->lParam+1);
        break;
      }
      case SCI_PAGEUPEXTEND: {
        text.format(_LUAMOD_".go(\"page\", -%ld, true)", mm->lParam+1);
        break;
      }
      case SCI_PAGEDOWN: {
        text.format(_LUAMOD_".go(\"page\", %ld)", mm->lParam+1);
        break;
      }
      case SCI_PAGEDOWNEXTEND: {
        text.format(_LUAMOD_".go(\"page\", %ld, true)", mm->lParam+1);
        break;
      }
      case SCI_LINEDOWNRECTEXTEND: {
        text.format(_LUAMOD_".go(\"line\", %ld, true, true)", mm->lParam+1);
        break;
      }
      case SCI_LINEUPRECTEXTEND: {
        text.format(_LUAMOD_".go(\"line\", -%ld, true, true)", mm->lParam+1);
        break;
      }
      case SCI_CHARLEFTRECTEXTEND: {
        text.format(_LUAMOD_".go(\"char\", -%ld, true, true)", mm->lParam+1);
        break;
      }
      case SCI_CHARRIGHTRECTEXTEND: {
        text.format(_LUAMOD_".go(\"char\", %ld, true, true)", mm->lParam+1);
        break;
      }
      case SCI_PAGEUPRECTEXTEND: {
        text.format(_LUAMOD_".go(\"page\", -%ld, true, true)", mm->lParam+1);
        break;
      }
      case SCI_PAGEDOWNRECTEXTEND: {
        text.format(_LUAMOD_".go(\"page\", %ld, true, true)", mm->lParam+1);
        break;
      }
      case SCI_TAB: {
        text.format(_LUAMOD_".indent(%ld)", mm->lParam+1);
        break;
      }
      case SCI_BACKTAB: {
        text.format(_LUAMOD_".indent(-%ld)", mm->lParam+1);
        break;
      }
      case SCI_DELETEBACK: {
        text.format(_LUAMOD_".delete(-%ld)", mm->lParam+1);
        break;
      }
      case SCI_CLEAR: {
        text.format(_LUAMOD_".delete(%ld)", mm->lParam+1);
        break;
      }
      case SCI_UPPERCASE: {
        text.format(_LUAMOD_".uppercase()");
        break;
      }
      case SCI_LOWERCASE: {
        text.format(_LUAMOD_".lowercase()");
        break;
      }
      case SCI_HOMEWRAP:                noimpl("SCI_HOMEWRAP");
      case SCI_HOMEWRAPEXTEND:          noimpl("SCI_HOMEWRAP");
      case SCI_LINEENDWRAP:             noimpl("SCI_LINEENDWRAP");
      case SCI_LINEENDWRAPEXTEND:       noimpl("SCI_LINEENDWRAPEXTEND");
      case SCI_LINECOPY:                noimpl("SCI_LINECOPY");
      case SCI_LINECUT:                 noimpl("SCI_LINECUT");
      case SCI_LINEDELETE:              noimpl("SCI_LINEDELETE");
      case SCI_COPYALLOWLINE:           noimpl("SCI_COPYALLOWLINE");
      case SCI_SEARCHANCHOR:            noimpl("SCI_SEARCHANCHOR");
      case SCI_ADDTEXT:                 noimpl("SCI_ADDTEXT");
      case SCI_INSERTTEXT:              noimpl("SCI_INSERTTEXT");
      case SCI_APPENDTEXT:              noimpl("SCI_APPENDTEXT");
      case SCI_WORDLEFTEND:             noimpl("SCI_WORDLEFTEND");
      case SCI_WORDLEFTENDEXTEND:       noimpl("SCI_WORDLEFTENDEXTEND");
      case SCI_WORDRIGHTEND:            noimpl("SCI_WORDRIGHTEND");
      case SCI_WORDRIGHTENDEXTEND:      noimpl("SCI_WORDRIGHTENDEXTEND");
      case SCI_STUTTEREDPAGEUP:         noimpl("SCI_STUTTEREDPAGEUP");
      case SCI_STUTTEREDPAGEUPEXTEND:   noimpl("SCI_STUTTEREDPAGEUPEXTEND");
      case SCI_STUTTEREDPAGEDOWN:       noimpl("SCI_STUTTEREDPAGEDOWN");
      case SCI_STUTTEREDPAGEDOWNEXTEND: noimpl("SCI_STUTTEREDPAGEDOWNEXTEND");
      case SCI_FORMFEED:                noimpl("SCI_FORMFEED");
      case SCI_DELWORDLEFT:             noimpl("SCI_DELWORDLEFT");
      case SCI_DELWORDRIGHT:            noimpl("SCI_DELWORDRIGHT");
      case SCI_EDITTOGGLEOVERTYPE:      noimpl("SCI_EDITTOGGLEOVERTYPE");
      case SCI_CANCEL:                  noimpl("SCI_CANCEL");

      case SCI_VCHOMEWRAP:              noimpl("SCI_VCHOMEWRAP");
      case SCI_VCHOMEWRAPEXTEND:        noimpl("SCI_VCHOMEWRAPEXTEND");
      case SCI_DELWORDRIGHTEND:         noimpl("SCI_DELWORDRIGHTEND");
      case SCI_DELLINELEFT:             noimpl("SCI_DELLINELEFT");
      case SCI_DELLINERIGHT:            noimpl("SCI_DELLINERIGHT");
      case SCI_LINETRANSPOSE:           noimpl("SCI_LINETRANSPOSE");
      case SCI_LINEDUPLICATE:           noimpl("SCI_LINEDUPLICATE");
      case SCI_HOMEDISPLAY:             noimpl("SCI_HOMEDISPLAY");
      case SCI_HOMEDISPLAYEXTEND:       noimpl("SCI_HOMEDISPLAYEXTEND");
      case SCI_LINEENDDISPLAY:          noimpl("SCI_LINEENDDISPLAY");
      case SCI_LINEENDDISPLAYEXTEND:    noimpl("SCI_LINEENDDISPLAYEXTEND");
      case SCI_SELECTIONDUPLICATE:      noimpl("SCI_SELECTIONDUPLICATE");

      case SCI_LINESCROLLDOWN:          noimpl("SCI_LINESCROLLDOWN");
      case SCI_LINESCROLLUP:            noimpl("SCI_LINESCROLLUP");
      case SCI_DELETEBACKNOTLINE:       noimpl("SCI_DELETEBACKNOTLINE");
      case SCI_SETSELECTIONMODE:        noimpl("SCI_SETSELECTIONMODE");
    }
    callback(text.text(), user_data);
  }
}
