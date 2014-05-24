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

#ifndef FXITE_PREFDLG_KBND
#define FXITE_PREFDLG_KBND

#include "menuspec.h"

class KeyBindingList: public FXIconList {
protected:
  FXDECLARE(KeyBindingList);
  KeyBindingList(){}
protected:
  MenuMgr*mnumgr;
  FXWindow*win;
  virtual bool AccelUnique(FXHotKey acckey, MenuSpec*spec);
  virtual bool AccelDelete(MenuSpec*spec);
  virtual void AccelInsert(FXHotKey acckey, MenuSpec*spec);
public:
  KeyBindingList(FXComposite*o, MenuMgr*mmgr, FXWindow*w);
  long onQueryTip(FXObject* sender,FXSelector,void*);
  long onAccelEdit(FXObject* sender,FXSelector,void*);
  enum {
    ID_ACCEL_EDIT=FXIconList::ID_LAST,
    ID_LAST
  };
};


#endif
