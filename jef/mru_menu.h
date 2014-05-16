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

#ifndef MRU_MENU_H

# define MRU_MENU_H

class RecentFilesMenu: public FXMenuPane {
  private:
    FXDECLARE(RecentFilesMenu)
    RecentFilesMenu(){}
  protected:
    FXString group;
    FXMenuCascade*casc;
    FXApp*app;
    bool created;
    void add_item(const FXString &txt, bool prepended);
    void append(const FXString &txt);
    FXMenuCommand*find(const FXString &txt);
  public:
    long onItemClick(FXObject*o,FXSelector sel,void*p);
    enum {
      ID_ITEM_CLICK=FXMenuPane::ID_LAST,
      ID_LAST
    };
    RecentFilesMenu(FXWindow *p, const FXString &caption, const FXString &groupname, FXObject *tgt, FXSelector sel);
    ~RecentFilesMenu();
    virtual void create();
    void prepend(const FXString &txt);
    void remove(const FXString &txt);
};

#endif

