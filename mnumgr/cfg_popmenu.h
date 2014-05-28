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

#ifndef MNUMGR_CFG_POPMENU_H
#define MNUMGR_CFG_POPMENU_H


#include "dual_list.h"

class UserMenu;
class ToolBarFrame;
class MenuMgr;

class PopupPrefs: public DualListForm {
private:
  FXDECLARE(PopupPrefs);
  PopupPrefs(){}
  FXButton* custom_btn;
  FXButton* separator_btn;
  virtual void PopulateAvail();
  virtual void PopulateUsed();
  virtual void CheckCount();
  MenuMgr*mnumgr;
  UserMenu**user_menus;
  FXint invalid;
public:
  PopupPrefs(FXComposite*p, UserMenu**ums, MenuMgr*mmgr);
  long onInsertCustomItem( FXObject*o, FXSelector sel, void*p);
  long onInsertSeparator(  FXObject*o, FXSelector sel, void*p);
  long onQueryTip(FXObject*o, FXSelector sel, void*p);
  enum {
    ID_INSERT_CUSTOM=DualListForm::ID_LAST,
    ID_INSERT_SEPARATOR,
    ID_LAST
  };
};


#endif