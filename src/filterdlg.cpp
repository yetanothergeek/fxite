/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2010 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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
#include "histbox.h"

#include "intl.h"
#include "filterdlg.h"


#define confirm(cpn,msg) \
(FXMessageBox::question(this,MBOX_YES_NO,cpn, msg)==MBOX_CLICKED_YES)

#define ConfirmDiscardChanges() \
  ((getText()==before)||confirm(_("Modified settings"),_("Discard changes and close dialog?")))

#define ConfirmSaveChanges() \
  ((getText()==before)||confirm(_("Modified settings"),_("Save filters and close dialog?")))


#define FILTERLIST_OPTS FRAME_SUNKEN | FRAME_THICK | ICONLIST_BROWSESELECT | LAYOUT_FILL

#define FILE_FILTER_DLG_OPTS LAYOUT_FILL | LAYOUT_SIDE_BOTTOM \
  | DECOR_BORDER | DECOR_RESIZE | DECOR_TITLE

#define BTN_OPTS FRAME_RAISED | FRAME_THICK | LAYOUT_CENTER_Y | LAYOUT_FILL_X

#define BAR_OPTS LAYOUT_SIDE_BOTTOM | LAYOUT_FILL_X | PACK_UNIFORM_WIDTH | FRAME_SUNKEN




FXDEFMAP(FileFiltersDlg) FileFiltersDlgMap[]={
  FXMAPFUNC(SEL_COMMAND, FileFiltersDlg::ID_ACCEPT, FileFiltersDlg::onCommand),
  FXMAPFUNC(SEL_COMMAND, FileFiltersDlg::ID_CANCEL, FileFiltersDlg::onCommand),
  FXMAPFUNC(SEL_CLOSE,   0, FileFiltersDlg::onClose),
  FXMAPFUNCS( SEL_COMMAND, FileFiltersDlg::ID_LIST_SEL,
                FileFiltersDlg::ID_NEW_CMD, FileFiltersDlg::onCommand ),
};

FXIMPLEMENT(FileFiltersDlg,FXDialogBox,FileFiltersDlgMap,ARRAYNUMBER(FileFiltersDlgMap))



class FileFilterList: public FXIconList {
  FXDECLARE(FileFilterList)
  protected:
  FileFilterList(){}
  public:
  FileFilterList(FXComposite*p, FXObject*tgt=NULL, FXSelector sel=0, FXuint opts=ICONLIST_NORMAL):
  FXIconList(p,tgt,sel,opts) {}
  long onDoubleClicked(FXObject* sender,FXSelector sel,void *ptr) {
    long rv=FXIconList::onDoubleClicked(sender,sel,ptr);
    ((FileFiltersDlg*)getShell())->editItem();
    return rv;
  }
};


FXDEFMAP(FileFilterList) FileFilterListMap[]={
  FXMAPFUNC(SEL_DOUBLECLICKED, 0, FileFilterList::onDoubleClicked),
};

FXIMPLEMENT(FileFilterList,FXIconList,FileFilterListMap,ARRAYNUMBER(FileFilterListMap))



void FileFiltersDlg::setText(const FXString str)
{
  filters->clearItems();
  FXString FileFilters=str;
  FileFilters.substitute('|', '\n', true);
  for (FXint i=0; i<FileFilters.contains('\n'); i++) {
    FXString sect=FileFilters.section('\n',i);
    sect.simplify();
    FXString desc=sect.section("(", 0);
    FXString mask=sect.section("(", 1);
    mask.substitute(")", "");
    desc.simplify();
    mask.simplify();
    FXString txt;
    txt.format("%s\t%s", desc.text(), mask.text());
    filters->appendItem(txt);
  }
}



const FXString& FileFiltersDlg::getText()
{
  after="";
  for (FXint i=0; i<filters->getNumItems(); i++) {
    FXString item=filters->getItemText(i);
    FXString desc=item.section('\t',0);
    FXString mask=item.section('\t',1);
    item.format("%s (%s)|", desc.text(), mask.text());
    after.append(item);
  }
  return after;
}



void FileFiltersDlg::enableButtons()
{
  if (filters->getNumItems()>0) {
    FXint icurr=filters->getCurrentItem();
    delete_btn->enable();
    edit_btn->enable();
    raise_btn->enable();
    lower_btn->enable();
    if (icurr==0) {
      raise_btn->disable();
    }
    if (icurr==(filters->getNumItems()-1)) {
      lower_btn->disable();
    }
  } else {
    delete_btn->disable();
    edit_btn->disable();
    raise_btn->disable();
    lower_btn->disable();
  }
}



long FileFiltersDlg::onClose(FXObject* sender,FXSelector sel,void *ptr)
{
  if (ConfirmDiscardChanges()) { getApp()->stopModal(this,0); }
  return 1;
}



long FileFiltersDlg::onCommand(FXObject*sender, FXSelector sel, void*ptr)
{
  FXint icurr=filters->getCurrentItem();
  switch (FXSELID(sel)) {
    case ID_LIST_SEL: {
      break;
    }
    case ID_DEFAULTS_CMD: {
      if (confirm(_("Restore defaults"), _("Restore application default filters?"))) {
        setText(Settings::defaultFileFilters());
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
      filters->moveItem(icurr,icurr-1);
      filters->setCurrentItem(icurr-1);
      break;
    }
    case ID_LOWER_CMD: {
      filters->moveItem(icurr,icurr+1);
      filters->setCurrentItem(icurr+1);
      break;
    }
    case ID_EDIT_CMD: {
      editItem();
      return 1;
    }
    case ID_DELETE_CMD: {
      filters->removeItem(icurr);
      if (icurr==filters->getNumItems()) { icurr--; }
      filters->setCurrentItem(icurr);
      break;
    }
    case ID_NEW_CMD: {
     if (icurr<0) { icurr=0; }
     filters->insertItem(icurr,"");
     filters->setCurrentItem(icurr);
     if (!editItem()) {
       filters->removeItem(icurr);
       if (icurr==filters->getNumItems()) { icurr--; }
       filters->setCurrentItem(icurr);
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



bool FileFiltersDlg::editItem()
{
  FXDialogBox dlg(this,_("Edit filter"));
  FXHorizontalFrame* btns=new FXHorizontalFrame(&dlg,BAR_OPTS);
  new FXButton(btns, _("&Accept"), NULL, &dlg, ID_ACCEPT, BTN_OPTS);
  new FXButton(btns, _("&Cancel"), NULL, &dlg, ID_CANCEL, BTN_OPTS);
  new FXLabel(&dlg, _("Description:"));
  FXString txt=filters->getItemText(filters->getCurrentItem());
  ClipTextField*desc=new ClipTextField(&dlg,64);
  desc->setText(txt.section('\t',0));
  new FXLabel(&dlg, _("\nFile masks:   (separated by comma)"));
  ClipTextField*mask=new ClipTextField(&dlg,64);
  mask->setText(txt.section('\t',1));
  new FXLabel(&dlg, " ");
  desc->setFocus();
  if (dlg.execute(PLACEMENT_SCREEN)) {
    FXString sdesc=desc->getText();
    sdesc.simplify();
    FXString smask=mask->getText();
    for (const char*c=" \t()"; *c; c++ ) {
      const char s[2]={*c,0};
      smask.substitute(s, "", true);
    }
    txt.format("%s\t%s", sdesc.text(), smask.text());
    filters->setItemText(filters->getCurrentItem(), txt);
    return true;
  } else {
    return false;
  }
}



FileFiltersDlg::FileFiltersDlg(FXWindow* w, const FXString init):
   FXDialogBox(w, _("File dialog filters"), FILE_FILTER_DLG_OPTS, 0, 0, 480, 320)
{
  FXButton* accept_btn;
  FXButton* cancel_btn;
  FXButton* defaults_btn;
  FXButton* revert_btn;

  setPadLeft(0);
  setPadTop(0);
  setPadRight(0);
  setPadBottom(0);

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

  defaults_btn->setTipText( _("Restore filters to installation defaults"));
  revert_btn->setTipText(   _("Undo all changes made in this dialog"));
  accept_btn->setTipText(   _("Save changes and close this dialog"));
  cancel_btn->setTipText(   _("Discard changes and close this dialog"));
  raise_btn->setTipText(    _("Move selected filter upwards in the list"));
  lower_btn->setTipText(    _("Move selected filter downwards in the list"));
  edit_btn->setTipText(     _("Edit the selected filter"));
  delete_btn->setTipText(   _("Delete the selected filter from the list"));
  new_btn->setTipText(      _("Create a new filter"));

  filters=new FileFilterList(this, this, ID_LIST_SEL,FILTERLIST_OPTS);
  filters->appendHeader(_("Description"));
  filters->appendHeader(_("File mask"));
  before=init;
  setText(init);
}



void FileFiltersDlg::create()
{
  FXDialogBox::create();
  FXint one_third=(filters->getParent()->getWidth()/3);
  filters->setHeaderSize(0, one_third);
  filters->setHeaderSize(1, one_third*2);
  raise_btn->disable();
  if (filters->getNumItems()>0) {
    filters->setCurrentItem(0);
    filters->selectItem(0);
  } else {
    lower_btn->disable();
    edit_btn->disable();
  }
  show(PLACEMENT_SCREEN);
}

