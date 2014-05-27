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

#ifndef FXITE_MENUDEFS_H
#define FXITE_MENUDEFS_H

#include "menuspec.h"
#include "toolmgr.h"
#include "toolbar.h"


class MyMenuMgr: public MenuMgr {
public:
  MyMenuMgr();
  void UpdateEolMenu(SciDoc*sci);
  void SetFileFormat(SciDoc*sci, FXSelector sel);
  char SetTabOrientation(FXSelector sel);
  void ShowPopupMenu(FXPoint*pt);
};


class MyToolsDialog: public ToolsDialog {
  FXString filelist;
protected:
  virtual bool CanRename(const FXString &oldpath, const FXString &newpath);
  virtual bool RenameItem(const FXString &oldpath, const FXString &newpath);
  virtual void EditFile();
public:
  MyToolsDialog(FXTopWindow*win, UserMenu**menus):ToolsDialog(win,menus){}
};


class MyToolBarFrame: public ToolBarFrame {
  FXDECLARE(MyToolBarFrame);
  static void EnableFilterBtnCB(FXButton*btn, void*user_data);
protected:
  MyToolBarFrame(){};
public:
  MyToolBarFrame(FXComposite *o, bool hideit):ToolBarFrame(o,hideit){}
  void EnableFilterBtn(bool enabled);
};


#endif

