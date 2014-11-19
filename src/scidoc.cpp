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
#include <fxkeys.h>
#include <fx.h>
#include <FXTextCodec.h>
#include <FXUTF16Codec.h>

#include "lang.h"
#include "prefs_base.h"
#include "compat.h"
#include "scisrch.h"

#include "intl.h"
#include "scidoc.h"


FXDEFMAP(SciDoc) SciDocMap[] = {
  FXMAPFUNC(SEL_KEYPRESS, 0, SciDoc::onKeyPress),
  FXMAPFUNC(SEL_RIGHTBUTTONPRESS, 0, SciDoc::onRightBtnPress),
  FXMAPFUNC(SEL_COMMAND, SciDoc::ID_RECORD_REPLACE, SciDoc::onRecordReplace),
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
  smart_home=false;
  _lang=NULL;
  _filetime=0;
  splitter_style=SPLIT_NONE;
  search=new SciSearch(this,ID_RECORD_REPLACE);
  user_undo_level=0;
  recording=false;
  memset(bom,0,sizeof(bom));
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
  sendMessage(SCI_ASSIGNCMDKEY,SCK_DELETE+(SCMOD_CTRL<<16), 0);
  sendMessage(SCI_ASSIGNCMDKEY,SCK_DELETE+(SCMOD_CTRL<<16)+(SCMOD_SHIFT<<16), 0);
  sendMessage(SCI_ASSIGNCMDKEY,SCK_BACK+(SCMOD_CTRL<<16),   0);
  sendMessage(SCI_ASSIGNCMDKEY,SCK_BACK+(SCMOD_CTRL<<16)+(SCMOD_SHIFT<<16), 0);
  sendMessage(SCI_SETSCROLLWIDTHTRACKING,true,0);
  sendMessage(SCI_SETSCROLLWIDTH,4000,0);
  sendMessage(SCI_SETEDGECOLOUR,ColorFuncs::HexToRGB("#FF0000"),0);
  sendMessage(SCI_SETEOLMODE,SettingsBase::instance()->DefaultFileFormat,0);
  sendMessage(SCI_SETXCARETPOLICY,CARET_SLOP,8);
}



SciDoc::~SciDoc()
{
  delete search;
}



void SciDoc::ShowPopupMenu(int x, int y)
{
  long pos=sendMessage(SCI_GETCURRENTPOS,0,0);
  if (x<0||y<0) {
    x=sendMessage(SCI_POINTXFROMPOSITION,0,pos);
    y=sendMessage(SCI_POINTYFROMPOSITION,0,pos);
    translateCoordinatesTo(x,y,getApp()->getRootWindow(),x,y);
  }
  FXPoint p(x,y);
  if (target && message) { target->tryHandle(this,FXSEL(SEL_PICKED,message), (void*)&p); }
}



long SciDoc::onRightBtnPress(FXObject *o, FXSelector sel, void *p)
{
  FXEvent* ev=(FXEvent*)p;
  ShowPopupMenu(ev->root_x-4,ev->root_y-2);
  return 1;
}



long SciDoc::onKeyPress(FXObject *o, FXSelector sel, void *p)
{
  FXEvent*ev=(FXEvent*)p;
  switch (ev->code) {
    case KEY_Page_Up:
    case KEY_Page_Down: {
      long ret=FXScintilla::onKeyPress(o,sel,p);
      sendMessage(SCI_SCROLLCARET,0,0);
      return ret;
    }
    case KEY_Tab: { // Ctrl+Tab forces a tab when "UseTabs" is off
      if ((ev->state & CONTROLMASK) && (sendMessage(SCI_GETUSETABS, 0, 0)==0)) {
        sendString(SCI_ADDTEXT, 1, "\t");
        return 1;
      } else { break; }
    }
    case KEY_Menu: {
      ShowPopupMenu(-1,-1);
      return 1;
    }
    case KEY_F10: {
      if (ev->state & SHIFTMASK) {
        ShowPopupMenu(-1,-1);
        return 1;
      } else { break; }
    }
  }
  return FXScintilla::onKeyPress(o,sel,p);
}



void SciDoc::moveContents(FXint x,FXint y)
{
  FXScintilla::moveContents(x,y);
  FXint sw=sendMessage(SCI_GETSCROLLWIDTH,0,0);
  horizontal->setRange(sw);
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



void SciDoc::AdjustHScroll()
{
  FXint linecount=sendMessage(SCI_GETLINECOUNT,0,0);
  FXint widest=0;
  FXint line=0;
  for (FXint i=0; i<linecount; i++) {
    FXint w=sendMessage(SCI_LINELENGTH,i,0);
    if (w>widest) {
      widest=w;
      line=i;
    }
  }
  FXint pos=sendMessage(SCI_GETLINEENDPOSITION,line,0);
  FXint x=sendMessage(SCI_POINTXFROMPOSITION,0,pos);
  if (sendMessage(SCI_GETSCROLLWIDTH,0,0)<x) {
    sendMessage(SCI_SETSCROLLWIDTH,x,0);
  }
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
  memset(bom,0,sizeof(bom));
  bool rv=true;
  bool ro=GetReadOnly();
  FXTextCodec *codec=NULL;
  if (ro&&insert) {
    _lasterror=_("Document is marked read-only.");
    return false;
  }
  if (FXStat::isDirectory(filename)) {
    _lasterror=_("is a directory");
    return false;
  }
  bool DefaultToAscii=SettingsBase::instance()->DefaultToAscii;
  bool DefaultToSbcs=SettingsBase::instance()->DefaultToSbcs;
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
    case 'U': { // UTF-8 encoded text file w/o BOM.
      if (!insert) { SetUTF8(true); }
      break;
    }
    case 'M': { // UTF-8 BOM.
      if (!insert) {
        strcpy(bom,"\0357\0273\0277");
        SetUTF8(true);
      }
      break;
    }
    case 'Z': { // Zero-length (empty) file.
      if (!insert) { SetUTF8(!DefaultToAscii); }
      break;
    }
    case 'e': { // UTF-16LE BOM.
      if (!insert) {
        codec=new FXUTF16LECodec();
        strcpy(bom,"\377\376");
        SetUTF8(false);
      }
      break;
    }
    case 'E': { // UTF-16BE BOM.
      if (!insert) {
        codec=new FXUTF16BECodec();
        strcpy(bom,"\376\377");
        SetUTF8(false);
      }
      break;
    }
    case 'F': { // Failure, could not read the file.
      _lasterror=SystemErrorStr();
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
    fh.position(strlen(bom),FXIO::Begin);
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
        _lasterror=SystemErrorStr();
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
        if (codec) {
          const char *orig=(const char*)sendMessage(SCI_GETCHARACTERPOINTER,0,0);
          FXString recode;
          recode.length(codec->mb2utflen(orig,sendMessage(SCI_GETLENGTH,0,0)));
          codec->mb2utf((char*)recode.text(),recode.length(),orig,sendMessage(SCI_GETLENGTH,0,0));
          delete codec;
          SetUTF8(true);
          sendString(SCI_SETTEXT,0,recode.text());
        }
        SetEolModeFromContent();
        sendMessage(SCI_EMPTYUNDOBUFFER,0,0);
      }
      AdjustHScroll();
    }
    if (ro) { sendMessage(SCI_SETREADONLY,1,0); }
  } else {
    _lasterror=SystemErrorStr();
    rv=false;
  }
  _loading=false;
  return rv;
}

const char*SciDoc::GetEncoding()
{
  switch ((FXuchar)bom[0]) {
    case 0xFF: { return "UTF-16LE"; }
    case 0xFE: { return "UTF-16BE"; }
    default: { return _utf8?"UTF-8":"ASCII"; }
  }
}


bool SciDoc::SaveToFile(const char*filename, bool as_itself)
{
#ifdef WIN32
  FXFile fh(filename, FXIO::WriteOnly|FXIO::Truncate|((FXStat::exists(filename))?0:FXIO::Create));
#else
  FXFile fh(filename, FXIO::Writing);
#endif
  _lasterror="";
  if (fh.isOpen()) {
    FXbool en=isEnabled();
    FXbool hf=hasFocus();
    FXTextCodec *codec=NULL;
    FXString recode=FXString::null;
    if (en) { disable(); }
    FXint len=sendMessage(SCI_GETLENGTH,0,0);
    const char*buf=(const char*)sendMessage(SCI_GETCHARACTERPOINTER,0,0);
    if (bom[0]) {
      fh.writeBlock(bom,strlen(bom));
      switch ((FXuchar)bom[0]) {
        case 0xFF: {
          codec=new FXUTF16LECodec();
          break;
        }
        case 0xFE: {
          codec=new FXUTF16BECodec();
          break;
        }
      }
    }
    if (codec) {
      FXint old_len=len;
      len=codec->utf2mblen(buf,old_len);
      recode.length(len);
      codec->utf2mb((char*)recode.text(),len,buf,old_len);
      delete codec;
      buf=recode.text();
    }
    FXival wrote=fh.writeBlock(buf,len);
    if (en) { enable(); }
    if (hf) { setFocus(); }
    if (fh.close() && (wrote==len)) {
      if (as_itself) {
        if (_filename.empty() && !getLanguage()) {
          if (!SetLanguageForHeader(filename)) {
            if (!setLanguageFromFileName(FXPath::name(filename).text())) {
              setLanguageFromContent();
            }
          }
        }
        _filename=FXPath::absolute(filename);
        _filetime=FXStat::modified(_filename);
        sendMessage(SCI_SETSAVEPOINT,0,0);
        DoStaleTest(true);
      }
      return true;
    }
  }
  _lasterror=SystemErrorStr();
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


#define IsCloser(c) ((c)&&(strchr(_closers,(c))))
#define IsOpener(c) ((c)&&(strchr(_openers,(c))))

inline bool SciDoc::IsInsideBrace(long &pos)
{
  char ThisBrace=sendMessage(SCI_GETCHARAT,pos,0);
  if (!IsCloser(ThisBrace)) {
    ThisBrace=sendMessage(SCI_GETCHARAT,pos-1,0);
    if (IsOpener(ThisBrace)) {
      pos--;
    } else {
      return false;
    }
  }
  int charwidth = sendMessage(SCI_POSITIONAFTER,pos,0)-sendMessage(SCI_POSITIONBEFORE,pos,0);
  return ((charwidth>=2)||((pos==0)&&(charwidth==1)));
}



inline bool SciDoc::IsOutsideBrace(long &pos)
{
  char ThisBrace=sendMessage(SCI_GETCHARAT,pos-1,0);
  if (IsCloser(ThisBrace)) {
    if (sendMessage(SCI_GETCURRENTPOS,0,0)==pos) { pos--; }
  } else {
    ThisBrace=sendMessage(SCI_GETCHARAT,pos,0);
    if (!IsOpener(ThisBrace)) {
      return false;
    }
  }
  int charwidth = sendMessage(SCI_POSITIONAFTER,pos,0)-sendMessage(SCI_POSITIONBEFORE,pos,0);
  return ((charwidth>=2)||((pos==0)&&(charwidth==1)));
}



inline bool SciDoc::IsAfterBrace(long &pos)
{
  if (pos<=0) { return false; }
  char ThisBrace=sendMessage(SCI_GETCHARAT,pos-1,0);
  if (IsOpener(ThisBrace)||IsCloser(ThisBrace)) {
    pos--;
  } else {
    return false;
  }
  int charwidth = sendMessage(SCI_POSITIONAFTER,pos,0)-sendMessage(SCI_POSITIONBEFORE,pos,0);
  return ((charwidth>=2)||((pos==0)&&(charwidth==1)));
}



inline bool SciDoc::IsBrace(long &pos, int mode)
{
  switch (mode) {
    case BRACEMATCH_INSIDE:  { return IsInsideBrace(pos); }
    case BRACEMATCH_OUTSIDE: { return IsOutsideBrace(pos); }
    case BRACEMATCH_EITHER:  { return (IsOutsideBrace(pos)||IsInsideBrace(pos)); }
    case BRACEMATCH_AFTER:   { return IsAfterBrace(pos); }
    default: { return false; }
  }
}



#define INVALID_RANGE 2147483647

#define StyleAt(idx) (sendMessage(SCI_GETSTYLEAT,(idx),0))

void SciDoc::MatchBrace()
{
  int mode=SettingsBase::instance()->BraceMatch;
  long CurrPos=sendMessage(SCI_GETCURRENTPOS,0,0);
  if ((CurrPos>0)&&(mode==BRACEMATCH_EITHER)) {
    char ThisChar=sendMessage(SCI_GETCHARAT,CurrPos,0);
    if (IsCloser(ThisChar)||IsOpener(ThisChar)) {
      long PrevPos=sendMessage(SCI_POSITIONBEFORE,CurrPos,0);
      char PrevChar=sendMessage(SCI_GETCHARAT,PrevPos,0);
      if (IsCloser(PrevChar)||IsOpener(PrevChar)) { CurrPos=PrevPos; }
    }
  }
  if (IsBrace(CurrPos,mode)) {
    int ThatBrace=sendMessage(SCI_BRACEMATCH,CurrPos,0);
    if ((ThatBrace>=0)&&(StyleAt(ThatBrace)!=StyleAt(CurrPos))) {
      sendMessage(SCI_COLOURISE,0,-1);
      ThatBrace=sendMessage(SCI_BRACEMATCH,CurrPos,0);
    }
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


// Try to guess if a *.h file is C or C++, return FALSE if file is not *.h, TRUE otherwise.
bool SciDoc::SetLanguageForHeader(const FXString &fn)
{
  if (FXPath::extension(fn)=="h") {
    FXString fnbase=FXPath::stripExtension(fn);
    // Check for matching source file and set language accordingly if found...
    if (FXStat::exists(fnbase+".c")) {
      setLanguage("c");
    } else if (FXStat::exists(fnbase+".cpp")||FXStat::exists(fnbase+".cxx")||FXStat::exists(fnbase+".cc")) {
      setLanguage("cpp");
    } else {
      // Take a wild guess - if the file contains the word "class" it's probably  C++
      const char *content=(const char*)(sendMessage(SCI_GETCHARACTERPOINTER,0,0));
#ifdef FOX_1_7_50_OR_NEWER
      if (FXRex("\\<class\\>").search(content,strlen(content),0,strlen(content))>=0) {
#else
      if (FXRex("\\<class\\>").match(content)) {
#endif
        setLanguage("cpp");
      } else {
        setLanguage("c");
      }
    }
    return true;
  } else {
    return false;
  }
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
        while (isblank(*p1))  { p1++; }
        p2=p1;
        while ((*p2)&&!isspace(*p2)) { p2++; }
        if (!*p2) { return false; }
        *p2='\0';
        appname=FXPath::name(p1);
        if (strcmp(appname.text(), "env")==0) {
          p1=p2+1;
          while (isblank(*p1))  { p1++; }
          p2=p1;
          while ((*p2)&&!isspace(*p2)) { p2++; }
          if (!*p2) { return false; }
          *p2='\0';
          appname=FXPath::name(p1);
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
      sendMessage(SCI_STYLESETFORE, sd->id, ColorFuncs::HexToRGB(sd->fg));
      sendMessage(SCI_STYLESETBACK, sd->id, ColorFuncs::HexToRGB(sd->bg));
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
        sendMessage(SCI_STYLESETFORE, sd->id, ColorFuncs::HexToRGB(sd->fg[0]?sd->fg:DefaultFG));
        sendMessage(SCI_STYLESETBACK, sd->id, ColorFuncs::HexToRGB(sd->bg[0]?sd->bg:DefaultBG));
        sendMessage(SCI_STYLESETITALIC, sd->id, sd->style & Italic);
        sendMessage(SCI_STYLESETBOLD, sd->id, sd->style & Bold);
//        sendMessage(SCI_STYLESETUNDERLINE, sd->id, sd->style & Underline);
//        sendMessage(SCI_STYLESETEOLFILLED, sd->id, sd->style & EOLFill);
      }
    }
    switch (_lang->tabs) {
      case TABS_DEFAULT: {
        UseTabs(SettingsBase::instance()->UseTabs);
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
          UseTabs(SettingsBase::instance()->UseTabs);
        }
      }
    }
    TabWidth(_lang->tabwidth>0?_lang->tabwidth:SettingsBase::instance()->TabWidth);
    SetProperty("lexer.cpp.track.preprocessor","0");
    SetProperty("lexer.cpp.update.preprocessor","0");
  } else {
    sendMessage(SCI_SETLEXER, 0, 0);
    sendMessage(SCI_SETLEXERLANGUAGE, 0, 0);
    UseTabs(SettingsBase::instance()->UseTabs);
    TabWidth(SettingsBase::instance()->TabWidth);
  }
  sendMessage(SCI_COLOURISE,0,-1);
  if (Slave()) { Slave()->UpdateStyle(); }
}



#define slave(f,v) { if (Slave()) { Slave()->f(v); } }



void SciDoc::CaretLineBG(const char*bgcolor)
{
  memset(_caretlinebg,0,sizeof(_caretlinebg));
  if (bgcolor) { strncpy(_caretlinebg, bgcolor, sizeof(_caretlinebg)-1); }
  sendMessage(SCI_SETCARETLINEBACK, ColorFuncs::HexToRGB(_caretlinebg), 0);
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
  sendMessage(SCI_SETEDGECOLOUR, ColorFuncs::HexToRGB(_rightmarginbg), 0);
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
    sendMessage(SCI_SETWHITESPACEBACK, 1, ColorFuncs::HexToRGB(_whitespacebg));
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
    sendMessage(SCI_SETWHITESPACEFORE, 1, ColorFuncs::HexToRGB(_whitespacefg));
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
    sendMessage(SCI_SETCARETFORE, ColorFuncs::HexToRGB(_caretfg), 0);
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
    sendMessage(SCI_SETSELBACK, 1, ColorFuncs::HexToRGB(_selectionbg));
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
  s.length(strlen(s.text()));
  return s.length();
}



long SciDoc::PrefixAtPos(FXString&s, long pos)
{
  s="";
  if (pos<0) { pos= sendMessage(SCI_GETCURRENTPOS,0,0); }
  long line=sendMessage(SCI_LINEFROMPOSITION,pos,0);
  GetLineText(line,s);
  if (!s.empty()) {
    long bow=sendMessage(SCI_GETCOLUMN,pos,0);
    long eow=bow;
    do { bow--; } while (strchr(WORD_CHARS, s[bow]));
    s.trunc(eow);
    s.erase(0,bow+1);
  }
  s.length(strlen(s.text()));
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
  if (GetWordWrap()) { return; }
  for (FXWindow *w=this; w; w=w->getParent()) { w->layout(); }
  if (pos>=0) { sendMessage(SCI_GOTOPOS,pos,0); }
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



void SciDoc::ScrollWrappedInsert()
{
  if (GetWordWrap()) {
    getApp()->runWhileEvents();
    sendMessage(SCI_SCROLLCARET,0,0);
  }
}



void SciDoc::ZoomStep(int direction)
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
  slave(ZoomStep,direction);
}



void SciDoc::SetZoom(int zoom)
{
  sendMessage(SCI_SETZOOM,zoom,0);
  slave(SetZoom,zoom);
}



int SciDoc::GetZoom()
{
  return sendMessage(SCI_GETZOOM,0,0);
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
  sendMessage(SCI_SETVIEWEOL, showit? (showit&&SettingsBase::instance()->WhitespaceShowsEOL):0 , 0);
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
     _lasterror=SystemErrorStr();
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
    sci->SetWordWrap(GetWordWrap());
  }
}



void SciDoc::SmartHome(bool smart)
{
  smart_home=smart;
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



void SciDoc::TabWidth(int w) {
  if (_lang && (_lang->tabwidth>0)) { w=_lang->tabwidth; }
  sendMessage(SCI_SETTABWIDTH,(w<1)?1:(w>16)?16:w,0);
}



void SciDoc::SetWordWrap(bool on)
{
  sendMessage(SCI_SETWRAPMODE,on?SC_WRAP_WORD:SC_WRAP_NONE,0);
  setScrollStyle(on?
    ((getScrollStyle()&~HSCROLLER_ALWAYS)|HSCROLLER_NEVER):
    ((getScrollStyle()&~HSCROLLER_NEVER)|HSCROLLER_ALWAYS)
  );
  slave(SetWordWrap,on);
}



bool SciDoc::GetWordWrap()
{
  return sendMessage(SCI_GETWRAPMODE,0,0)!=SC_WRAP_NONE;
}


#define LongFromTwoShorts(a,b) ( (a) | ((b) << 16) )


#define AssignKey(key,mods,cmd) sendMessage(SCI_ASSIGNCMDKEY, \
            LongFromTwoShorts(static_cast<short>(key), \
                    static_cast<short>(mods)), cmd);

void SciDoc::SetWrapAware(bool aware)
{
  if (aware) {
    if (smart_home) {
      AssignKey(SCK_HOME, 0, SCI_VCHOMEWRAP);
      AssignKey(SCK_HOME, SCMOD_SHIFT, SCI_VCHOMEWRAPEXTEND);
      AssignKey(SCK_HOME, SCMOD_SHIFT|SCMOD_ALT, SCI_VCHOMERECTEXTEND);
    } else {
      AssignKey(SCK_HOME, 0, SCI_HOMEWRAP);
      AssignKey(SCK_HOME, SCMOD_SHIFT, SCI_HOMEWRAPEXTEND);
      AssignKey(SCK_HOME, SCMOD_SHIFT|SCMOD_ALT, SCI_HOMERECTEXTEND);
    }
    AssignKey(SCK_END, 0, SCI_LINEENDWRAP);
    AssignKey(SCK_END, SCMOD_SHIFT, SCI_LINEENDWRAPEXTEND);
  } else {
    if (smart_home) {
      AssignKey(SCK_HOME, 0, SCI_VCHOME);
      AssignKey(SCK_HOME, SCMOD_SHIFT, SCI_VCHOMEEXTEND);
      AssignKey(SCK_HOME, SCMOD_SHIFT|SCMOD_ALT, SCI_VCHOMERECTEXTEND);
    } else {
      AssignKey(SCK_HOME, 0, SCI_HOME);
      AssignKey(SCK_HOME, SCMOD_SHIFT, SCI_HOMEEXTEND);
      AssignKey(SCK_HOME, SCMOD_SHIFT|SCMOD_ALT, SCI_HOMERECTEXTEND);
    }
    AssignKey(SCK_END, 0, SCI_LINEEND);
    AssignKey(SCK_END, SCMOD_SHIFT, SCI_LINEENDEXTEND);
  }
}



void SciDoc::SetCaseOfSelection(int msg)
{
  FXString seltext;
  long anchor=sendMessage(SCI_GETANCHOR,0,0);
  long currentpos=sendMessage(SCI_GETCURRENTPOS,0,0);
  long selcount=sendMessage(SCI_GETSELECTIONS,0,0);
  sendMessage(SCI_BEGINUNDOACTION, 0, 0);
  for (long selnum=0; selnum<selcount; selnum++) {
    Sci_TextRange trng;
    trng.chrg.cpMin=sendMessage(SCI_GETSELECTIONNSTART,selnum,0);
    trng.chrg.cpMax=sendMessage(SCI_GETSELECTIONNEND,selnum,0);
    seltext.length((trng.chrg.cpMax-trng.chrg.cpMin)+1);
    trng.lpstrText=&seltext[0];
    sendMessage(SCI_GETTEXTRANGE,0,reinterpret_cast<long>(&trng));
    seltext.length((trng.chrg.cpMax-trng.chrg.cpMin));
    switch (msg) {
      case SCI_UPPERCASE: {
        seltext.upper();
        break;
      }
      case SCI_LOWERCASE: {
       seltext.lower();
       break;
      }
    }
    sendMessage(SCI_SETTARGETSTART,trng.chrg.cpMin,0);
    sendMessage(SCI_SETTARGETEND,trng.chrg.cpMax,0);
    sendString(SCI_REPLACETARGET,seltext.length(),seltext.text());
  }
  sendMessage(SCI_SETCURRENTPOS,currentpos,0);
  sendMessage(SCI_SETANCHOR,anchor,0);
  sendMessage(SCI_ENDUNDOACTION, 0, 0);
}



void SciDoc::SelectionToUpper()
{
  SetCaseOfSelection(SCI_UPPERCASE);
}



void SciDoc::SelectionToLower()
{
  SetCaseOfSelection(SCI_LOWERCASE);
}



void SciDoc::EnableRecorder(bool enable_recorder)
{
  if (enable_recorder) {
    sendMessage(SCI_STARTRECORD, 0, 0);
  } else {
    sendMessage(SCI_STOPRECORD, 0, 0);
  }
  recording=enable_recorder;
}



long SciDoc::onRecordReplace(FXObject *o, FXSelector sel, void *p)
{
  if (!recording) { return 1;}
  if (target) { target->handle(this,FXSEL(SEL_COMMAND,message), p); }
  return 1;
}

