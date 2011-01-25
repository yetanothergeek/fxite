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

class Settings;
class StyleEdit;
class SciSearchOptions;

class PrefsDialog: public FXDialogBox {
private:
  FXDECLARE(PrefsDialog)
  PrefsDialog(){}
  friend class StyleEdit;
  FXTabBook*tabs;
  Settings*prefs;
  FXListBox *langlist;
  FXIconList*acclist;
  FXTextField* wildcardbox;
  FXTextField* shabangbox;
  FXListBox *wordlist;
  FXLabel *wildcardlab;
  FXLabel *shabanglab;
  FXLabel *kwordslab;
  FXLabel *taboptlab;
  FXListBox* tabopts;
  FXButton*wordbtn;
  FXVerticalFrame* style_pan;
  FXScrollWindow*scroll;
  FXuint maxw;
  SciSearchOptions*srchopts;
  FXFont*scifont;
  FXTextField* filters_edit;
  FXButton* tbar_ins_btn;
  FXButton* tbar_rem_btn;
  FXButton* tbar_custom_btn;
  FXButton* tbar_raise_btn;
  FXButton* tbar_lower_btn;
  FXList*   tbar_avail_items;
  FXList*   tbar_used_items;

  void MakeSyntaxTab();
  void MakeGeneralTab();
  void MakeEditorTab();
  void MakeKeybindingsTab();
  void MakeToolbarTab();
  void MakeThemeTab();
  FXHorizontalFrame*style_hdr;
  FXLabel* hint_lab;
  FXMainWindow* main_win;
  void AddToolbarButton(FXListItem*item, FXint &iUsed, FXint&nUsed);
public:
  long onTabSwitch(FXObject*o,FXSelector sel,void*p);
  long onLangSwitch(FXObject*o,FXSelector sel,void*p);
  long onTabOptsSwitch(FXObject*o,FXSelector sel,void*p);
  long onKeywordEdit(FXObject*o,FXSelector sel,void*p);
  long onAccelEdit(FXObject*o,FXSelector sel,void*p);
  long onFiltersEdit(FXObject*o,FXSelector sel,void*p);
  long onToolbarEdit(FXObject*o,FXSelector sel,void*p);
  enum {
    ID_SOMETHING=FXDialogBox::ID_LAST,
    ID_TAB_SWITCHED,
    ID_LANG_SWITCH,
    ID_TABOPTS_SWITCH,
    ID_KWORD_EDIT,
    ID_ACCEL_EDIT,
    ID_FILTERS_EDIT,
    ID_TBAR_AVAIL_ITEMS,
    ID_TBAR_USED_ITEMS,
    ID_TBAR_ITEM_INSERT,
    ID_TBAR_ITEM_REMOVE,
    ID_TBAR_ITEM_RAISE,
    ID_TBAR_ITEM_LOWER,
    ID_TBAR_INSERT_CUSTOM,
    ID_LAST
  };
  PrefsDialog(FXMainWindow* w, Settings* aprefs);
  virtual void create();
  virtual ~PrefsDialog();
  static FXuint ChangedToolbar();
};

