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


class SciSearchOptions: public FXHorizontalFrame {
  FXDECLARE(SciSearchOptions)
protected:
  SciSearchOptions(){}
  FXCheckButton*matchcase;
  FXCheckButton*wholeword;
  FXCheckButton*regexp;
public:
  SciSearchOptions(FXComposite *p, FXObject *tgt=NULL, FXSelector sel=0);
  long onToggleChk(FXObject*o, FXSelector sel, void*p);
  void SetSciFlags(FXuint n);
  enum {
    ID_TOGGLE_CHK=FXHorizontalFrame::ID_LAST,
    ID_EXTERN_MOD,
    ID_LAST
  };
};


typedef enum {
  SEARCH_WRAP_NEVER,
  SEARCH_WRAP_ALWAYS,
  SEARCH_WRAP_ASK
} SearchWrapPolicy;

class SciReplDlg;

class SearchDialogs: public FXObject {
  FXDECLARE(SearchDialogs);
  SearchDialogs() {}
private:
  bool DoFind(SciReplDlg*dlg, SciDoc*sci, bool forward);
  bool SearchFailed();
  SciReplDlg *find_dlg;
  FXWindow*_shell;
public:
  SearchWrapPolicy searchwrap;
  bool searchverbose;
  FXuint searchmode;
  FXuint searchdirn;
  FXuint defaultsearchmode;
  FXString searchstring;
  void SetPrefs(FXuint mode, FXuint wrap, bool verbose);
  bool ShowFindDialog(SciDoc*sci);
  void ShowReplaceDialog(SciDoc*sci);
  bool FindNext(SciDoc*sci);
  bool FindPrev(SciDoc*sci);
  bool FindPhrase(SciDoc*sci, const char* phrase, FXuint mode, bool forward);
  void FindSelected(SciDoc*sci, bool forward);
  bool GoToSelected(SciDoc*sci);
  bool ShowGoToDialog(SciDoc*sci);
  FXDialogBox*FindDialog() { return (FXDialogBox*)find_dlg; }
  SearchDialogs(FXWindow*shell);
  virtual ~SearchDialogs();
};


bool GetPrimarySelection(SciDoc*sci, FXString&target);

