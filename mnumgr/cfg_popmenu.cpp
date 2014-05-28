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
#include "toolbar.h"
#include "menuspec.h"
#include "tooltree.h"
#include "shmenu.h"

#include "intl.h"
#include "cfg_popmenu.h"



FXDEFMAP(PopupPrefs) PopupPrefsMap[]={
  FXMAPFUNC(SEL_COMMAND,PopupPrefs::ID_INSERT_CUSTOM,PopupPrefs::onInsertCustomItem),
  FXMAPFUNC(SEL_COMMAND,PopupPrefs::ID_INSERT_SEPARATOR,PopupPrefs::onInsertSeparator),
  FXMAPFUNC(SEL_QUERY_TIP, 0, PopupPrefs::onQueryTip),
};

FXIMPLEMENT(PopupPrefs,DualListForm,PopupPrefsMap,ARRAYNUMBER(PopupPrefsMap));



class TBarListItem: public FXListItem {
public:
  TBarListItem(const FXString &text, FXIcon *ic=NULL, void *ptr=NULL):FXListItem(text,ic,ptr) {}
};



class PopupListItem: public FXListItem {
public:
  PopupListItem(const FXString &text, FXIcon *ic=NULL, void *ptr=NULL):FXListItem() {
    data=strdup((const char*)ptr);
    setText(stripHotKey(text.section('\t',0)));
  }
  ~PopupListItem() { if (data) { free(data); }}
};



class PopupSeparator: public FXListItem {
public:
  PopupSeparator():FXListItem("--"){}
};



PopupPrefs::PopupPrefs(FXComposite*p, UserMenu**ums, MenuMgr*mmgr):DualListForm(p,NULL,0,POPUP_MAX_CMDS)
{
  mnumgr=mmgr;
  user_menus=ums;
  invalid=mnumgr->LastID();
  FXHorizontalFrame* AvailBtns=new FXHorizontalFrame( left_column,
                                                     FRAME_RAISED|LAYOUT_FILL_X|LAYOUT_CENTER_X|PACK_UNIFORM_WIDTH);
  custom_btn=new FXButton( AvailBtns, _("Custom &Tools..."),
                                NULL,this, ID_INSERT_CUSTOM,BUTTON_NORMAL|LAYOUT_CENTER_X);
  FXVerticalFrame* SepBtnFrm=new FXVerticalFrame( mid_column,
                                                     FRAME_NONE|LAYOUT_FILL|LAYOUT_CENTER_X|LAYOUT_CENTER_Y|LAYOUT_SIDE_BOTTOM);
  SepBtnFrm->setPadTop(32);
  separator_btn=new FXButton( SepBtnFrm, _("&Separator>>"),
                                NULL,this, ID_INSERT_SEPARATOR,BUTTON_NORMAL|LAYOUT_CENTER_X|LAYOUT_CENTER_Y);

}



void PopupPrefs::CheckCount()
{
  mnumgr->FreePopupCommands();
  char**commands=mnumgr->GetPopupCommands();
  for (FXint i=0; i<used_items->getNumItems(); i++) {
    if (dynamic_cast<TBarListItem*>(used_items->getItem(i))) {
      MenuSpec*ms=(MenuSpec*)(used_items->getItemData(i));
      if (ms) {
        commands[i]=strdup(ms->pref?ms->pref:"");
      } else {
        commands[i]=strdup("");
      }
    } else if (dynamic_cast<PopupListItem*>(used_items->getItem(i))) {
      const char*cmd=(const char*)used_items->getItemData(i);
      commands[i]=strdup(cmd?cmd:"");
    } else {
       commands[i]=strdup("");
    }
  }
  DualListForm::CheckCount();
  if (ins_btn->isEnabled()) { custom_btn->enable(); } else { custom_btn->disable(); }
}



long PopupPrefs::onInsertSeparator(FXObject*o, FXSelector sel, void*p)
{
  InsertItem(new PopupSeparator());
  return 1;
}



long PopupPrefs::onInsertCustomItem(FXObject*o, FXSelector sel, void*p)
{
  FXMenuCommand*mc;
  if (ToolsTree::SelectTool(this, user_menus, mc)) {
    const char*newpath=(const char*)mc->getUserData();
    if (newpath) {
      // If the command is already in the used items list, just select it...
      for (FXint i=0; i<used_items->getNumItems(); i++) {
        const char*oldpath=(const char*)used_items->getItemData(i);
        if (oldpath && (strcmp(newpath, oldpath)==0)) {
          used_items->selectItem(i);
          used_items->setCurrentItem(i);
          used_items->makeItemVisible(i);
          CheckIndex();
          return 1;
        }
      }
      FXString label;
      if (FXStat::exists(newpath) && UserMenu::MakeLabelFromPath(newpath,label)) {
        FXListItem*item=new PopupListItem(label,NULL,(void*)newpath);
        InsertItem(item);
      }
    }
  }
  return 1;
}



void PopupPrefs::PopulateAvail()
{
  for (MenuSpec*spec=mnumgr->MenuSpecs(); spec->sel!=invalid; spec++) {
    if ((spec->type=='m')&&(spec->ms_mc||(strncmp(spec->pref,"Popup",5)==0))) {
      avail_items->appendItem(new TBarListItem(spec->pref, NULL, (void*)spec));
    }
  }
}



void PopupPrefs::PopulateUsed()
{
  for (char**cmd=mnumgr->GetPopupCommands(); *cmd; cmd++) {
    if (*cmd[0]) {
      MenuSpec* spec=mnumgr->LookupMenuByPref(*cmd);
      if (spec) {
        FXint found=avail_items->findItemByData((void*)spec);
        if (found>=0) {
          TBarListItem*item=(TBarListItem*)avail_items->extractItem(found);
          item->setSelected(false);
          used_items->appendItem(item);
        } else {
          used_items->appendItem(new TBarListItem(spec->pref,NULL,(void*)spec));
        }
      } else {
        FXString label;
        if (FXStat::exists(*cmd) && UserMenu::MakeLabelFromPath(*cmd,label)) {
          used_items->appendItem(new PopupListItem(label,NULL,*cmd));
        }
      }
    } else {
      used_items->appendItem(new PopupSeparator());
    }
  }
}



long PopupPrefs::onQueryTip(FXObject*o, FXSelector sel, void*p)
{
  FXList*w=dynamic_cast<FXList*>((FXObject*)p);
  if (w) {
    FXint index,cx,cy;
    FXuint btns;
    if (w->getCursorPosition(cx,cy,btns) && (index=w->getItemAt(cx,cy))>=0) {
      FXListItem*item=w->getItem(index);
      FXString tip=FXString::null;
      if (dynamic_cast<PopupListItem*>(item)) {
        mnumgr->GetTipFromFilename((const char*)(item->getData()),tip);
      } else if (dynamic_cast<PopupSeparator*>(item)) {
        tip=_("<separator>");
      } else {
        MenuSpec*spec=(MenuSpec*)(item->getData());
        if (spec) {
          mnumgr->GetTBarBtnTip(spec,tip);
        }
      }
      if (!tip.empty()) {
        o->handle(this,FXSEL(SEL_COMMAND,ID_SETSTRINGVALUE),(void*)&tip);
        return 1;
      }
    }
  }
  return 0;
}

