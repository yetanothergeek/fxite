/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2010-2011 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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
#include "histbox.h"

#include "intl.h"
#include "desclistdlg.h"


#define confirm(cpn,msg) \
(FXMessageBox::question(this,MBOX_YES_NO,cpn, msg)==MBOX_CLICKED_YES)

#define ConfirmDiscardChanges() \
  ((getText()==before)||confirm(_("Modified settings"),_("Discard changes and close dialog?")))

#define ConfirmSaveChanges() \
  ((getText()==before)||confirm(_("Modified settings"),_("Save changes and close dialog?")))


#define FILTERLIST_OPTS FRAME_SUNKEN | FRAME_THICK | ICONLIST_BROWSESELECT | LAYOUT_FILL

#define DESC_LIST_DLG_OPTS LAYOUT_FILL | LAYOUT_SIDE_BOTTOM \
  | DECOR_BORDER | DECOR_RESIZE | DECOR_TITLE

#define BTN_OPTS FRAME_RAISED | FRAME_THICK | LAYOUT_CENTER_Y | LAYOUT_FILL_X

#define BAR_OPTS LAYOUT_SIDE_BOTTOM | LAYOUT_FILL_X | PACK_UNIFORM_WIDTH | FRAME_SUNKEN



FXDEFMAP(DescListDlg) DescListDlgMap[]={
  FXMAPFUNC(SEL_COMMAND, DescListDlg::ID_ACCEPT, DescListDlg::onCommand),
  FXMAPFUNC(SEL_COMMAND, DescListDlg::ID_CANCEL, DescListDlg::onCommand),
  FXMAPFUNC(SEL_CLOSE,   0, DescListDlg::onClose),
  FXMAPFUNC(SEL_COMMAND, DescListDlg::ID_BROWSE, DescListDlg::onBrowse),
  FXMAPFUNCS( SEL_COMMAND, DescListDlg::ID_LIST_SEL,
                DescListDlg::ID_NEW_CMD, DescListDlg::onCommand ),
};

FXIMPLEMENT(DescListDlg,FXDialogBox,DescListDlgMap,ARRAYNUMBER(DescListDlgMap))



class DescItemList: public FXIconList {
  FXDECLARE(DescItemList)
  protected:
  DescItemList(){}
  public:
  DescItemList(FXComposite*p, FXObject*tgt=NULL, FXSelector sel=0, FXuint opts=ICONLIST_NORMAL):
  FXIconList(p,tgt,sel,opts) {}
  long onDoubleClicked(FXObject* sender,FXSelector sel,void *ptr) {
    long rv=FXIconList::onDoubleClicked(sender,sel,ptr);
    const FXString txt=getItemText(getCurrentItem());
    ((DescListDlg*)getShell())->editItem(txt.section('\t',0),txt.section('\t',1));
    return rv;
  }
};


FXDEFMAP(DescItemList) DescItemListMap[]={
  FXMAPFUNC(SEL_DOUBLECLICKED, 0, DescItemList::onDoubleClicked),
};

FXIMPLEMENT(DescItemList,FXIconList,DescItemListMap,ARRAYNUMBER(DescItemListMap))



void DescListDlg::enableButtons()
{
  items->layout();
  if (items->getNumItems()>0) {
    FXint icurr=items->getCurrentItem();
    delete_btn->enable();
    edit_btn->enable();
    raise_btn->enable();
    lower_btn->enable();
    if (icurr==0) {
      raise_btn->disable();
    }
    if (icurr==(items->getNumItems()-1)) {
      lower_btn->disable();
    }
  } else {
    delete_btn->disable();
    edit_btn->disable();
    raise_btn->disable();
    lower_btn->disable();
  }
  if (items_max && (items->getNumRows()>=items_max) ) {
    new_btn->disable();
  } else {
    new_btn->enable();
  }
}



long DescListDlg::onClose(FXObject* sender,FXSelector sel,void *ptr)
{
  if (ConfirmDiscardChanges()) { getApp()->stopModal(this,0); }
  return 1;
}



long DescListDlg::onBrowse(FXObject* sender,FXSelector sel,void *ptr)
{
  FXTextField *edit=(FXTextField*)((FXButton*)sender)->getUserData();
  FXString text=edit->getText();
  if (Browse(text)) { edit->setText(text); }
  return 1;
}



long DescListDlg::onCommand(FXObject*sender, FXSelector sel, void*ptr)
{
  FXint icurr=items->getCurrentItem();
  switch (FXSELID(sel)) {
    case ID_LIST_SEL: {
      break;
    }
    case ID_DEFAULTS_CMD: {
      if (confirm(_("Restore defaults"), _("Restore application defaults?"))) {
        RestoreAppDefaults();
      }
      break;
    }
    case ID_REVERT_CMD: {
      if ((getText()!=before)&&confirm(_("Revert changes"),_("Undo all changes?"))) {
        setText(before);
      }
      break;
    }
    case ID_RAISE_CMD: {
      items->moveItem(icurr,icurr-1);
      items->setCurrentItem(icurr-1);
      break;
    }
    case ID_LOWER_CMD: {
      items->moveItem(icurr,icurr+1);
      items->setCurrentItem(icurr+1);
      break;
    }
    case ID_EDIT_CMD: {
      const FXString txt=items->getItemText(items->getCurrentItem());
      editItem(txt.section('\t',0),txt.section('\t',1));
      return 1;
    }
    case ID_DELETE_CMD: {
      items->removeItem(icurr);
      if (icurr==items->getNumItems()) { icurr--; }
      items->setCurrentItem(icurr);
      break;
    }
    case ID_NEW_CMD: {
     if (icurr<0) { icurr=0; }
     items->insertItem(icurr,"");
     items->setCurrentItem(icurr);
     if (!editItem(FXString::null,FXString::null)) {
       items->removeItem(icurr);
       if (icurr==items->getNumItems()) { icurr--; }
       items->setCurrentItem(icurr);
     }
     break;
    }
    case ID_ACCEPT: {
      if (ConfirmSaveChanges()) { getApp()->stopModal(this,1); }
      return 1;
    }
    case ID_CANCEL: {
      if (ConfirmDiscardChanges()) { getApp()->stopModal(this,0); }
      return 1;
    }
  }
  enableButtons();
  return 1;
}


#define EDIT_OPTS(LIMITED) TEXTFIELD_NORMAL|(LIMITED?TEXTFIELD_LIMITED:0)

bool DescListDlg::editItem(const FXString &desc, const FXString &item, bool focus_item)
{
  FXDialogBox dlg(this,_("Edit item"));
  FXHorizontalFrame* btns=new FXHorizontalFrame(&dlg,BAR_OPTS);
  new FXButton(btns, _("&Accept"), NULL, &dlg, ID_ACCEPT, BTN_OPTS);
  new FXButton(btns, _("&Cancel"), NULL, &dlg, ID_CANCEL, BTN_OPTS);
  ClipTextField*desc_edit=NULL;
  ClipTextField*item_edit=NULL;
  if (desc_max_len>=0) {
    new FXLabel(&dlg, _("Description:"));
    desc_edit=new ClipTextField(&dlg,desc_max_len?desc_max_len:64,NULL,0,EDIT_OPTS(desc_max_len));
    desc_edit->setText(desc);
  }
  (new FXLabel(&dlg, item_comment, NULL, JUSTIFY_LEFT))->setPadTop(12);
  FXHorizontalFrame *hframe=new FXHorizontalFrame(&dlg,FRAME_NONE);
  item_edit=new ClipTextField(hframe,item_max_len?item_max_len:64,NULL,0,EDIT_OPTS(item_max_len));
  item_edit->setText(item);
  if (has_browse_btn) {
    FXButton*btn = new FXButton(hframe,"...", NULL, this, ID_BROWSE);
    btn->setUserData(item_edit);
  }
  new FXLabel(&dlg, " ");
  dlg.create();
  if (focus_item||!desc_edit) { item_edit->setFocus(); } else { desc_edit->setFocus(); }
  if (dlg.execute(PLACEMENT_SCREEN)) {
    FXString sdesc=desc_edit?desc_edit->getText():FXString::null;
    sdesc.substitute('\t', ' ', true);
    sdesc.simplify();
    FXString sitem=item_edit->getText();
    if (desc_edit&&sdesc.empty()) {
      FXMessageBox::error(this, MBOX_OK, _("Error"), _("You must enter a description"));
      return editItem(sdesc,sitem);
    }
    if (Verify(sitem)) {
      items->setItemText(items->getCurrentItem(), sdesc+'\t'+sitem);
      return true;
    } else {
      return editItem(sdesc,sitem,true);
    }
  } else {
    return false;
  }
}



DescListDlg::DescListDlg( FXWindow* w, const char* name,
                          const char*hdr2, const char*howto, const char*intro,
                          int max_desc_len, int max_item_len, int max_items, bool browse_btn):
   FXDialogBox(w, name, DESC_LIST_DLG_OPTS, 0, 0, 480, 320)
{
  FXButton* accept_btn;
  FXButton* cancel_btn;
  FXButton* defaults_btn;
  FXButton* revert_btn;

  item_comment=howto;
  desc_max_len=max_desc_len;
  item_max_len=max_item_len;
  items_max=max_items;
  has_browse_btn=browse_btn;

  setPadLeft(0);
  setPadTop(0);
  setPadRight(0);
  setPadBottom(0);

  if (intro) {
    intro_label=new FXLabel(this,intro, NULL, JUSTIFY_LEFT|FRAME_GROOVE|LAYOUT_FILL_X);
    intro_label->setPadLeft(4);
    intro_label->setPadRight(4);
    intro_label->setPadTop(4);
    intro_label->setPadBottom(4);
    intro_label->setTextColor(getApp()->getTipforeColor());
    intro_label->setBackColor(getApp()->getTipbackColor());
  } else {
    intro_label=NULL;
  }
  FXHorizontalFrame* buttons1=new FXHorizontalFrame(this,BAR_OPTS);
  defaults_btn = new FXButton(buttons1, _("De&faults"), NULL, this, ID_DEFAULTS_CMD, BTN_OPTS);
  revert_btn   = new FXButton(buttons1, _("Re&set"),    NULL, this, ID_REVERT_CMD,   BTN_OPTS);
  accept_btn   = new FXButton(buttons1, _("&Accept"),   NULL, this, ID_ACCEPT,       BTN_OPTS);
  cancel_btn   = new FXButton(buttons1, _("&Cancel"),   NULL, this, ID_CANCEL,       BTN_OPTS);

  FXHorizontalFrame* buttons2=new FXHorizontalFrame(this,BAR_OPTS);
  raise_btn  = new FXButton(buttons2, _("&Raise"),   NULL, this, ID_RAISE_CMD,  BTN_OPTS);
  lower_btn  = new FXButton(buttons2, _("&Lower"),   NULL, this, ID_LOWER_CMD,  BTN_OPTS);
  edit_btn   = new FXButton(buttons2, _("&Edit..."), NULL, this, ID_EDIT_CMD,   BTN_OPTS);
  delete_btn = new FXButton(buttons2, _("&Delete"),  NULL, this, ID_DELETE_CMD, BTN_OPTS);
  new_btn    = new FXButton(buttons2, _("&New..."),  NULL, this, ID_NEW_CMD,    BTN_OPTS);

  defaults_btn->setTipText( _("Restore items to installation defaults"));
  revert_btn->setTipText(   _("Undo all changes made in this dialog"));
  accept_btn->setTipText(   _("Save changes and close this dialog"));
  cancel_btn->setTipText(   _("Discard changes and close this dialog"));
  raise_btn->setTipText(    _("Move selected item upwards in the list"));
  lower_btn->setTipText(    _("Move selected item downwards in the list"));
  edit_btn->setTipText(     _("Edit the selected item"));
  delete_btn->setTipText(   _("Delete the selected item from the list"));
  new_btn->setTipText(      _("Create a new item"));

  items=new DescItemList(this, this, ID_LIST_SEL,FILTERLIST_OPTS);
  items->appendHeader(_("Description"));
  if (desc_max_len<0) {items->setHeaderSize(0,0); }
  items->appendHeader(hdr2);
}



void DescListDlg::create()
{
  FXDialogBox::create();
  setText(before);
  items->layout();
  if (intro_label) {
    FXint want_width=intro_label->getDefaultWidth()+intro_label->getPadRight()*2;
    FXint items_want_width=items->getWidth()+items->verticalScrollBar()->getWidth()+8;
    if (items_want_width>want_width) { want_width=items_want_width; }
    FXint max_width=getApp()->getRootWindow()->getWidth();
    setWidth(want_width>max_width?max_width:want_width);
  }
  FXint want_hgt=getHeight()+items->getItemHeight();
  FXint max_hgt=getApp()->getRootWindow()->getHeight()*0.75;
  setHeight(want_hgt>max_hgt?max_hgt:want_hgt);
  if (desc_max_len>=0) {
    FXint one_third=getWidth()/3;
    items->setHeaderSize(0, one_third);
    items->setHeaderSize(1, one_third*2);
  } else {
    items->setHeaderSize(0, 0);
    items->setHeaderSize(1, getWidth());
  }
  enableButtons();
  show(PLACEMENT_SCREEN);
}

