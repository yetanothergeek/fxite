/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2009-2011 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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


#include "shady_tabs.h"

typedef enum {
  DOCTAB_CLEAN,
  DOCTAB_DIRTY,
  DOCTAB_LOCKED,
  DOCTAB_SAME,
} DocTabState;

class DocTab: public FXTabItem {
  private:
    FXDECLARE(DocTab)
    DocTab(){}
    bool dnd_accept;
    FXCursor*defaultDragCursor;
    DocTabState _state;
  public:
    long onDnd(FXObject* sender,FXSelector sel, void*p);
    DocTab(FXTabBar*bar, const FXString&text);
    void setDefaultDragCursor(FXCursor*ddc) { defaultDragCursor=ddc; setDragCursor(ddc); }
    void SetIcon(DocTabState state);
    enum {
      ID_DND=FXTabItem::ID_LAST,
      ID_LAST
    };
};



typedef bool (*TabCallback) (FXint index, DocTab*tab, FXWindow*page, void*user_data);


class DocTabs: public ShadyTabs {
  FXDECLARE(DocTabs);
private:
  enum {MOVEUP,MOVEDOWN,MOVETOFIRST,MOVETOLAST};
  FXMenuPane*tab_popup;

  DocTabs(){}
  void MoveTab(FXint how);
  FXint tab_width_max;
  FXuchar tabs_compact;
  bool dnd_accept;
  static bool UpdateTabWidths(FXint index, DocTab*tab, FXWindow*page, void*user_data);
public:
  long onTabPopupMenu ( FXObject* sender, FXSelector sel, void* p );
  long onPopupClick   ( FXObject* sender, FXSelector sel, void* p );
  DocTabs(FXComposite*p,FXObject*trg,FXSelector sel,FXuint opts);
  ~DocTabs();

#ifdef WIN32
  virtual void create();
#else
  long onDnd(FXObject* sender,FXSelector sel, void*p);
#endif
  DocTab*NewTab(FXString text);
  DocTab*ActiveTab();
  FXWindow*ActivePage();
  FXWindow*ActiveView();
  FXWindow*PageAt(FXint n);

  virtual void setCurrent(FXint panel,FXbool notify=false);
  virtual void show();

  bool ActivateTab(DocTab*tab);
  bool ActivateTab(FXint n);

  void MoveTabUp()    { MoveTab(MOVEUP); }
  void MoveTabDown()  { MoveTab(MOVEDOWN); }
  void MoveTabFirst() { MoveTab(MOVETOFIRST); }
  void MoveTabLast()  { MoveTab(MOVETOLAST); }

  void setTabStyle(FXuint style);
  void setTabStyleByChar(FXuchar c);
  void setTabsCompact(FXuchar compact);
  FXuchar getTabsCompact() { return tabs_compact; }
  void FocusNextTab(bool forward);
  void ForEachTab(TabCallback cb, void *user_data, bool hourglass=true);

  FXint Count() { return numChildren()/2; }
  FXint MaxTabWidth() { return tab_width_max; }
  void MaxTabWidth(FXint w);
  enum {
    ID_TAB_POPUP_MENU=FXTabBook::ID_LAST,
    ID_POPUP_CLICK,
    ID_LAST
  };
};

