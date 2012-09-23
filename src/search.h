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
  FXListBox*modelist;
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


class SciReplPan;


class SearchDialogs: public FXObject {
  FXDECLARE(SearchDialogs);
  SearchDialogs() {}
private:
  bool FindText(bool forward, bool wrap);
  bool DoFind(bool forward);
  FXHorizontalFrame*srchpan;
  SciReplPan*find_dlg;
  SciReplPan*repl_dlg;
  FXComposite*parent;
  FXSelector message;
  FXObject*target;
  bool find_initial;
  bool repl_initial;
  bool repl_ready;
  FXuint NextSearch(FXuint code);
  FXuint NextReplace(FXuint code, bool forward);
  FXString replacestring;
  bool SearchFailed();
public:
  static void SearchFailed(FXWindow*w);
  static bool SearchWrapAsk(FXWindow*w);
  SearchWrapPolicy searchwrap;
  bool searchverbose;
  FXuint searchmode;
  FXuint searchdirn;
  FXuint defaultsearchmode;
  FXString searchstring;
  void SetPrefs(FXuint mode, FXuint wrap, bool verbose);
  void ShowFindDialog();
  void ShowReplaceDialog();
  void FindNext();
  void FindPrev();
  void FindPhrase(const char* searchfor, FXuint mode, bool forward);
  void FindAndReplace(const char*searchfor, const char*replacewith, FXuint searchmode, bool forward);
  void ReplaceAllInSelection(const char*searchfor, const char*replacewith, FXuint searchmode);
  void ReplaceAllInDocument(const char*searchfor, const char*replacewith, FXuint searchmode);
  void FindSelected(bool forward);
  void setHaveSelection(bool have_sel);
  bool GoToSelected();
  bool ShowGoToDialog();
  FXDialogBox*FindDialog() { return (FXDialogBox*)find_dlg; }
  SearchDialogs(FXComposite*p, FXObject*trg=NULL, FXSelector sel=0);
  void setSelector(FXSelector sel) { message=sel; }
  void setTarget(FXObject*trg) { target=trg; }
  virtual ~SearchDialogs();
  long onSearch(FXObject*o, FXSelector sel, void *p);
  long onReplDone(FXObject*o, FXSelector sel, void *p);
  void hide();
  enum {
    ID_SEARCH=1,
    ID_REPL_DONE,
    ID_LAST
  };
};


bool GetPrimarySelection(FXString&target);

