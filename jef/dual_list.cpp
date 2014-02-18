/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2009-2013 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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
#include "intl.h"
#include "dual_list.h"


FXDEFMAP(DualListForm) DualListFormMap[]={
  FXMAPFUNC(SEL_COMMAND, DualListForm::ID_AVAIL_ITEMS, DualListForm::onHiliteItem),
  FXMAPFUNC(SEL_COMMAND, DualListForm::ID_USED_ITEMS,  DualListForm::onHiliteItem),
  FXMAPFUNC(SEL_COMMAND, DualListForm::ID_ITEM_RAISE,  DualListForm::onArrangeItem),
  FXMAPFUNC(SEL_COMMAND, DualListForm::ID_ITEM_LOWER,  DualListForm::onArrangeItem),
  FXMAPFUNC(SEL_COMMAND, DualListForm::ID_ITEM_REMOVE, DualListForm::onRemoveItem),
  FXMAPFUNC(SEL_COMMAND, DualListForm::ID_ITEM_INSERT, DualListForm::onInsertItem),
};

FXIMPLEMENT(DualListForm,FXHorizontalFrame,DualListFormMap,ARRAYNUMBER(DualListFormMap));



void DualListForm::NotifyChanged(void* cbdata)
{
  if (target&&message) { target->handle(this,FXSEL(SEL_CHANGED,message),cbdata); }
}



void DualListForm::CheckCount()
{
  avail_items->makeItemVisible(avail_items->getCurrentItem());
  if (max_items) {
    if (used_items->getNumItems()<max_items) {
      ins_btn->enable();
    } else {
      ins_btn->disable();
    }
  }
  if (used_items->getNumItems()>0) {
    used_items->makeItemVisible(used_items->getCurrentItem());
    rem_btn->enable();
  } else {
    rem_btn->disable();
  }
  if (used_items->getNumItems()>0) {
    avail_items->makeItemVisible(avail_items->getCurrentItem());
  }
  CheckIndex();
  NotifyChanged(udata);
}



void DualListForm::InsertItem(FXListItem*item)
{
  FXint iUsed=used_items->getCurrentItem();
  used_items->killSelection();
  if (iUsed<(used_items->getNumItems()-1)) {
    used_items->insertItem(iUsed+1,item);
  } else {
    used_items->appendItem(item);
  }
  iUsed++;
  used_items->selectItem(iUsed);
  used_items->setCurrentItem(iUsed);
  CheckCount();
}



long DualListForm::onHiliteItem(FXObject*o,FXSelector sel,void*p)
{
  if (FXSELID(sel)==ID_USED_ITEMS) { CheckIndex(); }
  return 1;
}



long DualListForm::onArrangeItem(FXObject*o,FXSelector sel,void*p)
{
  FXint iUsed=used_items->getCurrentItem();
  switch (FXSELID(sel)) {
    case ID_ITEM_RAISE: {
      used_items->moveItem(iUsed, iUsed-1);
      iUsed--;
      used_items->selectItem(iUsed);
      used_items->setCurrentItem(iUsed);
      break;
    }
    case ID_ITEM_LOWER: {
      used_items->moveItem(iUsed, iUsed+1);
      iUsed++;
      used_items->selectItem(iUsed);
      used_items->setCurrentItem(iUsed);
      break;
    }
  }
  CheckCount();
  return 1;
}



void DualListForm::CheckIndex()
{
  FXint iUsed=used_items->getCurrentItem();
  if (iUsed>0) { raise_btn->enable(); } else { raise_btn->disable(); }
  if (iUsed<(used_items->getNumItems()-1)) { lower_btn->enable(); } else { lower_btn->disable(); }
}



void DualListForm::create()
{
  PopulateAvail();
  for (FXint i=0; i<avail_items->getNumItems(); i++) { orig_avail.append(avail_items->getItem(i)); }
  PopulateUsed();
  if (avail_items->getNumItems()) { avail_items->selectItem(0); }
  if (used_items->getNumItems()) { used_items->selectItem(0); }
  onArrangeItem(NULL,0,NULL);
  FXHorizontalFrame::create();
}



void DualListForm::PopulateAvail()
{

}


void DualListForm::PopulateUsed()
{

}


class MyList: public FXList {
public:
  FXListItemList* getItems() { return &items; }
};


long DualListForm::onRemoveItem(FXObject*o,FXSelector sel,void*p)
{
  FXListItem *item=used_items->extractItem(used_items->getCurrentItem());
  if (orig_avail.find(item)>=0) {
    while (avail_items->getNumItems()) { avail_items->extractItem(0); }
    for (FXint i=0; i<orig_avail.no(); i++) {
      if ((((MyList*)used_items)->getItems())->find(orig_avail[i])<0) {
        avail_items->appendItem(orig_avail[i]);
      }
    }
    avail_items->killSelection();
    for (FXint i=0; i<avail_items->getNumItems(); i++) {
      if (avail_items->getItem(i)==item) {
        avail_items->setCurrentItem(i);
        avail_items->selectItem(i);
        break;
      }
    }
  }
  CheckCount();
  return 1;
}



long DualListForm::onInsertItem(FXObject*o,FXSelector sel,void*p)
{
  InsertItem(avail_items->extractItem(avail_items->getCurrentItem()));
  return 1;
}



DualListForm::DualListForm(FXComposite*p, FXObject* tgt, FXSelector sel, FXint max):FXHorizontalFrame(p,FRAME_RAISED|LAYOUT_FILL)
{
  left_column=new FXVerticalFrame(this,FRAME_SUNKEN|LAYOUT_FILL);
  mid_column=new FXVerticalFrame(this,FRAME_NONE|LAYOUT_CENTER_Y|PACK_UNIFORM_WIDTH);
  right_column=new FXVerticalFrame(this,FRAME_SUNKEN|LAYOUT_FILL);
  max_items=max;
  udata=NULL;
  target=tgt;
  message=sel;
  new FXLabel(left_column, _("&Available items:"));
  ins_btn=new FXButton(mid_column, _("&Insert>>"),NULL,this,ID_ITEM_INSERT,BUTTON_NORMAL|LAYOUT_CENTER_Y);
  rem_btn=new FXButton(mid_column, _("<<&Remove"),NULL,this,ID_ITEM_REMOVE,BUTTON_NORMAL|LAYOUT_CENTER_Y);
  new FXLabel(right_column, _("&Visible items:"));
  avail_items=new FXList(left_column,this,ID_AVAIL_ITEMS,LIST_BROWSESELECT|LAYOUT_FILL);
  used_items=new FXList(right_column,this,ID_USED_ITEMS,LIST_BROWSESELECT|LAYOUT_FILL);
  FXHorizontalFrame* UpDnBtns=new FXHorizontalFrame( right_column,
                                                     FRAME_RAISED|LAYOUT_FILL_X|LAYOUT_CENTER_X|PACK_UNIFORM_WIDTH);
  raise_btn=new FXButton(UpDnBtns, _("Move &Up"),  NULL,this, ID_ITEM_RAISE,BUTTON_NORMAL|LAYOUT_CENTER_X);
  lower_btn=new FXButton(UpDnBtns, _("Move &Down"),NULL,this, ID_ITEM_LOWER,BUTTON_NORMAL|LAYOUT_CENTER_X);
}
