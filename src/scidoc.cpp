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
#include <cerrno>
#include <fxkeys.h>
#include <fx.h>
#include <Scintilla.h>
#include <SciLexer.h>
#include <FXScintilla.h>

#include "lang.h"
#include "prefs.h"

#include "compat.h"
#include "scisrch.h"

#include "intl.h"
#include "scidoc.h"


FXDEFMAP(SciDoc) SciDocMap[] = {
  FXMAPFUNC(SEL_KEYPRESS, 0, SciDoc::onKeyPress)
};

FXIMPLEMENT(SciDoc,FXScintilla,SciDocMap,ARRAYNUMBER(SciDocMap));


static const char* c_openers="{[(";
static const char* c_closers="}])";
static const char* h_openers="{[(<";
static const char* h_closers="}])>";



SciDoc::SciDoc(FXComposite*p,FXObject*tgt,FXSelector sel):FXScintilla(p, tgt, sel, LAYOUT_FILL)
{
  _openers=c_openers;
  _closers=c_closers;
  _filename="";
  _lasterror="";
  _loading=false;
  _dirty=false;
  _utf8=false;
  need_styled=false;
  need_backup=false;
  _lang=NULL;
  _filetime=0;
  splitter_style=SPLIT_NONE;
  search=new SciSearch(this);
  user_undo_level=0;

  sendMessage(SCI_SETMARGINTYPEN, 0, SC_MARGIN_NUMBER);
  sendMessage(SCI_SETMARGINWIDTHN, 1, 4);
  for (const char*c="CDLVXY"; *c; c++) {
    sendMessage(SCI_ASSIGNCMDKEY,(*c)+(SCMOD_CTRL<<16),0);
  }
  for (const char*c="LUT"; *c; c++) {
    sendMessage(SCI_ASSIGNCMDKEY,(*c)+(SCMOD_CTRL<<16),0);
    sendMessage(SCI_ASSIGNCMDKEY,(*c)+(SCMOD_CTRL<<16)+(SCMOD_SHIFT<<16),0);
  }
  sendMessage(SCI_ASSIGNCMDKEY,SCK_HOME,SCI_HOME);
  sendMessage(SCI_SETSCROLLWIDTHTRACKING,true,0);
  sendMessage(SCI_SETSCROLLWIDTH,4000,0);
  sendMessage(SCI_SETEDGECOLOUR,HexToRGB("#FF0000"),0);
  sendMessage(SCI_SETEOLMODE,Settings::instance()->DefaultFileFormat,0);
}



SciDoc::~SciDoc()
{
  delete search;
}



long SciDoc::onKeyPress(FXObject *o, FXSelector sel, void *p)
{
  FXEvent*ev=(FXEvent*)p;
  if ((ev->state & CONTROLMASK) && (ev->code==KEY_Tab) && (sendMessage(SCI_GETUSETABS, 0, 0)==0)) {
    sendString(SCI_ADDTEXT, 1, "\t");
    return 1;
  } else {
    return FXScintilla::onKeyPress(o,sel,p);
  }
}



void SciDoc::SetEolModeFromContent()
{
  char*contents=(char*)sendMessage(SCI_GETCHARACTERPOINTER,0,0);
  FXuint nCR=0;
  FXuint nLF=0;
  for (char*p=contents; *p; p++) {
    switch (*p) {
      case '\r': { nCR++; break; }
      case '\n': { nLF++; break; }
    }
  }
  if (nCR==0) {
    if (nLF>0) {  // for sure, it's Unix
      sendMessage(SCI_SETEOLMODE,SC_EOL_LF,0);
    }
  } else {
    if (nCR==nLF) { // most likely, it's DOS
      sendMessage(SCI_SETEOLMODE, SC_EOL_CRLF, 0);
    } else {
      if (nLF==0) { // for sure, it's Mac
        sendMessage(SCI_SETEOLMODE, SC_EOL_CR, 0);
      } else { // mixed line endings, so take a wild guess
        if (nCR>nLF) { // Mac format is rare, so DOS wins here
          sendMessage(SCI_SETEOLMODE, SC_EOL_CRLF, 0);
        } else {
          if ((nLF/2)>nCR) { // More Unix lines than DOS lines
            sendMessage(SCI_SETEOLMODE,SC_EOL_LF,0);
          } else {  // More DOS lines than Unix lines
            sendMessage(SCI_SETEOLMODE,SC_EOL_CRLF,0);
          }
        }
      }
    }
  }
  sendMessage(SCI_CONVERTEOLS,sendMessage(SCI_GETEOLMODE,0,0),0);
}



extern "C" {
  char get_file_encoding(const char*filename);
}



const char* SciDoc::BinaryFileMessage() { return "Binary file detected."; }



static bool ConfirmOpenBinary(SciDoc*sci, const char*filename)
{
  return FXMessageBox::warning(sci->getShell(), MBOX_YES_NO, _("Binary file"),
           "%s\n%s\n\n%s", filename, _("does not appear to be a text file."),
            _("Are you sure you want to open it?")) == MBOX_CLICKED_YES;
}



bool SciDoc::DoLoadFromFile(const char*filename,bool insert)
{
  _lasterror="";
  errno=0;
  bool rv=true;
  bool ro=GetReadOnly();
  if (ro&&insert) {
    _lasterror=_("Document is marked read-only.");
    return false;
  }
  if (FXStat::isDirectory(filename)) {
    _lasterror=_("is a directory");
    return false;
  }
  bool DefaultToAscii=Settings::instance()->DefaultToAscii;
  bool DefaultToSbcs=Settings::instance()->DefaultToSbcs;
  switch (get_file_encoding(filename)) {
    case 'B': { // Binary file
      if ( !ConfirmOpenBinary(this,filename) ) {
        _lasterror=BinaryFileMessage();
        return false;
      }
      if (!insert) { SetUTF8(!DefaultToSbcs); }
      break;
    }
    case 'T': { // Plain US-ASCII text file
      if (!insert) { SetUTF8(!DefaultToAscii); }
      break;
    }
    case 'H': { // High (extended ASCII) text file.
      if (!insert) { SetUTF8(!DefaultToSbcs); }
      break;
    }
    case 'U': { // UTF-8 encoded text file.
      if (!insert) { SetUTF8(true); }
      break;
    }
    case 'Z': { // Zero-length (empty) file.
      if (!insert) { SetUTF8(!DefaultToAscii); }
      break;
    }
    case 'F': { // Failure, could not read the file.
      _lasterror=strerror(errno);
      return false;
      break;
    }
  }
  FXFile fh(filename, FXFile::Reading);
  if (fh.isOpen()) {
    if (ro) {
      // This might happen e.g. if we are updating a document that has been modified externally
      sendMessage(SCI_SETREADONLY,0,0);
    }
    static const int BUFSIZE=1025;
    char buf[BUFSIZE];
    fh.position(0,FXIO::Begin);
    long p=0;
    _loading=!insert;
    if (insert) {
      p=sendMessage(SCI_GETCURRENTPOS, 0,0);
      sendMessage(SCI_BEGINUNDOACTION, 0, 0);
    } else {
      sendMessage(SCI_CLEARALL,0,0);
    }
    do {
      memset(buf,0,BUFSIZE);
      FXival n=fh.readBlock(buf,BUFSIZE-1);
      if (n<0) {
        _lasterror=strerror(errno);
        rv=false;
        break;
      }
      buf[n]='\0';
      if (insert) {
        _dirty=true;
        if (GetSelLength()>0) {
          sendString(SCI_REPLACESEL,0,buf);
          p=sendMessage(SCI_GETCURRENTPOS, 0,0);
        } else {
          sendString(SCI_INSERTTEXT,p,buf);
          p+=n;
        }
      } else {
        sendString(SCI_APPENDTEXT,n,buf);
      }
    } while (!fh.eof());
    fh.close();
    if (rv) {
      if (insert) {
        GoToPos(p);
        sendMessage(SCI_CONVERTEOLS,sendMessage(SCI_GETEOLMODE,0,0),0);
        sendMessage(SCI_ENDUNDOACTION,0,0);
      } else {
        _filename=FXPath::absolute(filename);
        _filetime=FXStat::modified(_filename);
        _dirty=false;
        need_backup=false;
        SetEolModeFromContent();
        sendMessage(SCI_EMPTYUNDOBUFFER,0,0);
      }
    }
    if (ro) { sendMessage(SCI_SETREADONLY,1,0); }
  } else {
    _lasterror=strerror(errno);
    rv=false;
  }
  _loading=false;
  return rv;
}



bool SciDoc::SaveToFile(const char*filename, bool as_itself)
{
  FXFile fh(filename, FXIO::Writing);
  _lasterror="";
  if (fh.isOpen()) {
    FXint len=sendMessage(SCI_GETLENGTH,0,0);
    FXival wrote=0;
    char*buf=(char*)malloc(len+1);
    sendString(SCI_GETTEXT,len+1,buf);
    wrote=fh.writeBlock(buf,len);
    if (fh.close() && (wrote==len)) {
      if (as_itself) {
        if (_filename.empty() && !getLanguage()) {
          if (!setLanguageFromFileName(FXPath::name(filename).text())) {
            setLanguageFromContent();
          }
        }
        _filename=FXPath::absolute(filename);
        _filetime=FXStat::modified(_filename);
        sendMessage(SCI_SETSAVEPOINT,0,0);
      }
      return true;
    }
  }
  _lasterror=strerror(errno);
  return false;
}



void SciDoc::GtLtIsBrace(bool gtlt)
{
  _openers=gtlt?h_openers:c_openers;
  _closers=gtlt?h_closers:c_closers;
}



bool SciDoc::GtLtIsBrace()
{
  return _openers==h_openers;
}



inline bool SciDoc::IsBrace(long &pos)
{
  char ThisBrace=sendMessage(SCI_GETCHARAT,pos,0);
  if (!strchr(_closers,ThisBrace)) {
     ThisBrace=sendMessage(SCI_GETCHARAT,pos-1,0);
     if (strchr(_openers,ThisBrace)) { pos--;} else {
       return false;
     }
  }
  int charwidth = sendMessage(SCI_POSITIONAFTER,pos,0)-sendMessage(SCI_POSITIONBEFORE,pos,0);
  return ((charwidth>=2)||((pos==0)&&(charwidth==1)));
}



#define INVALID_RANGE 2147483647

void SciDoc::MatchBrace()
{
  long CurrPos=sendMessage(SCI_GETCURRENTPOS,0,0);
  if (IsBrace(CurrPos)) {
    int ThatBrace=sendMessage(SCI_BRACEMATCH,CurrPos,0);
    if (ThatBrace>=0) {
      sendMessage(SCI_BRACEHIGHLIGHT,CurrPos,ThatBrace);
    } else {
      sendMessage(SCI_BRACEBADLIGHT,CurrPos,0);
    }
  } else {
    sendMessage(SCI_BRACEHIGHLIGHT,INVALID_RANGE,INVALID_RANGE);
  }
}



void SciDoc::setFont(const FXString &font, int size)
{
  sendString(SCI_STYLESETFONT, STYLE_DEFAULT, font.text());
  sendMessage(SCI_STYLESETSIZE, STYLE_DEFAULT, size);
  UpdateStyle();
}



static StyleDef *GlobalStyle=NULL;



bool SciDoc::setLanguage(LangStyle*ls)
{
  _lang=ls;
  UpdateStyle();
  return (_lang!=NULL);
}



bool SciDoc::setLanguage(const char*name)
{
  return setLanguage(GetLangByName(name));
}



bool SciDoc::setLanguageFromFileName(const char*ext)
{
  return setLanguage(GetLangFromDocName(ext));
}


/*
  Currently, this only works for files beginning with "<?xml" or "<!DOCTYPE HTML"
  and "shabang" files, e.g.  #!/usr/bin/bash
*/
bool SciDoc::setLanguageFromContent()
{
  char buf[256];
  char*p1=buf;
  sendString(SCI_GETTEXT, sizeof(buf), buf);
  while (isspace(*p1)) {p1++;}
  switch(*p1) {
    case '<': { // maybe markup
      switch (p1[1]) {
        case '!': {
          if ((strncmp(p1, "<!DOCTYPE", 9) == 0) && isspace(p1[9])) {
            p1+=9;
            while (isspace(*p1)) {p1++;}
            return ( (strncasecmp(p1, "html", 4)==0) && setLanguage("html") );
          } else {
            return false;
          }
        }
        case '?':{
          return ((strncasecmp(p1, "<?xml", 5)==0) && isspace(p1[5]) && setLanguage("xml"));
        }
        default: return false;
      }
    }
    case '#': { // maybe shell script
      if (p1[1]=='!') {
        FXString appname="";
        char*p2;
        p1+=2;
        p2=p1;
        while ((*p2)&&!isspace(*p2)) { p2++; }
        if (!*p2) { return false; }
        *p2='\0';
        appname=FXPath::title(p1);
        if (strcmp(appname.text(), "env")==0) {
          p2++;
          p1=p2;
          while ((*p2)&&!isspace(*p2)) { p2++; }
          if (!*p2) { return false; }
          *p2='\0';
          appname=FXPath::title(p1);
        }
        return setLanguage(GetLangFromAppName(appname.text()));
      } else {
        return false;
      }
    }
    default:return false;
  }
  return false;
}



void SciDoc::UpdateStyle()
{
  StyleDef *sd;
  int i;
  sendMessage(SCI_STYLECLEARALL, 0, 0);
  sendString(SCI_STYLESETFONT, STYLE_CALLTIP, getApp()->getNormalFont()->getName().text());
  sendMessage(SCI_STYLESETSIZE, STYLE_CALLTIP, getApp()->getNormalFont()->getSize()/10);
  sendMessage(SCI_CALLTIPUSESTYLE,32,1);

  ColorName DefaultFG;
  ColorName DefaultBG;

  if (GlobalStyle) {
    for (sd=GlobalStyle; sd&&sd->key; sd++) {
      sendMessage(SCI_STYLESETFORE, sd->id, HexToRGB(sd->fg));
      sendMessage(SCI_STYLESETBACK, sd->id, HexToRGB(sd->bg));
      sendMessage(SCI_STYLESETITALIC, sd->id, sd->style & Italic);
      sendMessage(SCI_STYLESETBOLD, sd->id, sd->style & Bold);
    }
    strncpy(DefaultFG, GlobalStyle->fg, 7);
    strncpy(DefaultBG, GlobalStyle->bg, 7);
  } else {
    strncpy(DefaultFG, "#000000", 7);
    strncpy(DefaultBG, "#ffffff", 7);
  }
  DefaultFG[7]='\0';
  DefaultBG[7]='\0';
  if (_lang) {
    sendMessage(SCI_SETLEXER, _lang->id, 0);
    sendMessage(SCI_SETSTYLEBITS, sendMessage(SCI_GETSTYLEBITSNEEDED,0,0), 0);
    for (i=0; i<=KEYWORDSET_MAX; i++) { sendString(SCI_SETKEYWORDS, i, ""); }
    if (_lang->words) {
      for (i=0; _lang->words[i]; i++) {
        sendString(SCI_SETKEYWORDS, i, _lang->words[i]);
      }
    }
    if (_lang->styles) {
      for (sd=_lang->styles; sd->key; sd++) {
        sendMessage(SCI_STYLESETFORE, sd->id, HexToRGB(sd->fg[0]?sd->fg:DefaultFG));
        sendMessage(SCI_STYLESETBACK, sd->id, HexToRGB(sd->bg[0]?sd->bg:DefaultBG));
        sendMessage(SCI_STYLESETITALIC, sd->id, sd->style & Italic);
        sendMessage(SCI_STYLESETBOLD, sd->id, sd->style & Bold);
//        sendMessage(SCI_STYLESETUNDERLINE, sd->id, sd->style & Underline);
//        sendMessage(SCI_STYLESETEOLFILLED, sd->id, sd->style & EOLFill);
      }
    }
    switch (_lang->tabs) {
      case TABS_DEFAULT: {
        UseTabs(Settings::instance()->UseTabs);
        break;
      }
      case TABS_ALWAYS: {
        UseTabs(true);
        break;
      }
      case TABS_NEVER: {
        UseTabs(false);
      }
      case TABS_AUTO:{
        char*contents=(char*)sendMessage(SCI_GETCHARACTERPOINTER,0,0);
        if (contents && ( (contents[0]=='\t') || strstr(contents,"\n\t") ) ) {
          UseTabs(true);
        } else {
          UseTabs(Settings::instance()->UseTabs);
        }
      }
    }
  } else {
    sendMessage(SCI_SETLEXER, 0, 0);
    sendMessage(SCI_SETLEXERLANGUAGE, 0, 0);
  }
  sendMessage(SCI_COLOURISE,0,-1);
  if (Slave()) { Slave()->UpdateStyle(); }
}



#define slave(f,v) { if (Slave()) { Slave()->f(v); } }



void SciDoc::CaretLineBG(const char*bgcolor)
{
  if (bgcolor) {
    memset(_caretlinebg,0,sizeof(_caretlinebg));
    strncpy(_caretlinebg, bgcolor, sizeof(_caretlinebg)-1);
  } else {
    _caretlinebg[0]='\0';
  }
  sendMessage(SCI_SETCARETLINEBACK, HexToRGB(_caretlinebg), 0);
  sendMessage(SCI_SETCARETLINEVISIBLE, _caretlinebg[0]?1:0, 0);
  slave(CaretLineBG,bgcolor);
}



void SciDoc::RightMarginBG(const char*bgcolor)
{
  if (bgcolor) {
    memset(_rightmarginbg,0,sizeof(_rightmarginbg));
    strncpy(_rightmarginbg, bgcolor, sizeof(_rightmarginbg)-1);
  } else {
    _rightmarginbg[0]='\0';
  }
  sendMessage(SCI_SETEDGECOLOUR, HexToRGB(_rightmarginbg), 0);
  slave(RightMarginBG,bgcolor);
}



// 0=transparent to 255=opaque.
void SciDoc::CaretLineAlpha(const char*alpha)
{
// setting SCI_SETCARETLINEBACKALPHA does not work correctly!
// and furthermore, it even makes SCI_SETCARETLINEBACK fail!
#if 0
  if (alpha) {
    memset(_caretlinealpha,0,sizeof(_caretlinealpha));
    strncpy(_caretlinealpha, alpha, sizeof(_caretlinealpha)-1);
    unsigned int r=0,g=0,b=0, rgb;
    sscanf(_caretlinealpha+1,"%2x%2x%2x",&r,&g,&b);
    rgb=((r+g+b)/3);
    sendMessage(SCI_SETCARETLINEBACKALPHA, rgb, 0);
  } else {
    _caretlinebg[0]='\0';
  }
  slave(CaretLineAlpha,alpha);
#endif
}



void SciDoc::WhiteSpaceBG(const char*bgcolor)
{
  if (bgcolor) {
    strncpy(_whitespacebg, bgcolor, sizeof(_whitespacebg)-1);
    sendMessage(SCI_SETWHITESPACEBACK, 1, HexToRGB(_whitespacebg));
  } else {
    sendMessage(SCI_SETWHITESPACEBACK, 0, 0);
    _whitespacebg[0]='\0';
  }
  slave(WhiteSpaceBG,bgcolor);
}



void SciDoc::WhiteSpaceFG(const char*fgcolor)
{
  if (fgcolor) {
    strncpy(_whitespacefg, fgcolor, sizeof(_whitespacefg)-1);
    sendMessage(SCI_SETWHITESPACEFORE, 1, HexToRGB(_whitespacefg));
  } else {
    sendMessage(SCI_SETWHITESPACEFORE, 0, 0);
    _whitespacefg[0]='\0';
  }
  slave(WhiteSpaceFG,fgcolor);
}



void SciDoc::CaretFG(const char*fgcolor)
{
  if (fgcolor) {
    strncpy(_caretfg, fgcolor, sizeof(_caretfg)-1);
    sendMessage(SCI_SETCARETFORE, HexToRGB(_caretfg), 0);
  } else {
    sendMessage(SCI_SETCARETFORE, 0, 0);
    _caretfg[0]='\0';
  }
  slave(CaretFG,fgcolor);
}



void SciDoc::SelectionBG(const char*bgcolor)
{
  if (bgcolor) {
    strncpy(_selectionbg, bgcolor, sizeof(_selectionbg)-1);
    sendMessage(SCI_SETSELBACK, 1, HexToRGB(_selectionbg));
  } else {
    sendMessage(SCI_SETCARETFORE, 0, 0);
    _caretfg[0]='\0';
  }
  slave(SelectionBG,bgcolor);
}



void SciDoc::DefaultStyles(StyleDef*styles)
{
  GlobalStyle=styles;
}



StyleDef* SciDoc::DefaultStyles()
{
  return GlobalStyle;
}



long SciDoc::GetSelLength()
{
  long start=sendMessage(SCI_GETSELECTIONSTART,0,0);
  long end=sendMessage(SCI_GETSELECTIONEND,0,0);
  return (end>start)?end-start:start-end;
}



long SciDoc::GetSelText(FXString&txt)
{
  long len=sendMessage(SCI_GETSELTEXT,0,0);
  txt="";
  if (len<=1) { return 0; }
  txt.length(len-1);
  sendString(SCI_GETSELTEXT,0,txt.text());
  return len;
}



void SciDoc::SetSelText(const FXString&source)
{
  sendString(SCI_REPLACESEL,0,source.text());
}



long SciDoc::GetText(FXString&txt)
{
  long len=sendMessage(SCI_GETLENGTH,0,0);
  txt.length(len+1);
  sendString(SCI_GETTEXT, len+1, txt.text());
  return txt.length();
}



void SciDoc::SetText(const char *source) {
  sendMessage(SCI_SETTARGETSTART,0,0);
  sendMessage(SCI_SETTARGETEND,sendMessage(SCI_GETLENGTH,0,0),0);
  sendString(SCI_REPLACETARGET,strlen(source),source);
  repaint();
}



#define WORD_CHARS "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_"

long SciDoc::WordAtPos(FXString&s, long pos)
{
  s="";
  if (pos<0) { pos= sendMessage(SCI_GETCURRENTPOS,0,0); }
  long line=sendMessage(SCI_LINEFROMPOSITION,pos,0);
  GetLineText(line,s);
  if (!s.empty()) {
    long bow=sendMessage(SCI_GETCOLUMN,pos,0);
    long eow=bow;
    while (strchr(WORD_CHARS, s[bow])) { bow--; }
    while (strchr(WORD_CHARS, s[eow])) { eow++; }
    s.trunc(eow);
    s.erase(0,bow+1);
  }
  return s.length();
}


// Accept any one of these chars as a delimiter
// for line:column coordinates
static const char* RowColDelims=":,.";


/*
  Try to parse row:column coordinates from a string like "123:456"
  or "645,321" and navigate to that location.
  return true if we at least found a line number.
*/
bool SciDoc::GoToStringCoords(const char*coords)
{
  long row,col,rows,cols,pos;
  char buf[256];
  char*p1, *p2=NULL;
  if (!coords) { return false; }
  strncpy(buf,coords,sizeof(buf)-1);
  for (p1=buf; *p1; p1++) {
    if (strchr(RowColDelims, *p1)) {
      p2=p1+1;
      *p1='\0';
    }
  }
  row=strtoul(buf,&p1,10);
  if ((p1&&!*p1)&&row>0) {
    rows=sendMessage(SCI_GETLINECOUNT,0,0);
    if (row>rows) { row=rows; }
    if (p2) {
      col=strtoul(p2,&p1,10);
      if ((!p1)||(*p1)) { col=0; }
    } else { col=0; }
    cols=sendMessage(SCI_LINELENGTH,row-1,0);
    if (col>=cols) { col=(cols-1); }
    pos=sendMessage(SCI_POSITIONFROMLINE,row-1,0)+col;
    GoToPos(pos);
    return true;
  }
  return false;
}




// Move caret to specified row and column coordinates -
// NOTE: First row is one, NOT zero!!!
void SciDoc::GoToCoords(long row, long col)
{
  long rows,cols,pos;
  rows=sendMessage(SCI_GETLINECOUNT,0,0);
  if (row>rows) { row=rows; }
  cols=sendMessage(SCI_LINELENGTH,row-1,0);
  if (col>=cols) { col=(cols-1); }
  pos=sendMessage(SCI_POSITIONFROMLINE,row-1,0)+col;
  GoToPos(pos);
}



void SciDoc::ScrollCaret(long pos)
{
  long top,btm,cur;
  sendMessage(SCI_SETXOFFSET,0,0);
  if (pos<0) sendMessage(SCI_SCROLLCARET,0,0);
  top=sendMessage(SCI_GETFIRSTVISIBLELINE,0,0);
  cur=sendMessage(SCI_LINEFROMPOSITION,pos>=0?pos:sendMessage(SCI_GETCURRENTPOS,0,0),0);
  btm=sendMessage(SCI_LINESONSCREEN,0,0)+top;
  if ( (cur>top)&&(cur<btm) ) { return; }
  if ( (top>0) && (top>=cur) ) {
    sendMessage(SCI_LINESCROLL, 0, -2);
  }
}



void SciDoc::GoToPos(long pos)
{
  sendMessage(SCI_GOTOPOS,pos,0);
  ScrollCaret(pos);
}



void SciDoc::Zoom(int direction)
{
  long msg=SCI_SETZOOM;
  long val=0;
  switch (direction) {
    case -2: { msg=SCI_SETZOOM; val=-10; break; }
    case -1: { msg=SCI_ZOOMOUT; break; }
    case 0: { msg=SCI_SETZOOM; val=0; break; }
    case 1: { msg=SCI_ZOOMIN; break; }
    case 2: { msg=SCI_SETZOOM; val=20; break; }
  }
  sendMessage(msg,val,0);
  if ((msg==SCI_ZOOMOUT)||(msg==SCI_ZOOMIN)) { sendMessage(msg,val,0); }

  sendMessage(SCI_SCROLLCARET,0,0);
  if (ShowLineNumbers()) { ShowLineNumbers(true); }
  slave(Zoom,direction);
}



void SciDoc::ShowLineNumbers(bool showit)
{
  if (showit) {
    sendMessage(SCI_SETMARGINWIDTHN, 0, sendString(SCI_TEXTWIDTH,STYLE_LINENUMBER, "99999"));
  } else {
     sendMessage(SCI_SETMARGINWIDTHN, 0, 0);
  }
  slave(ShowLineNumbers,showit);
}



void SciDoc::ShowWhiteSpace(bool showit)
{
  sendMessage(SCI_SETVIEWWS, showit?SCWS_VISIBLEALWAYS:SCWS_INVISIBLE, 0);
  sendMessage(SCI_SETVIEWEOL, showit? (showit&&Settings::instance()->WhitespaceShowsEOL):0 , 0);
  slave(ShowWhiteSpace,showit);
}



void SciDoc::SetUTF8(bool utf8)
{
  _utf8=utf8;
  sendMessage(SCI_SETCODEPAGE, utf8 ? SC_CP_UTF8 : 0, 0);
  slave(SetUTF8,utf8);
}



void SciDoc::GetSelection(CharacterRange &crange)
{
  crange.cpMin = sendMessage(SCI_GETSELECTIONSTART,0,0);
  crange.cpMax = sendMessage(SCI_GETSELECTIONEND,0,0);
}



void SciDoc::SetLineIndentation(int line, int indent)
{
  if (indent < 0) { return; }
  sendMessage(SCI_BEGINUNDOACTION,0,0);
  CharacterRange crange;
  GetSelection(crange);
  int posBefore = sendMessage(SCI_GETLINEINDENTATION,line,0);
  sendMessage(SCI_SETLINEINDENTATION, line, indent);
  int posAfter = sendMessage(SCI_GETLINEINDENTATION,line,0);
  int posDifference = posAfter - posBefore;
  if (posAfter > posBefore) {
    if (crange.cpMin >= posBefore) { crange.cpMin += posDifference; }
    if (crange.cpMax >= posBefore) { crange.cpMax += posDifference; }
  } else {
    if (posAfter < posBefore) {
      if (crange.cpMin >= posAfter) {
        if (crange.cpMin >= posBefore) {
          crange.cpMin += posDifference;
        } else {
          crange.cpMin = posAfter;
        }
      }
      if (crange.cpMax >= posAfter) {
        if (crange.cpMax >= posBefore) {
          crange.cpMax += posDifference;
        } else {
          crange.cpMax = posAfter;
        }
      }
    }
  }
  sendMessage(SCI_SETSEL, crange.cpMin, crange.cpMax);
  sendMessage(SCI_ENDUNDOACTION,0,0);
}



int SciDoc::GetLineLength(int line)
{
  return sendMessage(SCI_GETLINEENDPOSITION, line, 0) - sendMessage(SCI_POSITIONFROMLINE, line, 0);
}



long SciDoc::GetLineText(long linenum, FXString &text)
{
  text="";
  if (linenum==-1) {
    long pos=sendMessage(SCI_GETCURRENTPOS,0,0);
    linenum=sendMessage(SCI_LINEFROMPOSITION,pos,0);
  }
  if ( (linenum>=GetLineCount()) || (linenum<0) ) {
    return -1;
  }
  long len=sendMessage(SCI_LINELENGTH,linenum,0);
  if (len>0) {
    text.length(len+1);
    sendString(SCI_GETLINE, linenum, &(text[0]));
    text[len]='\0';
  }
  return len;
}



int SciDoc::Stale() {
  if (!check_stale) { return 0; }
  if (_filename.empty()) { return 0; }
  FXStat info;
  if (!FXStat::statFile(_filename,info)) {
     _lasterror=strerror(errno);
    return 2;
  }
  return ((info.modified() != _filetime))?1:0;
}



void SciDoc::SetSplit(FXint style)
{
  FXSplitter*sp=(FXSplitter*)getParent();
  if (style==splitter_style) { return; }
  splitter_style=style;
  switch (style) {
    case SPLIT_NONE: {
      delete getNext();
      return;
    }
    case SPLIT_BESIDE: {
      sp->setSplitterStyle(SPLITTER_HORIZONTAL);
      sp->setSplit(0, sp->getWidth()/2);
      break;
    }
    case SPLIT_BELOW: {
      sp->setSplitterStyle(SPLITTER_VERTICAL);
      sp->setSplit(0, sp->getHeight()/2);
      break;
    }
  }
  if (sp->numChildren()==1) {
    SciDoc*sci=new SciDoc(sp,target,message);
    sci->create();
    sci->sendMessage(SCI_SETDOCPOINTER, 0, sendMessage(SCI_GETDOCPOINTER, 0, 0));
    sci->setLanguage(getLanguage());
    sci->ShowLineNumbers(ShowLineNumbers());
    sci->ShowWhiteSpace(ShowWhiteSpace());
    sci->GoToPos(GetCaretPos());
    sci->sendMessage(SCI_SETZOOM,sendMessage(SCI_GETZOOM,0,0),0);
    sci->SetShowEdge(GetShowEdge());
  }
}



void SciDoc::SmartHome(bool smart)
{
  sendMessage(SCI_ASSIGNCMDKEY,SCK_HOME,smart?SCI_VCHOME:SCI_HOME);
  sendMessage(SCI_ASSIGNCMDKEY,SCK_HOME+(SCMOD_SHIFT<<16),smart?SCI_VCHOMEEXTEND:SCI_HOMEEXTEND);
  sendMessage(SCI_ASSIGNCMDKEY,
    SCK_HOME+(SCMOD_SHIFT<<16)+(SCMOD_ALT<<16), smart?SCI_VCHOMERECTEXTEND:SCI_HOMERECTEXTEND);
}



// Strange things can happen when we allow grouping of undo actions by end-users.
// For example, if a Lua script calls SCI_BEGINUNDOACTION and forgets to call
// SCI_ENDUNDOACTION, or if the script terminates abnormally, the document might
// continue collecting actions that can't be individually un-done. To help prevent
// such situations, all script-generated calls to affect undo grouping should be
// made through this method:
void SciDoc::SetUserUndoLevel(FXint action)
{
  switch (action) {
    case -1: { // pop one undo level
      if (user_undo_level>0) {
        user_undo_level--;
        sendMessage(SCI_ENDUNDOACTION, 0, 0);
      }
      break;
    }
    case 0: { // pop all undo levels
      while (user_undo_level>0) {
        user_undo_level--;
        sendMessage(SCI_ENDUNDOACTION, 0, 0);
      }
      break;
    }
    case 1: { // push one undo level
      if (user_undo_level<16) {
        user_undo_level++;
        sendMessage(SCI_BEGINUNDOACTION, 0, 0);
      }
      break;
    }
  }
}



void SciDoc::SetProperty(const char*key, const char*value)
{
  sendMessage(SCI_SETPROPERTY, reinterpret_cast<long>(key), reinterpret_cast<long>(value));
}



//  Lookup the property 'key' and place the result in 'value'.
//  If 'expanded' is true "keyword replacement" will be performed as described
//  in the Scintilla documentation. Returns true if the key is found, otherwise
//  it sets 'value' to an empty string and returns false.
bool SciDoc::GetProperty(const FXString &key, FXString &value, bool expanded)
{
  int gp=expanded?SCI_GETPROPERTYEXPANDED:SCI_GETPROPERTY;
  int len=sendMessage(gp,reinterpret_cast<long>(key.text()),0);
  if (len>0) {
    value.length(len+1);
    sendMessage(gp,reinterpret_cast<long>(key.text()),reinterpret_cast<long>(value.text()));
    return true;
  } else {
    value="";
    return false;
  }
}



//  Lookup the integer property 'key' and return its value.
//  If 'key' is not found, return 'default_value'
//  If 'key' is found but is not an integer, return zero.
int SciDoc::GetPropertyInt(const char*key, int default_value)
{
  return sendMessage(SCI_GETPROPERTYINT, reinterpret_cast<long>(key), default_value);
}
