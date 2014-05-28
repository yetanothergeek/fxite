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

#ifndef MNUMGR_SHMENU_H
#define MNUMGR_SHMENU_H


class UserMenuHelper;

class UserMenu: public FXObject {
  FXDECLARE(UserMenu)
  UserMenu(){}
  friend class UserMenuHelper;
private:
  bool hasitems;
  FXObjectListOf<FXMenuPane> panes;
  FXMenuCascade*topcasc;
  FXString toplabel;
  FXString topdir;
  FXMenuPane *topowner;
  int level;
  FXWindow *target;
  FXSelector selector;
  const char**_flags;
  char _tag;
  int count;
  void ScanDir(FXMenuPane*parent, const char *directory);
  void create();
  void DeletePanes();
public:
  FXWindow*getTarget() { return target; }
  FXSelector getSelector() { return selector; }
  int getCount() { return count; }
  UserMenu(FXMenuPane *owner,
    const FXString &label, const FXString &dirname, char tag, FXObject *tgt, FXSelector sel, const char**flags=NULL);
  ~UserMenu();
  void rescan();
  void enable();
  void disable();
  const FXString getText();
  void setText(const FXString &s);
  const char** getFlags() const { return _flags; };
  const FXMenuPane*menu() const { return (panes.no()>0)?panes[0]:NULL; }
  FXString helptext;
  void helpText(const char*txt) { helptext=txt; }
  const char*getTopDir(){ return topdir.text(); }
  static bool MakeLabelFromPath(const char*path, FXString &label);
  char Tag() { return _tag; }
};

#endif

