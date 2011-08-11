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


#include <cctype>
#include <cerrno>

#include <fxkeys.h>
#include <fx.h>

#include "shmenu.h"
#include "compat.h"
#include "appwin.h"
#include "histbox.h"
#include "tooltree.h"

#include "intl.h"
#include "toolmgr.h"



#define IsTop(ti) ( (!ti->getParent()) || (!ti->getParent()->getParent()) )

#define SetPad(w,p) \
  (w)->setPadTop(p); \
  (w)->setPadBottom(p); \
  (w)->setPadLeft(p); \
  (w)->setPadRight(p);

static const char*default_help_text = _("Configure user-defined commands for the \"Tools\" menu.");

static const char*intro_text = _(
"Here you can customize the tools menu by\n"
"adding your own shell scripts, Lua macros,\n"
"and text snippets. The tools can also be\n"
"organized into sub-menu \"groups\".\n\n"
"To begin, select from one of the top-level\n"
"categories on the left, a brief description\n"
"of the category will appear at the bottom."
);


FXDEFMAP(ToolsDialog) ToolsDialogMap[] = {
  FXMAPFUNC(SEL_CHANGED,  ToolsDialog::ID_TREELIST_CHANGED, ToolsDialog::onTreeListChanged),
  FXMAPFUNC(SEL_CHORE,    ToolsDialog::ID_TREELIST_CHORE, ToolsDialog::onTreeListAfterChanged),
  FXMAPFUNC(SEL_KEYPRESS, ToolsDialog::ID_ACCELFIELD,  ToolsDialog::onAccelField),
  FXMAPFUNC(SEL_CHANGED,  ToolsDialog::ID_NAMEFIELD,   ToolsDialog::onNameField),
  FXMAPFUNC(SEL_COMMAND,  ToolsDialog::ID_OPTCHOOSE,   ToolsDialog::onChooseOpt),
  FXMAPFUNC(SEL_COMMAND,  ToolsDialog::ID_MODIFIED,    ToolsDialog::onModified),
  FXMAPFUNCS(SEL_COMMAND, ToolsDialog::ID_APPLY_CLICK, ToolsDialog::ID_NEW_TOOL_CLICK, ToolsDialog::onButtonClick),
  FXMAPFUNC(SEL_MIDDLEBUTTONRELEASE, ToolsDialog::ID_ACCELFIELD, ToolsDialog::onAccelField),
  FXMAPFUNC(SEL_CHORE, ToolsDialog::ID_NEW_SCAN_CHORE,ToolsDialog::onNewScanChore),
  FXMAPFUNC(SEL_TIMEOUT, ToolsDialog::ID_NEW_SCAN_CHORE,ToolsDialog::onNewScanChore),
  FXMAPFUNC(SEL_COMMAND, FXDialogBox::ID_ACCEPT, ToolsDialog::onClose),
  FXMAPFUNC(SEL_COMMAND, FXDialogBox::ID_CANCEL, ToolsDialog::onClose),
  FXMAPFUNC(SEL_CLOSE, 0, ToolsDialog::onClose),
  FXMAPFUNC(SEL_CHORE, FXDialogBox::ID_ACCEPT, ToolsDialog::onClose),
  FXMAPFUNC(SEL_COMMAND, ToolsDialog::ID_MOVELIST_CHOOSE,ToolsDialog::onMoveListChoose)
};

FXIMPLEMENT(ToolsDialog,FXDialogBox,ToolsDialogMap,ARRAYNUMBER(ToolsDialogMap));


long ToolsDialog::onModified(FXObject*o, FXSelector sel, void*p)
{
  setModified(true);
  return 0;
}



typedef struct {
  const char*filepath;
  FXString index;
  FXString name;
  FXString accel;
  FXString flag;
  FXString ext;
} ToolInfo;



static bool ParseToolInfo(const char *filepath, ToolInfo&info)
{
  info.filepath=filepath;
  FXString fn=FXPath::name(filepath);

  info.index=fn.section('.',0);
  if (!((info.index.length()==2)&&isdigit(info.index[0])&&isdigit(info.index[1]))) {
    return false;
  }
  fn.erase(0,3);
  if (fn.contains('@')) {
    info.name=fn.section('@',0);
    info.accel=fn.section('@',1);
    info.ext=FXPath::extension(info.accel);
    info.accel=FXPath::stripExtension(info.accel);
    info.flag=FXPath::extension(info.accel);
    info.accel=FXPath::stripExtension(info.accel);
  } else {
    info.name=fn;
    info.accel="";
    info.ext=FXPath::extension(info.name);
    info.name=FXPath::stripExtension(info.name);
    info.flag=FXPath::extension(info.name);
    info.name=FXPath::stripExtension(info.name);
  }
  return true;
}



long ToolsDialog::onNameField(FXObject*o, FXSelector sel, void*p)
{
  menukey_list->clearItems();
  char letter[2];
  letter[1]='\0';
  menukey_list->appendItem(" ",NULL,(void*)(FXival)(-1)); // Leave first item empty for no menu key
  for (int i=0; i<name_field->getText().length(); i++) {
    letter[0]=name_field->getText()[i];
    if ( (letter[0]!=' ')) {
      // Save the actual char index to the item's data pointer.
      menukey_list->appendItem(letter,NULL,(void*)(FXival)i);
    }
  }
  menukey_list->setNumVisible(menukey_list->getNumItems());
  if (o) { setModified(true); }
  return 0;
}



void ToolsDialog::clear()
{
  name_panel->hide();
  accel_panel->hide();
  new_panel->hide();
  delete_btn->hide();
  edit_btn->hide();
  change_panel->hide();
  move_panel->hide();
  move_list->clearItems();
  move_btn->disable();
  extn_field->hide();
  intro_lab->hide();
  extn_field->getPrev()->hide();
  accel_field->setText("");
  ctrl_chk->setCheck(false);
  alt_chk->setCheck(false);
  shift_chk->setCheck(false);
  index_field->setText("");
  name_field->setText("");
  extn_field->setText("");
  menukey_list->clearItems();
  if (opts_panel) {delete opts_panel; }
  opts_panel=NULL;
  opt_rad=NULL;
  opt_chk=NULL;
  setModified(false);
}



void ToolsDialog::create_options(FXTreeItem*item, const FXString &flag)
{
  UserMenu*um=tree->GetUserMenu(item);
  helptext->setText(um->helptext);
  const char**opts=um->getFlags();
  if (opts) {
    opts_panel=new FXGroupBox(right_box,"Options",FRAME_SUNKEN|FRAME_RAISED|FRAME_THICK|LAYOUT_FILL_X);
    int i,n=0;
    for (i=0; opts[i]; i++) { n++; }
    switch (n) {
      case 0:
      case 1:
      { break; }
      case 2: {
        opt_chk=new FXCheckButton(opts_panel,opts[1], this, ID_OPTCHOOSE);
        opt_chk->setUserData((void*)opts[0]);
        opt_chk->setCheck(strcmp(opts[0],flag.text())==0);
        break;
      }
      default: {
        for (i=0; opts[i]; i++) {
          if ((i%2)==0) {
            FXRadioButton*rb=new FXRadioButton(opts_panel,opts[i+1], this, ID_OPTCHOOSE);
            rb->setUserData((void*)opts[i]);
            rb->setCheck(strcmp(opts[i],flag.text())==0);
            if (!opt_rad) { opt_rad=rb; }
          }
        }
        bool have_check=false;
        for (FXWindow*w=opt_rad; dynamic_cast<FXRadioButton*>(w); w=w->getNext()) {
           if (((FXRadioButton*)w)->getCheck()) {
             have_check=true;
             break;
           }
        }
        if (!have_check) {
          opt_rad->setCheck(true);
        }
      }
    }
    opts_panel->create();
    opts_panel->reparent(right_box,apply_btn->getParent());
  }
}



static FXuint AskSaveChanges(FXWindow*w, bool hasitems,const char*name)
{
  return FXMessageBox::question(w, MBOX_SAVE_CANCEL_DONTSAVE,
    _("Tool was modified"), "%s %s \"%s\" %s\n\n%s\n",
    _("The"),
    hasitems?_("group"):_("tool"),
    name,
    _("has unsaved settings."),
    _("Do you want to save your changes?")
  );
}



long ToolsDialog::onClose(FXObject*o, FXSelector sel, void*p)
{
  if ( (FXSELTYPE(sel)==SEL_COMMAND) || (FXSELTYPE(sel)==SEL_CLOSE) ) {
    if (tree->PrevItem() && modified) {
      tree->setCurrentItem(tree->PrevItem());
      tree->selectItem(tree->PrevItem());
      FXuint q=AskSaveChanges(this, tree->PrevItem()->hasItems(), tree->PrevItem()->getText().text());
      switch (q) {
        case MBOX_CLICKED_SAVE:
        case MBOX_CLICKED_YES:{
          if (SaveChanges()) {
            // Calling stopModal() directly from here causes a crash, so use a chore...
            getApp()->addChore(this,ID_ACCEPT);
          }
          return 1;
        }
        case MBOX_CLICKED_NO: {
          // Discard changes and close...
          break;
        }
        default: {
          // User cancelled, so don't close...
          return 1;
        }
      }
    }
  }
  getApp()->stopModal(this,true);
  return 1;
}



long ToolsDialog::onMoveListChoose(FXObject*o, FXSelector sel, void*p)
{
  if (move_list->getItemData(move_list->getCurrentItem())) {
    move_btn->enable();
  } else {
    move_btn->disable();
  }
  return 1;
}



void ToolsDialog::SetMoveOptions(FXTreeItem *item)
{
  move_panel->show();
  move_list->clearItems();
  if (item->getParent()->getParent()!=tree->Tools()) {
    move_list->appendItem(_("^- Up one level"),NULL,item->getParent()->getParent());
  }
  for (FXTreeItem*ti=item->getParent()->getFirst(); ti; ti=ti->getNext()) {
    if ( ti->hasItems() && (ti!=item) ) {
      move_list->appendItem(ti->getText(),NULL,ti);
    }
  }
  if (move_list->getNumItems()) {
    move_list->prependItem(" ",NULL,NULL);
    move_lab->enable();
    move_list->enable();
    move_list->setCurrentItem(0);
    move_list->setNumVisible(move_list->getNumItems());
  } else {
    move_lab->disable();
    move_list->disable();
  }
}



long ToolsDialog::onTreeListAfterChanged(FXObject*o, FXSelector sel, void*p)
{
  FXTreeItem*item=(FXTreeItem*)p;
  if (tree->PrevItem() && (tree->PrevItem()!=p) && modified) {
    tree->setCurrentItem(tree->PrevItem());
    tree->selectItem(tree->PrevItem());
    FXuint q=AskSaveChanges(this,tree->PrevItem()->hasItems(),tree->PrevItem()->getText().text());

    switch (q) {
      case MBOX_CLICKED_SAVE:
      case MBOX_CLICKED_YES:{
        SaveChanges();
        return 0;
      }
      case MBOX_CLICKED_NO: {
        /*discard changes*/
        break;
      }
      case MBOX_CLICKED_CANCEL: {
         return 0;
      }
      default: {}
    }
  }
  tree->setCurrentItem(item);
  tree->selectItem(item);
  tree->SetPrevItem(item);

  clear();
  if (item->hasItems()) {
    if (item==tree->Tools()) { intro_lab->show(); } else { new_panel->show(); }
    if (!IsTop(item)) {
      name_panel->show();
      props_lab->setText(_("Group properties"));
      delete_btn->show();
      change_panel->show();
      SetMoveOptions(item);
      FXMenuCascade*casc=(FXMenuCascade*)item->getData();
      FXPopup*pop=casc->getMenu();
      ToolInfo info;
      ParseToolInfo((const char*)pop->getUserData(),info);
      index_field->setText(info.index);
      name_field->setText(casc->getText());
      onNameField(NULL,0,NULL);
      menukey_list->setCurrentItem(
        menukey_list->findItemByData((void*)(FXival)info.name.find_first_of('_'))
      );
    }
    if (item==tree->Tools()) {
      helptext->setText(default_help_text);
    } else {
      helptext->setText(tree->GetUserMenu(item)->helptext);
    }
  } else {
    name_panel->show();
    props_lab->setText(_("Tool properties"));
    accel_panel->show();
    delete_btn->show();
    edit_btn->show();
    change_panel->show();
    SetMoveOptions(item);
    extn_field->show();
    extn_field->getPrev()->show();
    FXMenuCommand*mnucmd=(FXMenuCommand*)item->getData();
    ToolInfo info;
    ParseToolInfo((const char*)mnucmd->getUserData(),info);
    create_options(item, info.flag);
    index_field->setText(info.index);
    name_field->setText(mnucmd->getText());
    extn_field->setText(info.ext);
    onNameField(NULL,0,NULL);
    menukey_list->setCurrentItem(
      menukey_list->findItemByData((void*)(FXival)info.name.find_first_of('_'))
    );
    FXHotKey hk=parseAccel(info.accel);
    FXuint mask=FXSELTYPE(hk);
    FXuint key=FXSELID(hk);
    ctrl_chk->setCheck(mask&CONTROLMASK);
    alt_chk->setCheck(mask&ALTMASK);
    shift_chk->setCheck(mask&SHIFTMASK);
    if (key) {
      accel_field->setText(unparseAccel(key).upper());
    } else {
      accel_field->setText("");
    }
  }
  return 0;
}



long ToolsDialog::onTreeListChanged(FXObject*o, FXSelector sel, void*p)
{
  //  If we pop up a dialog from here it causes the treelist to
  //  miss the next mouse-up event, so use a chore instead...
  getApp()->addChore(this,ID_TREELIST_CHORE,p);
  return 0;
}



long ToolsDialog::onChooseOpt(FXObject*o, FXSelector sel, void*p)
{
  if (opt_rad) {
    for (FXWindow*w=opt_rad; dynamic_cast<FXRadioButton*>(w); w=w->getNext()) {
      ((FXRadioButton*)w)->setCheck(w==o);
    }
  }
  setModified(true);
  return 1;
}



long ToolsDialog::onAccelField(FXObject*o, FXSelector sel, void*p)
{
  if (FXSELTYPE(sel)==SEL_MIDDLEBUTTONRELEASE) { return 1; }
  FXEvent*ev=(FXEvent*)p;
  if (
    (ev->code>=KEY_F1 && ev->code<=KEY_F12) ||
    (ev->code>=KEY_0 && ev->code<=KEY_9) ||
    (ev->code>=KEY_A && ev->code<=KEY_Z) ||
    (ev->code>=KEY_a && ev->code<=KEY_z)
  ) {
    accel_field->setText(unparseAccel(ev->code).upper());
    if (ev->state) {
      ctrl_chk->setCheck(ev->state&CONTROLMASK);
      alt_chk->setCheck(ev->state&ALTMASK);
      shift_chk->setCheck(ev->state&SHIFTMASK);
    }
    setModified(true);
  } else {
    switch (ev->code) {
      case KEY_Tab:
      case KEY_Up:
      case KEY_Down:
      case KEY_Escape: {
        return 0;
      }
      case KEY_Delete:
      case KEY_BackSpace: {
        accel_field->setText("");
        ctrl_chk->setCheck(false);
        alt_chk->setCheck(false);
        shift_chk->setCheck(false);
        setModified(true);
      }
    }
  }
  return 1;
}



void ToolsDialog::setModified(bool ismod)
{
  modified=ismod;
  if (modified) {
    apply_btn->enable();
    reset_btn->enable();
    move_list->clearItems();
    move_list->disable();
    move_lab->disable();
    move_btn->disable();
  } else {
    apply_btn->disable();
    reset_btn->disable();
    move_list->enable();
    move_lab->enable();
  }
}



bool ToolsDialog::BuildName(FXString &path, bool isdir)
{
  if (tree->PrevItem()->hasItems()) {
    tree->SetSavedPath((const char*)((((FXMenuCascade*)(tree->PrevItem()->getData()))->getMenu())->getUserData()));
  } else {
    tree->SetSavedPath((const char*)(((FXMenuCommand*)(tree->PrevItem()->getData()))->getUserData()));
  }
  path=FXPath::directory(tree->SavedPath());
  path.append(PATHSEP);
  switch (index_field->getText().length()) {
    case 0:  {
      path.append("00");
      break;
    }
    case 1:  {
      path.append("0");
      path.append(index_field->getText());
      break;
    }
    default: {
      path.append(index_field->getText());
    }
  }
  path.append(".");
  FXString tmp=name_field->getText().trim().simplify();

  if (tmp.empty()|| (compare(tmp, " ")==0)) {
      FXMessageBox::error(this, MBOX_OK, _("Invalid name"), "%s",
        _("You must specify a name for this item."));
      name_field->setFocus();
      name_field->selectAll();
      path="";
      return false;
  }
  tmp=tmp.lower();
  static const char*allowed="abcdefghijklmnopqrstuvwxyz0123456789 ";
  for (FXint i=0; i<tmp.length(); i++) {
    if (!strchr(allowed, tmp[i])) {
      FXMessageBox::error(this, MBOX_OK, _("Invalid name"), "%s",
        _("Name can only contain letters, numbers, and spaces."));
      name_field->setFocus();
      name_field->setSelection(i,1);
      path="";
      return false;
    }
  }
  tmp.substitute(' ', '-', true);
  if (menukey_list->getCurrentItem()>0) {
    tmp.insert((FXival)(menukey_list->getItemData(menukey_list->getCurrentItem())),"_");
  }
  path.append(tmp);
  if (!accel_field->getText().empty()) {
    FXuint accel_state=0;
    accel_state|=ctrl_chk->getCheck()?CONTROLMASK:0;
    accel_state|=alt_chk->getCheck()?ALTMASK:0;
    accel_state|=shift_chk->getCheck()?SHIFTMASK:0;
    if ( ((accel_state==0)||(accel_state==SHIFTMASK)) && (accel_field->getText().length()<2) ) {
      FXMessageBox::error(this,MBOX_OK, _("Invalid accelerator"), "%s\n\n%s",
        _("Alphanumeric accelerators must have a [Ctrl] or [Alt] modifier."),
        _("Only function keys [F1] - [F12] can be used without a modifier.")
      );
    accel_field->setFocus();
    accel_field->selectAll();
    path="";
    return false;
    }
    FXuint accel_key=parseAccel(accel_field->getText());
    path.append("@");
    path.append(unparseAccel(FXSEL(accel_state,accel_key)));
  }
  if (opt_chk && opt_chk->getCheck()) {
    path.append(".");
    path.append((const char*)opt_chk->getUserData());
  } else if (opt_rad) {
    for (FXWindow*w=opt_rad; dynamic_cast<FXRadioButton*>(w); w=w->getNext()) {
      if (((FXRadioButton*)w)->getCheck()) {
        path.append(".");
        path.append((const char*)(w->getUserData()));
        break;
      }
    }
  }
  if (!isdir) {
    path.append(".");
    path.append(extn_field->getText().empty()?"txt":extn_field->getText());
  }
  return true;
}



long ToolsDialog::onNewScanChore(FXObject*o, FXSelector sel, void*p)
{
  if (FXSELTYPE(sel)==SEL_CHORE) {
    if (tree->DummyItem()) {
      tree->SetPrevItem(tree->DummyItem());
      tree->scan(true);
      name_field->setFocus();
      getApp()->addTimeout(this,ID_NEW_SCAN_CHORE,ONE_SECOND/10);
    }
  } else {
    name_field->setFocus();
    name_field->selectAll();
  }
  return 1;
}



bool ToolsDialog::SaveChanges()
{
  FXString newpath;
  if (BuildName(newpath,tree->PrevItem()->hasItems())) {
    FXString oldpath=tree->getFilePath(tree->PrevItem());
    if (compare(oldpath,newpath)==0) { return true; }
    if (FXStat::exists(newpath)) {
      FXMessageBox::error(this, MBOX_OK,
        _("Rename error"), "%s %s:\n%s:\n %s\n%s:\n %s\n\n%s",
        _("Failed to rename"),
        tree->PrevItem()->hasItems()?_("folder"):_("file"),
        _("from"),  oldpath.text(),
        _("to"), newpath.text(),
        _("That name is already in use."));
    } else {
      bool success=tree->PrevItem()->hasItems() ? FXDir::rename(oldpath,newpath) : FXFile::rename(oldpath,newpath);
      if (success) {
        tree->SetSavedPath(newpath.text());
        FXStat::mode(newpath, GetPermsForItem(tree->PrevItem()));
        tree->scan(true);
        return true;
      } else {
          FXMessageBox::error(this, MBOX_OK, _("Rename error"), "%s %s:\n%s:\n %s\n%s:\n %s\n\n%s",
            _("Failed to rename"),
            tree->PrevItem()->hasItems()?_("folder"):_("file"),
            _("from"), oldpath.text(),
            _("to"), newpath.text(),
            SystemErrorStr());
      }
    }
  }
  return false;
}

#define IsExecSnippet() ( (comparecase(kind,"snippets")==0) && opt_rad && (!opt_rad->getCheck()) )


FXuint ToolsDialog::GetPermsForItem(FXTreeItem *item)
{
  if (item && item->hasItems()) { return FXIO::OwnerFull; }
  FXString kind=tree->GetUserMenu(item?item:tree->getCurrentItem())->getText();
  return ( (comparecase(kind,"commands")==0) || (comparecase(kind,"filters")==0)  || IsExecSnippet() )?
    FXIO::OwnerFull:FXIO::OwnerReadWrite;
}



long ToolsDialog::onButtonClick(FXObject*o, FXSelector sel, void*p)
{
  switch (FXSELID(sel)) {
    case ID_APPLY_CLICK: {
      SaveChanges();
      break;
    }
    case ID_RESET_CLICK: {
      onTreeListAfterChanged(this,0,tree->PrevItem());
      break;
    }
    case ID_DELETE_CLICK: {
      FXString remove_file;
      if (tree->PrevItem()->hasItems()) {
        if (tree->PrevItem()->getFirst()) {
          FXMessageBox::error(this, MBOX_OK, _("Directory not empty"),
            _("Unable to remove a group that still contains items,\n"
            "you should delete its tools and sub-groups first.")
          );
          return 1;
        } else {
          remove_file=tree->getFilePath(tree->PrevItem());
          if (!FXDir::remove(remove_file)) {
            FXMessageBox::error(this, MBOX_OK, _("Delete failed"), "%s:\n%s\n%s\n",
              _("Unable to remove directory"),
              remove_file.text(), SystemErrorStr()
            );
            return 1;
          }
        }
      } else {
        remove_file=tree->getFilePath(tree->PrevItem());
        if ( (FXStat::size(remove_file)==0)||(FXMessageBox::warning(this,MBOX_YES_NO,
             _("Warning!"), "%s \"%s\" %s?\n\n%s",
             _("Are you sure you want to remove the "),
              tree->PrevItem()->getText().text(),
             _("tool"),
             _("If you choose \"Yes\", the file associated with this\n"
               "item will be permanently deleted from disk!")
             ) == MBOX_CLICKED_YES)
           )
        {
          if (!FXFile::remove(remove_file)) {
            FXMessageBox::error(this, MBOX_OK, _("Delete failed"), "%s:\n%s\n\n%s\n",
              _("Unable to delete file"), remove_file.text(), SystemErrorStr()
            );
            return 1;
          }
        }
      }
      tree->removeItem(tree->PrevItem());
      tree->SetPrevItem(tree->getCurrentItem());
      tree->scan(true);
      break;
    }
    case ID_EDIT_CLICK: {
      ((TopWindow*)getOwner())->OpenFile(
        (const char*)(((FXMenuCommand*)(tree->PrevItem()->getData()))->getUserData()),
        NULL,false,false
      );
      break;
    }
    case ID_NEW_MENU_CLICK: {
      tree->MakeDummyMenu(tree->getCurrentItem());
      getApp()->addChore(this, ID_NEW_SCAN_CHORE);
      break;
    }
    case ID_NEW_TOOL_CLICK: {
      tree->MakeDummyTool(tree->getCurrentItem(),GetPermsForItem());
      getApp()->addChore(this, ID_NEW_SCAN_CHORE);
      break;
    }
    case ID_MOVE_CLICK: {
      FXTreeItem*dst=(FXTreeItem*)move_list->getItemData(move_list->getCurrentItem());
      FXTreeItem*src=tree->getCurrentItem();
      FXString dstname=tree->getFilePath(dst);
      FXString srcname=tree->getFilePath(src);
      dstname.append(PATHSEP);
      dstname.append(FXPath::name(srcname));
      if (FXStat::exists(dstname)) {
        FXMessageBox::error(this, MBOX_OK, _("Naming collision"),
          "%s \"%s\" %s \"%s\":\n\n%s \"%s/%s\" %s",
          _("Cannot move item"),
          src->getText().text(),
          _("to"),
          dst->getText().text(),
          _("An item named"),
          dst->getText().text(),
          src->getText().text(),
          _("aready exists.")
          );
      } else {
        bool success=src->hasItems()?FXDir::rename(srcname,dstname):FXFile::rename(srcname,dstname);
        if (success) {
          tree->setCurrentItem(dst);
          tree->SetPrevItem(dst);
          tree->scan(true);
        } else {
        FXMessageBox::error(this, MBOX_OK, _("Move failed"), "%s:\n%s:\n %s\n%s:\n %s\n\n%s",
          _("Failed to move item"),
          _("from"),
           srcname.text(),
          _("to"),
           dstname.text(),
           SystemErrorStr());
        }
      }
    }
  }
  return 1;
}



void ToolsDialog::create()
{
  FXDialogBox::create();
  FXint disp_wdt=getApp()->getRootWindow()->getWidth()*0.875;
  FXint ctrl_wdt=split->getSplit(1)*2;
  setWidth(disp_wdt<ctrl_wdt?disp_wdt:ctrl_wdt);
  FXint disp_hgt=getApp()->getRootWindow()->getHeight()*0.833;
  if (getDefaultHeight()>disp_hgt) { setHeight(disp_hgt); }
}



ToolsDialog::ToolsDialog(FXTopWindow*win, UserMenu**menus):
  FXDialogBox(win, _("Tools Manager"),DECOR_TITLE|DECOR_BORDER)
{
  FXVerticalFrame*vbox;
  opts_panel=NULL;
  SetPad(this,0);
  vbox = new FXVerticalFrame(this,LAYOUT_FILL);
  SetPad(vbox,0);

  split=new FXSplitter(vbox,LAYOUT_FILL|SPLITTER_REVERSED);

  left_box=new FXVerticalFrame(split,LAYOUT_FILL_Y|LAYOUT_FILL_X|FRAME_SUNKEN|FRAME_THICK);
  SetPad(left_box,0);
  right_box=new FXVerticalFrame(split,LAYOUT_FILL_Y|LAYOUT_FILL_X|FRAME_RAISED|FRAME_SUNKEN);

  intro_lab=new FXLabel(right_box,intro_text,NULL, LABEL_NORMAL|JUSTIFY_LEFT);

  name_panel=new FXVerticalFrame(right_box,FRAME_NONE|LAYOUT_FILL_X);
  props_lab=new FXLabel(name_panel, "",NULL, LAYOUT_FILL_X|JUSTIFY_CENTER_X);
  FXHorizontalFrame*strip;

  strip=new FXHorizontalFrame(name_panel,LAYOUT_FILL_X);
  new FXLabel(strip,_("&Name"));
  name_field=new ClipTextField(strip,24,this,ID_NAMEFIELD,FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X);

  strip=new FXHorizontalFrame(name_panel,LAYOUT_FILL_X);
  new FXLabel(strip,_("Menu &key"));
  menukey_list=new FXListBox(strip,this,ID_MODIFIED);
  menukey_list->appendItem("  ");
  new FXLabel(strip,_("  &Index"));
  index_field=new ClipTextField(strip,2,this,ID_MODIFIED,TEXTFIELD_LIMITED|TEXTFIELD_INTEGER|FRAME_SUNKEN|FRAME_THICK);
  new FXLabel(strip,_("  E&xt'n"));
  extn_field=new ClipTextField(strip,4,this,TEXTFIELD_LIMITED|FRAME_SUNKEN|FRAME_THICK);

  accel_panel=new FXGroupBox(right_box,"Shortcut",FRAME_SUNKEN|FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_X);

  strip=new FXHorizontalFrame(accel_panel,LAYOUT_FILL_X);
  new FXLabel(strip,_("&Hot key:"));
  accel_field=new ClipTextField(strip,3,this,ID_ACCELFIELD,TEXTFIELD_LIMITED|FRAME_SUNKEN|FRAME_THICK);


  ctrl_chk  = new FXCheckButton(strip,_("C&trl"),this,ID_MODIFIED);
  ctrl_chk->setPadLeft(22);
  alt_chk   = new FXCheckButton(strip,_("A&lt"),this,ID_MODIFIED);
  shift_chk = new FXCheckButton(strip,_("Shi&ft"),this,ID_MODIFIED);

  change_panel=new FXHorizontalFrame(right_box,LAYOUT_RIGHT|PACK_UNIFORM_WIDTH);
  apply_btn=new FXButton(change_panel,_(" &Save changes "), NULL, this, ID_APPLY_CLICK);
  reset_btn=new FXButton(change_panel,_(" &Undo changes "), NULL, this, ID_RESET_CLICK);


  strip=new FXHorizontalFrame(right_box,PACK_UNIFORM_WIDTH);
  delete_btn=new FXButton(strip, _(" &Delete Item "), NULL, this, ID_DELETE_CLICK);
  edit_btn=new FXButton(strip, _(" Open in &editor "), NULL, this, ID_EDIT_CLICK);

  move_panel=new FXHorizontalFrame(right_box,FRAME_GROOVE|LAYOUT_FILL_X);
  move_lab=new FXLabel(move_panel,_("Move &to:"));
  move_list=new FXListBox(move_panel,this,ID_MOVELIST_CHOOSE,FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X);
  move_btn=new FXButton(move_panel, _("Mo&ve"),NULL, this, ID_MOVE_CLICK);


  new_panel=new FXVerticalFrame(right_box,FRAME_NONE|LAYOUT_FILL_X|LAYOUT_BOTTOM);
  new_panel->setVSpacing(0);
  strip=new FXHorizontalFrame(new_panel,LAYOUT_FILL_X|PACK_UNIFORM_WIDTH);
  new_menu_btn=new FXButton(strip, _("New group"), NULL, this, ID_NEW_MENU_CLICK,BUTTON_NORMAL|LAYOUT_FILL_X);
  new_tool_btn=new FXButton(strip, _("New tool"), NULL, this, ID_NEW_TOOL_CLICK,BUTTON_NORMAL|LAYOUT_FILL_X);

  strip=new FXHorizontalFrame(vbox,FRAME_GROOVE|LAYOUT_FILL_X);
  SetPad(strip,0)
  helptext=new FXLabel(strip, default_help_text,NULL,FRAME_SUNKEN|FRAME_RAISED|FRAME_THICK|LAYOUT_FILL_X|JUSTIFY_LEFT);
  helptext->setPadLeft(5);
  new FXButton(strip,_(" &Close "), NULL, this,FXDialogBox::ID_ACCEPT,BUTTON_NORMAL|LAYOUT_RIGHT);

  setModified(false);
  tree=new ToolsTree(left_box,this,ID_TREELIST_CHANGED, menus);
}

