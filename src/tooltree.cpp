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


#include <cerrno>

#include <fxkeys.h>
#include <fx.h>

#include "shmenu.h"
#include "appwin.h"

#include "intl.h"
#include "tooltree.h"

#define TREEOPTS TREELIST_BROWSESELECT|TREELIST_SHOWS_LINES|TREELIST_SHOWS_BOXES|LAYOUT_FILL



ToolsTree::ToolsTree(FXComposite*p, FXObject*tgt, FXSelector sel, UserMenu**menus):FXTreeList(p,tgt,sel,TREEOPTS) {
  menu_list=menus;
  saved_path="";
  prev_item=NULL;
  dummy_item=NULL;
  scan(false);
}



const char*ToolsTree::getFilePath(FXTreeItem*item) {
  if ( item->getParent() ) {
    if ( item->getParent()->getParent()) {
      if (item->hasItems()) {
        return (const char*)((((FXMenuCascade*)(item->getData()))->getMenu())->getUserData());
      } else {
        return (const char*)(((FXMenuCommand*)(item->getData()))->getUserData());
      }
    } else {
      return ((UserMenu*)(item->getData()))->getTopDir();
    }
  } else {
    return NULL;
  }
}



void ToolsTree::build_tree(FXTreeItem*item, const FXPopup*menu)
{
  item->setHasItems(true);
  if (menu) {
    for (FXWindow*w=menu->getFirst(); w; w=w->getNext()) {
      FXTreeItem*node=new FXTreeItem(((FXMenuCaption*)w)->getText());
      if (!saved_path.empty()) {
         const char*this_path;
         this_path=(const char*)((FXMenuCommand*)w)->getUserData();
         if (!this_path) {
           this_path=(const char*)((((FXMenuCascade*)w)->getMenu())->getUserData());
         }
         if ( this_path && strcmp(saved_path.text(),this_path) == 0 ) {
             restored_item=node;
         }
      }
      insertItem(NULL,item,node);
      node->setData(w);
      node->setHasItems(false);
      if (!w->getUserData()) { /* Cascading menus don't carry the path, their popup does */
        build_tree(node,((FXMenuCascade*)w)->getMenu());
      }
    }
  }
}



void ToolsTree::scan(bool rebuild)
{
  FXString item_name="";
  if (prev_item) {
    if ((prev_item==tools)||(prev_item->getParent()==tools)) {
      item_name=prev_item->getText();
    } else {
      if (saved_path.empty()) { saved_path=getFilePath(prev_item); }
    }
  }
  if (dummy_item) {
    if (dummy_item->hasItems()) {
    FXMenuCascade*casc=(FXMenuCascade*)(dummy_item->getData());
    FXMenuPane*pane=(FXMenuPane*)casc->getMenu();
    free(pane->getUserData());
    delete pane;
    delete casc;
    dummy_item->setData(NULL);
    } else {
      FXMenuCommand*cmd=(FXMenuCommand*)(dummy_item->getData());
      free(cmd->getUserData());
      delete cmd;
      dummy_item->setData(NULL);
    }
    dummy_item=NULL;
  }
  clearItems();
  prev_item=NULL;
  restored_item=NULL;
  tools = new FXTreeItem(_("Tools"));
  tools->setHasItems(true);
  appendItem(NULL,tools);
  for (UserMenu**um=menu_list; *um; um++) {
    if (rebuild) { (*um)->rescan(); }
    FXTreeItem *item=new FXTreeItem((*um)->getText());
    item->setHasItems(true);
    item->setData(*um);
    insertItem(NULL,tools,item);
    build_tree(item,(*um)->menu());
  }
  saved_path="";
  if (item_name.empty()) {
    for (FXTreeItem *ti=restored_item; ti; ti=ti->getParent()) { expandTree(ti); }
  } else {
    if (compare(tools->getText(),item_name)==0) {
      restored_item=tools;
    } else {
      for (FXTreeItem *ti=tools->getFirst(); ti; ti=ti->getNext()) {
        if (compare(ti->getText(),item_name)==0) {
          restored_item=ti;
          break;
        }
      }
    }
  }
  expandTree(tools);
  if (!restored_item) { restored_item=tools; }
  expandTree(restored_item);
  setCurrentItem(restored_item);
  setFocus();
  if (target&&message) {
    target->handle(this,FXSEL(SEL_CHANGED,message),restored_item);
  }
}



UserMenu*ToolsTree::GetUserMenu(FXTreeItem*item) {
  for (FXTreeItem*i=item; i; i=i->getParent()) {
    if (i->getParent()==tools) {
      return (UserMenu*)(i->getData());
    }
  }
  return NULL;
}


void ToolsTree::MakeDummyMenu(FXTreeItem*parent_item)
{
  FXString path=getFilePath(parent_item);
  path.append(PATHSEP);
  FXint index=0;
  FXString tmp;
  while (index<100) {
    tmp.format("%s%02d.new-group",path.text(),index);
    if (!FXStat::exists(tmp)) { break; }
    index++;
  }
  if (FXStat::exists(tmp)) {
      FXMessageBox::error(getShell(), MBOX_OK, _("File error"), "%s:\n%s\n\n%s",
        _("Failed to create temporary directory"),
        tmp.text(), _("too many \"New Group\" folders")
      );
      dummy_item=NULL;
      return;
  }
  if (!FXDir::create(tmp)) {
    FXMessageBox::error(getShell(), MBOX_OK, _("File error"), "%s:\n%s\n\n%s",
      _("Failed to create temporary directory"),
      tmp.text(), strerror(errno)
    );
    dummy_item=NULL;
    return;
  }
  dummy_item=new FXTreeItem(_("New Group"));
  dummy_item->setHasItems(true);
  appendItem(parent_item,dummy_item);
  FXMenuPane*pane=new FXMenuPane(this);
  pane->setUserData(strdup(tmp.text()));
  FXMenuCascade*casc=new FXMenuCascade(this,"",NULL,pane,0);
  dummy_item->setData(casc);
  setCurrentItem(dummy_item);
  selectItem(dummy_item);
}



void ToolsTree::MakeDummyTool(FXTreeItem*parent_item, FXuint perm)
{
  FXString path=getFilePath(parent_item);
  const char *ext;
  switch(FXPath::name(path)[0]) {
    case 'c':
    case 'f': {
#ifdef WIN32
      ext="bat";
#else
      ext="sh";
#endif
      break;
    }
    case 'm': {
      ext="lua";
      break;
    }
    default: {
      ext="txt";
    }
  }
  path.append(PATHSEP);
  FXint index=0;
  FXString tmp;
  while (index<100) {
    tmp.format("%s%02d.new-tool.%s",path.text(),index, ext);
    if (!FXStat::exists(tmp)) { break; }
    index++;
  }
  if (FXStat::exists(tmp)) {
      FXMessageBox::error(getShell(), MBOX_OK, _("File error"), "%s:\n%s\n\n%s",
        _("Failed to create temporary file"),
        tmp.text(), _("too many \"New Tool\" files")
      );
      dummy_item=NULL;
      return;
  }
  if (!FXFile::create(tmp,perm)) {
    FXMessageBox::error(getShell(), MBOX_OK, _("File error"), "%s:\n%s\n\n%s",
      _("Failed to create temporary file"),
      tmp.text(), strerror(errno)
    );
    dummy_item=NULL;
    return;
  }
  dummy_item=new FXTreeItem(_("New Tool"));
  dummy_item->setHasItems(false);
  appendItem(parent_item,dummy_item);
  FXMenuCommand*cmd=new FXMenuCommand(this,_("New Tool"));
  cmd->setUserData(strdup(tmp.text()));
  dummy_item->setData(cmd);
  setCurrentItem(dummy_item);
  selectItem(dummy_item);

}



/* An OK button that is only enabled when the selected tree item is a tool (i.e. a leaf node) */
class OkBtn: public FXButton {
  FXDECLARE(OkBtn);
  OkBtn(){}
public:
  long onCheckEnable(FXObject*o, FXSelector sel, void*p)
  {
    FXTreeItem*item=(FXTreeItem*)p;
    if (item && !item->hasItems()) { enable(); } else { disable(); }
    return 1;
  }
  enum { ID_CHECK_ENABLE=FXButton::ID_LAST };
  OkBtn(FXComposite*p):FXButton(p,_("OK"),NULL,p,FXDialogBox::ID_ACCEPT,BUTTON_NORMAL|BUTTON_INITIAL|LAYOUT_CENTER_X){}
};



FXDEFMAP(OkBtn)OkBtnMap[]={ FXMAPFUNC(SEL_CHANGED,OkBtn::ID_CHECK_ENABLE,OkBtn::onCheckEnable) };

FXIMPLEMENT(OkBtn,FXButton,OkBtnMap,ARRAYNUMBER(OkBtnMap));



/* Display a dialog box to let the user select a tool item */
bool ToolsTree::SelectTool(FXWindow* owner, UserMenu** menus, FXMenuCommand*&mc)
{
  FXDialogBox dlg(owner,"Select tool",DECOR_TITLE|DECOR_BORDER,0,0,240,320,1,1,1,1,0,0);
  FXVerticalFrame*vbox=new FXVerticalFrame(&dlg,FRAME_RAISED|LAYOUT_FILL);
  OkBtn*ok=new OkBtn(&dlg);
  ToolsTree*tree=new ToolsTree(vbox, ok, OkBtn::ID_CHECK_ENABLE, menus);
  FXHorizontalFrame*btns=new FXHorizontalFrame(vbox,FRAME_RAISED|LAYOUT_FILL_X|LAYOUT_CENTER_X|PACK_UNIFORM_WIDTH);
  ok->reparent(btns,NULL);
  new FXButton(btns,_("Cancel"),NULL,&dlg,FXDialogBox::ID_CANCEL,BUTTON_NORMAL|LAYOUT_CENTER_X);
  if (dlg.execute(PLACEMENT_OWNER)) {
    mc=((FXMenuCommand*)(tree->getCurrentItem()->getData()));
    return true;
  } else {
    return false;
  }
}

