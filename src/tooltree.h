/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2009 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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


class ToolsDialog;
class UserMenu;


class ToolsTree: public FXTreeList {
private:
  FXTreeItem* prev_item;
  void build_tree(FXTreeItem*item, const FXPopup*menu);
  UserMenu** menu_list;
  FXTreeItem *tools;
  FXTreeItem* dummy_item;
  FXTreeItem* restored_item;
  FXString saved_path;
protected:
  friend class ToolsDialog;
  const char* SavedPath() {return saved_path.text(); }
  void SetSavedPath(const char* sp)  { saved_path=sp; }
  FXTreeItem*Tools() { return tools; }
  FXTreeItem*PrevItem() { return prev_item; }
  FXTreeItem* DummyItem() { return dummy_item; }
  void SetPrevItem(FXTreeItem*prv) { prev_item=prv; }
  void scan(bool rebuild);
  void MakeDummyMenu(FXTreeItem*parent_item);
  void MakeDummyTool(FXTreeItem*parent_item, FXuint perm);
public:
  ToolsTree(FXComposite*p, FXObject*tgt, FXSelector sel, UserMenu**menus);
  static const char*getFilePath(FXTreeItem*item);
  UserMenu*GetUserMenu(FXTreeItem*item);
  static bool SelectTool(FXWindow* owner, UserMenu** menus, FXMenuCommand*&mc);
};

