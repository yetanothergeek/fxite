/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2009-2012 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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


#include "dual_list.h"

class ToolbarPrefs: public DualListForm {
private:
  FXDECLARE(ToolbarPrefs);
  ToolbarPrefs(){}
  FXButton* custom_btn;
  virtual void PopulateAvail();
  virtual void PopulateUsed();
  virtual void CheckCount();
  void NotifyChanged(FXuint what);
  Settings* prefs;
  UserMenu**user_menus;
  FXSelector invalid;
public:
  long onRemoveItem(FXObject*o, FXSelector sel, void*p);
  long onInsertCustomItem(FXObject*o, FXSelector sel, void*p);
  long onChangeBtnSize(FXObject*o,FXSelector sel,void*p);
  long onChangeBtnWrap(FXObject*o,FXSelector sel,void*p);
  ToolbarPrefs(FXComposite*p, UserMenu**ums, FXSelector last, FXObject*tgt=NULL, FXSelector sel=0);
  enum {
    ID_INSERT_CUSTOM=DualListForm::ID_LAST,
    ID_CHANGE_BTN_WRAP,
    ID_CHANGE_BTN_SIZE,
    ID_LAST
  };
};



class PopupPrefs: public DualListForm {
private:
  FXDECLARE(PopupPrefs);
  PopupPrefs(){}
  FXButton* custom_btn;
  FXButton* separator_btn;
  virtual void PopulateAvail();
  virtual void PopulateUsed();
  virtual void CheckCount();
  UserMenu**user_menus;
  FXSelector invalid;
public:
  PopupPrefs(FXComposite*p, UserMenu**ums, FXSelector lastid, FXObject*tgt=NULL, FXSelector sel=0);
  long onInsertCustomItem( FXObject*o, FXSelector sel, void*p);
  long onInsertSeparator(  FXObject*o, FXSelector sel, void*p);
  enum {
    ID_INSERT_CUSTOM=DualListForm::ID_LAST,
    ID_INSERT_SEPARATOR,
    ID_LAST
  };
};

