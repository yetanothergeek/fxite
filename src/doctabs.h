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


class DocTab: public FXTabItem {
  private:
    FXDECLARE(DocTab)
    DocTab(){}
    bool dnd_accept;
    FXCursor*defaultDragCursor;
  public:
    long onDnd(FXObject* sender,FXSelector sel, void*p);
    DocTab(FXTabBar*bar, const FXString&text);
    void setDefaultDragCursor(FXCursor*ddc) { defaultDragCursor=ddc; setDragCursor(ddc); }
    enum {
      ID_DND=FXTabItem::ID_LAST,
      ID_LAST
    };
};



typedef bool (*TabCallback) (FXint index, DocTab*tab, FXWindow*page, void*user_data);


class DocTabs: public FXTabBook {
  FXDECLARE(DocTabs);
private:
  enum {MOVEUP,MOVEDOWN,MOVETOFIRST,MOVETOLAST};
  FXFont *normal_font;
  FXFont *bold_font;
  FXFontDesc desc;
  FXColor hot_fg;
  FXColor hot_bg;
  FXColor cold_fg;
  FXColor cold_bg;
  FXMenuPane*tab_popup;

  DocTabs(){}
  void setTabColor(DocTab*t, bool active);
  void UpdateTabs();
  void MoveTab(FXint how);

public:
  long onCmdOpenItem  ( FXObject* sender, FXSelector sel, void* p );
  long onTabPopupMenu ( FXObject* sender, FXSelector sel, void* p );
  long onPopupClick   ( FXObject* sender, FXSelector sel, void* p );
  DocTabs(FXComposite*p,FXObject*trg,FXSelector sel,FXuint opts);
  ~DocTabs();

  DocTab*NewTab(FXString text);
  DocTab*ActiveTab();
  FXWindow*ActivePage();
  FXWindow*ActiveView();
  FXWindow*PageAt(FXint n);

  void setCurrent(FXint i, FXbool notify=FALSE);
  bool ActivateTab(DocTab*tab);
  bool ActivateTab(FXint n);

  void MoveTabUp()    { MoveTab(MOVEUP); }
  void MoveTabDown()  { MoveTab(MOVEDOWN); }
  void MoveTabFirst() { MoveTab(MOVETOFIRST); }
  void MoveTabLast()  { MoveTab(MOVETOLAST); }

  void setTabStyle(FXuint style);
  void setTabsCompact(bool compact);
  void FocusNextTab(bool forward);
  void ForEachTab(TabCallback cb, void *user_data, bool hourglass=true);
  FXint Count() { return numChildren()/2; }
  enum {
    ID_TAB_POPUP_MENU=FXTabBook::ID_LAST,
    ID_POPUP_CLICK,
    ID_LAST
  };
};


