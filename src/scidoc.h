/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2009-2010 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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


typedef struct _StyleDef StyleDef;
typedef struct _LangStyle LangStyle;


enum {
  SPLIT_NONE,
  SPLIT_BELOW,
  SPLIT_BESIDE
};

class SciSearch;


class SciDoc : public FXScintilla {
  FXDECLARE(SciDoc)
  friend class SciSearch;
private:
  LangStyle *_lang;
  FXString _filename;
  const char*_openers;
  const char*_closers;
  FXString _lasterror;
  char _caretlinealpha[16];
  char _caretlinebg[16];
  char _whitespacebg[16];
  char _whitespacefg[16];
  char _rightmarginbg[16];
  char _caretfg[16];
  char _selectionbg[16];

  FXTime _filetime;
  FXint splitter_style;
  unsigned char user_undo_level;

  unsigned char _loading:1;
  unsigned char _dirty:1;
  unsigned char check_stale:1;
  unsigned char need_backup:1;
  unsigned char need_styled:1;
  unsigned char _utf8:1;
protected:
  SciDoc(){}
  inline bool IsBrace(long &pos);
  void GetSelection(CharacterRange &crange);
  bool DoLoadFromFile(const char*filename,bool insert=false);
  void AdjustHScroll();
public:
  SciDoc*Master() { return (SciDoc*)getPrev(); }
  SciDoc*Slave() { return (SciDoc*)getNext(); }
  SciSearch *search;
  long onKeyPress(FXObject *o, FXSelector sel, void *p);
  virtual void moveContents(FXint x,FXint y);
  SciDoc(FXComposite*p, FXObject*tgt=NULL, FXSelector sel=0);
  ~SciDoc();

  bool GtLtIsBrace(); /* Whether to consider <> for brace matching */
  void GtLtIsBrace(bool gtlt);
  void MatchBrace();
  void setFont(const FXString &font, int size);

  bool setLanguage(const char*name);
  bool setLanguage(LangStyle*ls);
  bool setLanguageFromFileName(const char*ext);
  bool setLanguageFromContent();
  LangStyle* getLanguage() { return _lang; }
  static void DefaultStyles(StyleDef*styles);
  static StyleDef* DefaultStyles();

  bool Loading() { return Master()?Master()->Loading():_loading; }
  bool Dirty() { return Master()?Master()->Dirty():_dirty; }
  void Dirty(bool dirty) { _dirty=dirty; if (Master()) { Master()->Dirty(dirty); } }
  bool NeedBackup() { return Master()?Master()->NeedBackup():need_backup; }
  void NeedBackup(bool need) { need_backup=need; if (Master()) { Master()->NeedBackup(need); } }

  void NeedStyled(bool need) { need_styled=need; }
  bool NeedStyled() { return need_styled; }

  int Stale(); /* 0=ok; 1=modified; 2=deleted; */
  void DoStaleTest(bool doit) { check_stale=doit; if (Master()) { Master()->DoStaleTest(doit); } }
  bool DoStaleTest() { return Master()?Master()->DoStaleTest():check_stale; }
  FXString Filename() { return Master()?Master()->Filename():_filename; }
  FXString GetLastError() { return Master()?Master()->GetLastError():_lasterror; }

  bool InsertFile(const char*filename) { return DoLoadFromFile(filename, true); }
  bool LoadFromFile(const char*filename) { return DoLoadFromFile(filename, false); }
  bool SaveToFile(const char*filename, bool as_itself=true);

  long GetSelLength();
  long GetSelText(FXString&txt);
  void SetSelText(const FXString&source);
  long WordAtPos(FXString&s, long pos=-1);
  long CharAt(long pos) { return sendMessage(SCI_GETCHARAT,pos,0); }
  long GetTextLength() { return sendMessage(SCI_GETLENGTH,0,0); }
  long GetText(FXString&txt);
  void SetText(const char *source);
  bool GoToStringCoords(const char*coords);
  void GoToPos(long pos);
  void GoToCoords(long row, long col);
  void ScrollCaret(long pos=-1);
  int GetLineLength(int line);
  long GetLineText(long linenum, FXString &text);
  long GetLineCount() { return sendMessage(SCI_GETLINECOUNT,0,0); }
  void SetLineIndentation(int line, int indent);
  long GetCaretPos() { return sendMessage(SCI_GETCURRENTPOS,0,0); }
  long GetLineNumber() { return sendMessage(SCI_LINEFROMPOSITION, GetCaretPos(), 0); }
  long GetColumnNumber() { return sendMessage(SCI_GETCOLUMN, GetCaretPos(), 0); }
  void Zoom(int direction);
  void CaretLineBG(const char*bgcolor);
  void CaretLineAlpha(const char*alpha); // <= This really screws up CaretLineBG()
  const char* CaretLineBG() {return _caretlinebg[0]?_caretlinebg:NULL; }

  void RightMarginBG(const char*bgcolor);
  const char* RightMarginBG() {return _rightmarginbg[0]?_rightmarginbg:NULL; }

  void WhiteSpaceBG(const char*bgcolor);
  void WhiteSpaceFG(const char*fgcolor);

  void CaretFG(const char*fgcolor);
  void SelectionBG(const char*bgcolor);

  const char* WhiteSpaceBG() {return _whitespacebg[0]?_whitespacebg:NULL; }

  void UpdateStyle();

  bool GetReadOnly() { return sendMessage(SCI_GETREADONLY,0,0); }

  bool ShowWhiteSpace() { return sendMessage(SCI_GETVIEWWS,0,0)!=SCWS_INVISIBLE; }
  void ShowWhiteSpace(bool showit);

  int TabWidth() { return sendMessage(SCI_GETTABWIDTH,0,0); }
  void TabWidth(int w);
  void ShowLineNumbers(bool showit);
  bool ShowLineNumbers() { return sendMessage(SCI_GETMARGINWIDTHN, 0, 0)?true:false; }

  void SetEdgeColumn(int edge) { sendMessage(SCI_SETEDGECOLUMN,edge,0); }
  int GetEdgeColumn() { return sendMessage(SCI_GETEDGECOLUMN,0,0); }
  bool GetShowEdge() { return sendMessage(SCI_GETEDGEMODE,0,0)!=EDGE_NONE; }
  void SetShowEdge(bool showit) {sendMessage(SCI_SETEDGEMODE, showit?EDGE_LINE:EDGE_NONE,0); }
  bool GetShowIndent() { return sendMessage(SCI_GETINDENTATIONGUIDES,0,0)!=SC_IV_NONE; }
  void SetShowIndent(bool showit) { sendMessage(SCI_SETINDENTATIONGUIDES,showit?SC_IV_LOOKBOTH:SC_IV_NONE,0); }

  void SmartHome(bool smart);

  bool UseTabs() { return sendMessage(SCI_GETUSETABS, 0, 0)?true:false; }
  void UseTabs(bool use) {sendMessage(SCI_SETUSETABS, use, 0); }
  void CaretWidth(int w)  { sendMessage(SCI_SETCARETWIDTH, w<1?1:w>3?3:w, 0); }
  int CaretWidth() { return sendMessage(SCI_GETCARETWIDTH, 0, 0); }

  bool SmoothScroll() { return (getScrollStyle()&SCROLLERS_DONT_TRACK)==0; }
  void SmoothScroll(bool smooth) {
    setScrollStyle(smooth?(getScrollStyle()&!SCROLLERS_DONT_TRACK):(getScrollStyle()|SCROLLERS_DONT_TRACK));
  }
  void SetSplit(FXint style);
  FXint GetSplit() { return splitter_style; }
  static const char* BinaryFileMessage();
  void SetEolModeFromContent();
  void SetUTF8(bool utf8);
  bool GetUTF8() { return _utf8; }
  void SetUserUndoLevel(FXint action); // 1:push level; -1: pop level; 0:reset level
  void SetProperty(const char*key, const char*value);
  bool GetProperty(const FXString &key, FXString &value, bool expanded=false);
  int GetPropertyInt(const char*key, int default_value);
  void SetWordWrap(bool on);
  bool GetWordWrap();
};

#define sendString(iMessage, wParam, lParam) sendMessage(iMessage, wParam, reinterpret_cast<long>(lParam))



