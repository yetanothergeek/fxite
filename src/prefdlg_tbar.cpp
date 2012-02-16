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
#include "prefs.h"
#include "menuspec.h"
#include "appwin.h"
#include "tooltree.h"

#include "intl.h"
#include "prefdlg_tbar.h"



// Subclass FXListItem to show custom tooltip...
class TBarListItem: public FXListItem {
public:
  TBarListItem(const FXString &text, FXIcon *ic=NULL, void *ptr=NULL):FXListItem(text,ic,ptr){ }
  virtual FXString getTipText() const {
    FXString tip;
    MenuSpec*spec=(MenuSpec*)getData();
    if (spec) { MenuMgr::GetTBarBtnTip(spec,tip); } else { tip=label.text(); }
    return tip;
  }
};




FXDEFMAP(ToolbarPrefs) ToolbarPrefsMap[]={
  FXMAPFUNC(SEL_COMMAND,ToolbarPrefs::ID_ITEM_REMOVE,ToolbarPrefs::onRemoveItem),
  FXMAPFUNC(SEL_COMMAND,ToolbarPrefs::ID_INSERT_CUSTOM,ToolbarPrefs::onInsertCustomItem),
  FXMAPFUNC(SEL_COMMAND,ToolbarPrefs::ID_CHANGE_BTN_SIZE,ToolbarPrefs::onChangeBtnSize),
  FXMAPFUNC(SEL_COMMAND,ToolbarPrefs::ID_CHANGE_BTN_WRAP,ToolbarPrefs::onChangeBtnWrap),
};

FXIMPLEMENT(ToolbarPrefs,DualListForm,ToolbarPrefsMap,ARRAYNUMBER(ToolbarPrefsMap));



ToolbarPrefs::ToolbarPrefs(FXComposite*p, FXObject* tgt, FXSelector sel):
  DualListForm(p,tgt,sel,TBAR_MAX_BTNS)
{
  udata = (void*)((FXival)ToolbarChangedLayout);
  prefs=Settings::instance();
  FXHorizontalFrame* AvailBtns=new FXHorizontalFrame( left_column,
                                                     FRAME_RAISED|LAYOUT_FILL_X|LAYOUT_CENTER_X|PACK_UNIFORM_WIDTH);
  custom_btn=new FXButton( AvailBtns, _("Custom &Tools..."),
                                NULL,this, ID_INSERT_CUSTOM,BUTTON_NORMAL|LAYOUT_CENTER_X);

  FXLabel*btn_size_cpn=new FXLabel(mid_column, _("Button size:"), NULL, LABEL_NORMAL|JUSTIFY_LEFT);
  btn_size_cpn->setPadTop(48);
  FXListBox *size_list=new FXListBox(mid_column,this,ID_CHANGE_BTN_SIZE);
  size_list->appendItem(_("small"),  NULL,NULL);
  size_list->appendItem(_("medium"), NULL,NULL);
  size_list->appendItem(_("large"),  NULL,NULL);
  size_list->setNumVisible(3);
  size_list->setCurrentItem(prefs->ToolbarButtonSize);

  FXCheckButton*wrap_tbar_chk=new FXCheckButton(mid_column,_("Wrap buttons"),this,ID_CHANGE_BTN_WRAP);
  wrap_tbar_chk->setCheck(prefs->WrapToolbar);
}


void ToolbarPrefs::NotifyChanged(FXuint what)
{
  DualListForm::NotifyChanged((void*)((FXival)what));
}



long ToolbarPrefs::onChangeBtnSize(FXObject*o,FXSelector sel,void*p)
{
  prefs->handle(o,FXSEL(SEL_COMMAND,Settings::ID_SET_TOOLBAR_BTN_SIZE),p);
  NotifyChanged(ToolbarChangedFont);
  return 1;
}



long ToolbarPrefs::onChangeBtnWrap(FXObject*o,FXSelector sel,void*p)
{
  prefs->handle(o,FXSEL(SEL_COMMAND,Settings::ID_TOGGLE_WRAP_TOOLBAR),p);
  NotifyChanged(ToolbarChangedWrap);
  return 1;
}



long ToolbarPrefs::onRemoveItem(FXObject*o,FXSelector sel,void*p)
{
  FXint iUsed=used_items->getCurrentItem();
  MenuSpec*spec=(MenuSpec*)used_items->getItemData(iUsed);
  if (spec->type=='u') {
    MenuMgr::RemoveTBarUsrCmd(spec);
    used_items->removeItem(iUsed);
    CheckCount();
  } else {
    DualListForm::onRemoveItem(o,sel,p);
  }
  return 1;
}



long ToolbarPrefs::onInsertCustomItem(FXObject*o,FXSelector sel,void*p)
{
  FXMenuCommand*mc;
  if (ToolsTree::SelectTool(this, TopWindow::instance()->UserMenus(), mc)) {
    const char*newpath=(const char*)mc->getUserData();
    if (newpath) {
      // If the command is already in the used items list, just select it...
      for (FXint i=0; i<used_items->getNumItems(); i++) {
        const char*oldpath=MenuMgr::GetUsrCmdPath((MenuSpec*)used_items->getItemData(i));
        if (oldpath && (strcmp(newpath, oldpath)==0)) {
          used_items->selectItem(i);
          used_items->setCurrentItem(i);
          used_items->makeItemVisible(i);
          CheckIndex();
          return 1;
        }
      }
    }
    MenuSpec*spec=MenuMgr::AddTBarUsrCmd(mc);
    FXListItem*item=new TBarListItem(spec->pref,NULL,(void*)spec);
    InsertItem(item);
  }
  return 1;
}



void ToolbarPrefs::CheckCount()
{
  FXint*btns=MenuMgr::TBarBtns();
  FXint iUsed;
  for (iUsed=0; iUsed<max_items; iUsed++) {
    btns[iUsed]=TopWindow::ID_LAST;
  }
  for (iUsed=0; iUsed<used_items->getNumItems(); iUsed++) {
    MenuSpec*spec=(MenuSpec*)(used_items->getItemData(iUsed));
    btns[iUsed]=spec->sel;
  }
  DualListForm::CheckCount();
  if (ins_btn->isEnabled()) { custom_btn->enable(); } else { custom_btn->disable(); }
}



void ToolbarPrefs::PopulateAvail()
{
  for (MenuSpec*spec=MenuMgr::MenuSpecs(); spec->sel!=TopWindow::ID_LAST; spec++) {
    if (spec->ms_mc) {
      avail_items->appendItem(new TBarListItem(spec->pref, NULL, (void*)spec));
    }
  }
}



void ToolbarPrefs::PopulateUsed()
{
  for (FXint*isel=MenuMgr::TBarBtns(); *isel!=TopWindow::ID_LAST; isel++) {
    MenuSpec* spec=MenuMgr::LookupMenu(*isel);
    if (spec) {
      FXint found=avail_items->findItemByData((void*)spec);
      if (found>=0) {
        TBarListItem*item=(TBarListItem*)avail_items->extractItem(found);
        item->setSelected(false);
        used_items->appendItem(item);
      } else {
        used_items->appendItem(new TBarListItem(spec->pref,NULL,(void*)spec));
      }
    }
  }
}

