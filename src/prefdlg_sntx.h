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



class LangGUI:public FXVerticalFrame {
private:
  FXDECLARE(LangGUI)
  LangGUI(){}
  Settings* prefs;
  FXListBox *wordlist;
  FXListBox *langlist;
  FXListBox* tabopts;
  FXHorizontalFrame*style_hdr;
  FXScrollWindow*scroll;
  FXMatrix* style_pan;
  FXButton *wildcardbtn;
  FXButton *shabangbtn;
  FXGroupBox*kwordsgrp;
  FXButton*wordbtn;
  FXLabel *taboptlab;
  FXSpinner* tabwidthspin;
  FXLabel* tabwidthlab;
  FXTabBook *syntabs;
  FXTabItem *opts_tab;
  FXFont*scifont;

  void MakeOptsTab();
  void MakeStyleTab();
public:
  long onLangSwitch(FXObject*o,FXSelector sel,void*p);
  long onKeywordEdit(FXObject*o,FXSelector sel,void*p);
  long onTabOptsSwitch(FXObject*o,FXSelector sel,void*p);
  long onSyntaxFiletypeEdit(FXObject*o,FXSelector sel,void*p);
  LangGUI(FXComposite*o, Settings* aprefs, FXObject*trg, FXSelector sel);
  ~LangGUI();
  enum {
    ID_LANG_SWITCH=FXVerticalFrame::ID_LAST,
    ID_EDIT_FILETYPES,
    ID_EDIT_SHABANGS,
    ID_KWORD_EDIT,
    ID_TABOPTS_SWITCH
  };
};
