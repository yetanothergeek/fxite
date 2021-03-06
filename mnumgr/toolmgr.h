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

#ifndef FXITE_TOOLMGR_H
#define FXITE_TOOLMGR_H

#include "tooltree.h"

class UserMenu;
class ShortcutEditor;

class ToolsDialog: public FXDialogBox {
private:
  FXDECLARE(ToolsDialog)
  ToolsDialog(){}

  FXVerticalFrame* left_box;
  FXVerticalFrame* right_box;
  FXVerticalFrame* name_panel;
  ShortcutEditor*accel_panel;
  FXGroupBox* opts_panel;
  FXVerticalFrame* new_panel;
  FXHorizontalFrame* change_panel;
  FXHorizontalFrame* move_panel;

  FXTextField* index_field;
  FXTextField* name_field;
  FXTextField* extn_field;
  FXListBox* menukey_list;
  FXCheckButton* opt_chk;
  FXRadioButton* opt_rad;
  FXLabel* props_lab;
  FXLabel* intro_lab;
  FXButton* reset_btn;
  FXButton* apply_btn;
  FXButton* delete_btn;
  FXButton* edit_btn;
  FXButton* new_menu_btn;
  FXButton* new_tool_btn;

  FXLabel* move_lab;
  FXListBox* move_list;
  FXButton* move_btn;
  FXLabel* helptext;
  FXSplitter*split;

  void clear();

  void create_options(FXTreeItem*item, const FXString &flag);
  bool modified;
  void setModified(bool ismod);
  bool BuildName(FXString &path,bool isdir);
  bool SaveChanges();
  void SetMoveOptions(FXTreeItem *item);
  FXuint GetPermsForItem(FXTreeItem *item=NULL);
protected:
  ToolsTree* tree;
  FXTreeItem*PrevItem() { return tree->PrevItem(); }
  virtual bool CanRename(const FXString &oldpath, const FXString &newpath);
  virtual bool RenameItem(const FXString &oldpath, const FXString &newpath);
  virtual void EditFile() {}
public:
  ToolsDialog(FXTopWindow*win, UserMenu**menus);
  virtual void create();
  long onTreeListChanged(FXObject*o, FXSelector sel, void*p);
  long onTreeListAfterChanged(FXObject*o, FXSelector sel, void*p);
  long onAccelField(FXObject*o, FXSelector sel, void*p);
  long onNameField(FXObject*o, FXSelector sel, void*p);
  long onChooseOpt(FXObject*o, FXSelector sel, void*p);
  long onModified(FXObject*o, FXSelector sel, void*p);
  long onButtonClick(FXObject*o, FXSelector sel, void*p);
  long onNewScanChore(FXObject*o, FXSelector sel, void*p);
  long onClose(FXObject*o, FXSelector sel, void*p);
  long onMoveListChoose(FXObject*o, FXSelector sel, void*p);
  enum {
    ID_TREELIST_CHANGED=FXDialogBox::ID_LAST,
    ID_ACCELFIELD,
    ID_NAMEFIELD,
    ID_OPTCHOOSE,
    ID_TREELIST_CHORE,
    ID_NEW_SCAN_CHORE,
    ID_MOVELIST_CHOOSE,
    ID_MODIFIED,
    ID_APPLY_CLICK,
    ID_RESET_CLICK,
    ID_MOVE_CLICK,
    ID_DELETE_CLICK,
    ID_EDIT_CLICK,
    ID_NEW_MENU_CLICK,
    ID_NEW_TOOL_CLICK,
    ID_LAST
  };
};

#endif

