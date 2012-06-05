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

class Settings;
class StyleEdit;
class SciSearchOptions;
class LangGUI;

class PrefsDialog: public FXDialogBox {
private:
  FXDECLARE(PrefsDialog)
  PrefsDialog(){}
  friend class StyleEdit;
  FXTabBook*tabs;
  Settings*prefs;
  FXIconList*acclist;
  SciSearchOptions*srchopts;
  FXTextField* filters_edit;
  LangGUI* langs;

  void MakeSyntaxTab();
  void MakeGeneralTab();
  void MakeEditorTab();
  void MakeKeybindingsTab();
  void MakeToolbarTab();
  void MakePopupTab();
  void MakeThemeTab();
  FXLabel* hint_lab;
  FXMainWindow* main_win;
public:
  long onTabSwitch(FXObject*o,FXSelector sel,void*p);
  long onAccelEdit(FXObject*o,FXSelector sel,void*p);
  long onFiltersEdit(FXObject*o,FXSelector sel,void*p);
  long onErrPatsEdit(FXObject*o,FXSelector sel,void*p);
  long onSysIncsEdit(FXObject*o,FXSelector sel,void*p);
  long onChooseFont(FXObject*o,FXSelector sel,void*p);
  long onChangedToolbar(FXObject*o,FXSelector sel,void*p);
  enum {
    ID_SOMETHING=FXDialogBox::ID_LAST,
    ID_TAB_SWITCHED,
    ID_LANG_SWITCH,
    ID_ACCEL_EDIT,
    ID_FILTERS_EDIT,
    ID_ERRPATS_EDIT,
    ID_SYSINCS_EDIT,
    ID_CHOOSE_FONT,
    ID_CHANGED_TOOLBAR,
    ID_LAST
  };
  PrefsDialog(FXMainWindow* w, Settings* aprefs);
  virtual void create();
  static FXuint ChangedToolbar();
};

