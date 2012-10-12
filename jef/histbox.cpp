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
#include <fxkeys.h>
#include <FXUTF16Codec.h>
#include <FX88591Codec.h>

#include "popmenu.h"
#include "intl.h"

#include "histbox.h"

FXDEFMAP(MainWinWithClipBrd) MainWinWithClipBrdMap[]={
  FXMAPFUNC(SEL_CLIPBOARD_REQUEST, 0, MainWinWithClipBrd::onClipboardRequest),
};

FXIMPLEMENT(MainWinWithClipBrd,FXMainWindow,MainWinWithClipBrdMap,ARRAYNUMBER(MainWinWithClipBrdMap))


void MainWinWithClipBrd::SaveClipboard()
{
  if (getDNDData(FROM_CLIPBOARD,utf8Type,cliptext)) { // <= OK as is
  } else if (getDNDData(FROM_CLIPBOARD,utf16Type,cliptext)) {
    FXUTF16LECodec unicode;
    cliptext=unicode.mb2utf(cliptext);
  } else if (getDNDData(FROM_CLIPBOARD,stringType,cliptext)) {
    FX88591Codec ascii;
    cliptext=ascii.mb2utf(cliptext);
  } else { return; }
  FXDragType types[4]={stringType,textType,utf8Type,utf16Type};
  acquireClipboard(types,4);
}



long MainWinWithClipBrd::onClipboardRequest(FXObject*o, FXSelector sel, void*p)
{
  FXEvent *ev=(FXEvent*)p;
  FXDragType dt=ev->target;
  if (FXMainWindow::onClipboardRequest(o,sel,p)) { return 1; } // <= Base class handled it
  if ( dt==stringType || dt==textType || dt==utf8Type || dt==utf16Type) {
    FXString string=cliptext;
    if (dt==utf8Type) {
      setDNDData(FROM_CLIPBOARD,dt,string);
      return 1;
    }
    if (dt==stringType || dt==textType) {
      FX88591Codec ascii;
      setDNDData(FROM_CLIPBOARD,dt,ascii.utf2mb(string));
      return 1;
    }
    if (dt==utf16Type) {
      FXUTF16LECodec unicode;
      setDNDData(FROM_CLIPBOARD,dt,unicode.utf2mb(string));
      return 1;
    }
  }
  return 0;
}



FXDEFMAP(ClipTextField) ClipTextFieldMap[] = {
  FXMAPFUNC(SEL_RIGHTBUTTONPRESS,0,ClipTextField::onRightBtnPress),
};

FXIMPLEMENT(ClipTextField,FXTextField,ClipTextFieldMap,ARRAYNUMBER(ClipTextFieldMap));


long ClipTextField::onRightBtnPress(FXObject*o, FXSelector sel, void*p)
{
  FXTextField::onRightBtnPress(o,sel,p);
  FXEvent* ev=(FXEvent*)p;
  if (!ev->moved) {
    FXMenuPane *outpop=new FXMenuPane(this);
    new PopUpMnuCmd(outpop,_("Cu&t"),NULL,this,ID_CUT_SEL);
    new PopUpMnuCmd(outpop,_("&Copy"),NULL,this,ID_COPY_SEL);
    new PopUpMnuCmd(outpop,_("&Paste"),NULL,this,ID_PASTE_SEL);
    new FXMenuSeparator(outpop);
    new PopUpMnuCmd(outpop,_("Select &All"),NULL,this,ID_SELECT_ALL);
    outpop->create();
    outpop->popup(NULL,ev->root_x-4,ev->root_y-2);
    outpop->grabKeyboard();
    getApp()->runModalWhileShown(outpop);
    delete outpop;
  }
  return 1;
}



void ClipTextField::SurrenderClipboard() {
  if (!hasClipboard()) { return; }
  for (FXWindow*w=getShell()->getOwner(); w; w=w->getOwner()) {
    if (dynamic_cast<MainWinWithClipBrd*>(w)) {
      ((MainWinWithClipBrd*)w)->SaveClipboard();
      break;
    }
  }
}



void ClipTextField::destroy() {
  SurrenderClipboard();
}


ClipTextField::~ClipTextField() {
  SurrenderClipboard();
}



/*
  RegHistory maintains a list of text-based values, and it can also associate an integer value
  with each one, for example to associate a search term with a set of search flags.
  The current value of the external variable is saved to the registry whenever its associated
  string is stored there by the append() method. The external variable's value is also modified
  by the RegHistory object whenever the associated string is retrieved by the text() method.
  Storage of these integers in the registry is only enabled when the pointer is non-null.
*/

class RegHistory: public FXObject {
private:
  friend class HistoryTextField;
  typedef struct {
    FXchar *text;
    FXint mode;
  } Entry;
  Entry entries[64];
  FXchar* group;
  FXint numents;
  FXchar spfx; // prefix for strings
  FXchar ipfx; // prefix for modes
  FXint current;
  FXApp* app;
  FXint* modep; // If non-null, we read from this on append() and write to it on text()
  FXchar keybuf[8];
  const FXchar*name(FXchar pfx, FXchar key ) {
    memset(keybuf,0,sizeof(keybuf));
    snprintf(keybuf,sizeof(keybuf)-1,"%c%02d", pfx, key);
    return keybuf;
  }
  const FXchar*sname(FXchar key) { return name(spfx,key); }
  const FXchar*iname(FXchar key) { return name(ipfx,key); }
protected:
  void append(const FXchar* txt); // add a new item
  void older() { if ( ((current+1)<numents) && entries[current+1].text) { current++; } } // back-history
  void newer() { if (current>=0) current--; } // forward-history
  void start() { current=-1; } // always begin in the present.
  const FXchar* text(); // read from current item
  void setModeVar(FXint *modevar) { modep=modevar; }
  FXint*getModeVar() { return modep; }
public:
  RegHistory(FXApp *a, const char* groupname, FXchar text_prefix, FXchar mode_prefix='\0');
  virtual ~RegHistory();
};



RegHistory::~RegHistory()
{
  for (FXint i=0; (i<numents)&& entries[i].text; i++) {
    app->reg().writeStringEntry(group,sname(i),entries[i].text);
    free(entries[i].text);
    if (ipfx&&(entries[i].mode>=0)) {
      app->reg().writeIntEntry(group,iname(i),entries[i].mode);
    }
  }
  free(group);
}



void RegHistory::append(const FXchar* newtext)
{
  if ( !(newtext&&*newtext) ) { return; }
  memmove(&entries[1],&entries[0],(numents-1)*sizeof(Entry));
  entries[0].text=strdup(newtext);
  entries[0].mode=(ipfx && modep && (*modep>=0)) ? *modep : -1;
  for (FXint i=1; (i<numents) && entries[i].text; i++) {
    if (((strcmp(entries[i].text,entries[0].text))==0)&&(entries[i].mode==entries[0].mode)) {
      free(entries[i].text);
      memmove(&entries[i],&entries[i+1],(numents-i)*sizeof(Entry));
    }
  }
}



const FXchar*RegHistory::text()
{
  if (current>=0) {
    const FXchar*s=entries[current].text;
    if (!s) { s=""; }
    if (modep && ipfx && s && *s) { *modep=entries[current].mode; }
    return s;
  } else {
     if (modep) { *modep=0; }
     return "";
  }
}



RegHistory::RegHistory(FXApp*a, const char* groupname, FXchar text_prefix, FXchar mode_prefix)
{
  FXASSERT(groupname!=NULL);
  FXASSERT(a!=NULL);
  FXASSERT(mode_prefix!=text_prefix);
  FXASSERT((text_prefix>='A')&&(text_prefix<='Z'));
  FXASSERT( ((mode_prefix>='A')&&(mode_prefix<='Z')) || (mode_prefix=='\0') );
  modep=NULL;
  group=strdup(groupname);
  app=a;
  current=-1;
  spfx=text_prefix;
  ipfx=mode_prefix;
  memset(&entries, 0, sizeof(entries));
  numents=(sizeof(entries)/sizeof(Entry))-1;
  for (FXint i=0; i<numents; i++) {
    const FXchar*tmp=app->reg().readStringEntry(group,sname(i),NULL);
    if (tmp) {
      entries[i].text=strdup(tmp);
      app->reg().deleteEntry(group,sname(i));
      if (ipfx) {
        entries[i].mode=app->reg().readIntEntry(group,iname(i),-1);
        app->reg().deleteEntry(group,iname(i));
      }
    } else {
      break;
    }
  }
}



/*
  Browsing the history is done either using the up/down arrow keys within the edit field,
  or by pressing the up/down arrow buttons with the mouse, so there is never any reason
  for the arrow buttons to get keyboard focus, as this becomes quite confusing when
  trying to navigate the dialog using the TAB key. This subclass prevents the buttons
  from ever getting focused.
*/
class HistArrowBtn: public FXArrowButton {
  FXDECLARE(HistArrowBtn)
  HistArrowBtn(){}
public:
  HistArrowBtn(FXComposite *p,
    FXObject *tgt=NULL, FXSelector sel=0, FXuint opts=ARROW_NORMAL):FXArrowButton(p,tgt,sel,opts,0,0,16,0,1,1,1,1){}
  long onFocusSelf(FXObject*o,FXSelector sel,void*p);
};

FXDEFMAP(HistArrowBtn) HistArrowBtnMap[]={
  FXMAPFUNC(SEL_FOCUS_SELF,0,HistArrowBtn::onFocusSelf),
};

FXIMPLEMENT(HistArrowBtn,FXArrowButton,HistArrowBtnMap,ARRAYNUMBER(HistArrowBtnMap));


long HistArrowBtn::onFocusSelf(FXObject*o,FXSelector sel,void*p) {
   if ( o==this ) { // Message sent from self means the mouse did it, so let our text field have the focus...
     getParent()->getPrev()->setFocus();
   } else { // Message sent from another control indicates a Tab or Shift+Tab, determined by which button got it...
   if (options&ARROW_UP) { // Tabbing onto the UP arrow means we must be tabbing forward, so focus the next control...
     if(isEnabled() && canFocus()){ setFocus(); }
       for (FXWindow *child=getParent()->getParent()->getNext(); child; child=child->getNext()) {
         if(child->shown()){
           if(child->handle(this,FXSEL(SEL_FOCUS_SELF,0),p)) return 1;
           if(child->handle(this,FXSEL(SEL_FOCUS_NEXT,0),p)) return 1;
         }
      }
    } else { // Tabbing onto the DOWN arrow means we must be tabbing backward (Shift+Tab), so focus our text field...
      getParent()->getPrev()->setFocus();
    }
  }
  return 1;
}





FXIMPLEMENT(HistBox,FXInputDialog,NULL,0)

FXDEFMAP(HistoryTextField) HistoryTextFieldMap[]={
  FXMAPFUNC(SEL_KEYPRESS,HistoryTextField::ID_HIST_KEY,HistoryTextField::onKeyPress),
  FXMAPFUNCS(SEL_COMMAND,HistoryTextField::ID_HIST_UP,HistoryTextField::ID_HIST_DN,HistoryTextField::onHistory),
  FXMAPFUNC(SEL_CHANGED,HistoryTextField::ID_HIST_KEY,HistoryTextField::onChanged),
};


FXIMPLEMENT(HistoryTextField,ClipTextField,HistoryTextFieldMap,ARRAYNUMBER(HistoryTextFieldMap));



/*
  Synchronizes another HistoryTextField to browse as we browse,
  e.g. to sync history of "replace" phrases with "find" phrases.
*/
void HistoryTextField::enslave(HistoryTextField*enslaved)
{
  slave=enslaved;
}



long HistoryTextField::onHistory(FXObject*o,FXSelector sel,void*p)
{
  switch (FXSELID(sel)) {
    case ID_HIST_UP:{
      hist->older();
      if (slave) { slave->handle(NULL,sel,NULL); }
      break;
    }
    case ID_HIST_DN:{
      if (slave) { slave->handle(NULL,sel,NULL); }
      hist->newer();
      break;
    }
    default: {return 0;}
  }
  setText(hist->text());
  if (hframe->getTarget()&&hframe->getSelector()&&hist->getModeVar()) {
    hframe->getTarget()->tryHandle(this,FXSEL(SEL_PICKED, hframe->getSelector()),(void*)((FXival)*(hist->getModeVar())));
  }
  return 1;
}



long HistoryTextField::onChanged(FXObject*o,FXSelector sel,void*p)
{
  if (hframe->getTarget()&&hframe->getSelector()) {
    return hframe->getTarget()->tryHandle(this,FXSEL(SEL_CHANGED,hframe->getSelector()),p);
  } else { return 1; }
}



long HistoryTextField::onKeyPress(FXObject*o,FXSelector sel,void*p)
{
  FXEvent*ev=(FXEvent*)p;
  switch(ev->code) {
    case KEY_Up:
    case KEY_KP_Up: {
      onHistory(arUP,FXSEL(SEL_COMMAND,ID_HIST_UP),NULL);
      return 1;
    }
    case KEY_Down:
    case KEY_KP_Down: {
      onHistory(arDN,FXSEL(SEL_COMMAND,ID_HIST_DN),NULL);
      return 1;
    }
    case KEY_Return:
    case KEY_KP_Enter: {
      append();
      if (hframe->getTarget()&&hframe->getSelector()) {
        return hframe->getTarget()->tryHandle(this,FXSEL(SEL_COMMAND,hframe->getSelector()),p);
      } else { return 1; }
    }
    default:return 0;
  }
}



void HistoryTextField::append()
{
  hist->append(getText().text());
}



void HistoryTextField::setWatch(FXint *watch)
{
  hist->setModeVar(watch);
}



void HistoryTextField::setWatch(FXuint *watch)
{
  hist->setModeVar((FXint*)watch);
}



void HistoryTextField::start()
{
  hist->start();
}


void HistoryTextField::hide()
{
  hframe->hide();
}



void HistoryTextField::show()
{
  hframe->show();
}



HistoryTextField::HistoryTextField(FXComposite *p,
    FXint ncols, const FXString &regname, const FXchar prefixes[2], FXObject *tgt, FXSelector sel,
    FXuint opts, FXint x, FXint y, FXint w, FXint h, FXint pl, FXint pr, FXint pt, FXint pb
) : ClipTextField(p,ncols,this,ID_HIST_KEY,opts,x,y,w,h,pl,pr,pt,pb)
{
  slave=NULL;
  hist=new RegHistory(getApp(), regname.text(), prefixes[0], prefixes[0]?prefixes[1]:0);
  hframe= new FXHorizontalFrame(p,LAYOUT_TOP|LAYOUT_FILL_X,0,0,0,0, 0,0,0,0, 0,0);
  if (tgt&&sel) {
    hframe->setTarget(tgt);
    hframe->setSelector(sel);
  }
  reparent(hframe);
  searcharrows=new FXVerticalFrame(hframe,LAYOUT_RIGHT|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0, 0,0);
  FXuint arrowopts=FRAME_RAISED|FRAME_THICK|ARROW_REPEAT|LAYOUT_FILL_Y|LAYOUT_FIX_WIDTH;
  arUP=new HistArrowBtn(searcharrows,this,ID_HIST_UP,arrowopts|ARROW_UP);
  arDN=new HistArrowBtn(searcharrows,this,ID_HIST_DN,arrowopts|ARROW_DOWN);
  arUP->setArrowSize(3);
  arDN->setArrowSize(3);
}



HistoryTextField::~HistoryTextField()
{
  delete hist;
}



FXuint HistBox::execute(FXuint placement)
{
  ((HistoryTextField*)input)->start();
  FXuint rv=FXInputDialog::execute(placement);
  if (rv) {
    ((HistoryTextField*)input)->append();
  }
  return rv;
}



HistBox::HistBox(FXWindow* p,
  const FXString& caption, const FXString& label, const FXString &regname):FXInputDialog(p,caption,label)
{
  FXuint textopts=TEXTFIELD_ENTER_ONLY|FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X;
  FXComposite*c=(FXComposite*)input->getParent();
  delete input;
  input=new HistoryTextField(c,20,regname,"H",this,ID_ACCEPT,textopts,0,0,0,0, 8,8,4,4);
}




ClipTextDialog::ClipTextDialog(FXWindow*p,const FXString&caption,const FXString&label):
  FXInputDialog(p,caption,label)
{
  FXuint textopts=TEXTFIELD_ENTER_ONLY|FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X;
  FXComposite *entry=(FXComposite*)input->getParent();
  FXint n=input->getNumColumns();
  delete input;
  input=new ClipTextField(entry,n,this,ID_ACCEPT,textopts,0,0,0,0, 8,8,4,4);
}


