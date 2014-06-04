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

#include <fx.h>
#include <fxkeys.h>

#include "cfg_shortcut.h"

#include "intl.h"
#include "cfg_keybind.h"


class MyIconList: public FXIconList {
protected:
  FXDECLARE(MyIconList);
  MyIconList(){}
public:
  MyIconList(ShortcutList*o,FXSelector sel,FXuint opts):FXIconList(o,o,sel,opts) {}
  long onQueryTip(FXObject*o,FXSelector sel,void*p) {
    if (getParent()->handle(o,sel,this)) {
      return 1;
    } else {
      return FXIconList::onQueryTip(o,sel,p);
    }
  }
};

FXDEFMAP(MyIconList) MyIconListMap[] = {
  FXMAPFUNC(SEL_QUERY_TIP,0,MyIconList::onQueryTip),
};

FXIMPLEMENT(MyIconList,FXIconList,MyIconListMap,ARRAYNUMBER(MyIconListMap))



FXDEFMAP(ShortcutList) ShortcutListMap[]={
  FXMAPFUNC(SEL_CHANGED, ShortcutList::ID_SEL_KEYBIND, ShortcutList::onSelectKeybinding),
  FXMAPFUNC(SEL_CHANGED, ShortcutList::ID_EDIT_KEYBIND, ShortcutList::onEditKeybinding),
  FXMAPFUNC(SEL_COMMAND, ShortcutList::ID_APPLY_CHANGES, ShortcutList::onApplyChanges),
  FXMAPFUNC(SEL_COMMAND, ShortcutList::ID_REMOVE_KEYBIND, ShortcutList::onRemoveKeybinding),
  FXMAPFUNC(SEL_QUERY_TIP, 0, ShortcutList::onQueryTip),
};
FXIMPLEMENT(ShortcutList,FXHorizontalFrame,ShortcutListMap,ARRAYNUMBER(ShortcutListMap));



bool ShortcutList::ConfirmOverwrite(FXHotKey acckey, MenuSpec*spec)
{
  return sce->ConfirmOverwrite(win->getAccelTable());
}



long ShortcutList::onQueryTip(FXObject*o,FXSelector sel,void*p)
{
  MyIconList*w=dynamic_cast<MyIconList*>((FXObject*)p);
  if (w) {
    FXint index,cx,cy;
    FXuint btns;
    if (w->getCursorPosition(cx,cy,btns) && (index=w->getItemAt(cx,cy))>=0) {
      MenuSpec*spec=(MenuSpec*)(w->getItem(index)->getData());
      if (spec) {
        FXString tip=FXString::null;
        mnumgr->GetTBarBtnTip(spec,tip);
        o->handle(this,FXSEL(SEL_COMMAND,ID_SETSTRINGVALUE),(void*)&tip);
        return 1;
      }
    }
  }
  return 0;
}



bool ShortcutList::DeleteShortcut(MenuSpec*spec)
{
  FXHotKey acckey=parseAccel(spec->accel);
  FXAccelTable*table=win->getAccelTable();
  if (!table->hasAccel(acckey)) {
    return true;
  } else {
    if (FXMessageBox::question(getShell(), MBOX_YES_NO, _("Confirm delete"),
      _("Are you sure you want to remove this keybinding?")
    )==MBOX_CLICKED_YES) {
      table->removeAccel(acckey);
      memset(spec->accel,0,sizeof(spec->accel));
      UpdateListItem(spec);
      return true;
    } else {
      return false;
    }
  }
}



void ShortcutList::UpdateListItem(MenuSpec*spec)
{
  FXint n=acclist->findItemByData(spec);
  if (n>=0) {
    FXString txt;
    txt.format("%s\t%s", spec->pref, spec->accel);
    acclist->setItemText(n, txt);
    handle(acclist,FXSEL(SEL_CHANGED,ID_SEL_KEYBIND),(void*)(FXival)n);
  } 
}



void ShortcutList::ApplyShortcut(FXHotKey acckey, MenuSpec*spec)
{
  FXAccelTable*table=win->getAccelTable();
  FXString accel=unparseAccel(acckey);
  if (spec->accel[0]) {
    FXHotKey old=parseAccel(spec->accel);
    if (table->hasAccel(old)) table->removeAccel(old);
  }
  mnumgr->SetAccelerator(spec,accel);
  if (spec->ms_mc) {
    spec->ms_mc->setAccelText(spec->accel);
    table->addAccel(acckey,spec->ms_mc->getTarget(),FXSEL(SEL_COMMAND,spec->sel));
  } else {
    table->addAccel(acckey,win,FXSEL(SEL_COMMAND,spec->sel));
  }
  UpdateListItem(spec);
}



long ShortcutList::onRemoveKeybinding(FXObject*o,FXSelector sel,void*p)
{
  FXIconItem*item=acclist->getItem(acclist->getCurrentItem());
  MenuSpec*spec=(MenuSpec*)item->getData();
  if (spec->accel[0] && DeleteShortcut(spec)) { apply_btn->disable(); }
  return 1;
}



bool ShortcutList::Verify(FXHotKey acckey, MenuSpec*spec)
{
  return sce->verify();
}



long ShortcutList::onApplyChanges(FXObject*o,FXSelector sel,void*p)
{
  FXHotKey hk=sce->getChord();
  FXIconItem*item=acclist->getItem(acclist->getCurrentItem());
  MenuSpec*spec=(MenuSpec*)item->getData();
  if (hk) {
    if (Verify(hk,spec) && ConfirmOverwrite(hk,spec)) {
      ApplyShortcut(hk,spec);
    }
  } else {
    if (DeleteShortcut(spec)) { apply_btn->disable(); }
  }
  return 1;
}



#define GetCaption(p) ((FXMenuCaption*)((p)->getParent()->getUserData()))

long ShortcutList::onSelectKeybinding(FXObject*o, FXSelector sel, void*p)
{
  apply_btn->disable();
  FXIconItem*item=acclist->getItem((FXint)(FXival)p);
  if (item) {
    MenuSpec*spec=(MenuSpec*)item->getData();
    sce->setShortcut(spec->accel[0]?spec->accel:FXString::null);
    if (spec->accel[0]) { remove_btn->enable(); } else { remove_btn->disable(); }
    FXWindow*w;
    for (w=menupath->getFirst(); w; w=w->getNext()) {
      ((FXLabel*)w)->setText(" ");
      ((FXLabel*)w)->setBackColor(getApp()->getBackColor());
    }
    FXMenuCaption*cpn=NULL;
    FXint i=-1;
    for (cpn=spec->ms_mc; cpn!=NULL; cpn=GetCaption(cpn)) { i++; }
    for (cpn=spec->ms_mc; cpn!=NULL; cpn=GetCaption(cpn)) {
      w=menupath->childAtIndex(i--);
      ((FXLabel*)w)->setText(cpn->getText());
      ((FXLabel*)w)->setBackColor(getApp()->getBaseColor());
      ((FXLabel*)w)->setIcon((cpn==spec->ms_mc)?NULL:arrow);
      
    }
  }
  return 0;
}



long ShortcutList::onEditKeybinding(FXObject*o, FXSelector sel, void*p)
{
  apply_btn->enable();
  return 1;
}



class MyLabel: public FXLabel {
  FXDECLARE(MyLabel)
  MyLabel() {}
protected:
public:
  MyLabel(FXComposite*p):FXLabel(p," ",NULL,LABEL_NORMAL|LAYOUT_FIX_X) {
    setIconPosition(ICON_AFTER_TEXT);
  }
  long onPaint(FXObject*o, FXSelector sel, void*p) {
    FXLabel::onPaint(o,sel,p);
    if (icon) {
      FXDCWindow dc(icon);
      dc.setForeground(getTextColor());
      FXPoint points[3]={FXPoint(1,1),FXPoint(11,6),FXPoint(1,11)};
      icon->fill(getBaseColor());
      dc.fillPolygon(points,3);
    }
    return 1;
  }
};
FXDEFMAP(MyLabel) MyLabelMap[]={
  FXMAPFUNC(SEL_PAINT, 0, MyLabel::onPaint)
};
FXIMPLEMENT(MyLabel,FXLabel,MyLabelMap,ARRAYNUMBER(MyLabelMap));



static const char * arrow_xpm[] = {
"16 16 2 1",
" 	c #FFFFFF",
".	c #000000",
"                ",
"                ",
"                ",
"                ",
"   ...          ",
"    ....        ",
"     .....      ",
"............    ",
"............    ",
"     .....      ",
"    ....        ",
"   ...          ",
"                ",
"                ",
"                ",
"                "};



void ShortcutList::create()
{
  FXHorizontalFrame::create();
  arrow=new FXXPMIcon(getApp(),arrow_xpm,0,IMAGE_OWNED|IMAGE_KEEP);
  FXLabel*label=(FXLabel*)(menupath->getFirst());
  FXColor bg=label->getBaseColor();
  FXColor fg=label->getTextColor();
  for (FXint x=0; x<arrow->getWidth(); x++) {
    for (FXint y=0; y<arrow->getHeight(); y++) {
      if (arrow->getPixel(x,y)==FXRGB(0,0,0)) { 
        arrow->setPixel(x,y,fg);
      } else {
        arrow->setPixel(x,y,bg);
      }
    }
  }
  arrow->create();
  arrow->render();
  
#ifdef FOX_1_6
  acclist->handle(acclist,FXSEL(SEL_CLICKED,acclist->ID_HEADER_CHANGE),0);
#else
  acclist->handle(acclist,FXSEL(SEL_CLICKED,acclist->ID_HEADER),0);
#endif
  acclist->setHeaderSize(0,acclist->getHeaderSize(0)+4);
  acclist->setHeaderSize(1,acclist->getFont()->getTextWidth("Ctrl+Alt+Shift+Return")+2);
  acclist->setWidth(acclist->getHeaderSize(0)+acclist->getHeaderSize(1)+16);
  handle(acclist,FXSEL(SEL_CHANGED,ID_SEL_KEYBIND),NULL);
}

ShortcutList::~ShortcutList()
{
  delete arrow;
}

ShortcutList::ShortcutList(FXComposite*o, MenuMgr*mmgr, FXWindow*w, FXuint opts):FXHorizontalFrame(o,opts)
{
  mnumgr=mmgr;
  win=w;
  acclist=new MyIconList(this,ID_SEL_KEYBIND,LAYOUT_FILL_Y|LAYOUT_FIX_WIDTH|ICONLIST_BROWSESELECT);
  acclist->appendHeader(_("Action"));
  acclist->appendHeader(_("Keybinding"));
  for (MenuSpec*spec=mnumgr->MenuSpecs(); spec->sel!=mnumgr->LastID(); spec++) {
    FXString txt;
    txt.format("%s\t%s", spec->pref, spec->accel);
    acclist->appendItem(new FXIconItem(txt, NULL, NULL, (void*)spec));
  }
  acclist->selectItem(0);
  FXVerticalFrame *vframe=new FXVerticalFrame(this,FRAME_NONE);
  FXGroupBox*grp=new FXGroupBox(vframe,_("Menu Path:"),GROUPBOX_NORMAL|FRAME_RIDGE|LAYOUT_FILL_X);
  menupath=new FXVerticalFrame(grp,FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X);
  menupath->setBackColor(getApp()->getBackColor());
  menupath->setVSpacing(0);
  for (FXuint i=0; i<8; i++) {
    FXLabel*label=new FXLabel(menupath," ",NULL,LABEL_NORMAL|LAYOUT_FIX_X);
    label->setIconPosition(ICON_AFTER_TEXT);
    label->setX((i+1)*4);
  }
  sce=new ShortcutEditor(vframe,this,ID_EDIT_KEYBIND);
  sce->setText(_("Shortcut:"));
  FXHorizontalFrame*btns=new FXHorizontalFrame(vframe,PACK_UNIFORM_WIDTH);
  apply_btn=new FXButton(btns,_("Apply"),NULL,this,ID_APPLY_CHANGES, BUTTON_NORMAL|LAYOUT_CENTER_X);
  remove_btn=new FXButton(btns,_("Remove"),NULL,this,ID_REMOVE_KEYBIND, BUTTON_NORMAL|LAYOUT_CENTER_X);
}

