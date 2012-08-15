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


#include <fx.h>

#include "compat.h"
#include "scidoc.h"
#include "scisrch.h"
#include "histbox.h"

#include "intl.h"
#include "search.h"


FXDEFMAP(SciSearchOptions) SciSearchOptionsMap[]={
  FXMAPFUNC(SEL_COMMAND,SciSearchOptions::ID_TOGGLE_CHK,SciSearchOptions::onToggleChk),
};

FXIMPLEMENT(SciSearchOptions,FXHorizontalFrame,SciSearchOptionsMap,ARRAYNUMBER(SciSearchOptionsMap))



void SciSearchOptions::SetSciFlags(FXuint n)
{
  // Favor REGEXP over WHOLEWORD
  if ((n&SCFIND_WHOLEWORD)&&(n&SCFIND_REGEXP)) {
    n&=!SCFIND_WHOLEWORD;
  }
  matchcase->setCheck((n & SCFIND_MATCHCASE)?1:0);
  wholeword->setCheck((n & SCFIND_WHOLEWORD)?1:0);
  regexp->setCheck((n & SCFIND_REGEXP)?1:0);
}



long SciSearchOptions::onToggleChk(FXObject*o, FXSelector sel, void*p)
{
  FXuint n=0;
  // regexp and wholeword can't work together
  if ( (o==regexp) && regexp->getCheck()) {
    wholeword->setCheck(false);
    matchcase->setCheck(true);
  } else {
    if ( (o==wholeword)&&wholeword->getCheck()) {
      regexp->setCheck(false);
    }
  }
  if (matchcase->getCheck()) { n|=SCFIND_MATCHCASE; }
  if (wholeword->getCheck()) { n|=SCFIND_WHOLEWORD; }
  if (regexp->getCheck()) { n|=SCFIND_REGEXP; }
  if (target&&message) {
    target->handle(this, FXSEL(SEL_COMMAND,message), (void*)(FXuval)n);
  }
  return 1;
}



SciSearchOptions::SciSearchOptions(FXComposite *p, FXObject *tgt, FXSelector sel):FXHorizontalFrame(p, 0)
{
  target=tgt;
  message=sel;
  matchcase = new FXCheckButton(this, _("&Match case "), this, ID_TOGGLE_CHK);
  wholeword = new FXCheckButton(this, _("W&hole word "), this, ID_TOGGLE_CHK);
  regexp =    new FXCheckButton(this, _("&Reg expr. "), this, ID_TOGGLE_CHK);
}



class SciReplDlg: public FXDialogBox {
  FXDECLARE(SciReplDlg)
protected:
  SciReplDlg() {}
  FXLabel*srch_lab;
  HistoryTextField*srch_hist;
  FXLabel*repl_lab;
  HistoryTextField*repl_hist;
  SciSearchOptions*opts;
  FXCheckButton* rev_chk;
  FXCheckButton* keep_chk;
  FXuint sciflags;
  FXHorizontalFrame *btns;
  FXButton*fwd_btn;
  FXButton*bwd_btn;
  FXButton*cncl_btn;
  FXButton*repall_indoc_btn;
  FXButton*repall_insel_btn;
  FXButton*repl_once_btn;
  void stop(FXuint stopval);
  FXWindow*owner;
public:
  long onSciOpts(FXObject*o, FXSelector sel, void*p);
  long onSrchHist(FXObject*o, FXSelector sel, void*p);
  long onButton(FXObject*o, FXSelector sel, void*p);
  SciReplDlg(FXWindow*p, bool find_only=false);

  void setSearchText(const FXString& text);
  FXString getSearchText() const;
  void setReplaceText(const FXString& text);
  FXString getReplaceText() const;
  void have_selection(bool have_sel);

  FXuint getSearchMode() { return sciflags; }
  void setSearchMode(FXuint mode);
  FXuint getSearchReverse() { return rev_chk->getCheck(); }
  void setSearchReverse(bool reverse) { rev_chk->setCheck(reverse); }

  bool kept() { return keep_chk->getCheck(); }
  void AppendHist(const FXString& search,const FXString& replace,FXuint mode);

  FXuint DoExecute(bool first_time);
  virtual void create();
  virtual void destroy();

  enum {
    ID_SCI_OPTS=FXDialogBox::ID_LAST,
    ID_SRCH_HIST,
    ID_REPL_HIST,
    ID_PREV,
    ID_NEXT,
    ID_REPLACE,
    ID_ALL_INDOC,
    ID_ALL_INSEL,
    ID_KEEP_CHK,
    ID_REV_CHK,
    ID_LAST
  };
  enum {
    DONE              = 0,    /// Cancel search
    SEARCH            = 1,    /// Search first occurrence
    SEARCH_NEXT       = 2,    /// Search next occurrence
    REPL_ALL_INDOC = 3,       /// Replace all occurrences in document
    REPL_ALL_INSEL = 4        /// Replace all occurrences in selection
  };
};



FXDEFMAP(SciReplDlg) SciReplDlgMap[]={
  FXMAPFUNC(SEL_COMMAND, SciReplDlg::ID_SCI_OPTS, SciReplDlg::onSciOpts),
  FXMAPFUNCS(SEL_COMMAND, SciReplDlg::ID_SRCH_HIST,SciReplDlg::ID_REPL_HIST, SciReplDlg::onSrchHist),
  FXMAPFUNC(SEL_CHANGED, SciReplDlg::ID_SRCH_HIST, SciReplDlg::onSrchHist),
  FXMAPFUNCS(SEL_COMMAND, SciReplDlg::ID_PREV,SciReplDlg::ID_ALL_INSEL,SciReplDlg::onButton)
};

FXIMPLEMENT(SciReplDlg,FXDialogBox,SciReplDlgMap,ARRAYNUMBER(SciReplDlgMap))



void SciReplDlg::AppendHist(const FXString& search,const FXString& replace,FXuint mode)
{
  FXString tmp;
  FXuint oldmode=sciflags;
  sciflags=mode;
  if (!search.empty()) {
    tmp=getSearchText();
    setSearchText(search);
    srch_hist->append();
    setSearchText(tmp);
  }
  if (!replace.empty()) {
    tmp=getReplaceText();
    setReplaceText(replace);
    repl_hist->append();
    setSearchText(tmp);
  }
  sciflags=oldmode;
}



void SciReplDlg::stop(FXuint stopval)
{
  srch_hist->append();
  repl_hist->append();
  getApp()->stopModal(this,stopval);
  if (stopval!=SEARCH_NEXT) { hide(); }
}



long SciReplDlg::onButton(FXObject*o, FXSelector sel, void*p)
{
  if (getSearchText().empty()) { return 0; }
  switch (FXSELID(sel)) {
    case ID_PREV:
    case ID_NEXT:
    {
      rev_chk->setCheck(FXSELID(sel)==ID_PREV);
      stop(SEARCH_NEXT);
      break;
    }
    case ID_REPLACE: {
      stop(SEARCH);
      break;
    }
    case ID_ALL_INDOC: {
      stop(REPL_ALL_INDOC);
      break;
    }
    case ID_ALL_INSEL: {
      stop(REPL_ALL_INSEL);
      break;
    }
    default: {return 0; }
  }
  return 1;
}



long SciReplDlg::onSrchHist(FXObject*o, FXSelector sel, void*p)
{
  if (FXSELID(sel)!=ID_SRCH_HIST) { return 0; }
  switch (FXSELTYPE(sel)) {
    case SEL_CHANGED: {
      sciflags=(FXuint)((FXival)p);
      opts->SetSciFlags(sciflags);
      return 1;
    }
    case SEL_COMMAND: {
      if (!getSearchText().empty()) {
        if (repl_hist->isEnabled()) {
           repl_hist->setFocus();
        } else {
          stop(SEARCH);
        }
      }
      return 1;
    }
    default: return 0;
  }
}



long SciReplDlg::onSciOpts(FXObject*o, FXSelector sel, void*p)
{
  sciflags=(FXuint)((FXival)p);
  return 1;
}



void SciReplDlg::setSearchText(const FXString& text)
{
  srch_hist->setText(text);
}



FXString SciReplDlg::getSearchText() const
{
  return srch_hist->getText();
}



void SciReplDlg::setReplaceText(const FXString& text)
{
  repl_hist->setText(text);
}



FXString SciReplDlg::getReplaceText() const
{
  return repl_hist->getText();
}



void SciReplDlg::have_selection(bool have_sel)
{
  if (have_sel) { repall_insel_btn->enable(); } else { repall_insel_btn->disable(); }
}



void SciReplDlg::setSearchMode(FXuint mode)
{
  sciflags=mode;
  opts->SetSciFlags(mode);
}



void SciReplDlg::create()
{
  FXDialogBox::create();
  srch_hist->setFocus();
}



void SciReplDlg::destroy()
{
  srch_hist->killSelection();
  repl_hist->killSelection();
  FXDialogBox::destroy();
}



FXuint SciReplDlg::DoExecute(bool first_time)
{
  if (first_time) { setSearchText(""); }
  create();
  if ( (!bwd_btn->hasFocus())&&!fwd_btn->hasFocus() ) { srch_hist->setFocus(); }
  srch_hist->start();
  repl_hist->start();
  return FXDialogBox::execute(first_time?PLACEMENT_OWNER:PLACEMENT_DEFAULT);
}



SciReplDlg::SciReplDlg(FXWindow*p, bool find_only):FXDialogBox(p,find_only?_("Find"):_("Replace"))
{
  sciflags=0;
  owner=p;
  FXuint textopts=TEXTFIELD_ENTER_ONLY|FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X;
  const char*group=find_only?"Search":"Replace";
  srch_lab=new FXLabel(this, _("Search &for:"));
  srch_hist=new HistoryTextField(this,48,group,"SM",this,ID_SRCH_HIST,textopts);
  repl_lab=new FXLabel(this, _("Replace &with:"));
  repl_hist=new HistoryTextField(this,48,group,"R",this,ID_REPL_HIST,textopts);
  new FXFrame(this, FRAME_NONE|LAYOUT_FIX_WIDTH,0,0,0);
  opts=new SciSearchOptions(this,this,ID_SCI_OPTS);
  rev_chk=new FXCheckButton(opts, _("&Backwards"),this,ID_REV_CHK);
  keep_chk=new FXCheckButton(opts, _("&Keep dialog"),this,ID_KEEP_CHK);
  srch_hist->setWatch(&sciflags);

  btns=new FXHorizontalFrame(this,LAYOUT_FILL_X|PACK_UNIFORM_WIDTH);
  FXHorizontalFrame*nav_btns=new FXHorizontalFrame(btns,LAYOUT_FILL_X|PACK_UNIFORM_WIDTH,
    0, 0, 0, 0, DEFAULT_SPACING, DEFAULT_SPACING, 0, 0, DEFAULT_SPACING, 0 );

  bwd_btn=new FXButton(nav_btns," < ",NULL,this,ID_PREV);
  fwd_btn=new FXButton(nav_btns," > ",NULL,this,ID_NEXT);
  if (find_only) {
    repl_once_btn=new FXButton(btns,_("&Find"),NULL,this,ID_REPLACE);
  } else {
    repl_once_btn=new FXButton(btns,_("&Replace"),NULL,this,ID_REPLACE);
  }
  repall_indoc_btn=new FXButton(btns,_("Re&place All"),NULL,this,ID_ALL_INDOC);
  repall_insel_btn=new FXButton(btns,_("All in &Selected"),NULL,this,ID_ALL_INSEL);
  new FXFrame(btns, FRAME_NONE|LAYOUT_FIX_WIDTH,0,0,12);
  cncl_btn=new FXButton(btns,_("&Close"),NULL,this,ID_CANCEL);
  if (find_only) {
    repl_lab->hide();
    repl_hist->hide();
    repl_hist->disable();
    repall_indoc_btn->hide();
    repall_insel_btn->hide();
    keep_chk->hide();
  } else {
    srch_hist->enslave(repl_hist);
  }
  setSearchText("");
}



FXIMPLEMENT(SearchDialogs, FXObject, NULL, 0);



SearchDialogs::SearchDialogs(FXWindow*shell) {
  defaultsearchmode=0;
  searchdirn=SCI_SEARCHNEXT;
  searchstring=FXString::null;
  find_dlg=new SciReplDlg(shell, true);
  _shell=shell;
}



SearchDialogs::~SearchDialogs()
{
  delete find_dlg;
  find_dlg=NULL;
}



bool SearchDialogs::SearchFailed()
{
  if (searchverbose) {
    FXMessageBox::error(_shell, MBOX_OK, _("Not found"), "%s.", _("Search term not found"));
  }
  return false;
}



bool SearchDialogs::DoFind(SciReplDlg*dlg, SciDoc*sci, bool forward)
{
  if (!sci) return false;
  searchdirn=forward?SCI_SEARCHNEXT:SCI_SEARCHPREV; // <= this is only used for the macro recorder
  switch (searchwrap) {
    case SEARCH_WRAP_NEVER: {
      return sci->search->FindText(searchstring,searchmode,forward,false)?true:SearchFailed();
    }
    case SEARCH_WRAP_ALWAYS: {
      return sci->search->FindText(searchstring,searchmode,forward,true)?true:SearchFailed();
    }
    case SEARCH_WRAP_ASK: {
      if (sci->search->FindText(searchstring,searchmode,forward,false)) { return true; }
      long pos=sci->sendMessage(SCI_GETCURRENTPOS,0,0);
      if (forward) {
        if (pos==0) { return SearchFailed(); }
      } else {
        if (pos==sci->sendMessage(SCI_GETLENGTH,0,0)) { return SearchFailed(); }
      }
      if (FXMessageBox::question(dlg?dlg:sci->getShell(), MBOX_YES_NO, _("Wrap search?"), "%s:\n%s",
            _("Search term not found"),
            _("Wrap search and try again?"))==MBOX_CLICKED_YES) {
        return (sci->search->FindText(searchstring,searchmode,forward,true))?true:SearchFailed();
      }
    }
    default: { return false; }
  }
}



bool SearchDialogs::ShowFindDialog(SciDoc*sci)
{
  if (!sci) return false;
  FXuint code;
  static FXint prev_x=-1;
  static FXint prev_y=-1;
  if ((prev_x>=0) && (prev_y>=0)) {
    find_dlg->setX(prev_x);
    find_dlg->setY(prev_y);
  }
  find_dlg->setSearchMode(searchmode);
  bool initial=true;
  do {
    code=find_dlg->DoExecute(initial);
    searchstring=find_dlg->getSearchText();
    searchmode=find_dlg->getSearchMode();
    if (code==SciReplDlg::DONE) {
      break;
    }
    initial=false;
    switch(code) {
      case SciReplDlg::DONE:{
        break;
      }
      case SciReplDlg::SEARCH:{
        DoFind(find_dlg, sci, !find_dlg->getSearchReverse());
        break;
      }
      case SciReplDlg::SEARCH_NEXT: {
        DoFind(find_dlg, sci, !find_dlg->getSearchReverse());
        break;
      }
    }
  } while (code==SciReplDlg::SEARCH_NEXT);
  // JWM refuses to re-focus the main window unless we destroy this dialog's resources,
  // and FOX refuses to honor the next PLACEMENT_OWNER if we do destroy it!
  // so save the current placement, destroy the dialog, and restore X and Y next time.
  prev_x=find_dlg->getX();
  prev_y=find_dlg->getY();
  find_dlg->destroy();
  return !(initial);
}



void SearchDialogs::ShowReplaceDialog(SciDoc*sci)
{
  if (!sci) return;
  FXString replacestring;
  SciReplDlg dlg(sci, false);
  dlg.setSearchMode(searchmode);
  FXuint code;
  bool ready=false;
  bool initial=true;
  do {
    dlg.have_selection(sci->GetSelLength()>0);
    code=dlg.DoExecute(initial?PLACEMENT_OWNER:PLACEMENT_DEFAULT);
    initial=false;
    if (code!=SciReplDlg::DONE) {
      searchmode=dlg.getSearchMode();
      searchstring=dlg.getSearchText();
      replacestring=dlg.getReplaceText();
    }
    switch (code) {
      case SciReplDlg::SEARCH:{ // Replace selection
        if (!ready) { ready=DoFind(&dlg, sci, !dlg.getSearchReverse()); }
        if (ready) {
          sci->search->ReplaceSelection(replacestring,searchmode);
          ready=false;
        }
        break;
      }
      case SciReplDlg::SEARCH_NEXT:{ // Find next
        ready=DoFind(&dlg, sci, !dlg.getSearchReverse());
        break;
      }
      case SciReplDlg::REPL_ALL_INDOC:{ // Replace all
        if (sci->search->ReplaceAllInDoc(searchstring, replacestring, searchmode)==0) {
          SearchFailed();
        }
        if (!dlg.kept()) { code=FXReplaceDialog::DONE; }
        break;
      }
      case SciReplDlg::REPL_ALL_INSEL: { // Replace all in selection
        if (sci->search->ReplaceAllInSel(searchstring,replacestring,searchmode)==0) {
          SearchFailed();
        }
        if (!dlg.kept()) { code=FXReplaceDialog::DONE; }
        break;
      }
      default:{ // Cancel
        break;
      }
    }
  } while (code!=FXReplaceDialog::DONE);
}



bool SearchDialogs::FindNext(SciDoc*sci)
{
  searchmode &= ~SEARCH_BACKWARD;
  if (searchstring.empty()) {
    return ShowFindDialog(sci);
  } else {
    DoFind(NULL, sci,true);
    return true;
  }
}



bool SearchDialogs::FindPrev(SciDoc*sci)
{
  searchmode |= SEARCH_BACKWARD;
  if (searchstring.empty()) {
    return ShowFindDialog(sci);
  } else {
    DoFind(NULL, sci,false);
    return true;
  }
}



bool SearchDialogs::FindPhrase(SciDoc*sci, const char* phrase, FXuint mode, bool forward)
{
  searchstring=phrase;
  searchmode=mode;
  return forward?FindNext(sci):FindPrev(sci);
}



bool GetPrimarySelection(SciDoc*sci, FXString&target)
{
  return sci->getShell()->getDNDData(FROM_SELECTION,FXWindow::stringType, target);
}



void SearchDialogs::FindSelected(SciDoc*sci,bool forward)
{
  if (!sci) return;
  FXString srch="";
  if (!GetPrimarySelection(sci,srch)) {
    if (sci->GetSelLength()>0) { sci->GetSelText(srch); }
  }
  if (!srch.empty()) {
    searchmode=defaultsearchmode;
    searchstring=srch;
    DoFind(NULL, sci,forward);
    find_dlg->AppendHist(searchstring,"",defaultsearchmode);
  }
}



bool SearchDialogs::GoToSelected(SciDoc*sci)
{
  FXString tmp;
  if (sci && GetPrimarySelection(sci,tmp)) {
    tmp.trim();
    return sci->GoToStringCoords(tmp.text());
  } else {
    return false;
  }
}



bool SearchDialogs::ShowGoToDialog(SciDoc*sci)
{
  if (!sci) return false;
  FXInputDialog dlg(sci->getShell(),_("Go to line"), _("Enter line number\n(or line:column)"));
  bool rv=dlg.execute(PLACEMENT_OWNER);
  if (rv) { sci->GoToStringCoords(dlg.getText().text()); }
  sci->setFocus();
  return rv;
}

