/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2009-2014 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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
#include <fxkeys.h>

#include "compat.h"
#include "scidoc.h"
#include "scisrch.h"
#include "histbox.h"

#include "appwin_pub.h"

#include "intl.h"
#include "search.h"



FXDEFMAP(SciSearchOptions) SciSearchOptionsMap[]={
  FXMAPFUNC(SEL_COMMAND,SciSearchOptions::ID_TOGGLE_CHK,SciSearchOptions::onToggleChk),
};

FXIMPLEMENT(SciSearchOptions,FXHorizontalFrame,SciSearchOptionsMap,ARRAYNUMBER(SciSearchOptionsMap))


enum {
  MODELIST_NORMAL,
  MODELIST_REGEXP,
  MODELIST_WHOLEWORD
};

void SciSearchOptions::SetSciFlags(FXuint n)
{
  // Favor REGEXP over WHOLEWORD
  if ( (n&SCFIND_WHOLEWORD) && (n&SCFIND_REGEXP) ) { n&=!SCFIND_WHOLEWORD; }
  matchcase->setCheck((n & SCFIND_MATCHCASE)?1:0);
  if (n & SCFIND_WHOLEWORD) {
    modelist->setCurrentItem(MODELIST_WHOLEWORD);
  } else if (n & SCFIND_REGEXP) {
    modelist->setCurrentItem(MODELIST_REGEXP);
  } else {
    modelist->setCurrentItem(MODELIST_NORMAL);
  }
}



long SciSearchOptions::onToggleChk(FXObject*o, FXSelector sel, void*p)
{
  FXuval n=(FXuval)modelist->getItemData(modelist->getCurrentItem());
  if ((o==modelist) && (modelist->getCurrentItem()==MODELIST_REGEXP)) { matchcase->setCheck(true); }
  if (matchcase->getCheck()) { n|=SCFIND_MATCHCASE; }
  if (target) { target->handle(this, FXSEL(SEL_COMMAND,message), (void*)(FXuval)n); }
  return 1;
}



SciSearchOptions::SciSearchOptions(FXComposite *p, FXObject *tgt, FXSelector sel):FXHorizontalFrame(p, 0)
{
  target=tgt;
  message=sel;
  new FXLabel(this,_(" M&ode:"));
  modelist=new FXListBox(this,this,ID_TOGGLE_CHK);
  modelist->appendItem(FXString::null);
  modelist->appendItem(FXString::null);
  modelist->appendItem(FXString::null);
  modelist->setNumVisible(modelist->getNumItems());
  modelist->setItem(MODELIST_NORMAL,    _("normal"),     NULL, (void*)(FXuval)0);
  modelist->setItem(MODELIST_REGEXP,    _("reg. expr."), NULL, (void*)(FXuval)SCFIND_REGEXP);
  modelist->setItem(MODELIST_WHOLEWORD, _("whole word"), NULL, (void*)(FXuval)SCFIND_WHOLEWORD);
  matchcase = new FXCheckButton(this, _("&Match case "), this, ID_TOGGLE_CHK);
}



class SciReplGui: public FXMatrix {
FXDECLARE(SciReplGui)
SciReplGui(){}
private:
  FXVerticalFrame *txt_fields;
  FXVerticalFrame *btn_ctrls;
  FXLabel*srch_lab;
  HistoryTextField*srch_hist;
  FXLabel*repl_lab;
  HistoryTextField*repl_hist;
  SciSearchOptions*opts;
  FXCheckButton* rev_chk;
  FXuint sciflags;
  FXHorizontalFrame *btns;
  FXButton*fwd_btn;
  FXButton*bwd_btn;
  FXButton*cncl_btn;
  FXButton*repall_indoc_btn;
  FXButton*repall_insel_btn;
  FXButton*repl_once_btn;
  FXObject*target;
  FXSelector message;
  void EnableSearch();
public:
  SciReplGui(FXComposite*p, FXObject*tgt, FXSelector sel, bool find_only, bool floating);
  ~SciReplGui() { target=NULL; stop(DONE); }
  void stop(FXuint stopval);
  long onSciOpts(FXObject*o, FXSelector sel, void*p);
  long onSrchHist(FXObject*o, FXSelector sel, void*p);
  long onButton(FXObject*o, FXSelector sel, void*p);
  long onFakeBtnUp(FXObject*o, FXSelector sel, void*p);

  long onKeyPress(FXObject*o, FXSelector sel, void*p);
  long onConfigure(FXObject*o, FXSelector sel, void*p);
  long onFocusIn(FXObject*o, FXSelector sel, void*p);

  void setSearchText(const FXString& text);
  FXString getSearchText() const;
  void setReplaceText(const FXString& text);
  FXString getReplaceText() const;
  void setHaveSelection(bool have_sel);
  FXuint getSearchMode() { return sciflags; }
  void setSearchMode(FXuint mode);
  FXuint getSearchReverse() { return rev_chk->getCheck(); }
  void setSearchReverse(bool reverse) { rev_chk->setCheck(reverse); }
  void AppendHist(const FXString& search,const FXString& replace,FXuint mode);
  void DoExecute();
  void HandleKeyPress(FXEvent*ev);
  virtual void create();
  virtual void destroy();
  enum {
    ID_SCI_OPTS=1,
    ID_SRCH_HIST,
    ID_REPL_HIST,
    ID_PREV,
    ID_NEXT,
    ID_REPLACE,
    ID_ALL_INDOC,
    ID_ALL_INSEL,
    ID_CANCEL_SRCH,
    ID_REV_CHK,
    ID_FAKE_BTN_UP,
    ID_LAST
  };
  enum {
    DONE           = 0,  // Cancel search
    SEARCH         = 1,  // Search first occurrence
    SEARCH_NEXT    = 2,  // Search next occurrence
    REPL_ALL_INDOC = 3,  // Replace all occurrences in document
    REPL_ALL_INSEL = 4   // Replace all occurrences in selection
  };
};



FXDEFMAP(SciReplGui) SciReplGuiMap[]={
  FXMAPFUNC(  SEL_COMMAND,  SciReplGui::ID_SCI_OPTS,    SciReplGui::onSciOpts),
  FXMAPFUNCS( SEL_COMMAND,  SciReplGui::ID_SRCH_HIST,   SciReplGui::ID_REPL_HIST,   SciReplGui::onSrchHist),
  FXMAPFUNC(  SEL_CHANGED,  SciReplGui::ID_SRCH_HIST,   SciReplGui::onSrchHist),
  FXMAPFUNC(  SEL_PICKED,   SciReplGui::ID_SRCH_HIST,   SciReplGui::onSrchHist),
  FXMAPFUNCS( SEL_COMMAND,  SciReplGui::ID_PREV,        SciReplGui::ID_CANCEL_SRCH, SciReplGui::onButton),
  FXMAPFUNC(  SEL_TIMEOUT,  SciReplGui::ID_FAKE_BTN_UP, SciReplGui::onFakeBtnUp),
  FXMAPFUNC(  SEL_KEYPRESS,  0, SciReplGui::onKeyPress),
  FXMAPFUNC(  SEL_CONFIGURE, 0, SciReplGui::onConfigure),
  FXMAPFUNC(  SEL_FOCUSIN,   0, SciReplGui::onFocusIn),
};

FXIMPLEMENT(SciReplGui,FXMatrix,SciReplGuiMap,ARRAYNUMBER(SciReplGuiMap))



#define ICO_SIZE 8

static const char close_icon[] =
  "X_____X_"
  "_X___X__"
  "__X_X___"
  "___X____"
  "__X_X___"
  "_X___X__"
  "X_____X_"
  "________"
;


SciReplGui::SciReplGui(FXComposite*p, FXObject*tgt, FXSelector sel, bool find_only, bool floating):FXMatrix(p,2)
{
  sciflags=0;
  target=tgt;
  message=sel;
  setLayoutHints(floating?LAYOUT_FILL:LAYOUT_FILL_X);
  setMatrixStyle(floating?MATRIX_BY_ROWS:MATRIX_BY_COLUMNS);
  setFrameStyle(floating?FRAME_NONE:FRAME_GROOVE);
  FXuint textopts=TEXTFIELD_ENTER_ONLY|FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X;
  const char*group=find_only?"Search":"Replace";

  txt_fields=new FXVerticalFrame(this,FRAME_NONE|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);
  btn_ctrls=new  FXVerticalFrame(this,FRAME_NONE);

  SetPad(txt_fields,0);

  srch_lab=new FXLabel(txt_fields, _("Se&arch for:"));
  srch_hist=new HistoryTextField(txt_fields,48,group,"SM",this,ID_SRCH_HIST,textopts);
  if (find_only) {
    repl_hist=NULL;
  } else {
    repl_lab=new FXLabel(txt_fields, _("Replace &with:"));
    repl_hist=new HistoryTextField(txt_fields,48,group,"R",this,ID_REPL_HIST,textopts);
  }

  SetPad(btn_ctrls,0);

  opts=new SciSearchOptions(btn_ctrls,this,ID_SCI_OPTS);
  rev_chk=new FXCheckButton(opts, _("&Backward"),this,ID_REV_CHK);
  srch_hist->setWatch(&sciflags);

  btns=new FXHorizontalFrame(btn_ctrls,LAYOUT_FILL_X);
  FXHorizontalFrame*nav_btns=new FXHorizontalFrame(btns);
  SetPadLRTB(nav_btns,DEFAULT_SPACING,DEFAULT_SPACING*2,0,0);
  bwd_btn=new FXButton(nav_btns," &< ",NULL,this,ID_PREV);
  fwd_btn=new FXButton(nav_btns," &> ",NULL,this,ID_NEXT);
  if (find_only) {
    repl_once_btn=new FXButton(btns,_("F&ind"),NULL,this,ID_REPLACE);
    repall_indoc_btn=NULL;
    repall_insel_btn=NULL;
  } else {
    repl_once_btn=new FXButton(btns,_("Rep&lace"),NULL,this,ID_REPLACE);
    repall_indoc_btn=new FXButton(btns,_("Re&place All"),NULL,this,ID_ALL_INDOC);
    repall_insel_btn=new FXButton(btns,_("All in Sele&cted"),NULL,this,ID_ALL_INSEL);
  }

  if (!floating) {
    FXVerticalFrame*cncl_box=new FXVerticalFrame(btns,LAYOUT_RIGHT|LAYOUT_SIDE_BOTTOM|PACK_UNIFORM_HEIGHT|LAYOUT_FILL_Y);
    cncl_box->setVSpacing(0);
    SetPad(cncl_box,0);
    cncl_btn=new FXButton(cncl_box,FXString::null,NULL,this,ID_CANCEL_SRCH,BUTTON_NORMAL|LAYOUT_BOTTOM);
    if (!find_only) {
      new FXVerticalFrame(cncl_box,FRAME_NONE);
      FXHorizontalFrame*cncl_frm=new FXHorizontalFrame(btn_ctrls,LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_SIDE_BOTTOM);
      SetPad(cncl_frm,0);
      cncl_box->reparent(cncl_frm);
    }
    cncl_btn->setTipText(_("Close   {ESC}"));
    FXColor ico_buf[ICO_SIZE*ICO_SIZE];
    FXColor bg=cncl_btn->getBackColor();
    FXColor fg=cncl_btn->getTextColor();
    for (FXint i=0; i<ICO_SIZE*ICO_SIZE; i++) { ico_buf[i]=close_icon[i]=='_'?bg:fg; }
    FXIcon *ico=new FXIcon(cncl_btn->getApp(),ico_buf,0,IMAGE_OPAQUE,ICO_SIZE,ICO_SIZE);
    ico->create();
    cncl_btn->setIcon(ico);
  } else {
    btn_ctrls->setLayoutHints(LAYOUT_FILL_X);
    new FXHorizontalFrame(btns,LAYOUT_FILL_X);
    cncl_btn=new FXButton(btns,_("Close"),NULL,this,ID_CANCEL_SRCH,BUTTON_NORMAL|LAYOUT_BOTTOM);
  }
  SetPad(btns,0);
  if (repl_hist) { srch_hist->enslave(repl_hist); }
  setSearchText(FXString::null);
}



void SciReplGui::AppendHist(const FXString& search,const FXString& replace,FXuint mode)
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
    if (repl_hist) { repl_hist->append(); }
    setSearchText(tmp);
  }
  sciflags=oldmode;
}



void SciReplGui::stop(FXuint stopval)
{
  srch_hist->append();
  if (repl_hist) { repl_hist->append(); }
  if (target) { target->handle(this,FXSEL(SEL_COMMAND,message),(void*)(FXuval)stopval); }
}



long SciReplGui::onFakeBtnUp(FXObject*o, FXSelector sel, void*p)
{
 ((FXButton*)p)->setState(STATE_UP);
  return 0;
}



long SciReplGui::onButton(FXObject*o, FXSelector sel, void*p)
{
  if ( getSearchText().empty() && (FXSELID(sel)!=ID_CANCEL_SRCH) ) { return 0; }
  FXButton*btn=NULL;
  switch (FXSELID(sel)) {
    case ID_PREV:
    {
      rev_chk->setCheck(true);
      btn=bwd_btn;
      stop(SEARCH_NEXT);
      break;
    }
    case ID_NEXT:
    {
      rev_chk->setCheck(false);
      btn=fwd_btn;
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
    case ID_CANCEL_SRCH: {
       stop(DONE);
      break;
    }
    default: {return 0; }
  }
  if (btn && !o) { // If sender was NULL, fake the appearance of a button press.
    btn->setFocus();
    btn->setState(STATE_DOWN);
    btn->update();
    btn->getApp()->addTimeout(this,ID_FAKE_BTN_UP,ONE_SECOND/10,(void*)btn);
  }
  return 1;
}



void SciReplGui::HandleKeyPress(FXEvent*ev)
{
  switch (ev->code) {
    case KEY_less:
    case KEY_comma: {
      if (ev->state&ALTMASK) { handle(NULL,FXSEL(SEL_COMMAND,SciReplGui::ID_PREV),NULL); }
      break;
    }
    case KEY_period:
    case KEY_greater: {
      if (ev->state&ALTMASK) { handle(NULL,FXSEL(SEL_COMMAND,SciReplGui::ID_NEXT),NULL); }
      break;
    }
    case KEY_Escape: {
      stop(SciReplGui::DONE);
      break;
    }
  }
}



void SciReplGui::EnableSearch()
{
  if (srch_hist->getText().empty()) {
    bwd_btn->disable();
    fwd_btn->disable();
    repl_once_btn->disable();
    if (repall_indoc_btn) { repall_indoc_btn->disable(); }
    if (repall_insel_btn) { repall_insel_btn->disable(); }
  } else {
    bwd_btn->enable();
    fwd_btn->enable();
    repl_once_btn->enable();
    if (repall_indoc_btn) { repall_indoc_btn->enable(); }
    if (repall_insel_btn) { repall_insel_btn->enable(); }
  }
}



long SciReplGui::onSrchHist(FXObject*o, FXSelector sel, void*p)
{
  if (FXSELID(sel)!=ID_SRCH_HIST) { return 0; }
  switch (FXSELTYPE(sel)) {
    case SEL_CHANGED: {
      EnableSearch();
      return 1;
    }
    case SEL_PICKED: {
      sciflags=(FXuint)((FXival)p);
      opts->SetSciFlags(sciflags);
      EnableSearch();
      return 1;
    }
    case SEL_COMMAND: {
      if (!getSearchText().empty()) {
        if (repl_hist && repl_hist->isEnabled()) {
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



long SciReplGui::onSciOpts(FXObject*o, FXSelector sel, void*p)
{
  sciflags=(FXuint)((FXival)p);
  return 1;
}



long SciReplGui::onConfigure(FXObject*o, FXSelector sel, void*p)
{
  setLayoutHints((getDefaultWidth()>getParent()->getWidth())?LAYOUT_FILL_X:0);
  return FXMatrix::onConfigure(o,sel,p);
}



long SciReplGui::onKeyPress(FXObject*o, FXSelector sel, void*p)
{
  HandleKeyPress(((FXEvent*)p));
  return FXMatrix::onKeyPress(o,sel,p);
}



long SciReplGui::onFocusIn(FXObject*o, FXSelector sel, void*p)
{
  FXMatrix::onFocusIn(o,sel,p);
  TopWinPub::ActiveWidget(id());
  return 1;
}



void SciReplGui::setSearchText(const FXString& text)
{
  srch_hist->setText(text);
  EnableSearch();
}



FXString SciReplGui::getSearchText() const
{
  return srch_hist->getText();
}



void SciReplGui::setReplaceText(const FXString& text)
{
  if (repl_hist) { repl_hist->setText(text); }
}



FXString SciReplGui::getReplaceText() const
{
  return repl_hist?repl_hist->getText():FXString::null;
}



void SciReplGui::setHaveSelection(bool have_sel)
{
  if (repall_insel_btn) {
    if (have_sel) { repall_insel_btn->enable(); } else { repall_insel_btn->disable(); }
  }
}



void SciReplGui::setSearchMode(FXuint mode)
{
  sciflags=mode;
  opts->SetSciFlags(mode);
}



void SciReplGui::create()
{
  FXMatrix::create();
  srch_hist->setFocus();
}



void SciReplGui::destroy()
{
  srch_hist->killSelection();
  if (repl_hist) repl_hist->killSelection();
}



void SciReplGui::DoExecute()
{
  if ( (!bwd_btn->hasFocus())&&!fwd_btn->hasFocus() ) { srch_hist->setFocus(); }
  srch_hist->start();
  if (repl_hist) { repl_hist->start(); }
}




class MainPanel: public FXHorizontalFrame {
  FXDECLARE(MainPanel)
  MainPanel() {}
public:
  long onConfigure(FXObject*o, FXSelector sel, void*p) {
    for (FXWindow*w=getFirst(); w; w=w->getNext()) { w->handle(o,sel,p); }
    return FXHorizontalFrame::onConfigure(o,sel,p);
  }
  MainPanel(FXComposite *p):FXHorizontalFrame(p,FRAME_NONE|LAYOUT_FILL_X) { setBaseColor(FXRGB(255,0,0)); }
};



FXDEFMAP(MainPanel) MainPanelMap[]={
  FXMAPFUNC(SEL_CONFIGURE, 0, MainPanel::onConfigure)
};

FXIMPLEMENT(MainPanel,FXHorizontalFrame,MainPanelMap,ARRAYNUMBER(MainPanelMap));



static FXint srch_dlg_box_prev_x=0;
static FXint srch_dlg_box_prev_y=0;
static bool srch_dlg_box_placed=false;


class SrchDlgBox:public FXDialogBox {
public:
  SrchDlgBox(FXWindow*o):FXDialogBox(o,FXString::null) {}
  virtual void show() {
    if (srch_dlg_box_placed) {
      position(srch_dlg_box_prev_x,srch_dlg_box_prev_y,getFirst()->getDefaultWidth(),getFirst()->getDefaultHeight());
      FXDialogBox::show();
    } else {
      resize(getFirst()->getDefaultWidth(),getFirst()->getDefaultHeight());
      FXDialogBox::show(PLACEMENT_OWNER);
    }
    srch_dlg_box_placed=true;
    raise();
  }
  virtual void show(FXuint placement) { SrchDlgBox::show(); }
  virtual void hide() {
    srch_dlg_box_prev_x=getX();
    srch_dlg_box_prev_y=getY();
    FXDialogBox::hide();
  }
  virtual void destroy() {
    srch_dlg_box_prev_x=getX();
    srch_dlg_box_prev_y=getY();
    FXDialogBox::destroy();
  }
};



FXDEFMAP(SearchDialogs) SearchDialogsMap[] = {
  FXMAPFUNC(SEL_COMMAND, SearchDialogs::ID_SEARCH,        SearchDialogs::onSearch),
  FXMAPFUNC(SEL_CHORE,   SearchDialogs::ID_SEARCH_DONE,   SearchDialogs::onSearchDone),
};

FXIMPLEMENT(SearchDialogs, FXObject, SearchDialogsMap, ARRAYNUMBER(SearchDialogsMap));


SearchDialogs::SearchDialogs(FXComposite*p, FXObject*trg, FXSelector sel) {
  defaultsearchmode=0;
  searchdirn=SCI_SEARCHNEXT;
  searchstring=FXString::null;
  target=trg;
  message=sel;
  parent=p;
  find_initial=true;
  repl_initial=true;
  srchdlg=NULL;
  srchpan=NULL;
  repl_gui=NULL;
  find_gui=NULL;
}



void SearchDialogs::SetGuiStyle(FXuint style)
{
  delete repl_gui;
  repl_gui=NULL;
  delete find_gui;
  find_gui=NULL;
  gui_style=(SearchDialogStyle)style;
  if (style==SEARCH_GUI_FLOAT) {
    if (!srchdlg) { srchdlg=new SrchDlgBox(parent->getShell()); }
    SetPad(srchdlg,0);
    container=srchdlg;
    delete srchpan;
    srchpan=NULL;
  } else {
    if (!srchpan) { srchpan=new MainPanel(parent); }
    SetPad(srchpan,0);
    container=srchpan;
    delete srchdlg;
    srchdlg=NULL;
  }
}



SearchDialogs::~SearchDialogs()
{
  delete repl_gui;
  repl_gui=NULL;
  if (hist_queue.no()) {
    if (!find_gui) {
      find_gui=new SciReplGui(container, this, ID_SEARCH, true, container==srchdlg);
    }
    SaveHistoryQueue();
  }
  delete find_gui;
  find_gui=NULL;
}



void SearchDialogs::SetPrefs(FXuint mode, FXuint wrap, bool verbose, FXuint style)
{
  searchmode=mode;
  defaultsearchmode=mode;
  searchwrap=(SearchWrapPolicy)wrap;
  searchverbose=verbose;
  SetGuiStyle(style);
}



void SearchDialogs::SearchFailed(FXWindow*w)
{
  FXMessageBox::error(w->getShell(), MBOX_OK, _("Not found"), "%s.", _("Search term not found"));
}



bool SearchDialogs::SearchFailed()
{
  if (searchverbose) { SearchFailed(parent); }
  return false;
}


bool SearchDialogs::SearchWrapAsk(FXWindow*w)
{
  return (FXMessageBox::question(w->getShell(), MBOX_YES_NO, _("Wrap search?"), "%s:\n%s",
            _("Search term not found"),
            _("Wrap search and try again?"))==MBOX_CLICKED_YES);
}



void SearchDialogs::SaveHistoryQueue()
{
  for (FXint i=0; i<hist_queue.no(); i++) {
    FXString*s=(FXString*)hist_queue[i];
    find_gui->AppendHist(*s, FXString::null, defaultsearchmode);
    delete s;
    hist_queue[i]=NULL;
  }
  hist_queue.clear();
}



void SearchDialogs::ShowFindDialog()
{
  delete repl_gui;
  repl_gui=NULL;
  if (!find_gui) {
    find_gui=new SciReplGui(container, this, ID_SEARCH, true, container==srchdlg);
    SaveHistoryQueue();
    container->create();
    if (srchdlg) {
      srchdlg->setTitle(_("Search"));
    } else {
      srchpan->reparent(parent,(gui_style==SEARCH_GUI_ABOVE)?parent->getFirst():NULL);
    }
  } else {
    find_gui->setSearchText(FXString::null);
  }
  if (srchdlg) {
    srchdlg->hide();
    srchdlg->show();
  }
  parent->layout();
  find_gui->setFocus();
  find_gui->setSearchMode(searchmode);
  find_gui->DoExecute();
}



void SearchDialogs::ShowReplaceDialog()
{
  delete find_gui;
  find_gui=NULL;
  if (!repl_gui) {
    repl_gui=new SciReplGui(container, this, ID_SEARCH, false, container==srchdlg);
    container->create();
    if (srchdlg) {
      srchdlg->setTitle(_("Replace"));
      srchdlg->show();
    } else {
      srchpan->reparent(parent,(gui_style==SEARCH_GUI_ABOVE)?parent->getFirst():NULL);
    }
  }
  if (srchdlg) {
    srchdlg->hide();
    srchdlg->show();
  }
  repl_gui->setSearchMode(searchmode);
  repl_ready=false;
  repl_gui->setHaveSelection(TopWinPub::FocusedDoc()->GetSelLength()>0);
  repl_gui->DoExecute();
  repl_initial=false;
}



bool SearchDialogs::FindText(bool forward, bool wrap)
{
  SciDoc*sci=TopWinPub::FocusedDoc();
  if (sci->search->FindText(searchstring,searchmode,forward,wrap)) {
    if (target) { target->handle(this,FXSEL(SEL_COMMAND,message),NULL); }
    return true;
  } else {
    return false;
  }
}



bool SearchDialogs::DoFind(bool forward)
{
  SciDoc*sci=TopWinPub::FocusedDoc();
  searchdirn=forward?SCI_SEARCHNEXT:SCI_SEARCHPREV; // <= this is only used for the macro recorder
  switch (searchwrap) {
    case SEARCH_WRAP_NEVER: {
      return FindText(forward,false)?true:SearchFailed();
    }
    case SEARCH_WRAP_ALWAYS: {
      return FindText(forward,true)?true:SearchFailed();
    }
    case SEARCH_WRAP_ASK: {
      if (FindText(forward,false)) { return true; }
      long pos=sci->sendMessage(SCI_GETCURRENTPOS,0,0);
      if (forward) {
        if (pos==0) { return SearchFailed(); }
      } else {
        if (pos==sci->sendMessage(SCI_GETLENGTH,0,0)) { return SearchFailed(); }
      }
      if (SearchWrapAsk(parent)) { return (FindText(forward,true))?true:SearchFailed(); }
    }
    default: { return false; }
  }
}



void SearchDialogs::FindNext()
{
  searchmode &= ~SEARCH_BACKWARD;
  if (searchstring.empty()) {
    ShowFindDialog();
  } else {
    DoFind(true);
  }
}



void SearchDialogs::FindPrev()
{
  searchmode |= SEARCH_BACKWARD;
  if (searchstring.empty()) {
    ShowFindDialog();
  } else {
    DoFind(false);
  }
}



void SearchDialogs::FindPhrase(const char* searchfor, FXuint mode, bool forward)
{
  searchstring=searchfor;
  searchmode=mode;
  if (forward) { FindNext(); } else { FindPrev(); }
}



void SearchDialogs::FindAndReplace(const char*searchfor, const char*replacewith, FXuint mode, bool forward)
{
  searchstring=searchfor;
  replacestring=replacewith;
  searchmode=mode;
  repl_ready=false;
  NextReplace(SciReplGui::SEARCH,forward);
}



void SearchDialogs::ReplaceAllInSelection(const char*searchfor, const char*replacewith, FXuint mode)
{
  searchstring=searchfor;
  replacestring=replacewith;
  searchmode=mode;
  NextReplace(SciReplGui::REPL_ALL_INSEL,true);
}



void SearchDialogs::ReplaceAllInDocument(const char*searchfor, const char*replacewith, FXuint mode)
{
  searchstring=searchfor;
  replacestring=replacewith;
  searchmode=mode;
  NextReplace(SciReplGui::REPL_ALL_INDOC,true);
}



bool GetPrimarySelection(SciDoc*sci, FXString&target)
{
  return sci->getShell()->getDNDData(FROM_SELECTION,FXWindow::stringType, target);
}



void SearchDialogs::FindSelected(bool forward)
{
  SciDoc*sci=TopWinPub::FocusedDoc();
  FXString srch=FXString::null;
  if (!GetPrimarySelection(sci,srch)) {
    if (sci->GetSelLength()>0) { sci->GetSelText(srch); }
  }
  if (!srch.empty()) {
    searchmode=defaultsearchmode;
    searchstring=srch;
    DoFind(forward);
    if (find_gui) {
      find_gui->AppendHist(searchstring, FXString::null, defaultsearchmode);
      find_gui->setSearchText(srch);
    } else {
      hist_queue.append((FXObject*)(new FXString(searchstring)));
    }
  }
}



bool SearchDialogs::GoToSelected()
{
  SciDoc*sci=TopWinPub::FocusedDoc();
  FXString tmp;
  if (sci && GetPrimarySelection(sci,tmp)) {
    tmp.trim();
    return sci->GoToStringCoords(tmp.text());
  } else {
    return false;
  }
}



bool SearchDialogs::ShowGoToDialog()
{
  SciDoc*sci=TopWinPub::FocusedDoc();
  FXInputDialog dlg(sci->getShell(),_("Go to line"), _("Enter line number\n(or line:column)"));
  bool rv=dlg.execute(PLACEMENT_OWNER);
  if (rv) { sci->GoToStringCoords(dlg.getText().text()); }
  sci->setFocus();
  return rv;
}



FXuint SearchDialogs::NextSearch(FXuint code)
{
  searchstring=find_gui->getSearchText();
  searchmode=find_gui->getSearchMode();
  if (code!=SciReplGui::DONE) {
    find_initial=false;
    switch(code) {
      case SciReplGui::DONE:{
        break;
      }
      case SciReplGui::SEARCH:
      case SciReplGui::SEARCH_NEXT: {
        DoFind(!find_gui->getSearchReverse());
        break;
      }
    }
  }
  TopWinPub::FocusedDoc()->update();
  return code;
}



FXuint SearchDialogs::NextReplace(FXuint code, bool forward)
{
  SciDoc*sci=TopWinPub::FocusedDoc();
  switch (code) {
    case SciReplGui::SEARCH:{ // Replace selection
      if (!repl_ready) { repl_ready=DoFind(forward); }
      if (repl_ready) {
        sci->search->ReplaceSelection(replacestring,searchmode);
        repl_ready=false;
      }
      break;
    }
    case SciReplGui::SEARCH_NEXT:{ // Find next
      repl_ready=DoFind(forward);
      break;
    }
    case SciReplGui::REPL_ALL_INDOC:{ // Replace all
      if (sci->search->ReplaceAllInDoc(searchstring, replacestring, searchmode)==0) {
        SearchFailed();
      }
      break;
    }
    case SciReplGui::REPL_ALL_INSEL: { // Replace all in selection
      if (sci->search->ReplaceAllInSel(searchstring,replacestring,searchmode)==0) {
        SearchFailed();
      }
      break;
    }
    default:{ // Cancel
      break;
    }
  }
  return code;
}



long SearchDialogs::onSearchDone(FXObject*o, FXSelector sel, void *p)
{
  SciReplGui**gui=(SciReplGui**)p;
  delete *gui;
  *gui=NULL;
  TopWinPub::FocusedDoc()->setFocus();
  return 1;
}



long SearchDialogs::onSearch(FXObject*o, FXSelector sel, void *p)
{
  FXuint code=(FXuint)(FXuval)p;
  if (repl_gui) {
    if (code==SciReplGui::DONE) {
      container->getApp()->addChore(this,ID_SEARCH_DONE,&repl_gui);
    } else {
      searchmode=repl_gui->getSearchMode();
      searchstring=repl_gui->getSearchText();
      replacestring=repl_gui->getReplaceText();
      NextReplace(code,!repl_gui->getSearchReverse());
    }
  } else {
    switch (code) {
      case SciReplGui::SEARCH:
      case SciReplGui::SEARCH_NEXT: {
        NextSearch(code);
        break;
      }
      case SciReplGui::DONE: {
        container->getApp()->addChore(this,ID_SEARCH_DONE,&find_gui);
        break;
      }
    }
  }
  if (srchdlg&&(code==SciReplGui::DONE)) { srchdlg->hide(); }
  return 1;
}



void SearchDialogs::setHaveSelection(bool have_sel)
{
  if (repl_gui) { repl_gui->setHaveSelection(have_sel); }
}



void SearchDialogs::hide()
{
  delete repl_gui;
  repl_gui=NULL;
  delete find_gui;
  find_gui=NULL;
  if (srchdlg) { srchdlg->hide(); }
}

