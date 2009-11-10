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
protected:
  int count;
  void ScanDir(FXMenuPane*parent, const char *directory);
  void create();
  void DeletePanes();
public:
  FXWindow*getTarget() { return target; }
  FXSelector getSelector() { return selector; }
  int getCount() { return count; }
  UserMenu(FXMenuPane *owner,
    const FXString &label, const FXString &dirname, FXObject *tgt, FXSelector sel, const char**flags=NULL);
  ~UserMenu();
  void rescan();
  void enable();
  void disable();
  FXString getText();
  void setText(const FXString &s);
  const char** getFlags() const { return _flags; };
  const FXMenuPane*menu() const { return (panes.no()>0)?panes[0]:NULL; }
  FXString helptext;
  void helpText(const char*txt) { helptext=txt; }
  const char*getTopDir(){ return topdir.text(); }
};




class HistMenu: public FXMenuPane {
  private:
    FXDECLARE(HistMenu)
    HistMenu(){}
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
    HistMenu(FXWindow *p, const FXString &caption, const FXString &groupname, FXObject *tgt, FXSelector sel);
    ~HistMenu();
    virtual void create();
    void prepend(const FXString &txt);
    void remove(const FXString &txt);
};




