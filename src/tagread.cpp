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


#include <cctype>
#include <fxkeys.h>
#include <fx.h>
#include <Scintilla.h>
#include <SciLexer.h>
#include <FXScintilla.h>

#include "compat.h"
#include "scidoc.h"
#include "readtags.h"

#include "intl.h"
#include "tagread.h"

extern "C" {
  // Let the "readtags.c" file use fxwarning instead of perror...
  void tag_warning(const char*msg);
}


void tag_warning(const char*msg)
{
  fxwarning("%s\n", msg);
}

typedef struct {
  char *file;
  char *pattern;
  unsigned long lineNumber;
  char *kind;
  short fileScope;
} MytagEntry;



static MytagEntry*CopyTag(tagEntry*te)
{
  MytagEntry*rv=(MytagEntry*)calloc(1,sizeof(tagEntry));
  rv->file=te->file?strdup(te->file):NULL;
  rv->pattern=te->address.pattern?strdup(te->address.pattern):NULL;
  rv->kind=te->kind?strdup(te->kind):NULL;
  rv->lineNumber=te->address.lineNumber;
  rv->fileScope=te->fileScope;
  return rv;
}



static void FreeTag(MytagEntry*t)
{
  if (t) {
    if (t->pattern) { free(t->pattern); }
    if (t->file) { free(t->file); }
    if (t->kind) { free(t->kind); }
    free(t);
  }
}


class TagDialog: public FXDialogBox {
  FXDECLARE(TagDialog);
protected:
  TagDialog() {}
private:
  FXList* taglist;
  FXString tagname;
  FXLabel *desc;
  void SetLabel();
public:
  long onListClick(FXObject*o, FXSelector sel, void*p);
  enum {
    ID_LIST_CLICK=FXDialogBox::ID_LAST,
    ID_LAST
  };
  TagDialog(FXWindow*p, const FXString &tn);
  ~TagDialog();
  FXList*list() { return taglist; }
  void appendItem(FXListItem*item) { taglist->appendItem(item); }
  virtual FXuint execute(FXuint placement=PLACEMENT_OWNER);
};



FXuint TagDialog::execute(FXuint placement)
{
  SetLabel();
  return FXDialogBox::execute(placement);
}



FXDEFMAP(TagDialog) TagDialogMap[]={
  FXMAPFUNC(SEL_DOUBLECLICKED,TagDialog::ID_LIST_CLICK, TagDialog::onListClick),
  FXMAPFUNC(SEL_CLICKED,TagDialog::ID_LIST_CLICK, TagDialog::onListClick),
  FXMAPFUNC(SEL_KEYPRESS,TagDialog::ID_LIST_CLICK, TagDialog::onListClick)
};


FXIMPLEMENT(TagDialog,FXDialogBox,TagDialogMap,ARRAYNUMBER(TagDialogMap));

#define PACK_UNIFORM PACK_UNIFORM_WIDTH|PACK_UNIFORM_HEIGHT
#define BTN_OPTS FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_Y|LAYOUT_RIGHT

TagDialog::TagDialog(FXWindow*p,
  const FXString &tn):FXDialogBox(p,_("Choose tag"),DECOR_TITLE|DECOR_BORDER,0,0,480,320)
{
  tagname=tn;
  FXHorizontalFrame* buttons=new FXHorizontalFrame(this,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|PACK_UNIFORM_WIDTH);
  buttons->setPadRight(8);
  buttons->setPadLeft(8);
  buttons->setHSpacing(12);
  new FXButton(buttons,_("&OK"),         NULL,this,ID_ACCEPT,BTN_OPTS);
  new FXButton(buttons,_("  &Cancel  "), NULL,this,ID_CANCEL,BTN_OPTS);
  new FXHorizontalSeparator(this,SEPARATOR_GROOVE|LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X);

  FXVerticalFrame*vbox=new FXVerticalFrame(this,FRAME_RAISED|FRAME_THICK|LAYOUT_SIDE_TOP|LAYOUT_FILL);
  taglist=new FXList(vbox,this,ID_LIST_CLICK, LIST_BROWSESELECT|LAYOUT_SIDE_TOP|LAYOUT_FILL);
  desc=new FXLabel(vbox, " \n ",
    NULL,LABEL_NORMAL|LAYOUT_SIDE_BOTTOM|LAYOUT_SIDE_RIGHT|LAYOUT_FILL_X);
  changeFocus(taglist);
}


TagDialog::~TagDialog()
{
  FXint i;
  for (i=0; i<taglist->getNumItems(); i++) {
    FreeTag((MytagEntry*)(taglist->getItemData(i)));
    taglist->setItemData(i,NULL);
  }
}



long TagDialog::onListClick(FXObject*o, FXSelector sel, void*p)
{
  switch ( FXSELTYPE(sel) ) {
    case SEL_DOUBLECLICKED: {
      getApp()->stopModal(this,true);
      return 1;
    }
    case SEL_KEYPRESS: {
      FXint code=((FXEvent*)p)->code;
      if ((code==KEY_Return)||(code==KEY_KP_Enter)) {
        getApp()->stopModal(this,true);
        return 1;
      }
    }
  }
  SetLabel();
  return 0;
}



static inline const char*kind(char *c)
{
  if (c) {
    switch (*c) {
      case 'c': return "class";
      case 'd': return "define";
      case 'e': return "enum value";
      case 'f': return "function";
      case 'g': return "enum";
      case 'm': return "member";
      case 'n': return "namespace";
      case 's': return "struct";
      case 't': return "typedef";
      case 'u': return "union";
      case 'v': return "variable";
      case 'x': return "external";
    default:return "";
    }
  } else {
    return "";
  }
}



void TagDialog::SetLabel()
{
  MytagEntry*mt=(MytagEntry*)(taglist->getItemData(taglist->getCurrentItem()));
  FXString txt;
  txt.format("%s %s %s\n%s", kind(mt->kind), tagname.text(), mt->fileScope?"local ":"",mt->file);
  desc->setText(txt);
}



static bool IsCallTipFile(const char*filename)
{
  FXFile fh(filename,FXIO::Reading);
  if (fh.isOpen()) {
    char buf[64];
    memset(buf,0,sizeof(buf));
    fh.readBlock(buf,sizeof(buf)-1);
    fh.close();
    if (strstr(buf,"!_TAG_FILE_FORMAT\t")==buf) {
      return false;
    }
  }
  return true;
}



static FXListItem *NewListItem(tagEntry *te)
{
  char *txt;
  if (te->address.pattern) {
    // Strip the pattern-matching meta-chars from the start and end of the label.
    int p=0;
    if (strncmp(te->address.pattern,"/^", 2)==0) { p+=2; }
    while (isspace(te->address.pattern[p])) {p++;}
    txt=strdup(te->address.pattern+p);
    int n=strlen(txt);
    if ((n>2)&&(txt[n-1]=='/')&&(txt[n-2]=='$')) { txt[n-2]='\0'; }
  } else {
    txt=strdup("");
  }
  FXListItem *rv=new FXListItem(txt,NULL,CopyTag(te));
  free(txt);
  return rv;
}



static bool FindTag(FXWindow*shell,
  const FXString &tagname, FXMenuCascade* unloadtagsmenu, FXString &outfile, FXString &outcoords, FXString &outpat)
{
  outfile="";
  outcoords="";
  outpat="";
  if (unloadtagsmenu->getMenu()->numChildren() && !tagname.empty()) {
    FXWindow*w;
    MytagEntry*mt=NULL;
    TagDialog dlg(shell,tagname);
    tagEntry te;
    for (w=unloadtagsmenu->getMenu()->getFirst(); w; w=w->getNext()) {
      const char* filename=((FXMenuCommand*)w)->getText().text();
      if (IsCallTipFile(filename)) { continue; }
      tagFileInfo ti;
      memset(&ti,0,sizeof(ti));
      tagFile *tf=tagsOpen(filename, &ti);
      int tagflags=(TAG_FULLMATCH|TAG_OBSERVECASE);
      if (tf) {
        if (ti.status.opened) {
          if (tagsFind(tf, &te, tagname.text(), tagflags)==TagSuccess) {
            dlg.appendItem(NewListItem(&te));
            while (tagsFindNext(tf,&te)==TagSuccess) {
              dlg.appendItem(NewListItem(&te));
            }
          }
        }
        tagsClose(tf);
      }
    }
    switch (dlg.list()->getNumItems()) {
      case 0:{
        FXMessageBox::information(shell, MBOX_OK, _("Tag not found."),_("Tag \"%s\" not found\n"), tagname.text());
        break;
      }
      case 1: {
        mt=(MytagEntry*)(dlg.list()->getItem(0)->getData());
        break;
      }
      default:{
        dlg.list()->selectItem(0);
        if (dlg.execute(PLACEMENT_OWNER)) {
          mt=(MytagEntry*)(dlg.list()->getItemData(dlg.list()->getCurrentItem()));
        }
      }
    }
    if (mt) {
      outfile=mt->file;
      if (mt->lineNumber) { outcoords.format("%ld", mt->lineNumber); } else { outcoords=""; }
      outpat=mt->pattern;
      return true;
    }
  }
  return false;
}



bool FindTag(SciDoc*sci, FXMenuCascade* unloadtagsmenu, FXString &outfile, FXString &outcoords, FXString &outpat)
{
  FXString s;
  sci->GetSelText(s);
  if (s.empty()) { sci->WordAtPos(s); }
  return FindTag(sci->getShell(),s,unloadtagsmenu,outfile,outcoords,outpat);
}



static void TagToTip(tagEntry *te, FXString&calltip)
{
  const char*sig=tagsField(te,"signature");
  if (sig && te->name) {
    const char*acc=tagsField(te,"access");
    if (acc && (strcmp(acc, "public")!=0)) { return; }
    FXString tip=te->name;
    tip.append(sig);
    if (calltip.contains(tip)) { return; }
    if (!calltip.empty()) { calltip.append("\n"); }
    calltip.append(tip);
  }
}


/*
  Check to see if the file's contents begin with a valid "exuberant C tags" signature,
  if so, return true. Otherwise assume the file is in our own "calltips" format,
  and parse it for the tagname. If found, append its tip text to the calltip string.
*/

static bool IsTagFile(const char*filename,const FXString &tagname, FXString &calltip) {
  FXint size=FXStat::size(filename);
  if (size>0) {
    FXFile fh(filename,FXIO::Reading);
    if (fh.isOpen()) {
      char buf[64];
      memset(buf,0,sizeof(buf));
      fh.readBlock(buf,sizeof(buf)-1);
      if (strstr(buf,"!_TAG_FILE_FORMAT\t")==buf) {
        fh.close();
        return true;
      } else {
        FXString all;
        all.length(size+1);
        fh.position(0);
        fh.readBlock(&all[0],size);
        FXint pos;
        if ( strncmp(all.text(), (tagname+"\n").text(), tagname.length()+1) == 0 )
        {
          pos=0;
        } else {
          pos=all.find("\n"+tagname+"\n");
        }
        if (pos>=0) {
          FXint start=pos+1+tagname.length()+1;
          if (pos==0) { start--; } // account for no newline before first file entry.
          FXint stop=all.find("\n\n",start);

          calltip.append("\n"+all.mid(start,stop-start));
        }
      }
      fh.close();
    }
  }
  return false;
}



void ShowCallTip(SciDoc*sci, FXMenuCascade* unloadtagsmenu)
{
  FXString tagname;
  FXString calltip="";
  long pos=sci->GetCaretPos();
  sci->GetSelText(tagname);
  if (tagname.empty()) {
    if (sci->sendMessage(SCI_GETCHARAT,pos-1, 0)=='(') {
      long p=pos-2;
      while ((p>0) && isspace(sci->sendMessage(SCI_GETCHARAT,p, 0))) {p--;}
      sci->WordAtPos(tagname, p-1);
    } else {
      sci->WordAtPos(tagname);
    }
  }
  if (!tagname.empty()) {
    FXWindow*w;
    tagEntry te;
    for (w=unloadtagsmenu->getMenu()->getFirst(); w; w=w->getNext()) {
      const char*filename=((FXMenuCommand*)w)->getText().text();
      if (IsTagFile(filename,tagname,calltip)) {
        tagFileInfo ti;
        memset(&ti,0,sizeof(ti));
        tagFile *tf=tagsOpen(filename, &ti);
        int tagflags=(TAG_FULLMATCH|TAG_OBSERVECASE);
        if (tf) {
          if (ti.status.opened) {
            if (tagsFind(tf, &te, tagname.text(), tagflags)==TagSuccess) {
              TagToTip(&te,calltip);
              while (tagsFindNext(tf,&te)==TagSuccess) {
                TagToTip(&te,calltip);
              }
            }
          }
          tagsClose(tf);
        }
      }

    }
    if (!calltip.empty()) {
      FXString wrapped="";
      calltip.append('\n');
      FXint nlines=calltip.contains('\n');
      for (FXint iline=0; iline<nlines; iline++) {
        FXString line=calltip.section('\n', iline);
        if (line.length()>80) {
          char sep=line.contains(',')?',':line.contains(';')?';':line.contains(' ')?' ':'\0';
          FXint nseps=sep?line.contains(sep):0;
          if (nseps) {
            FXString str="";
            FXString substr="";
            for (FXint isep=0; isep<=nseps; isep++) {
              FXString sect=line.section(sep,isep);
              if (isep<nseps) {
                sect.append(sep);
                sect.append(' ');
              }
              if ( (substr.length()+sect.length()) < 80 ) {
                substr.append(sect);
              } else {
                if (!str.empty()) { str.append("\n    "); }
                str.append(substr);
                substr=sect;
              }
            }
            line=str+"\n    "+substr;
          }
        }
        if (!wrapped.empty())wrapped.append('\n');
        wrapped.append(line);
      }
      sci->sendString(SCI_CALLTIPSHOW, pos, wrapped.text());
    }
  }
}



void ParseAutoCompleteFile(FXDict*dict, char startchar, const char*filename)
{
  FXFile file(filename,FXIO::Reading);
  if (file.isOpen()) {
    char*lines=(char*)malloc(file.size()+1);
    lines[file.size()]='\0';
    if (file.readBlock(lines,file.size())==file.size()) {
      if (strncmp(lines,"!_TAG_FILE_FORMAT\t",18)==0) {
        char*p1=lines;
        do {
          char*p3=strchr(p1,'\n');
          if (!p3) { p3=strchr(p1,'\0'); }
          if (*p1==startchar) {
            char*p2=strchr(p1,'\t');
            if (p2&&p3&&(p3>p2)) {
              *p2='\0';
              dict->insert(p1,NULL);
            } else { break; }
          }
          if (*p3) { p1=p3+1; } else { break; }
        } while (1);
      } else {
        char*p1=lines;
        char*p3=strchr(p1,'\0');
        while (Ascii::isSpace(*p1)) { p1++; }
        while ((p3>p1)&&Ascii::isSpace(*(p3-1))) {
          p3--;
          *p3='\0';
        }
        if (strstr(p1,"\n\n")||strstr(p1,"\r\n\r\n")) {
          // Blank lines are not allowed, it might be a calltips file, so skip it for now.
        } else {
          do {
            char*p2=strchr(p1,'\n');
            if (!p2) { p2=p3; }
            if (*p1==startchar) {
              *p2='\0';
              dict->insert(p1,NULL);
            }
            if (p2<p3) { p1=p2+1; } else { break; }
          } while (1);
        }
      }
    }
    file.close();
    free(lines);
  }
}

