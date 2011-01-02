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


#include <unistd.h>
#include <fx.h>

#include "compat.h"
#include "appwin.h"

#include "intl.h"
#include "doctabs.h"


FXDEFMAP(DocTabs) DocTabsMap[]={
  FXMAPFUNC(SEL_COMMAND,DocTabs::ID_TAB_POPUP_MENU,DocTabs::onTabPopupMenu),
  FXMAPFUNC(SEL_COMMAND,DocTabs::ID_POPUP_CLICK,DocTabs::onPopupClick),
  FXMAPFUNC(SEL_COMMAND,DocTabs::ID_OPEN_ITEM,DocTabs::onCmdOpenItem),
};

FXIMPLEMENT(DocTabs,FXTabBook,DocTabsMap,ARRAYNUMBER(DocTabsMap));



long DocTabs::onPopupClick( FXObject* sender, FXSelector sel, void* p )
{
  DocTab*tab=(DocTab*)(((FXMenuCommand*)sender)->getParent()->getUserData());
  DocTab*curr=ActiveTab();
  if ( tab && ActivateTab(tab) ) {
    ((TopWindow*)getShell())->CloseFile(false,true);
  }
  if (curr && (tab!=curr)) { ActivateTab(curr); }
  return 1;
}



long DocTabs::onTabPopupMenu( FXObject* sender, FXSelector sel, void* p )
{
  FXEvent*ev=(FXEvent*)p;
  if (!ev->moved) {
    if (!tab_popup->shown()) { tab_popup->create(); }
    tab_popup->setUserData(sender);
    tab_popup->popup(NULL,ev->root_x-2,ev->root_y-2);
    getApp()->runModalWhileShown(tab_popup);
  }
  return 1;
}



void DocTabs::create()
{
  FXTabBook::create();
  bold_font->create();
}



void DocTabs::destroy()
{
  bold_font->destroy();
  FXTabBook::destroy();
}



DocTabs::DocTabs(FXComposite*p,FXObject*trg,FXSelector sel,FXuint opts):
  FXTabBook(p,trg,sel,opts,0,0,0,0,0,0,0,0)
{
  FXApp*a=getApp();
  normal_font=a->getNormalFont();
  GetFontDescription(desc,normal_font);
  desc.weight=FXFont::Bold;
  bold_font=new FXFont(a,desc);
  hot_fg=a->getSelforeColor();
  hot_bg=a->getSelbackColor();
  cold_fg=a->getForeColor();
  cold_bg=a->getBaseColor();
  tab_popup=new FXMenuPane(this);
  new FXMenuCommand(tab_popup,_("&Close"),NULL,this,ID_POPUP_CLICK);
}



DocTabs::~DocTabs()
{
  delete bold_font;
  delete tab_popup;
}



void DocTabs::setTabColor(DocTab*t, bool active)
{
  t->setBorderColor(active?hot_fg:cold_fg);
  t->setShadowColor(active?hot_fg:cold_fg);
  t->setHiliteColor(active?hot_fg:cold_fg);
  t->setTextColor(active?hot_fg:cold_fg);
  t->setBackColor(active?hot_bg:cold_bg);
  t->setFont(active?bold_font:normal_font);
}



void DocTabs::UpdateTabs()
{
  bool istab;
  FXWindow*w;
  for (w=getFirst(),istab=true; w; w=w->getNext(),istab=!istab) {
    if (istab) {
      setTabColor((DocTab*)w,false);
    }
  }
  if ((w=childAtIndex(getCurrent()*2))) {
    setTabColor((DocTab*)w,true);
    if ( (w=w->getNext()) && (w=w->getFirst()) ) { w->setFocus(); }
  }
}

void DocTabs::setCurrent(FXint i, FXbool notify)
{
  FXTabBook::setCurrent(i, notify);
  UpdateTabs();
}


class MySplitter: public FXSplitter {
  FXDECLARE(MySplitter);
  MySplitter(){}
protected:
  FXWindow *last_focused_child;
  virtual void changeFocus(FXWindow*child);
public:
  long onFocusIn(FXObject*o,FXSelector sel,void* p);
  long onFixFocus(FXObject*o,FXSelector sel,void* p);
  MySplitter(FXComposite *p, FXuint opts):FXSplitter(p,opts) { last_focused_child=NULL; }
  ~MySplitter() { getApp()->removeChore(this,ID_FIX_FOCUS); }
  FXWindow *LastFocusedChild() { return last_focused_child; }
  enum {
    ID_FIX_FOCUS=FXSplitter::ID_LAST,
    ID_LAST
  };
};



FXDEFMAP(MySplitter) MySplitterMap[] = {
  FXMAPFUNC(SEL_FOCUSIN,0,MySplitter::onFocusIn),
  FXMAPFUNC(SEL_CHORE,MySplitter::ID_FIX_FOCUS,MySplitter::onFixFocus)
};

FXIMPLEMENT(MySplitter,FXSplitter,MySplitterMap,ARRAYNUMBER(MySplitterMap))



void MySplitter::changeFocus(FXWindow*child)
{
  if (child) { last_focused_child=child; }
  FXSplitter::changeFocus(child);
}



long MySplitter::onFixFocus(FXObject*o,FXSelector sel,void* p)
{
  if (p) {
    ((FXWindow*)p)->setFocus();
  }
  return 1;
}



long MySplitter::onFocusIn(FXObject*o,FXSelector sel,void* p)
{
  long rv=FXSplitter::onFocusIn(o,sel,p);
  if ((numChildren()>1) && last_focused_child) {
    getApp()->addChore(this,ID_FIX_FOCUS,(void*)last_focused_child);
  }
  return rv;
}

FXWindow*DocTabs::ActiveView()
{
  FXWindow*w=ActivePage();
  return w?((MySplitter*)w)->LastFocusedChild():NULL;
  return NULL;
}

DocTab*DocTabs::NewTab(FXString text)
{
  DocTab*tab=new DocTab(this,text);
  MySplitter*frame=new MySplitter(this,FRAME_RAISED|LAYOUT_FILL|SPLITTER_VERTICAL);
  if (shown()) {
    tab->create();
    frame->create();
  }
  return tab;
}



long DocTabs::onCmdOpenItem(FXObject* sender, FXSelector sel, void* p)
{
  long rv=FXTabBook::onCmdOpenItem(sender,sel,p);
  UpdateTabs();
  return rv;
}



void DocTabs::MoveTab(FXint how){
  FXTabItem*tab=ActiveTab();
  if (!tab) { return; }
  FXWindow*page=tab->getNext();
  if (!page) { return; }
  FXint iCurr=getCurrent();
  switch (how) {
    case MOVETOLAST: {
      tab->reparent(this, NULL);
      page->reparent(this, NULL);
      setCurrent((numChildren()/2)-1);
      break;
    }
    case MOVETOFIRST: {
      page->reparent(this, getFirst());
      tab->reparent(this, page);
      setCurrent(0);
      break;
    }
    case MOVEUP: {
      FXWindow*prv=tab->getPrev();
      if (!prv) {return;}
      prv=prv->getPrev();
      page->reparent(this, prv);
      tab->reparent(this, page);
      setCurrent(iCurr-1);
      break;
    }
    case MOVEDOWN: {
      FXWindow*nxt=page->getNext(); /* is tab */
      if (!nxt) {return;}
      nxt=nxt->getNext();/* is page */
      nxt=nxt->getNext();/* is tab, or NULL */
      page->reparent(this, nxt);
      tab->reparent(this, page);
      setCurrent(iCurr+1);
      break;
    }
  }
}



bool DocTabs::ActivateTab(DocTab*tab)
{

  FXint i=indexOfChild(tab);
  if (i>=0) {
    setCurrent(i/2,true);
    return true;
  } else {
    return false;
  }

}



bool DocTabs::ActivateTab(FXint n)
{
  FXWindow*w=childAtIndex((n*2)+1);
  if (w) {
    setCurrent(n,true);
    return true;
  } else {
    return false;
  }
}



DocTab*DocTabs::ActiveTab() {
  return (DocTab*)childAtIndex(getCurrent()*2);
}



FXWindow*DocTabs::ActivePage() {
  FXWindow*page=childAtIndex((getCurrent()*2)+1);
  if (page) {
    return page;
  } else {
    layout();
    return childAtIndex((getCurrent()*2)+1);
  }
}



/* ForEachTab walks through the open tabs, and passes the tab index, the tab item object,
   and the page item object to the callback, along with the user data. The callback should
   return true to continue iterating, or false to break out of the loop. By default, the
   "wait" cursor is displayed during execution, unless you call it with hourglass=false.
*/
void DocTabs::ForEachTab(TabCallback cb, void *user_data, bool hourglass)
{
  FXWindow*tab,*page;
  FXint index=0;
  update();
  if ( hourglass ) {
    getApp()->beginWaitCursor();
  }
  for (tab=getFirst(); tab && (page=tab->getNext()); tab=page->getNext(), index++) {
    if (!cb(index,(DocTab*)tab,page,user_data)) { break; }
  }
  if ( hourglass ) { getApp()->endWaitCursor(); }
}



FXWindow*DocTabs::PageAt(FXint n)
{
  return childAtIndex((n*2)+1);
}


bool OrientTabsCB(FXint index, DocTab*tab, FXWindow*page, void*user_data)
{
  FXint ornt=*((FXint*)user_data);
  tab->setTabOrientation(ornt);
  tab->setDefaultDragCursor(
  tab->getApp()->getDefaultCursor(((ornt==TAB_TOP)||(ornt==TAB_BOTTOM)) ? DEF_HSPLIT_CURSOR : DEF_VSPLIT_CURSOR));
  return true;
}



void DocTabs::setTabStyle(FXuint style)
{
  FXTabBook::setTabStyle(style);
  FXint TabOrientation=TAB_TOP;
  switch(style){
    case TABBOOK_TOPTABS:
      TabOrientation=TAB_TOP;
      break;
    case TABBOOK_BOTTOMTABS:
      TabOrientation=TAB_BOTTOM;
      break;
    case TABBOOK_LEFTTABS:
      TabOrientation=TAB_LEFT;
      break;
    case TABBOOK_RIGHTTABS:
      TabOrientation=TAB_RIGHT;
      break;
  }
  ForEachTab(OrientTabsCB, &TabOrientation, false);
}


// Set tab header widths based on largest tab label string, or not
void DocTabs::setTabsCompact(bool compact)
{
  FXuint packing_hints = getPackingHints();
  if ( compact ) {
    packing_hints &= ~PACK_UNIFORM_WIDTH;
  } else {
    packing_hints |= PACK_UNIFORM_WIDTH;
  }
  setPackingHints(packing_hints);
}


// If forward is TRUE, activate next tab, else activate previous tab.
// Behavior is circular - that is, last->next==first; first->prev==last;.
void DocTabs::FocusNextTab(bool forward)
{
  int ntabs=numChildren()/2;
  int itab=getCurrent();
  if (forward) {
    if ((itab+1)==ntabs) { itab=0; } else { itab++; }
  } else {
    if (itab==0) { itab=ntabs-1; } else { itab--; }
  }
  setCurrent(itab);
}








#define TAB_DND_NAME "FxteDnDTab"
static FXDragType FxteDnDTabType=0;



FXDEFMAP(DocTab) DocTabMap[]={
  FXMAPFUNC(SEL_DND_ENTER, 0, DocTab::onDnd),
  FXMAPFUNC(SEL_DND_LEAVE, 0, DocTab::onDnd),
  FXMAPFUNC(SEL_DND_DROP, 0, DocTab::onDnd),
  FXMAPFUNC(SEL_DND_MOTION, 0, DocTab::onDnd),
  FXMAPFUNC(SEL_DND_REQUEST, 0, DocTab::onDnd),
  FXMAPFUNC(SEL_BEGINDRAG, 0, DocTab::onDnd),
  FXMAPFUNC(SEL_ENDDRAG, 0, DocTab::onDnd),
  FXMAPFUNC(SEL_LEFTBUTTONPRESS, 0, DocTab::onDnd),
  FXMAPFUNC(SEL_LEFTBUTTONRELEASE, 0, DocTab::onDnd),
  FXMAPFUNC(SEL_DRAGGED, 0, DocTab::onDnd),
  FXMAPFUNC(SEL_MOTION, 0, DocTab::onDnd),
  FXMAPFUNC(SEL_RIGHTBUTTONRELEASE, 0, DocTab::onDnd)
};

FXIMPLEMENT(DocTab,FXTabItem,DocTabMap,ARRAYNUMBER(DocTabMap));





long DocTab::onDnd(FXObject* sender,FXSelector sel, void*p)
{
  FXEvent* ev=(FXEvent*)p;
  switch(FXSELTYPE(sel)) {
    case SEL_DND_ENTER:  {
      setDragRectangle(0,0,width,height,FALSE);
      if (offeredDNDType(FROM_DRAGNDROP,FxteDnDTabType)) {
        acceptDrop();
        dnd_accept=true;
      } else {
        dnd_accept=false;
      }
      break;
    }
    case SEL_DND_LEAVE:  {
      dnd_accept=false;
      break;
    }
    case SEL_DND_DROP:   {
      dnd_accept=false;
      FXuchar*dnd_data=NULL;
      FXuint size=0;
      if (getDNDData(FROM_DRAGNDROP,FxteDnDTabType,dnd_data,size)) {
        if (dnd_data)  {
          dnd_accept=false;
          DocTab*src;
          FXint pid;
          sscanf((const char*)dnd_data, "%p,%d", &src, &pid);
          FXFREE(&dnd_data);
          if ((pid==fxgetpid())&&(src!=this)) {
            FXWindow *trg;
            if (defaultDragCursor==getApp()->getDefaultCursor(DEF_HSPLIT_CURSOR)) {
              trg=((src->getX()+ev->win_x)-getX())>(getWidth()/2)?this->getNext()->getNext():this;
            } else {
              trg=((src->getY()+ev->win_y)-getY())>(getHeight()/2)?this->getNext()->getNext():this;
            }
            FXWindow*doc=src->getNext();
            doc->reparent(getParent(), trg);
            src->reparent(getParent(), doc);
            ((DocTabs*)getParent())->ActivateTab(src);
          }
        }
      }
      break;
    }
    case SEL_DND_MOTION: {
      if (dnd_accept) { acceptDrop(); }
      break;
    }


    case SEL_LEFTBUTTONPRESS: {
      grab();
      flags|=FLAG_TRYDRAG;
      break;
    }


    case SEL_MOTION: {
      if ((flags&FLAG_DODRAG)) {
        handle(this,FXSEL(SEL_DRAGGED,0),p);
        return 1;
      }
      if ((flags&FLAG_TRYDRAG) && (ev->moved) ) {
        flags&=~FLAG_TRYDRAG;
        if (handle(this,FXSEL(SEL_BEGINDRAG,0),p)) {
          flags|=FLAG_DODRAG;
        }
        return 1;
      }
      return 0;
    }
    case SEL_LEFTBUTTONRELEASE: {
      if (flags&FLAG_DODRAG) {
        handle(this,FXSEL(SEL_ENDDRAG,0),p);
      } else {
        if (((DocTabs*)getParent())->ActiveTab()!=this) { ((DocTabs*)getParent())->ActivateTab(this); }
      }
      flags&=~(FLAG_DODRAG|FLAG_TRYDRAG);
      ungrab();
      break;
    }
    case SEL_BEGINDRAG:{
      beginDrag(&FxteDnDTabType,1);
      break;
    }
    case SEL_DRAGGED: {
      handleDrag(ev->root_x,ev->root_y,DRAG_MOVE);
      if (didAccept()!=DRAG_REJECT) {
        setDragCursor(defaultDragCursor);
      } else {
        setDragCursor(getApp()->getDefaultCursor(DEF_DNDSTOP_CURSOR));
      }
      break;
    }
    case SEL_ENDDRAG:{
      endDrag();
      setDragCursor(getApp()->getDefaultCursor(DEF_ARROW_CURSOR));
      break;
    }
    case SEL_DND_REQUEST: {
      FXString fmt="";
      fmt.format("%p,%d", this, fxgetpid());
      FXint len=fmt.length()+1;
      char*dnd_data=NULL;
      FXMALLOC(&dnd_data,char,len);
      strncpy(dnd_data,fmt.text(), len);
      dnd_data[len-1]='\0';
      setDNDData(FROM_DRAGNDROP,FxteDnDTabType,(FXuchar*)dnd_data,len);
    }
    case SEL_RIGHTBUTTONRELEASE: {
      return ((DocTabs*)getParent())->onTabPopupMenu(this, sel,p);
    }
    default: return 0;
  }
  return 1;
}



DocTab::DocTab(FXTabBar*bar, const FXString&text):FXTabItem(bar,text)
{
  FXApp*a=bar->getApp();
  if (!FxteDnDTabType) {
    FXString DnDName;
    DnDName.format("%s_%d", TAB_DND_NAME, fxgetpid());
    FxteDnDTabType=a->registerDragType(DnDName);
  }
  setDefaultDragCursor(a->getDefaultCursor(
  ((bar->getTabStyle()==TABBOOK_TOPTABS)||(bar->getTabStyle()==TABBOOK_BOTTOMTABS))?
    DEF_HSPLIT_CURSOR:DEF_VSPLIT_CURSOR
  ));
  setDragCursor(getApp()->getDefaultCursor(DEF_ARROW_CURSOR));
  dropEnable();
}


