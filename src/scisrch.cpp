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
#include <Scintilla.h>
#include <SciLexer.h>
#include <FXScintilla.h>
#include "scidoc.h"

#include "compat.h"

#include "intl.h"
#include "scisrch.h"

#define SciMsg(im,wp,lp) sci->sendMessage(im,wp,lp)
#define SciStr(im,wp,lp) sci->sendString(im,wp,lp)



void SciSearch::SelectTarget(bool forward)
{
  long ts=SciMsg(SCI_GETTARGETSTART,0,0);
  long te=SciMsg(SCI_GETTARGETEND,0,0);
  if (sci->GetWordWrap()) {
    SciMsg(SCI_GOTOLINE,SciMsg(SCI_LINEFROMPOSITION,ts,0),0);
    if (forward) {
      SciMsg(SCI_SETSEL, ts, te);
    } else {
      SciMsg(SCI_SETSEL, te, ts);
    }
    SciMsg(SCI_SETXOFFSET,0,0);
    SciMsg(SCI_SCROLLCARET,0,0);
    sci->getApp()->runWhileEvents();
  }
  if (forward) {
    SciMsg(SCI_SETSEL, ts, te);
  } else {
    SciMsg(SCI_SETSEL, te, ts);
  }
  SciMsg(SCI_SETXOFFSET,0,0);
  SciMsg(SCI_SCROLLCARET,0,0);
}


// This helps make sure we don't "re-find" our selection when the search direction changes
void SciSearch::EnsureAnchorDirection(bool forward)
{
  long start=SciMsg(SCI_GETSELECTIONSTART,0,0);
  long end=SciMsg(SCI_GETSELECTIONEND,0,0);
  long anchor=SciMsg(SCI_GETANCHOR,0,0);
  if (forward) {
    if (anchor==end) { SciMsg(SCI_SETSEL,start,end); }
  } else {
    if (anchor==start) { SciMsg(SCI_SETSEL,end,start); }
  }
}



/*
  It would be nice if we could take advantage of FXRex::Verbatim mode
  for literal string matching, but unfortunately I can't see any clean
  way to implement "whole word" searches with FXRex::Verabtim.
  So instead we *always* use regex matching, and for literal searches
  we escape all special characters, and then wrap the whole thing
  with \< \> if we want to do a whole word search.
*/
static void rx_escape(FXString &pat,bool whole)
{
  static const char escapes[]="\\(){}[]|^$*+?.";
  static const char wordchars[]="ACBDEFGHIJKLMNOPQRSTUVWXYZ_acbdefghijklmnopqrstuvwxyz0123456789";
  const char*c;
  char sub[3]="\\ ";
  sub[2]='\0';
  char org[2];
  org[1]='\0';
  for (c=escapes; *c; c++) {
    org[0]=*c;
    sub[1]=*c;
    pat=pat.substitute(org,sub,true);
  }
  if (whole) {
    if (strchr(wordchars, pat[0])) { pat.prepend("\\<"); }
    if (strchr(wordchars, pat[pat.length()-1])) { pat.append("\\>"); }
  }
}



// Check for valid regex syntax
bool SciSearch::CheckRegex(const FXRex &rx, bool quiet)
{
  if (rx.empty()) {
    if (!quiet) {
      FXMessageBox::error(sci->getShell(), MBOX_OK, _("Syntax error"), "%s:\n%s",
        _("Error parsing regular expression"), FXRex::getError(rxerr));
    }
    return false;
  } else {
    return true;
  }
}



/*
  Searches for 'what' from 'beg' to 'end', but does not select it.
  If the text is found, it sets 'beg' and 'end' to the begin and
  endpoints of the found text and returns (1), or if NOT found,
  'beg' and 'end' are untouched and it returns zero.
  If the regex is invalid, it returns the negative value of rxerr.
*/
int SciSearch::FindTextNoSel(const FXString &what, FXuint sciflags, long &beg, long &end)
{
  FXuint rexflags=0;
  FXString search_pattern=what;
  if (sciflags & SCFIND_REGEXP) {
    rexflags |= REX_CAPTURE;
  } else {
    rx_escape(search_pattern,sciflags & SCFIND_WHOLEWORD);
  }
  if (!(sciflags & SCFIND_MATCHCASE)) rexflags |= REX_ICASE;
  FXRex rx(search_pattern, rexflags, &rxerr);
  if (!CheckRegex(rx, true)) { return -rxerr; }
  content=(const char*)SciMsg(SCI_GETCHARACTERPOINTER,0,0);
  bool isfwd=end>beg;
  long len=SciMsg(SCI_GETLENGTH,0,0);
  long fm = isfwd?beg:end;
  long to = isfwd?end:beg;
  if (fm<0) { fm=0; }
  if (to>len) { to=len; }
  FXuint dirn=isfwd?REX_FORWARD:REX_BACKWARD;
  if (rx.match(content,len,begs,ends,dirn,MAX_CAPTURES,fm,to)) {
    beg=isfwd?begs[0]:ends[0];
    end=isfwd?ends[0]:begs[0];
    return 1;
  } else {
    return 0;
  }
}



bool SciSearch::FindText(const FXString &what, FXuint sciflags, bool isfwd, bool wrap)
{
  FXuint rexflags=0;
  FXString search_pattern=what;
  if (sciflags & SCFIND_REGEXP) {
    rexflags |= REX_CAPTURE;
  } else {
    rx_escape(search_pattern,sciflags & SCFIND_WHOLEWORD);
  }
  if (!(sciflags & SCFIND_MATCHCASE)) rexflags |= REX_ICASE;
  FXRex rx(search_pattern, rexflags, &rxerr);
  if (!CheckRegex(rx)) {
    sci->setFocus();
    return true;
  }
  content=(const char*)SciMsg(SCI_GETCHARACTERPOINTER,0,0);
  EnsureAnchorDirection(isfwd);
  long pos=SciMsg(SCI_GETCURRENTPOS,0,0);
  long len=SciMsg(SCI_GETLENGTH,0,0);
  FXuint dirn = isfwd? REX_FORWARD : REX_BACKWARD;
  long fm = isfwd? pos : 0;
  long to = isfwd? len : (pos-1);
  if (
    rx.match(content,len,begs,ends,dirn,MAX_CAPTURES,fm,to) ||
    (wrap && rx.match(content,len,begs,ends,dirn,MAX_CAPTURES,0,len))
  ) {
    SciMsg(SCI_SETTARGETSTART,begs[0],0);
    SciMsg(SCI_SETTARGETEND,ends[0],0);
    SelectTarget(isfwd);
    sci->setFocus();
    return true;
  } else {
    sci->setFocus();
    return false;
  }
}



/*
  When the user enters escape sequences in a regex replacement, we interpret them literally.
  But we need to convert them beforehand so they won't get mixed up with sequences in the
  document's text, because we want to leave those unconverted. But for literal searches,
  we need to make sure that backslashes and ampersands do *not* get interpreted by FXRex,
  so instead we escape them now.
*/
static FXString PrepareReplacement(const FXString &repl_in, FXuint opts)
{
  FXString repl_out;
  if (opts & SCFIND_REGEXP) {
    repl_out.length(repl_in.length());
    repl_out.trunc(0);
    for (int i=0; i<repl_in.length(); i++) {
      if (repl_in[i] == '\\') {
        i++;
        switch (repl_in[i]) { // Convert escape sequences to their literal chars.
          case '\\': { repl_out.append("\\\\"); break; }
          case 'a':  { repl_out.append('\a'); break; }
          case 'b':  { repl_out.append('\b'); break; }
          case 'f':  { repl_out.append('\f'); break; }
          case 'n':  { repl_out.append('\n'); break; }
          case 'r':  { repl_out.append('\r'); break; }
          case 't':  { repl_out.append('\t'); break; }
          case 'v':  { repl_out.append('\v'); break; }
          default: {
            repl_out.append("\\");
            repl_out.append(repl_in[i]);
          }
        }
      } else { // Pass anything else through as-is
        repl_out.append(repl_in[i]);
      }
    }
  } else { // Escape these special chars if we're doing a literal search/replace
    repl_out=repl_in;
    repl_out.substitute("\\", "\\\\", true);
    repl_out.substitute("&", "\\&", true);
  }
  return repl_out;
}



void SciSearch::ReplaceSelection(const FXString &replacewith, FXuint opts)
{
  long start=SciMsg(SCI_GETSELECTIONSTART,0,0);
  long end=SciMsg(SCI_GETSELECTIONEND,0,0);
  if ( start == end ) { return; }
  SciMsg(SCI_SETSEARCHFLAGS,0,0);
  SciMsg(SCI_TARGETFROMSELECTION,0,0);
  FXString repl_template=PrepareReplacement(replacewith,opts);
  FXString newstr=FXRex::substitute(content,begs,ends,repl_template,MAX_CAPTURES);
  SciStr(SCI_REPLACETARGET,newstr.length(),newstr.text());
  SelectTarget(start<end);
}



long SciSearch::ReplaceAllInDoc(const FXString &searchfor, const FXString &replacewith, FXuint opts)
{
  if (searchfor.empty()) { return 0; }
  bool bol_only=(opts & SCFIND_REGEXP)&&(compare(searchfor,"^")==0); // Match empty string at beginning of line.
  bool eol_only=(opts & SCFIND_REGEXP)&&(compare(searchfor,"$")==0); // Match empty string at end of line.
  FXuint srchflags=REX_FORWARD|((eol_only||bol_only) ? 0 : REX_NOT_EMPTY); // Allow empty matches only at start or end.
  FXuint rexflags=0;
  FXString search_pattern=searchfor;
  if (opts & SCFIND_REGEXP) {
    rexflags |= REX_CAPTURE;
  } else {
    rx_escape(search_pattern,opts & SCFIND_WHOLEWORD);
  }
  if (!(opts & SCFIND_MATCHCASE)) rexflags |= REX_ICASE;
  FXRex rx(search_pattern, rexflags, &rxerr);
  if (!CheckRegex(rx)) { return 0; }
  FXString repl_template=PrepareReplacement(replacewith,opts);
  long start=0;
  long end=SciMsg(SCI_GETLENGTH,0,0);
  if (end==0) { return 0; }
  content=(const char*)SciMsg(SCI_GETCHARACTERPOINTER,0,0);
  long patlen=search_pattern.length();
  long count=0;
  if (patlen==0) { return 0; }
  SciMsg(SCI_SETSEARCHFLAGS,0,0);
  SciMsg(SCI_BEGINUNDOACTION,0,0);
  while (1) {
    if (end==start) { break; }
    if (rx.match(content,end,begs,ends,srchflags,MAX_CAPTURES,start,end)) {
      SciMsg(SCI_SETTARGETSTART,begs[0],0);
      SciMsg(SCI_SETTARGETEND,ends[0],0);
      FXString newstr=FXRex::substitute(content,begs,ends,repl_template,MAX_CAPTURES);
      SciStr(SCI_REPLACETARGET,newstr.length(),newstr.text());
      content=(const char*)SciMsg(SCI_GETCHARACTERPOINTER,0,0);
      count++;
      start=SciMsg(SCI_GETTARGETEND,0,0);
      end=SciMsg(SCI_GETLENGTH,0,0);
      if (eol_only) {start++;}
    } else { break; }
  }
  if (count>0) { SciMsg(SCI_GOTOPOS,start,0); }
  SciMsg(SCI_ENDUNDOACTION,0,0);
  return count;
}



long SciSearch::ReplaceAllInSel(const FXString &searchfor, const FXString &replacewith, FXuint opts)
{
  if (searchfor.empty()) { return 0; }
  bool bol_only=(opts & SCFIND_REGEXP)&&(compare(searchfor,"^")==0); // Match empty string at beginning of line.
  bool eol_only=(opts & SCFIND_REGEXP)&&(compare(searchfor,"$")==0); // Match empty string at end of line.
  FXuint srchflags=REX_FORWARD|((eol_only||bol_only) ? 0 : REX_NOT_EMPTY); // Allow empty matches only at start or end.
  FXuint rexflags=0;
  FXString search_pattern=searchfor;
  if (opts & SCFIND_REGEXP) {
    rexflags |= REX_CAPTURE;
  } else {
    rx_escape(search_pattern,opts & SCFIND_WHOLEWORD);
  }
  if (!(opts & SCFIND_MATCHCASE)) rexflags |= REX_ICASE;
  FXRex rx(search_pattern, rexflags, &rxerr);
  if (!CheckRegex(rx)) { return 0; }
  FXString repl_template=PrepareReplacement(replacewith,opts);
  long patlen=search_pattern.length();
  long count=0;
  if (patlen==0) { return 0; }
  SciMsg(SCI_SETSEARCHFLAGS,0,0);

  for (long isel=SciMsg(SCI_GETSELECTIONS,0,0)-1; isel>=0; isel--) {
    SciMsg(SCI_SETMAINSELECTION,isel,0);
    long start=SciMsg(SCI_GETSELECTIONSTART,0,0);
    long end=SciMsg(SCI_GETSELECTIONEND,0,0);
    if (end==start) { continue; }
    content=(const char*)SciMsg(SCI_GETCHARACTERPOINTER,0,0);

    bool swapped=start>end;
    SciMsg(SCI_BEGINUNDOACTION,0,0);
    if (swapped) {
      long tmp=start;
      start=end;
      end=tmp;
      SciMsg(SCI_SETSEL,start,end);
    }
    SciMsg(SCI_SETSEARCHFLAGS,0,0);
    if (bol_only) { // Special case, start of first line
      if (rx.match(content,end,begs,ends,REX_FORWARD|REX_NOT_EMPTY,MAX_CAPTURES,start,end)) {
        SciMsg(SCI_SETTARGETSTART,begs[0],0);
        SciMsg(SCI_SETTARGETEND,ends[0],0);
        FXString newstr=FXRex::substitute(content,begs,ends,repl_template,MAX_CAPTURES);
        SciStr(SCI_REPLACETARGET,newstr.length(),newstr.text());
        content=(const char*)SciMsg(SCI_GETCHARACTERPOINTER,0,0);
        count++;
        start=SciMsg(SCI_GETTARGETEND,0,0);
        end=SciMsg(SCI_GETSELECTIONEND,0,0);
      }
    }
    while (1) {
      if (end<=start) { break; }
      if (rx.match(content,end,begs,ends,srchflags,MAX_CAPTURES,start,end)) {
        SciMsg(SCI_SETTARGETSTART,begs[0],0);
        SciMsg(SCI_SETTARGETEND,ends[0],0);
        FXString newstr=FXRex::substitute(content,begs,ends,repl_template,MAX_CAPTURES);
        SciStr(SCI_REPLACETARGET,newstr.length(),newstr.text());
        content=(const char*)SciMsg(SCI_GETCHARACTERPOINTER,0,0);
        count++;
        start=SciMsg(SCI_GETTARGETEND,0,0);
        end=SciMsg(SCI_GETSELECTIONEND,0,0);
        if (eol_only) { start++; }
      } else { break; }
    }
    end=SciMsg(SCI_GETSELECTIONEND,0,0);
    if (swapped) {
      long tmp=start;
      start=end;
      end=tmp;
      SciMsg(SCI_SETSEL,start,end);
    }
  }
  SciMsg(SCI_ENDUNDOACTION,0,0);
  return count;
}

