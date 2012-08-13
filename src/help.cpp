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


#include <fx.h>
#include <Scintilla.h>
#include <FXScintilla.h>

#include "sl.h"

#include "compat.h"
#include "appname.h"
#include "macro.h"

#include "intl.h"
#include "help.h"

#include "helptext.h"
#include "help_lua.h"

#define sendString(iMessage, wParam, lParam) sendMessage(iMessage, wParam, reinterpret_cast<long>(lParam))



enum {
  SCHLP_FIXED,
  SCHLP_ITALIC,
  SCHLP_BOLD,
  SCHLP_LINK,
  SCHLP_H1,
  SCHLP_NORMAL
};

#define SCHLP_FIRST SCHLP_FIXED
#define SCHLP_LAST SCHLP_NORMAL
#define SCHLP_LOGO SCHLP_LAST+1

/*
  The help files are written in a very crude markup (markdown?) language.
  Anything that matches one of the regex phrases below will be formatted accordingly.
  Link text is formatted as a "hyperlink" that, when clicked, will jump to the
  section header containing the identical text. Nested markup is not allowed!
*/

static const char* phrases[] = {
  "===(.+?)===",  // monospace font
  "///(.+?)///",  // italic font
  "%%%(.+?)%%%",  // bold font
  "@@@(.+?)@@@",  // link text
  "###(.+?)###",  // section header
  NULL
};


#if ((FOX_MAJOR>1) || (FOX_MINOR>7) || ((FOX_MINOR==7) && (FOX_LEVEL>=23)))
# define CONFIG_DIR_BASE_NAME ".config"
#else
# define CONFIG_DIR_BASE_NAME ".foxrc"
#endif

/*
  Instead of "hard-coding" these strings in hundreds of places in the text files,
  just use these generic "macros" - each regex placeholder will be replaced
  with its corresponding "real" name.
*/
static const char* replacements[] = {
  "\\<__APP__\\>",  APP_NAME,
  "\\<__EXE__\\>",  EXE_NAME,
  "\\<__MOD__\\>",  LUA_MODULE_NAME,
  "\\<__CFG__\\>",  CONFIG_DIR_BASE_NAME,
  NULL,             NULL
};



typedef struct _HelpLink {
  struct _HelpLink*next;
  int pos;
  char* href;
} HelpLink;



class SciHelp: public FXScintilla {
  FXDECLARE(SciHelp)
  SciHelp(){}
  void*links;
  void*sects;
  void freelists();
  long last_pos;
  long jump_pos;
  bool find(const FXString &what);
  void replace(const char*oldstr, const char*newstr);
public:
  SciHelp(FXComposite*p, FXObject*tgt=NULL, FXSelector sel=0, FXuint opts=LAYOUT_FILL, bool dark=false);
  ~SciHelp();
  void parse(const char*txt, unsigned int size);
  long onLeftBtnRelease(FXObject*o, FXSelector sel, void*p);
  long onCommand(FXObject*o, FXSelector sel, void*p);
  FXTextField *srchbox;
  const char*loaded;
  enum {
    ID_SCINTILLA=FXScintilla::ID_LAST,
    ID_GOBACK,
    ID_SEARCH,
    ID_ZOOMIN,
    ID_ZOOMOUT,
    ID_LAST
  };

};



FXDEFMAP(SciHelp) SciHelpMap[]={
  FXMAPFUNCS(SEL_COMMAND, SciHelp::ID_SCINTILLA,SciHelp::ID_ZOOMOUT, SciHelp::onCommand),
  FXMAPFUNC(SEL_LEFTBUTTONRELEASE, 0, SciHelp::onLeftBtnRelease)
};

FXIMPLEMENT(SciHelp,FXScintilla,SciHelpMap,ARRAYNUMBER(SciHelpMap))



SciHelp::SciHelp(FXComposite*p,FXObject*tgt,FXSelector sel, FXuint opts, bool dark):
  FXScintilla(p, this, ID_SCINTILLA, opts)
{
  links=NULL;
  sects=NULL;
  last_pos=0;
  jump_pos=-1;
  loaded=NULL;

  sendMessage(SCI_ASSIGNCMDKEY,SCK_HOME,SCI_DOCUMENTSTART);
  sendMessage(SCI_ASSIGNCMDKEY,SCK_END,SCI_DOCUMENTEND);

  long def_fg  = dark? FXRGB(250, 250, 250) : FXRGB( 80,  80,  80);
  long def_bg  = dark? FXRGB( 60,  60,  80) : FXRGB(250, 250, 250);

  long lnk_fg  = dark? FXRGB(128, 160, 255) : FXRGB(  0,   0, 192);
  long lnk_bg = def_bg;

  long hot_fg  = dark? FXRGB(224, 224,  80) : FXRGB(  0,   0, 255);
  long hot_bg  = dark? FXRGB(128,  80,   0) : FXRGB(255, 255, 128);

  long mno_fg  = dark? FXRGB(255, 144, 128) : FXRGB(128,   0,   0);
  long mno_bg=def_bg;

  long hdr_fg  = dark? FXRGB( 96,  96, 160) : FXRGB(228, 228, 192);
  long hdr_bg  = dark? FXRGB(192, 255, 192) : FXRGB(  0,   0,  96);

  long lgo_fg  = dark? FXRGB(255, 144, 128) : FXRGB(128,   0,   0);
  long lgo_bg=def_bg;

  sendMessage(SCI_SETENDATLASTLINE, false,0);
  sendMessage(SCI_SETWRAPMODE,SC_WRAP_WORD,0);
  sendMessage(SCI_SETMARGINWIDTHN,1,0);
  sendMessage(SCI_SETCARETSTYLE,CARETSTYLE_INVISIBLE,0);
  sendMessage(SCI_SETMARGINLEFT,0,4);
  sendMessage(SCI_SETMARGINRIGHT,0,4);
  sendMessage(SCI_SETHOTSPOTACTIVEFORE,true,hot_fg);
  sendMessage(SCI_SETHOTSPOTACTIVEBACK,true,hot_bg);
  sendMessage(SCI_STYLESETBACK,STYLE_DEFAULT,def_bg);

  for (FXint i=SCHLP_FIRST; i<=SCHLP_LAST; i++) {
    sendMessage(SCI_STYLESETSIZE, i, 10);
    sendString(SCI_STYLESETFONT, i, "Sans Serif");
    sendMessage(SCI_STYLESETFORE,i,def_fg);
    sendMessage(SCI_STYLESETBACK,i,def_bg);
    sendMessage(SCI_STYLESETEOLFILLED,i,true);
  }

  sendMessage(SCI_STYLESETHOTSPOT, SCHLP_LINK, true);
  sendMessage(SCI_STYLESETUNDERLINE, SCHLP_LINK, true);
  sendMessage(SCI_STYLESETFORE,SCHLP_LINK,lnk_fg);
  sendMessage(SCI_STYLESETBACK,SCHLP_LINK,lnk_bg);

  sendMessage(SCI_STYLESETSIZE, SCHLP_H1, 14);
  sendMessage(SCI_STYLESETBACK,SCHLP_H1,hdr_fg);
  sendMessage(SCI_STYLESETFORE,SCHLP_H1,hdr_bg);

  sendMessage(SCI_STYLESETBOLD,SCHLP_BOLD,true);
  sendMessage(SCI_STYLESETITALIC,SCHLP_ITALIC,true);

  sendString(SCI_STYLESETFONT, SCHLP_FIXED, "Courier");
  sendMessage(SCI_STYLESETSIZE, SCHLP_FIXED, 10);
  sendMessage(SCI_STYLESETFORE,SCHLP_FIXED,mno_fg);
  sendMessage(SCI_STYLESETBACK,SCHLP_FIXED,mno_bg);

  sendString(SCI_STYLESETFONT, SCHLP_LOGO, "Serif");
  sendMessage(SCI_STYLESETSIZE, SCHLP_LOGO, 10);
  sendMessage(SCI_STYLESETFORE,SCHLP_LOGO,lgo_fg);
  sendMessage(SCI_STYLESETBACK,SCHLP_LOGO,lgo_bg);

  sendMessage(SCI_STARTSTYLING,0,0xff);

  horizontalScrollBar()->hide();
  sendMessage(SCI_SETHSCROLLBAR,false,0);
  sendMessage(SCI_SETYCARETPOLICY,CARET_EVEN|CARET_STRICT,0);

}



void SciHelp::replace(const char*oldstr, const char*newstr)
{
  const char*content;
  FXint beg[3]={0,0,0};
  FXint end[3]={0,0,0};
  FXRex generic_rx(oldstr, REX_NORMAL|REX_NEWLINE);
  content=(const char*)(sendMessage(SCI_GETCHARACTERPOINTER,0,0));
  FXint n=strlen(newstr);
  while (generic_rx.match(content,beg,end,REX_FORWARD,1)) {
    sendMessage(SCI_SETTARGETSTART,beg[0],0);
    sendMessage(SCI_SETTARGETEND,end[0],0);
    sendString(SCI_REPLACETARGET,n,newstr);
    content=(const char*)(sendMessage(SCI_GETCHARACTERPOINTER,0,0));
  }
}



static char*my_strndup(const char*src,int len)
{
  char*dst=(char*)calloc(len+1,1);
  strncpy(dst,src,len);
  return dst;
}



void SciHelp::parse(const char*txt, unsigned int size)
{
  FXint beg[3]={0,0,0};
  FXint end[3]={0,0,0};
  loaded=txt;
  const char *content=NULL;
  getApp()->beginWaitCursor();
  freelists();
  sendMessage(SCI_SETREADONLY,false,0);
  sendMessage(SCI_CLEARALL,0,0);
  sendString(SCI_APPENDTEXT, size, txt);
  sendString(SCI_APPENDTEXT, 32,FXString('\n',32).text());
  sendMessage(SCI_SETSTYLING,sendMessage(SCI_GETLENGTH,0,0),SCHLP_NORMAL);
  sendMessage(SCI_STARTSTYLING,0,0xff);
  // Translate markup tags into scintilla styles...
  for (FXint i=SCHLP_FIRST; i<SCHLP_LAST; i++) {
    content=(const char*)(sendMessage(SCI_GETCHARACTERPOINTER,0,0));
    FXRex rx(phrases[i], REX_CAPTURE|REX_NEWLINE);
    while (rx.match(content,beg,end,REX_FORWARD,2)) {
      sendMessage(SCI_SETTARGETSTART,beg[0],0);
      sendMessage(SCI_SETTARGETEND,end[0],0);
      char*tmp=my_strndup(content+beg[1], end[1]-beg[1]);
      sendString(SCI_REPLACETARGET,end[1]-beg[1],tmp);
      if ((i==SCHLP_H1)||(i==SCHLP_LINK)) {
        HelpLink*link=(HelpLink*)calloc(1, sizeof(HelpLink));
        link->pos=0;
        link->href=tmp;
        if (i==SCHLP_H1) {
          sects=sl_push(sects,link);
        } else {
          links=sl_push(links,link);
        }
      } else {
        free(tmp);
      }
      sendMessage(SCI_STARTSTYLING,beg[0],0xff);
      sendMessage(SCI_SETSTYLING,(end[1]-beg[1])+((i==SCHLP_H1)?1:0),i);
      content=(const char*)(sendMessage(SCI_GETCHARACTERPOINTER,0,0));
    }
  }
  // replace generic placeholders with the real names...
  for (const char**c=replacements; *c; c+=2) {
    replace(*c,*(c+1));
  }

  // Make the editor's name stand out a little...
  FXRex appname_rx("\\<"APP_NAME"\\>", REX_NORMAL|REX_NEWLINE);
  content=(const char*)(sendMessage(SCI_GETCHARACTERPOINTER,0,0));
  FXint p=0;
  while (appname_rx.match(content,beg,end,REX_FORWARD,1, p)) {
    sendMessage(SCI_STARTSTYLING,beg[0],0xff);
    sendMessage(SCI_SETSTYLING,(end[0]-beg[0]),SCHLP_LOGO);
    p=end[0];
  }

  sendMessage(SCI_SETREADONLY,true,0);

  links=sl_reverse(links);
  sects=sl_reverse(sects);

  // Populate our list elements with their positions in the document...
  HelpLink*link=(HelpLink*)links;
  HelpLink*sect=(HelpLink*)sects;
  long len=sendMessage(SCI_GETLENGTH,0,0);

  for (long p1=0; p1<len;) {
    int style=sendMessage(SCI_GETSTYLEAT,p1,0);
    switch (style) {
      case SCHLP_H1: {
        sect->pos=p1;
        sect=sect->next;
        break;
      }
      case SCHLP_LINK: {
        link->pos=p1;
        link=link->next;
        break;
      }
    }
    while ( (sendMessage(SCI_GETSTYLEAT,p1,0)==style) && (p1<len) ) { p1++; }
  }
  getApp()->endWaitCursor();
}



static void free_link_cb(void *p) {
  if (p) {
    HelpLink*link=(HelpLink*)p;
    if (link->href) { delete link->href; }
    delete link;
  }
}



void SciHelp::freelists()
{
  if (links) {
    sl_free(links, free_link_cb);
    links=NULL;
  }
  if (sects) {
    sl_free(sects, free_link_cb);
    sects=NULL;
  }
  last_pos=0;
  jump_pos=-1;
}



SciHelp::~SciHelp()
{
  freelists();
}



static int lookup_link(void *rec, void *p)
{
  HelpLink*link=(HelpLink*)rec;
  if ( link && ( link->pos == *(int*)p) ) {
    return -1;
  } else {
    return 0;
  }
}



static int lookup_sect(void *rec, void *p)
{
  HelpLink*sect=(HelpLink*)rec;
  char*href=(char*)p;
  if ( sect && sect->href && ( strcmp(sect->href,href) == 0 ) ) {
    return -1;
  } else {
    return 0;
  }
}



long SciHelp::onLeftBtnRelease(FXObject*o, FXSelector sel, void*p)
{
  FXint prev_x;
  FXint prev_y;
  getPosition(prev_x,prev_y);

  long rv = FXScintilla::onLeftBtnRelease(o,sel,p);

  if (jump_pos>=0) {
    sendMessage(SCI_GOTOPOS,jump_pos,0);
    long th=sendMessage(SCI_TEXTHEIGHT,0,0);
#ifdef FOX_1_6
   setPosition(0,-((vertical->getPosition()+(getViewportHeight()/2))-th));
#else
    setPosition(0,-((vertical->getPosition()+(getVisibleHeight()/2))-th));
#endif
    last_pos=jump_pos;
    jump_pos=-1;
  } else {
    setPosition(prev_x,prev_y);
  }
  return rv;
}



long SciHelp::onCommand(FXObject*o, FXSelector sel, void*p)
{
  switch (FXSELID(sel)) {
    case ID_SCINTILLA: {
      SCNotification* scn = static_cast<SCNotification*>(p);
      if ((scn->nmhdr.code==SCN_HOTSPOTCLICK) && links && sects) {
        int pos=scn->position;
        while (sendMessage(SCI_GETSTYLEAT, pos,0)==SCHLP_LINK) { pos--; }
        pos++;
        HelpLink*link = (HelpLink*) sl_map(links, lookup_link, &pos);
        if (link && link->href) {
          HelpLink*sect = (HelpLink*) sl_map(sects, lookup_sect, link->href);
          if (sect) { jump_pos=sect->pos; }
        }
      }
      return 0;
    }
    case ID_GOBACK:{
      FXint x,y;
      getPosition(x,y);
      setPosition(0,last_pos);
      last_pos=y;
      return 1;
    }
    case ID_SEARCH: {
      if (srchbox->getText().length()) {
        find(srchbox->getText());
      } else {
        srchbox->setFocus();
      }
      return 1;
    }
    case ID_ZOOMIN:{
      sendMessage(SCI_ZOOMIN,0,0);
      return 1;
    }
    case ID_ZOOMOUT:{
      sendMessage(SCI_ZOOMOUT,0,0);
      return 1;
    }
  }
  return 0;
}



bool SciHelp::find(const FXString &what)
{
  long start=sendMessage(SCI_GETCURRENTPOS,0,0);
  long end=sendMessage(SCI_GETLENGTH,0,0);
  sendMessage(SCI_SETTARGETSTART,start,0);
  sendMessage(SCI_SETTARGETEND,end,0);
  long found=sendString(SCI_SEARCHINTARGET,what.length(), what.text());
  if (found==-1) {
    start=0;
    sendMessage(SCI_SETTARGETSTART,start,0);
    sendMessage(SCI_SETTARGETEND,end,0);
    found=sendString(SCI_SEARCHINTARGET,what.length(), what.text());
  }
  if (found>=0) {
    sendMessage(SCI_SETSEL, sendMessage(SCI_GETTARGETSTART,0,0), sendMessage(SCI_GETTARGETEND,0,0) );
    sendMessage(SCI_SCROLLCARET,0,0);
    return true;
  }
  return false;
}



#define PAD(w,p) \
  w->setPadTop(p); \
   w->setPadBottom(p); \
    w->setPadLeft(p); \
     w->setPadRight(p);


class HelpDialog;

static HelpDialog *instance = NULL;

#define HELP_DECOR DECOR_TITLE|DECOR_BORDER|DECOR_MINIMIZE|DECOR_MAXIMIZE|DECOR_CLOSE|DECOR_RESIZE
#define RAISED FRAME_RAISED|FRAME_THICK
#define TEXTFIELD_OPTS TEXTFIELD_NORMAL|TEXTFIELD_ENTER_ONLY


class HelpDialog: public FXDialogBox {
  FXDECLARE(HelpDialog)
  HelpDialog(){}
  SciHelp*sc;
public:
  HelpDialog(FXMainWindow*win, bool dark):FXDialogBox(win->getApp(),_(APP_NAME" Help"),HELP_DECOR) {
    FXint w=getApp()->getRootWindow()->getWidth();
    FXint h=getApp()->getRootWindow()->getHeight();
    setWidth( (FXint)( w>800 ? w*0.6875 : w*0.875 ) );
    setHeight( (FXint)( h>600 ? h*0.667 : h*0.75 ) );
    setX((w-getWidth())/2);
    setY((h-getHeight())/2);
    PAD(this,1);
    FXVerticalFrame *vbox=new FXVerticalFrame(this, LAYOUT_FILL|FRAME_NONE);
    PAD(vbox,0);
    vbox->setVSpacing(1);
    FXHorizontalFrame*scfrm=new FXHorizontalFrame(vbox, LAYOUT_FILL|FRAME_SUNKEN|FRAME_THICK);
    PAD(scfrm,0);
    sc=new SciHelp(scfrm, NULL,0, LAYOUT_FILL|HSCROLLER_NEVER,dark);
    setUserData(sc);
    FXHorizontalFrame *btns=new FXHorizontalFrame(vbox, LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|RAISED);
    new FXButton(btns,_(" &Back "), NULL, sc, SciHelp::ID_GOBACK, RAISED);
    new FXLabel(btns, "  ");
    sc->srchbox=new FXTextField(btns,24,sc, SciHelp::ID_SEARCH,TEXTFIELD_OPTS);
    new FXButton(btns,_(" &Find "), NULL, sc, SciHelp::ID_SEARCH, RAISED);
    btns=new FXHorizontalFrame(btns, LAYOUT_RIGHT|LAYOUT_FILL_X|FRAME_NONE,0,0,0,0, 16,8,0,0,0,0);
    new FXButton(btns," + ", NULL, sc, SciHelp::ID_ZOOMIN,  RAISED|LAYOUT_LEFT);
    new FXButton(btns," - ", NULL, sc, SciHelp::ID_ZOOMOUT, RAISED|LAYOUT_LEFT);
    new FXButton(btns,_(" &Close "), NULL, this, FXDialogBox::ID_CLOSE, RAISED|LAYOUT_RIGHT);
    setIcon(win->getIcon());
    changeFocus(sc->srchbox);
    create();
    SetupXAtoms(this, "help", APP_NAME);
    show();
    getApp()->runWhileEvents();
  }
  long onCmdClose(FXObject*o, FXSelector sel, void*p) {
    instance=NULL;
    return FXDialogBox::onCmdClose(o,sel,p);
  }
  void Load(FXint which) {
    sc->hide();
    const char*todo=NULL;
    unsigned int len=0;
    switch (which) {
      case 0: {
        todo=(const char*)helptext;
        len=sizeof(helptext);
        break;
      }
      case 1: {
        todo=(const char*)help_lua;
        len=sizeof(help_lua);
        break;
      }
    }
    if (sc->loaded!=todo) { sc->parse(todo,len); }
    sc->show();
    if (shown()) { hide(); }
    show(PLACEMENT_SCREEN);
  }
};



FXDEFMAP(HelpDialog) HelpDialogMap[] = {
  FXMAPFUNC(SEL_COMMAND, FXDialogBox::ID_CLOSE, HelpDialog::onCmdClose),
};


FXIMPLEMENT(HelpDialog,FXDialogBox,HelpDialogMap,ARRAYNUMBER(HelpDialogMap));



void show_help(FXMainWindow*win, FXint which, bool dark)
{
  if (!instance) { instance=new HelpDialog(win,dark); }
  instance->Load(which);
}

