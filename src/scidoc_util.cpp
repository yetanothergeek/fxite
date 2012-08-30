/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2009-2012 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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
#include <SciLexer.h>

#include "prefs.h"
#include "scidoc.h"
#include "appwin_pub.h"

#include "intl.h"
#include "scidoc_util.h"

static FXSelector id_scintilla=0;
static FXSelector id_macro_record=0;

void SciDocUtils::SetScintillaSelector(FXSelector sel) { id_scintilla=sel; }

void SciDocUtils::SetMacroRecordSelector(FXSelector sel) { id_macro_record=sel; }


void SciDocUtils::CharAdded(SciDoc*sci, long line, long pos, int ch, Settings*prefs, SciDoc*recording)
{
  if ( (line<=0) || (prefs->AutoIndent==AUTO_INDENT_NONE)) { return; }
  bool smart=prefs->AutoIndent==AUTO_INDENT_SMART;
  int indent_width=prefs->IndentWidth;
  if (recording) { recording->sendMessage(SCI_STOPRECORD,0,0); }
  switch (ch) {
    case '\r': {
      if (sci->sendMessage(SCI_GETEOLMODE,0,0)!=SC_EOL_CR) { break; } // or fall through for Mac.
    }
    case '\n': {
      int prev_line=line-1;
      bool tmp_tab=false;
      if ( (sci->GetLineLength(prev_line)>0) && !sci->UseTabs() ) {
        long prev_pos=sci->sendMessage(SCI_POSITIONFROMLINE,prev_line,0);
        if (sci->sendMessage(SCI_GETCHARAT,prev_pos,0)=='\t') {
          tmp_tab=true; // If previous line has a tab, override the editor preference.
        }
      }
      long prev_indent=sci->sendMessage(SCI_GETLINEINDENTATION, prev_line, 0);
      long curr_indent=sci->sendMessage(SCI_GETLINEINDENTATION, line, 0);
      int tab_width=sci->TabWidth();
      if (smart) {
        long prev_pos=pos-2;
        long eolmode=sci->sendMessage(SCI_GETEOLMODE,0,0);
        if (eolmode==SC_EOL_CRLF) { prev_pos--; }
        int prev_char=sci->sendMessage(SCI_GETCHARAT,prev_pos,0);
        if (prev_char=='{') {
          if (sci->sendMessage(SCI_GETCHARAT,pos,0)=='}') {
            sci->sendString(SCI_INSERTTEXT,pos,
              (eolmode==SC_EOL_LF)?"\n":(eolmode==SC_EOL_CRLF)?"\r\n":"\r");
            sci->SetLineIndentation(line+1,prev_indent);
            sci->sendMessage(SCI_GOTOPOS,pos,0);
          }
          prev_indent += sci->UseTabs()?tab_width:indent_width;
        }
      }
      if ( curr_indent < prev_indent ) {
        if (tmp_tab) {
          sci->UseTabs(true);
          sci->SetLineIndentation(line,prev_indent);
          sci->UseTabs(false);
        } else {
          sci->SetLineIndentation(line,prev_indent);
        }
        if (sci->UseTabs()||tmp_tab) {
          sci->GoToPos(sci->sendMessage(SCI_POSITIONFROMLINE,line,0)+(prev_indent/tab_width));
        }
      }
      break;
    }
    case '}': {
      if (smart) {
        sci->getApp()->runWhileEvents();
        int opener=sci->sendMessage(SCI_BRACEMATCH,pos-1,0);
        if (opener>=0) {
          long match_line=sci->sendMessage(SCI_LINEFROMPOSITION,opener,0);
          if (match_line<line) {
            long match_indent=sci->sendMessage(SCI_GETLINEINDENTATION, match_line, 0);
            sci->sendMessage(SCI_SETLINEINDENTATION, line, match_indent);
          }
        }
      }
    }
  }
  if (recording) { recording->sendMessage(SCI_STARTRECORD,0,0); }
}



void SciDocUtils::AdjustIndent(SciDoc*sci, char ch, Settings*prefs, SciDoc*recording)
{
  sci->getApp()->runWhileEvents();
  long pos=sci->sendMessage(SCI_GETCURRENTPOS,0,0);
  long line=sci->sendMessage(SCI_LINEFROMPOSITION,pos,0);
  CharAdded(sci, line, pos, ch, prefs, recording);
}



// Check for an already-selected filename
static void GetFilenameFromSelection(FXMainWindow*tw,SciDoc*sci, FXString &filename)
{
#ifdef WIN32
  sci->GetSelText(filename);
#else // On X11 platforms, try first to get a filename from the X-Selection
  FXuchar*xsel=NULL;
  FXuint xlen=0;
  FXDragType types[] = { tw->textType, tw->utf8Type, tw->stringType, 0 };
  for ( FXDragType*type=types; *type; type++ ) {
    if (tw->getDNDData(FROM_SELECTION,*type, xsel, xlen) && xsel && *xsel) {
      FXuchar*eol=(FXuchar*)memchr(xsel,'\n', xlen);
      FXuint n = eol ? (eol-xsel) : xlen;
      filename.assign((FXchar*)xsel,n);
      filename=filename.simplify();
      if (!FXStat::exists(filename.contains(':')?filename.section(':',0):filename)) {
        filename=FXString::null;
      }
      break;
    }
    if ( filename.empty() ) { sci->GetSelText(filename); }
  }
#endif
}



// Try to find a filename at the current position in the document.
static bool GetFilenameAtCursor(SciDoc*sci, FXString &filename)
{
  long max=sci->GetTextLength();
  if (max<=0) { return false; }
  TextRange range;
  memset(&range,0,sizeof(range));
  range.chrg.cpMin=sci->GetCaretPos();
  if ( (range.chrg.cpMin>0) && (sci->CharAt(range.chrg.cpMin)<='*') && (sci->CharAt(range.chrg.cpMin-1)>'*') ) {
    // Caret is at the end of a phrase, back up one before looking for start...
    range.chrg.cpMin--;
  }
  // Read backwards till we find the start of our phrase...
  while ( (range.chrg.cpMin>0) && (sci->CharAt(range.chrg.cpMin)>'*') ) { range.chrg.cpMin--; }
  if ( (range.chrg.cpMin<max) && (sci->CharAt(range.chrg.cpMin)<='*') ) { range.chrg.cpMin++; }
  if (range.chrg.cpMin>=max) { return false; }
  range.chrg.cpMax=range.chrg.cpMin+1;
  // Now read forward, looking for the end of our phrase...
  while ( (range.chrg.cpMax<max) && (sci->CharAt(range.chrg.cpMax)>'*') ) { range.chrg.cpMax++; }
  long len=range.chrg.cpMax-range.chrg.cpMin;
  if (len<=0) { return false; }
  range.lpstrText=(char*)calloc(len+1,1);
  sci->sendMessage(SCI_GETTEXTRANGE,0,reinterpret_cast<sptr_t>(&range));
  filename=range.lpstrText;
  free(range.lpstrText);
  return filename.empty()?false:true;
}



// Look for file: first in active document's directory; then in current working directory
static bool OpenLocalIncludeFile(SciDoc*sci, const FXString &filename, const FXString &line)
{
  if (!sci->Filename().empty()) {
    FXString fullpath=FXPath::directory(sci->Filename())+PATHSEPSTRING+filename;
    if (FXStat::exists(fullpath)) {
      TopWinPub::OpenFile(fullpath.text(),line.text(),false,true);
      return true;
    }
  }
  if (FXStat::exists(filename)) {
    TopWinPub::OpenFile(filename.text(),line.text(),false,true);
    return true;
  }
  return false;
}



// Look for file in system include directories
static bool OpenSystemIncludeFile(SciDoc*sci, const FXString &filename, const FXString &line)
{
  const FXString paths=Settings::SystemIncludePaths();
  for (FXint i=0; i<paths.contains('\n'); i++) {
    FXString fullpath=paths.section('\n',i);
    if (fullpath.empty()) { continue; }
    fullpath+=PATHSEPSTRING;
    fullpath+=filename;
    if (FXStat::exists(fullpath)) {
      TopWinPub::OpenFile(fullpath.text(),line.text(),false,true);
      return true;
    }
  }
  return false;
}



// Look for line number after filename in the form of FILE.EXT:NNN
static void ParseLineNumberFromFilename(FXString &filename, FXString &line)
{
  #ifdef WIN32 // Ignore colon in drive spec on WIN32
  FXint colons=filename.contains(':');
  if (FXPath::isAbsolute(filename)) {
    if (colons>1) {
      line=filename.section(':',2);
      filename=filename.section(':',0,2);
    }
  } else {
    if (colons>0) {
      line=filename.section(':',1) ;
      filename=filename.section(':',0);
    }
  }
#else
  if (filename.contains(':')) {
    line=filename.section(':',1) ;
    filename=filename.section(':',0);
  }
#endif
  for (FXint i=0; i<line.length(); i++) {
    if (!Ascii::isDigit(line[i])) { // If it's not all digits, forget it.
      line=FXString::null;
      break;
    }
  }
}



void SciDocUtils::OpenSelected(FXMainWindow*tw, SciDoc*sci)
{
  FXString filename=FXString::null;
  FXString line=FXString::null;
  GetFilenameFromSelection(tw,sci,filename);
  if (filename.empty()) {
    // Even if nothing is selected, look around for something that might be a filename...
    if (!GetFilenameAtCursor(sci,filename)) { return; }
  }
  ParseLineNumberFromFilename(filename,line);
  if (sci->sendMessage(SCI_GETLEXER,0,0)==SCLEX_CPP) {
    bool syshdr=false;
    if ( (filename[0]=='<') && (filename[filename.length()-1]=='>') ) {
      filename.erase(0,1);
      filename.trunc(filename.length()-1);
      if (filename.empty()) { return; }
      syshdr=true;
    }
    if (FXPath::isAbsolute(filename)&&FXStat::exists(filename)) {
      TopWinPub::OpenFile(filename.text(),line.text(),false,true);
      return;
    }
    if (syshdr) {
      if (OpenSystemIncludeFile(sci,filename,line)) { return; }
      if (OpenLocalIncludeFile( sci,filename,line)) { return; }
    } else {
      if (OpenLocalIncludeFile( sci,filename,line)) { return; }
      if (OpenSystemIncludeFile(sci,filename,line)) { return; }
    }
  } else {
    if (FXStat::exists(filename)) {
      TopWinPub::OpenFile(filename.text(),line.text(),false,true);
      return;
    } else {
      if ( (!FXPath::isAbsolute(filename)) && (!sci->Filename().empty()) ) {
        FXString fullpath=FXPath::directory(sci->Filename())+PATHSEPSTRING+filename;
        if (FXStat::exists(fullpath)) {
          TopWinPub::OpenFile(fullpath.text(),line.text(),false,true);
          return;
        }
      }
    }
  }
  // Looks like we failed - pretty up the filename so we can use it in an error message
  filename=filename.section("\n",0);
  filename.trunc(128);
  FXMessageBox::error(tw, MBOX_OK, _("File not found"), "%s:\n%s", _("Cannot find file"), filename.text());
}




void SciDocUtils::SetSciDocPrefs(SciDoc*sci, Settings*prefs)
{
  sci->sendMessage(SCI_SETMULTIPLESELECTION,false,0);
  sci->sendMessage(SCI_SETADDITIONALSELECTIONTYPING,true,0);
  sci->ShowLineNumbers(prefs->ShowLineNumbers);
  sci->ShowWhiteSpace(prefs->ShowWhiteSpace);
  sci->SetShowEdge(prefs->ShowRightEdge);
  sci->SetZoom(prefs->ZoomFactor);
  sci->setFont(prefs->fontdesc.face, prefs->fontdesc.size / 10);
  sci->sendMessage(SCI_SETEXTRAASCENT,prefs->FontAscent,0);
  sci->sendMessage(SCI_SETEXTRADESCENT,prefs->FontDescent,0);
  sci->CaretLineBG(prefs->ShowCaretLine?prefs->CaretLineBG():NULL);
  sci->RightMarginBG(prefs->RightMarginBG());
  sci->CaretWidth(prefs->CaretWidth);
  sci->SmartHome(prefs->SmartHome);
  sci->SetWrapAware(prefs->WrapAwareHomeEnd);
  sci->SmoothScroll(prefs->SmoothScroll);
  sci->TabWidth(prefs->TabWidth);
  sci->UseTabs(prefs->UseTabs);
  sci->WhiteSpaceBG(prefs->WhiteSpaceBG());
  sci->WhiteSpaceFG(prefs->WhiteSpaceFG());
  sci->SetEdgeColumn(prefs->RightEdgeColumn);
  sci->SetShowIndent(prefs->ShowIndentGuides);

  sci->CaretFG(prefs->CaretFG());
  sci->SelectionBG(prefs->SelectionBG());
  if (prefs->ShowWhiteSpace) { sci->ShowWhiteSpace(true); }
  if (prefs->ShowLineNumbers) { sci->ShowLineNumbers(true); }

  sci->sendMessage(SCI_SETVIRTUALSPACEOPTIONS,
    (SCVS_RECTANGULARSELECTION|(prefs->CaretPastEOL?SCVS_USERACCESSIBLE:0)), 0);

}



void SciDocUtils::CycleSplitter(SciDoc*sci, Settings*prefs)
{
    switch (prefs->SplitView) {
    case SPLIT_NONE: {
      switch (sci->GetSplit()) {
        case SPLIT_NONE: {
          sci->SetSplit(SPLIT_BELOW);
          break;
        }
        case SPLIT_BELOW: {
          sci->SetSplit(SPLIT_BESIDE);
          break;
        }
        case SPLIT_BESIDE: {
          sci->SetSplit(SPLIT_NONE);
          break;
        }
      }
      break;
    }
    case SPLIT_BELOW:
    case SPLIT_BESIDE: {
      switch (sci->GetSplit()) {
        case SPLIT_NONE: {
          sci->SetSplit(prefs->SplitView);
          break;
        }
        case SPLIT_BELOW:
        case SPLIT_BESIDE: {
          sci->SetSplit(SPLIT_NONE);
          break;
        }
      }
      break;
    }
  }
  SciDoc* sci2=(SciDoc*)sci->getNext();
  if (sci2) {
    SetSciDocPrefs(sci2,prefs);
    sci2->setFocus();
  } else {
    sci->setFocus();
  }
}


void SciDocUtils::Cut(SciDoc*sci)
{
  sci->setFocus();
  if (sci->GetSelLength()>0) { sci->sendMessage(SCI_CUT,0,0); }
}



void SciDocUtils::Copy(SciDoc*sci)
{
  sci->setFocus();
  // If any text is already selected, make sure the selection is "alive"
  long start=sci->sendMessage(SCI_GETSELECTIONSTART,0,0);
  long end=sci->sendMessage(SCI_GETSELECTIONEND,0,0);
  if (start!=end) {
    sci->sendMessage(SCI_SETSELECTIONSTART,start,0);
    sci->sendMessage(SCI_SETSELECTIONEND,end,0);
  }
  if (sci->GetSelLength()>0) { sci->sendMessage(SCI_COPY,0,0); }
}



void SciDocUtils::Paste(SciDoc*sci)
{
  sci->setFocus();
  if (sci->sendMessage(SCI_CANPASTE,0,0)) {
    // If any text is already selected, make sure the selection is "alive"
    long start=sci->sendMessage(SCI_GETSELECTIONSTART,0,0);
    long end=sci->sendMessage(SCI_GETSELECTIONEND,0,0);
    if (start!=end) {
      sci->sendMessage(SCI_SETSELECTIONSTART,start,0);
      sci->sendMessage(SCI_SETSELECTIONEND,end,0);
    }
    sci->sendMessage(SCI_PASTE,0,0);
    sci->sendMessage(SCI_CONVERTEOLS,sci->sendMessage(SCI_GETEOLMODE,0,0),0);
    sci->ScrollWrappedInsert();
  }
}



void SciDocUtils::Indent(SciDoc*sci, bool forward, bool single_space, int indent_width)
{
  long msg=forward?SCI_TAB:SCI_BACKTAB;
  int tab_width=sci->TabWidth();
  if (single_space)
  {
    FXbool use_tabs=sci->UseTabs();
    sci->UseTabs(false);
    sci->sendMessage(SCI_SETTABWIDTH,1,0);
    sci->sendMessage(msg,0,0);
    sci->TabWidth(tab_width);
    sci->UseTabs(use_tabs);
  } else {
    sci->TabWidth(sci->UseTabs()?tab_width:indent_width);
    sci->sendMessage(msg,0,0);
    sci->TabWidth(tab_width);
  }
}



SciDoc* SciDocUtils::NewSci(FXComposite*p, FXObject*trg, Settings*prefs)
{
  SciDoc*sci=new SciDoc(p,trg,id_scintilla);
  SetSciDocPrefs(sci,prefs);
  sci->SetWordWrap(prefs->WordWrap);
  sci->DoStaleTest(true);
  return sci;
}



bool SciDocUtils::InsertFile(SciDoc *sci, const FXString &filename)
{
  if (sci->InsertFile(filename.text())) {
     sci->ScrollWrappedInsert();
    return true;
  } else {
    FXMessageBox::error(sci->getShell(), MBOX_OK, _("Error opening file"), "%s:\n%s\n%s",
       _("Could not open file"), filename.text(), sci->GetLastError().text());
  }
  return false;
}



void SciDocUtils::DoneSci(SciDoc*sci, SciDoc*recording)
{
  if (recording==sci) { TopWinPub::instance()->handle(NULL,FXSEL(SEL_COMMAND,id_macro_record),NULL); }
  if (sci->hasClipboard()) { TopWinPub::SaveClipboard(); }
  FXWindow*page=sci->getParent();
  FXWindow*tab=page->getPrev();
  delete sci;
  delete (FXMenuCommand*)tab->getUserData();
  delete tab;
  delete page;
}



FXString SciDocUtils::Filename(SciDoc*sci)
{
  return sci->Filename();
}



void SciDocUtils::SetFocus(SciDoc*sci)
{
  sci->setFocus();
}



void SciDocUtils::KillFocus(SciDoc*sci)
{
  sci->killFocus();
}



void SciDocUtils::CopyText(SciDoc*sci, const FXString &txt)
{
  sci->sendString(SCI_COPYTEXT, txt.length(), txt.text());
}



bool SciDocUtils::Reload(SciDoc*sci)
{
  long pos=sci->sendMessage(SCI_GETCURRENTPOS,0,0);
  SciDoc*sci2=sci->Slave();
  long pos2=sci2?sci2->sendMessage(SCI_GETCURRENTPOS,0,0):0;
  if ( !sci->LoadFromFile(sci->Filename().text()) ) {
    FXMessageBox::error(sci->getShell(),
      MBOX_OK,_("Reload failed"), "%s\n%s", sci->Filename().text(), sci->GetLastError().text());
    return false;
  }
  sci->sendMessage(SCI_GOTOPOS,pos,0);
  if (sci2) { sci2->sendMessage(SCI_GOTOPOS,pos2,0); }
  FXTabItem*tab=(FXTabItem*)sci->getParent()->getPrev();
  tab->setText(FXPath::name(sci->Filename()));
  sci->DoStaleTest(true);
  return true;
}



bool SciDocUtils::Dirty(SciDoc*sci)
{
  return sci->Dirty();
}



bool SciDocUtils::SaveToFile(SciDoc*sci, const char*filename, bool as_itself)
{
  if (!sci->SaveToFile(filename,as_itself)) { return false; }
  if (as_itself) {
    ((FXTabItem*)(sci->getParent()->getPrev()))->setText(FXPath::name(filename));
  }
  return true;
}



const FXString SciDocUtils::GetLastError(SciDoc*sci)
{
  return sci->GetLastError();
}



FXival  SciDocUtils::ID(SciDoc*sci)
{
  return (FXival)sci->id();
}


void SciDocUtils::NeedBackup(SciDoc*sci, bool need)
{
  sci->NeedBackup(need);
}

