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

#ifndef MNUMGR_CFG_KEYBIND_H
#define MNUMGR_CFG_KEYBIND_H

#include "menuspec.h"

class ShortcutEditor;

class ShortcutList: public FXHorizontalFrame {
  FXDECLARE(ShortcutList)
  ShortcutList() {}
  void UpdateListItem(MenuSpec*spec);
  FXIcon*arrow;
protected:
  FXIconList*acclist;
  ShortcutEditor*sce;
  FXVerticalFrame*menupath;
  FXButton*apply_btn;
  FXButton*remove_btn;
  MenuMgr*mnumgr;
  FXWindow*win;
  virtual bool ConfirmOverwrite(FXHotKey acckey, MenuSpec*spec);
  virtual bool DeleteShortcut(MenuSpec*spec);
  virtual void ApplyShortcut(FXHotKey acckey, MenuSpec*spec);
  virtual bool Verify(FXHotKey acckey, MenuSpec*spec);
public:
  ShortcutList(FXComposite*o, MenuMgr*mmgr, FXWindow*w, FXuint opts=0);
  ~ShortcutList();
  long onQueryTip(FXObject*o,FXSelector sel,void*p);
  long onSelectKeybinding(FXObject*o,FXSelector sel,void*p);
  long onEditKeybinding(FXObject*o,FXSelector sel,void*p);
  long onApplyChanges(FXObject*o,FXSelector sel,void*p);
  long onRemoveKeybinding(FXObject*o,FXSelector sel,void*p);
  virtual void create();
  enum {
    ID_SEL_KEYBIND=FXHorizontalFrame::ID_LAST,
    ID_EDIT_KEYBIND,
    ID_APPLY_CHANGES,
    ID_REMOVE_KEYBIND,
    ID_LAST
  };
};


#endif
