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

#include <cctype>

#include <fxkeys.h>
#include <fx.h>
#include <Scintilla.h>
#include <FXScintilla.h>

class WordList;
class Accessor;

#include <ILexer.h>
#include <LexerModule.h>
#include <Catalogue.h>

#include "compat.h"
#include "scidoc.h"
#include "prefs.h"
#include "lang.h"
#include "appwin.h"
#include "search.h"
#include "histbox.h"
#include "tooltree.h"
#include "menuspec.h"
#include "desclistdlg.h"
#include "prefdlg_ext.h"
#include "theme.h"

#include "intl.h"
#include "prefdlg.h"


#define PACK_UNIFORM   ( PACK_UNIFORM_WIDTH | PACK_UNIFORM_HEIGHT )
#define LAYOUT_CENTER  ( LAYOUT_CENTER_Y | LAYOUT_CENTER_X )
#define SPIN_OPTS      ( SPIN_NORMAL | FRAME_SUNKEN | FRAME_THICK )
#define LIST_BOX_OPTS  ( FRAME_SUNKEN | FRAME_THICK | LISTBOX_NORMAL | LAYOUT_FILL_X )
#define STYLE_BTN_OPTS ( TOGGLEBUTTON_NORMAL | TOGGLEBUTTON_KEEPSTATE )


static const char* hint_list[] = {
  _("General application settings"),
  _("Editor specific settings"),
  _("Drag a color square to FG/BG, or double-click FG/BG to edit"),
  _("Double-click an item to edit"),
  _("Edit toolbar buttons"),
  _("User interface colors and font"),
  NULL
};

static FXival whichtab=0;
static int whichlang=0;

FXDEFMAP(PrefsDialog) PrefsDialogMap[]={
  FXMAPFUNC(SEL_COMMAND,PrefsDialog::ID_TAB_SWITCHED,PrefsDialog::onTabSwitch),
  FXMAPFUNC(SEL_COMMAND,PrefsDialog::ID_LANG_SWITCH,PrefsDialog::onLangSwitch),
  FXMAPFUNC(SEL_COMMAND,PrefsDialog::ID_KWORD_EDIT,PrefsDialog::onKeywordEdit),
  FXMAPFUNC(SEL_COMMAND,PrefsDialog::ID_TABOPTS_SWITCH,PrefsDialog::onTabOptsSwitch),
  FXMAPFUNC(SEL_DOUBLECLICKED,PrefsDialog::ID_ACCEL_EDIT,PrefsDialog::onAccelEdit),
  FXMAPFUNC(SEL_KEYPRESS,PrefsDialog::ID_ACCEL_EDIT,PrefsDialog::onAccelEdit),
  FXMAPFUNC(SEL_COMMAND, PrefsDialog::ID_FILTERS_EDIT,PrefsDialog::onFiltersEdit),
  FXMAPFUNC(SEL_COMMAND, PrefsDialog::ID_ERRPATS_EDIT,PrefsDialog::onErrPatsEdit),
  FXMAPFUNC(SEL_COMMAND, PrefsDialog::ID_SYSINCS_EDIT,PrefsDialog::onSysIncsEdit),
  FXMAPFUNCS(SEL_COMMAND,PrefsDialog::ID_EDIT_FILETYPES,PrefsDialog::ID_EDIT_SHABANGS,PrefsDialog::onSyntaxFiletypeEdit),
  FXMAPFUNCS(SEL_COMMAND,PrefsDialog::ID_TBAR_AVAIL_ITEMS,PrefsDialog::ID_TBAR_INSERT_CUSTOM,PrefsDialog::onToolbarEdit)
};

FXIMPLEMENT(PrefsDialog,FXDialogBox,PrefsDialogMap,ARRAYNUMBER(PrefsDialogMap));



class MyColorWell: public FXColorWell {
  FXDECLARE(MyColorWell);
  MyColorWell(){};
public:
  bool grayscale;
  long onDoubleClicked(FXObject*o,FXSelector sel,void*p);
  MyColorWell(FXComposite *p, FXColor clr=0, FXObject *tgt=NULL, FXSelector sel=0,
  FXuint opts=COLORWELL_NORMAL, FXint x=0, FXint y=0, FXint w=0, FXint h=0,
    FXint pl=DEFAULT_PAD, FXint pr=DEFAULT_PAD, FXint pt=DEFAULT_PAD, FXint pb=DEFAULT_PAD):
      FXColorWell(p,clr,tgt,sel,opts,x,y,w,h,pl,pr,pt,pb) { grayscale=false; }
};



FXDEFMAP(MyColorWell) MyColorWellMap[]={
  FXMAPFUNC(SEL_DOUBLECLICKED,0,MyColorWell::onDoubleClicked)
};

FXIMPLEMENT(MyColorWell,FXColorWell,MyColorWellMap,ARRAYNUMBER(MyColorWellMap));



long MyColorWell::onDoubleClicked(FXObject*o,FXSelector sel,void*p)
{
  if(target && target->tryHandle(this,FXSEL(SEL_DOUBLECLICKED,message),(void*)(FXuval)rgba)) return 1;
  if(options&COLORWELL_SOURCEONLY) return 1;
  FXColorDialog colordialog(getShell(),_("Color Dialog"));
  FXColor oldcolor=getRGBA();
  colordialog.setTarget(this);
  colordialog.setSelector(ID_COLORDIALOG);
  colordialog.setRGBA(oldcolor);
  colordialog.setOpaqueOnly(isOpaqueOnly());
  if(!colordialog.execute(PLACEMENT_SCREEN)) { setRGBA(oldcolor,TRUE); }
  return 1;
}



class StyleEdit: public FXHorizontalFrame {
  FXDECLARE(StyleEdit)
protected:
  StyleEdit(){}
private:
  FXLabel *caption;
  FXToggleButton*bold_btn;
  FXToggleButton*italic_btn;
  MyColorWell*fg_btn;
  MyColorWell*bg_btn;
  StyleDef*styledef;
  PrefsDialog*dlg;
  FXFontDesc fontdesc;
  FXFont* labelfont;
public:
  long onStyleBtn(FXObject*o,FXSelector sel,void*p);
  long onColorBtn(FXObject*o,FXSelector sel,void*p);
  StyleEdit(FXComposite *p, StyleDef*def, FXint labelwidth, bool bgonly=false);
  ~StyleEdit(){ delete labelfont; }
  enum {
    ID_STYLE_BTN=FXHorizontalFrame::ID_LAST,
    ID_COLOR_BTN,
    ID_LAST
  };
};


FXDEFMAP(StyleEdit) StyleEditMap[]={
  FXMAPFUNC(SEL_COMMAND,StyleEdit::ID_STYLE_BTN,StyleEdit::onStyleBtn),
  FXMAPFUNC(SEL_COMMAND,StyleEdit::ID_COLOR_BTN,StyleEdit::onColorBtn)
};

FXIMPLEMENT(StyleEdit,FXHorizontalFrame,StyleEditMap,ARRAYNUMBER(StyleEditMap));



long StyleEdit::onStyleBtn(FXObject*o,FXSelector sel,void*p)
{
  FXival state=(FXival)p;
  if (o==bold_btn) {
    if (state==STATE_DOWN) {
      fontdesc.weight=FXFont::Bold;
      styledef->style=(SciDocFontStyle)(styledef->style|Bold);
    } else {
      fontdesc.weight=FXFont::Normal;
      styledef->style=(SciDocFontStyle)(styledef->style&~Bold);
    }
  } else {
    if (state==STATE_DOWN) {
      fontdesc.slant=FXFont::Italic;
      styledef->style=(SciDocFontStyle)(styledef->style|Italic);
    } else {
      fontdesc.slant=FXFont::Straight;
      styledef->style=(SciDocFontStyle)(styledef->style&~Italic);
    }
  }
  FXFont*tmpfont=new FXFont(dlg->getApp(), fontdesc);
  tmpfont->create();
  caption->setFont(tmpfont);
  caption->update();
  return 1;
}



long StyleEdit::onColorBtn(FXObject*o,FXSelector sel,void*p)
{
  FXColorWell*cw=(FXColorWell*)o;
  FXColor rgb=cw->getRGBA();
  if (cw==fg_btn) {
    caption->setTextColor(rgb);
    RgbToHex(rgb,styledef->fg);
  } else {
    caption->setBackColor(rgb);
    setBackColor(rgb);
    RgbToHex(rgb,styledef->bg);
    if (!fg_btn->isEnabled()) {
      caption->setTextColor(rgb>FXRGB(0x80,0x80,0x80)?FXRGB(0,0,0):FXRGB(0xFF,0xFF,0xFF));
    }
  }
  caption->update();
  return 1;
}



static void conceal(FXFrame*w) {
  FXColor c=w->getShell()->getBackColor();
  w->disable();
  w->setShadowColor(c);
  w->setHiliteColor(c);
  w->setBaseColor(c);
  w->setBorderColor(c);
}



StyleEdit::StyleEdit(FXComposite *p, StyleDef*sd, FXint labelwidth, bool bgonly):
   FXHorizontalFrame(p,LAYOUT_FILL_X,0,0,0,0,0,0,0,0,0,0)
{
  dlg=(PrefsDialog*)p->getShell();
  styledef=sd;
  FXColor bg=HexToRGB(sd->bg[0]?sd->bg:dlg->prefs->globalStyle()->bg);
  FXColor fg=HexToRGB(sd->fg[0]?sd->fg:dlg->prefs->globalStyle()->fg);
  setBackColor(bg);
  FXHorizontalFrame*stylebtns=new FXHorizontalFrame(this,LAYOUT_FILL_X|PACK_UNIFORM_WIDTH,0,0,0,0,0,0,0,0,0,0);
  stylebtns->setBackColor(bg);
  FXString tmp=sd->key;
  while (tmp.length()<labelwidth) { tmp.append(' '); }
  caption= new FXLabel(this,tmp,NULL,LABEL_NORMAL|LAYOUT_FILL_X);
  caption->setBackColor(bg);
  caption->setTextColor(fg);

  FXHorizontalFrame*colorbtns=new FXHorizontalFrame(this,LAYOUT_FILL_X,0,0,0,0,0,0,0,0,0,0);
  fg_btn=new MyColorWell(colorbtns, fg, this, ID_COLOR_BTN,COLORWELL_OPAQUEONLY|LAYOUT_FIX_WIDTH,0,0,32,32);
  bg_btn=new MyColorWell(colorbtns, bg, this, ID_COLOR_BTN,COLORWELL_OPAQUEONLY|LAYOUT_FIX_WIDTH,0,0,32,32);
  italic_btn=new FXToggleButton(stylebtns, "S", "S", NULL, NULL, this, ID_STYLE_BTN, STYLE_BTN_OPTS);
  bold_btn = new FXToggleButton(stylebtns, "B", "B", NULL, NULL, this, ID_STYLE_BTN, STYLE_BTN_OPTS);
  italic_btn->setState(sd->style&Italic?STATE_DOWN:STATE_UP);
  bold_btn->setState(sd->style&Bold?STATE_DOWN:STATE_UP);

  caption->setJustify(JUSTIFY_LEFT|JUSTIFY_CENTER_Y);
  GetFontDescription(fontdesc,dlg->scifont);
  fontdesc.weight=sd->style&Bold?FXFont::Bold:FXFont::Normal;
  fontdesc.slant=sd->style&Italic?FXFont::Italic:FXFont::Straight;
  labelfont=new FXFont(dlg->getApp(), fontdesc);
  caption->setFont(labelfont);

  if (bgonly) {
    caption->setTextColor(bg>FXRGB(0x80,0x80,0x80)?FXRGB(0,0,0):FXRGB(0xFF,0xFF,0xFF));
    fg_btn->setRGBA(getShell()->getBackColor());
    conceal(fg_btn);
    conceal(italic_btn);
    conceal(bold_btn);
  }
}



PrefsDialog::~PrefsDialog()
{
  delete scifont;
}


static bool AccelSanity(FXWindow*w, FXHotKey acckey)
{
  FXushort key=FXSELID(acckey);
  FXushort mod=FXSELTYPE(acckey);
  if (key==0) {
    FXMessageBox::error(w, MBOX_OK, _("Invalid keybinding"), _("That keybinding does not end with a valid key name"));
    return false;
  }
  if ((key>=KEY_F1)&&(key<=KEY_F12)) { return true; }
  if ((mod&CONTROLMASK)||(mod&ALTMASK)||(mod&METAMASK)) { return true; }
  return (FXMessageBox::question(w, MBOX_YES_NO, _("Weak keybinding"), "%s\n\n%s",
    _("That key binding doesn't contain any [Ctrl] or [Alt]\n"
      "modifiers, which might cause you some problems."),
    _("Are you sure you want to continue?")
   )==MBOX_CLICKED_YES);
}



static bool AccelUnique(FXWindow*w, FXAccelTable *table, FXHotKey acckey, MenuSpec*spec)
{
  MenuSpec*killcmd=MenuMgr::LookupMenu(TopWindow::ID_KILL_COMMAND);

  if ( (acckey==parseAccel(killcmd->accel)) && (spec!=killcmd)) {
    FXMessageBox::error(w, MBOX_OK, _("Conflicting keybinding"),
      _("Keybinding for \"%s\" must not conflict with \"%s\""),spec->pref,killcmd->pref);
    return false;
  }

  if (!table->hasAccel(acckey)) { return true; }

  if (FXMessageBox::question(w, MBOX_YES_NO, _("Conflicting keybinding"), "%s\n\n%s",
    _("This keybinding appears to conflict with an existing one."),
    _("Are you sure you want to continue?")
  )==MBOX_CLICKED_YES) {
    table->removeAccel(acckey);
    return true;
  } else {
    return false;
  }

}



static bool AccelDelete(FXWindow*w, FXAccelTable *table, const FXString &acctxt)
{
  FXHotKey acckey=parseAccel(acctxt);
  if (!table->hasAccel(acckey)) {
    return true;
  } else {
    if (FXMessageBox::question(w, MBOX_YES_NO, _("Confirm delete"),
      _("Are you sure you want to remove this keybinding?")
    )==MBOX_CLICKED_YES) {
     table->removeAccel(acckey);
      return true;
    } else {
      return false;
    }
  }
}



static bool EditAccel(FXString&acctxt, FXWindow*w, MenuSpec*spec, FXHotKey &acckey)
{
  FXInputDialog dlg(w,"","");
  FXint maxlen=sizeof(spec->accel)-1;
  dlg.setNumColumns(maxlen);
  FXString msg;
  msg.format(
    "%s:\n"
    "   Ctrl+Shift+F12\n"
    "   F3\n"
    "   Alt+G\n\n"
    "%s \"%s\"",
    _("Examples"), _("Keybinding for"), spec->pref);
  FXString orig=acctxt.text();
  while (true) {
    acckey=0;
    if (dlg.getString(acctxt, w->getShell(), _("Edit keybinding"), msg )) {
      if (acctxt.empty()) {
        if (spec->sel==TopWindow::ID_KILL_COMMAND) {
          FXMessageBox::error(w->getShell(), MBOX_OK, _("Empty keybinding"), "%s \"%s\"",
            _("You cannot remove the keybinding for"), spec->pref);
          acctxt=orig.text();
          continue;
        } else {
          return true;
        }
      }
      acckey=parseAccel(acctxt);
      if (acckey) {
        acctxt=unparseAccel(acckey);
        if ((acctxt.length())<maxlen) {
          if (AccelSanity(w,acckey)) { return strcmp(spec->accel, acctxt.text())!=0; }
        } else {
          FXMessageBox::error(w->getShell(), MBOX_OK,
            _("Keybinding too long"), _("Text of keybinding specification must not exceed %d bytes\n"), maxlen);
        }
      } else {
        FXMessageBox::error(w->getShell(), MBOX_OK,
          _("Invalid keybinding"), "%s:\n%s", _("Failed to parse accelerator"), acctxt.text());
        acctxt=orig.text();
      }
    } else {
      return false;
    }
  }
}



long PrefsDialog::onAccelEdit(FXObject*o, FXSelector s, void*p)
{
  if (o!=acclist) { return 0; }
    switch ( FXSELTYPE(s) ) {
    case SEL_DOUBLECLICKED: {  break;  }
    case SEL_KEYPRESS: {
      FXint code=((FXEvent*)p)->code;
      switch (code) {
        case KEY_Return: { break; }
        case KEY_KP_Enter: { break; }
        case KEY_F2: { break; }
        case KEY_space: { break; }
        default: { return 0; }
      }
      break;
    }
    default: { return 0; }
  }
  MenuSpec*spec=(MenuSpec*)(acclist->getItemData(acclist->getCurrentItem()));
  FXWindow*own=main_win;
  FXAccelTable *table=own?own->getAccelTable():NULL;
  if (spec && own && table) {
    FXString acctxt=spec->accel;
    FXHotKey acckey;
    if ( EditAccel(acctxt,this,spec,acckey) ) {
      if (acctxt.empty()) {
        if (AccelDelete(this,table,spec->accel)) {
          memset(spec->accel,0,sizeof(spec->accel));
          if (spec->ms_mc) { spec->ms_mc->setAccelText(spec->accel); }
          FXString txt;
          txt.format("%s\t",spec->pref);
          acclist->setItemText(acclist->getCurrentItem(),txt);
        }
      } else {
        if (AccelUnique(this, table, acckey, spec)) {
          FXHotKey oldkey=parseAccel(spec->accel);
          memset(spec->accel,0,sizeof(spec->accel));
          strncpy(spec->accel, acctxt.text(),sizeof(spec->accel)-1);
          if (oldkey && table->hasAccel(oldkey)) { table->removeAccel(oldkey); }
          if (spec->sel==TopWindow::ID_KILL_COMMAND){
            ((TopWindow*)own)->SetKillCommandAccelKey(acckey);
          } else  {
            if (spec->ms_mc) {
              spec->ms_mc->setSelector(0);
              spec->ms_mc->setAccelText(spec->accel);
              table->addAccel(acckey,spec->ms_mc->getTarget(),FXSEL(SEL_COMMAND,spec->sel));
            } else {
              table->addAccel(acckey,own,FXSEL(SEL_COMMAND,spec->sel));
            }
          }
          FXString txt;
          txt.format("%s\t%s",spec->pref,spec->accel);
          acclist->setItemText(acclist->getCurrentItem(),txt);
        }
      }
    }
  } else {
    FXMessageBox::error(getShell(), MBOX_OK, _("Internal error"), _("Failed to retrieve keybinding information"));
  }
  return 1;
}



long PrefsDialog::onTabOptsSwitch(FXObject*o,FXSelector sel,void*p)
{
  LangStyle*ls=(LangStyle*)langlist->getItemData(langlist->getCurrentItem());
  ls->tabs=(TabPolicy)tabopts->getCurrentItem();
  return 1;
}

#define SetPad(padwin, padsize) \
(padwin)->setPadLeft(padsize); \
(padwin)->setPadTop(padsize); \
(padwin)->setPadRight(padsize); \
(padwin)->setPadBottom(padsize);


long PrefsDialog::onKeywordEdit(FXObject*o,FXSelector sel,void*p)
{
  LangStyle*ls=(LangStyle*)langlist->getItemData(langlist->getCurrentItem());
  int n=wordlist->getCurrentItem();
  char *words=(char*)wordlist->getItemData(wordlist->getCurrentItem());
  FXDialogBox kwdlg(this,_("Edit word list"),DECOR_TITLE|DECOR_BORDER,0,0,480,320);
  SetPad(&kwdlg, 0);
  FXVerticalFrame*vframe=new FXVerticalFrame(&kwdlg, LAYOUT_FILL);
  SetPad(vframe, 0);
  FXVerticalFrame*scframe=new FXVerticalFrame(vframe, LAYOUT_FILL|FRAME_SUNKEN|FRAME_THICK);
  SetPad(scframe, 0);
  FXScintilla*sc=new FXScintilla(scframe, NULL,0,TEXT_WORDWRAP|LAYOUT_FILL|HSCROLLER_NEVER);
  sc->sendMessage(SCI_SETHSCROLLBAR,false, 0);
  sc->sendString(SCI_STYLESETFONT, STYLE_DEFAULT, scifont->getName().text());
  sc->sendMessage(SCI_STYLESETSIZE, STYLE_DEFAULT, 10);
  sc->sendMessage(SCI_SETWRAPMODE,SC_WRAP_WORD,0);
  sc->sendMessage(SCI_SETMARGINWIDTHN,1,0);
  sc->sendMessage(SCI_SETMARGINLEFT,0,4);
  sc->sendMessage(SCI_SETMARGINRIGHT,0,4);
  sc->sendString(SCI_APPENDTEXT,strlen(words),words);
  FXHorizontalFrame *btns=new FXHorizontalFrame(vframe, PACK_UNIFORM|LAYOUT_FILL_X|LAYOUT_SIDE_BOTTOM);
  new FXButton(btns,_(" &OK "), NULL, &kwdlg, FXDialogBox::ID_ACCEPT);
  new FXButton(btns,_(" &Cancel "), NULL, &kwdlg, FXDialogBox::ID_CANCEL);
  kwdlg.changeFocus(sc);
  if (kwdlg.execute(PLACEMENT_SCREEN)) {
    FXString newwords;
    newwords.length(sc->sendMessage(SCI_GETLENGTH,0,0)+1);
    sc->sendString(SCI_GETTEXT,newwords.length(), newwords.text());
    const char*whitechars="\t\r\n";
    const char *c;
    for (c=whitechars; *c; c++) {
      newwords.substitute(*c, ' ', true);
      newwords.trim();
      newwords.simplify();
    }
    if (strcmp(words, newwords.text())!=0) {
      SetKeywordList(ls,n,newwords);
      wordlist->setItemData(wordlist->getCurrentItem(), ls->words[n]);
    }
  }
  if (sc->hasClipboard()) {
    ((TopWindow*)main_win)->SaveClipboard();
  }
  return 1;
}


#define MakeStylePan(_w) { \
  style_pan=new FXVerticalFrame((FXComposite*)_w,\
  FRAME_SUNKEN|LAYOUT_FILL,0,0,0,0,0,0,0,0,0,0); \
}



long PrefsDialog::onLangSwitch(FXObject*o,FXSelector sel,void*p)
{
  FXListBox* list=(FXListBox*)o;
  FXival index=(FXival)p;
  whichlang=index;
  StyleDef*sd0,*sdN;
  wordlist->clearItems();
  LangStyle* ls=(LangStyle*)list->getItemData(index);
  if (ls) {
    wildcardbtn->enable();
    wildcardbtn->setUserData(ls);
    shabangbtn->enable();
    shabangbtn->setUserData(ls);
    kwordslab->enable();
    tabopts->enable();
    taboptlab->enable();
    tabopts->setCurrentItem(ls->tabs);
    tabwidthspin->enable();
    tabwidthspin->setUserData(ls);
    tabwidthspin->setValue(ls->tabwidth);
    tabwidthlab->enable();
    if (ls->words){
      char**words;
      const LexerModule*lm=Catalogue::Find(ls->id);
      int nwl=lm?lm->GetNumWordLists():-1;
      int i=0;
      for (words=ls->words; *words; words++) {
        FXString wld=(lm&&(i<nwl))?lm->GetWordListDescription(i++):NULL;
        if (wld=="Unused") { wld=_("User defined"); }
        if (wld.empty()) { wld.format(_("words #%d"), wordlist->getNumItems()+1); }
        wordlist->appendItem(wld,NULL,*words);
      }
    }
    if (wordlist->getNumItems()>0) {
      kwordslab->enable();
      wordlist->enable();
      wordbtn->enable();
      wordlist->setNumVisible(wordlist->getNumItems());
    } else {
      kwordslab->disable();
      wordlist->disable();
      wordbtn->disable();
    }
    sd0=ls->styles;
  } else {
    kwordslab->disable();
    wordlist->disable();
    wordbtn->disable();
    tabopts->setCurrentItem(0);
    tabopts->disable();
    taboptlab->disable();
    tabwidthspin->setUserData(NULL);
    tabwidthspin->disable();
    tabwidthlab->disable();
    wildcardbtn->setUserData(NULL);
    wildcardbtn->disable();
    shabangbtn->setUserData(NULL);
    shabangbtn->disable();
    sd0=prefs->globalStyle();
  }
  FXWindow* w=style_pan->getParent();
  delete style_pan;
  MakeStylePan(w);
  if (shown()) { style_pan->create(); }
  StyleEdit*se;
  for (sdN=sd0; sdN->key; sdN++) {
    se=new StyleEdit(style_pan,sdN,maxw);
    if (shown()) { se->create(); }
  }
  if (sd0==prefs->globalStyle()) {

    se=new StyleEdit(style_pan,prefs->selectionStyle(),maxw,true);
    if (shown()) { se->create(); }

    se=new StyleEdit(style_pan,prefs->caretStyle(),maxw,true);
    if (shown()) { se->create(); }

    se=new StyleEdit(style_pan,prefs->caretlineStyle(),maxw,true);
    if (shown()) { se->create(); }

    se=new StyleEdit(style_pan,prefs->rightmarginStyle(),maxw,true);
    if (shown()) { se->create(); }

    se=new StyleEdit(style_pan,prefs->whitespaceStyle(),maxw,true);
    if (shown()) { se->create(); }

  }
  if (style_pan->numChildren()==0) {
    style_hdr->hide();
    FXLabel*lab=new FXLabel(style_pan, _("No font styles available for this item"),
      NULL,LAYOUT_FILL_X|JUSTIFY_CENTER_X);
    lab->setPadTop(32);
    if (shown()) { lab->create(); }
  } else {
    style_hdr->show();
  }
  return 1;
}


// Subclass FXListItem to show custom tooltip...
class TBarListItem: public FXListItem {
public:
  TBarListItem(const FXString &text, FXIcon *ic=NULL, void *ptr=NULL):FXListItem(text,ic,ptr){ }
  virtual FXString getTipText() const {
    FXString tip;
    MenuSpec*spec=(MenuSpec*)getData();
    if (spec) { MenuMgr::GetTBarBtnTip(spec,tip); } else { tip=label.text(); }
    return tip;
  }
};



// Subclass FXIconItem to show custom tooltip...
class KBindListItem: public FXIconItem {
public:
  KBindListItem(const FXString &text, FXIcon*bi=NULL, FXIcon*mi=NULL, void*ptr=NULL):FXIconItem(text,bi,mi,ptr) {}
  virtual FXString getTipText() const {
    FXString tip;
    MenuSpec*spec=(MenuSpec*)getData();
    if (spec) { MenuMgr::GetTBarBtnTip(spec,tip); } else { tip=label.section('\t',0).text(); }
    return tip;
  }
};



static FXuint changed_toolbar = ToolbarUnchanged;

FXuint PrefsDialog::ChangedToolbar()
{
  if (Theme::changed() & ThemeChangedFont) { changed_toolbar |= ToolbarChangedFont; }
  return changed_toolbar;
}


void PrefsDialog::AddToolbarButton(FXListItem*item, FXint &iUsed, FXint&nUsed)
{
  if (iUsed<0) {
    tbar_used_items->appendItem(item);
  } else {
    tbar_used_items->getItem(iUsed)->setSelected(false);
    if (iUsed<(nUsed-1)) {
      tbar_used_items->insertItem(iUsed+1,item);
    } else {
      tbar_used_items->appendItem(item);
    }
  }
  iUsed++;
  tbar_used_items->selectItem(iUsed);
  tbar_used_items->setCurrentItem(iUsed);
  nUsed++;
  changed_toolbar|=ToolbarChangedLayout;
}



long PrefsDialog::onToolbarEdit(FXObject*o,FXSelector sel,void*p)
{
  FXint iAvail=tbar_avail_items->getCurrentItem();
  FXint iUsed=tbar_used_items->getCurrentItem();
  FXint nUsed=tbar_used_items->getNumItems();
  MenuSpec*spec=NULL;
  TBarListItem*item=NULL;
  switch(FXSELID(sel)) {
    case ID_TBAR_AVAIL_ITEMS: { return 1; }
    case ID_TBAR_INSERT_CUSTOM: {
      FXMenuCommand*mc;
      if (ToolsTree::SelectTool(this, ((TopWindow*)main_win)->UserMenus(), mc)) {
        const char*newpath=(const char*)mc->getUserData();
        if (newpath) {
          // If the command is already in the used items list, just select it...
          for (FXint i=0; i<nUsed; i++) {
            const char*oldpath=MenuMgr::GetUsrCmdPath((MenuSpec*)tbar_used_items->getItemData(i));
            if (oldpath && (strcmp(newpath, oldpath)==0)) {
              tbar_used_items->selectItem(i);
              tbar_used_items->setCurrentItem(i);
              tbar_used_items->makeItemVisible(i);
              return 1;
            }
          }
        }
        spec=MenuMgr::AddTBarUsrCmd(mc);
        item=new TBarListItem(spec->pref,NULL,(void*)spec);
        AddToolbarButton(item, iUsed, nUsed);
      }
    }
    case ID_TBAR_USED_ITEMS: { break; }
    case ID_TBAR_ITEM_INSERT: {
      item=(TBarListItem*)(tbar_avail_items->extractItem(iAvail));
      AddToolbarButton(item, iUsed, nUsed);
      break;
    }
    case ID_TBAR_ITEM_REMOVE: {
      item=(TBarListItem*)(tbar_used_items->extractItem(tbar_used_items->getCurrentItem()));
      spec=(MenuSpec*)item->getData();
      if (spec->type=='u') {
        MenuMgr::RemoveTBarUsrCmd(spec);
      } else {
        tbar_avail_items->clearItems();
        for (spec=MenuMgr::MenuSpecs(); spec->sel!=TopWindow::ID_LAST; spec++) {
          if (spec->sel==TopWindow::ID_KILL_COMMAND) { continue; }
          if (tbar_used_items->findItemByData((void*)spec)<0) {
            tbar_avail_items->appendItem(new TBarListItem(spec->pref, NULL, (void*)spec));
          }
        }
        iAvail=tbar_avail_items->findItemByData(item->getData());
        tbar_avail_items->selectItem(iAvail);
        tbar_avail_items->setCurrentItem(iAvail);
        tbar_avail_items->makeItemVisible(iAvail);
      }
      nUsed--;
      changed_toolbar|=ToolbarChangedLayout;
      break;
    }
    case ID_TBAR_ITEM_RAISE: {
      tbar_used_items->moveItem(iUsed, iUsed-1);
      iUsed--;
      tbar_used_items->selectItem(iUsed);
      tbar_used_items->setCurrentItem(iUsed);
      changed_toolbar|=ToolbarChangedLayout;
      break;
    }
    case ID_TBAR_ITEM_LOWER: {
      tbar_used_items->moveItem(iUsed, iUsed+1);
      iUsed++;
      tbar_used_items->selectItem(iUsed);
      tbar_used_items->setCurrentItem(iUsed);
      changed_toolbar|=ToolbarChangedLayout;
      break;
    }
  }
  tbar_used_items->makeItemVisible(iUsed);
  if (nUsed<TBAR_MAX_BTNS) {
    tbar_ins_btn->enable();
    tbar_custom_btn->enable();
  } else {
    tbar_ins_btn->disable();
    tbar_custom_btn->disable();
  }
  if (nUsed>0) { tbar_rem_btn->enable(); } else { tbar_rem_btn->disable(); }
  if (iUsed>0) { tbar_raise_btn->enable(); } else {tbar_raise_btn->disable(); }
  if (iUsed<(nUsed-1)) { tbar_lower_btn->enable(); } else {tbar_lower_btn->disable(); }
  FXint*btns=MenuMgr::TBarBtns();
  for (iUsed=0; iUsed<TBAR_MAX_BTNS; iUsed++) {
    btns[iUsed]=TopWindow::ID_LAST;
  }
  for (iUsed=0; iUsed<nUsed; iUsed++) {
    spec=(MenuSpec*)(tbar_used_items->getItemData(iUsed));
    btns[iUsed]=spec->sel;
  }
  return 1;
}



void PrefsDialog::MakeToolbarTab()
{
  new FXTabItem(tabs,_("toolbar"));
  FXHorizontalFrame *frame=new FXHorizontalFrame(tabs,FRAME_RAISED|LAYOUT_FILL);
  FXVerticalFrame* left_column=new FXVerticalFrame(frame,FRAME_SUNKEN|LAYOUT_FILL);
  FXVerticalFrame* mid_column=new FXVerticalFrame(frame,FRAME_NONE|LAYOUT_CENTER_Y|PACK_UNIFORM_WIDTH);
  FXVerticalFrame* right_column=new FXVerticalFrame(frame,FRAME_SUNKEN|LAYOUT_FILL);
  new FXLabel(left_column, _("&Available items:"));

  tbar_ins_btn=new FXButton(mid_column, _("&Insert>>"),NULL,this,ID_TBAR_ITEM_INSERT,BUTTON_NORMAL|LAYOUT_CENTER_Y);
  tbar_rem_btn=new FXButton(mid_column, _("<<&Remove"),NULL,this,ID_TBAR_ITEM_REMOVE,BUTTON_NORMAL|LAYOUT_CENTER_Y);
  FXLabel*btn_size_cpn=new FXLabel(mid_column, _("Button size:"), NULL, LABEL_NORMAL|JUSTIFY_LEFT);
  btn_size_cpn->setPadTop(48);
  FXListBox *tbar_size_list=new FXListBox(mid_column,prefs,Settings::ID_SET_TOOLBAR_BTN_SIZE);
  tbar_size_list->setUserData(&changed_toolbar);
  tbar_size_list->appendItem(_("small"),  NULL,NULL);
  tbar_size_list->appendItem(_("medium"), NULL,NULL);
  tbar_size_list->appendItem(_("large"),  NULL,NULL);
  tbar_size_list->setNumVisible(3);
  tbar_size_list->setCurrentItem(prefs->ToolbarButtonSize);

  FXCheckButton*wrap_tbar_chk=new FXCheckButton(mid_column,_("Wrap buttons"),prefs,Settings::ID_TOGGLE_WRAP_TOOLBAR);
  wrap_tbar_chk->setCheck(prefs->WrapToolbar);
  wrap_tbar_chk->setUserData(&changed_toolbar);

  new FXLabel(right_column, _("&Visible items:"));
  tbar_avail_items=new FXList(left_column,this,ID_TBAR_AVAIL_ITEMS,LIST_BROWSESELECT|LAYOUT_FILL);
  tbar_used_items=new FXList(right_column,this,ID_TBAR_USED_ITEMS,LIST_BROWSESELECT|LAYOUT_FILL);

  FXHorizontalFrame* AvailBtns=new FXHorizontalFrame( left_column,
                                                     FRAME_RAISED|LAYOUT_FILL_X|LAYOUT_CENTER_X|PACK_UNIFORM_WIDTH);

  tbar_custom_btn=new FXButton( AvailBtns, _("Custom &Tools..."),
                                NULL,this, ID_TBAR_INSERT_CUSTOM,BUTTON_NORMAL|LAYOUT_CENTER_X);

  FXHorizontalFrame* UpDnBtns=new FXHorizontalFrame( right_column,
                                                     FRAME_RAISED|LAYOUT_FILL_X|LAYOUT_CENTER_X|PACK_UNIFORM_WIDTH);

  tbar_raise_btn=new FXButton(UpDnBtns, _("Move &Up"),  NULL,this, ID_TBAR_ITEM_RAISE,BUTTON_NORMAL|LAYOUT_CENTER_X);
  tbar_lower_btn=new FXButton(UpDnBtns, _("Move &Down"),NULL,this, ID_TBAR_ITEM_LOWER,BUTTON_NORMAL|LAYOUT_CENTER_X);

  for (MenuSpec*spec=MenuMgr::MenuSpecs(); spec->sel!=TopWindow::ID_LAST; spec++) {
    if (spec->ms_mc) {
      tbar_avail_items->appendItem(new TBarListItem(spec->pref, NULL, (void*)spec));
    }
  }
  for (FXint*sel=MenuMgr::TBarBtns(); *sel!=TopWindow::ID_LAST; sel++) {
    MenuSpec* spec=MenuMgr::LookupMenu(*sel);
    if (spec) {
      FXint found=tbar_avail_items->findItemByData((void*)spec);
      if (found>=0) {
        TBarListItem*item=(TBarListItem*)tbar_avail_items->extractItem(found);
        item->setSelected(false);
        tbar_used_items->appendItem(item);
      } else {
        tbar_used_items->appendItem(new TBarListItem(spec->pref,NULL,(void*)spec));
      }
    }
  }
  tbar_avail_items->selectItem(0);
  if (tbar_used_items->getNumItems()) { tbar_used_items->selectItem(0); }
  onToolbarEdit(NULL,0,NULL);
}



void PrefsDialog::MakeKeybindingsTab()
{
  new FXTabItem(tabs,_("keys"));
  FXHorizontalFrame *frame=new FXHorizontalFrame(tabs,FRAME_RAISED|LAYOUT_FILL);
  acclist=new FXIconList(frame,
    this,ID_ACCEL_EDIT,FRAME_SUNKEN|FRAME_THICK|LAYOUT_SIDE_TOP|LAYOUT_FILL_Y|LAYOUT_FIX_WIDTH|ICONLIST_BROWSESELECT);

  acclist->appendHeader(_("action"));
  acclist->appendHeader(_("keybinding"));
  FXString spaces;
  for (MenuSpec*spec=MenuMgr::MenuSpecs(); spec->sel!=TopWindow::ID_LAST; spec++) {
    FXString txt;
    txt.format("%s\t%s", spec->pref, spec->accel);
    acclist->appendItem(new KBindListItem(txt, NULL, NULL, (void*)spec));
  }
  acclist->selectItem(0);
}



static ColorName rainbow[] = {
  "#000000","#AA0000","#CC0000","#EE0000",
  "#DDB000","#CCCC00","#B0CC00","#00DD00",
  "#00CCB0","#00DDDD","#00B0DD","#0000EE",
  "#B000DD","#DD00DD","#FFCCEE","#FFCCCC",
  "#FFEECC","#FFFFCC","#EEFFCC","#CCFFCC",
  "#CCFFEE","#CCFFFF","#CCEEFF","#CCCCFF",
  "#EECCFF","#FFCCFF","#FFE0FF","#FFFFFF",
  "\0\0\0\0\0\0\0"
};



long PrefsDialog::onSyntaxFiletypeEdit(FXObject*o,FXSelector sel,void*p)
{
  ClipTextDialog *tb=NULL;
  LangStyle*ls=(LangStyle*)((FXButton*)o)->getUserData();
  FXString txt;
  switch (FXSELID(sel)) {
    case ID_EDIT_FILETYPES: {
      txt.format( _(
        "A list of wildcard masks, each separated by the pipe \"|\" symbol.\n"
        "\n"
        "Filenames that match any of these patterns will default to \"%s\" language.\n"
      ), ls->name );
      tb=new ClipTextDialog(this, _("Edit file types"), txt);
      tb->setText(ls->mask);
      sel=FXSEL(SEL_COMMAND,Settings::ID_SET_FILETYPES);
      break;
    }
    case ID_EDIT_SHABANGS: {
      txt.format( _(
        "A list of program names, each separated by the pipe \"|\" symbol.\n"
        "\n"
        "Files beginning with a #! interpreter directive containing any of\n"
        "these program names will default to \"%s\" language.\n"
      ), ls->name );
      tb=new ClipTextDialog(this,_("Edit shabang apps"),txt);
      tb->setText(ls->apps);
      sel=FXSEL(SEL_COMMAND,Settings::ID_SET_SHABANGS);
      break;
    }
  }
  tb->setUserData(ls);
  tb->setNumColumns(40);
  if (tb->execute()) {
    prefs->handle(tb,sel,NULL);
  }
  delete tb;
  return 1;
}



void PrefsDialog::MakeSyntaxTab()
{
  new FXTabItem(tabs,_("syntax"));
  FXHorizontalFrame *frame;
  FXVerticalFrame *left_column;
  FXVerticalFrame *right_column;
  FXint i=0;

  FXVerticalFrame*vframe= new FXVerticalFrame(tabs,FRAME_RAISED|LAYOUT_FILL);
  frame=new FXHorizontalFrame(vframe,FRAME_RAISED|LAYOUT_FILL);
  left_column=new FXVerticalFrame(frame,FRAME_SUNKEN|LAYOUT_FILL);
  right_column=new FXVerticalFrame(frame,FRAME_SUNKEN|LAYOUT_FILL,0,0,0,0,0,0,0,0,0,0);
  style_hdr=new FXHorizontalFrame(right_column,LAYOUT_FILL_X|FRAME_GROOVE,0,0,0,0,0,0,0,0,0,0);
  new FXLabel(style_hdr, _("Slant/Bold"),NULL,FRAME_SUNKEN);
  new FXLabel(style_hdr, _("Style Name"), NULL, LAYOUT_FILL_X|FRAME_SUNKEN);
  new FXLabel(style_hdr, "  FG  /  BG  ",NULL,FRAME_SUNKEN);

  scroll=new FXScrollWindow(right_column,LAYOUT_FILL|HSCROLLING_OFF|VSCROLLING_ON|VSCROLLER_ALWAYS );
  MakeStylePan(scroll);

  frame=new FXHorizontalFrame(left_column,FRAME_NONE|LAYOUT_FILL_X);
  new FXLabel(frame,_("language:"));
  langlist=new FXListBox(frame,this,ID_LANG_SWITCH,LIST_BOX_OPTS);
  StyleDef*sd;
  for (sd=prefs->globalStyle(); sd->key; sd++) {if (strlen(sd->key)>maxw) { maxw=strlen(sd->key); } }
  langlist->appendItem(_("global"),NULL,NULL);
  LangStyle*ls;
  for (ls=languages; ls->name; ls++) {
    langlist->appendItem(ls->name,NULL,ls);
    for (sd=ls->styles; sd->key; sd++) {if (strlen(sd->key)>maxw) { maxw=strlen(sd->key); } }
  }
  langlist->setNumVisible(langlist->getNumItems()>12?12:langlist->getNumItems());
  langlist->setCurrentItem(whichlang);
  FXVerticalFrame*btns=new FXVerticalFrame(left_column,FRAME_NONE);
  btns->setPadLeft(12);
  wildcardbtn=new FXButton(btns,_("File types..."),NULL,this,ID_EDIT_FILETYPES,BUTTON_NORMAL|LAYOUT_FILL_X);
  shabangbtn=new FXButton(btns,_("#! programs..."),NULL,this,ID_EDIT_SHABANGS,BUTTON_NORMAL|LAYOUT_FILL_X);
  kwordslab=new FXLabel(left_column,_("keywords:"));
  kwordslab->setPadTop(12);
  frame=new FXHorizontalFrame(left_column,FRAME_NONE|LAYOUT_FILL);
  wordlist=new FXListBox(frame,NULL,0,LIST_BOX_OPTS);
  wordbtn=new FXButton(frame, "...", NULL, this, ID_KWORD_EDIT);

  frame=new FXHorizontalFrame(left_column,FRAME_NONE|LAYOUT_FILL_X|LAYOUT_SIDE_BOTTOM);
  taboptlab=new FXLabel(frame,_("Use tabs:"));
  tabopts=new FXListBox(frame,this,ID_TABOPTS_SWITCH,LIST_BOX_OPTS);
  tabopts->appendItem(_("use default setting"));
  tabopts->appendItem(_("always use tabs"));
  tabopts->appendItem(_("never use tabs"));
  tabopts->appendItem(_("detect from content"));
  tabopts->setNumVisible(4);

  frame=new FXHorizontalFrame(left_column,FRAME_NONE|LAYOUT_FILL_X|LAYOUT_SIDE_BOTTOM);
  tabwidthspin=new FXSpinner(frame, 3, prefs, Settings::ID_SET_TAB_WIDTH_FOR_LANG,SPIN_OPTS);
  tabwidthspin->setRange(0,16);
  tabwidthspin->setValue(0);
  tabwidthlab=new FXLabel(frame,"Tab width (zero to use default)");

  frame=new FXHorizontalFrame(vframe,FRAME_RAISED|LAYOUT_FILL,0,0,0,0,0,0,0,0,0,0);

  for (i=0;rainbow[i][0]; i++) {
    new FXColorWell(frame,
    HexToRGB(rainbow[i]),NULL,0,FRAME_NONE|COLORWELL_OPAQUEONLY|COLORWELL_SOURCEONLY|COLORWELL_NORMAL,0,0,0,0,0,0,0,0);
  }
  onLangSwitch(langlist,FXSEL(SEL_COMMAND,ID_LANG_SWITCH), (void*)whichlang);
}



long PrefsDialog::onFiltersEdit(FXObject*o,FXSelector sel,void*p)
{
  FileFiltersDlg(this).execute();
#ifdef WIN32
  hide();
  show();
  ((FXWindow*)o)->setFocus();
#endif
  return 1;
}



long PrefsDialog::onErrPatsEdit(FXObject*o,FXSelector sel,void*p)
{
  ErrPatDlg(this).execute();
#ifdef WIN32
  hide();
  show();
  ((FXWindow*)o)->setFocus();
#endif
  return 1;
}



long PrefsDialog::onSysIncsEdit(FXObject*o,FXSelector sel,void*p)
{
  SysIncDlg(this).execute();
#ifdef WIN32
  hide();
  show();
  ((FXWindow*)o)->setFocus();
#endif
  return 1;
}



void PrefsDialog::MakeGeneralTab()
{
  new FXTabItem(tabs,_("general"));
  FXHorizontalFrame* hframe;
  FXVerticalFrame* vframe;
  FXVerticalFrame* left_column;
  FXVerticalFrame* right_column;
  FXCheckButton*chk;
  ClipTextField *tf;
  FXSpinner *spin;
  vframe=new FXVerticalFrame(tabs,FRAME_RAISED|LAYOUT_FILL,0,0,0,0,0,0,0,0,0,0);
  hframe=new FXHorizontalFrame(vframe,FRAME_NONE|LAYOUT_FILL|PACK_UNIFORM_WIDTH);
  left_column=new FXVerticalFrame(hframe,FRAME_SUNKEN|LAYOUT_FILL/*|PACK_UNIFORM_HEIGHT*/);
  right_column=new FXVerticalFrame(hframe,FRAME_SUNKEN|LAYOUT_FILL/*|PACK_UNIFORM_HEIGHT*/);

  chk=new FXCheckButton(left_column, _("Watch files for external changes."), prefs, Settings::ID_TOGGLE_WATCH_EXTERN);
  chk->setCheck(prefs->WatchExternChanges, FALSE);
  chk->setPadBottom(8);

  hframe=new FXHorizontalFrame(left_column);
  SetPad(hframe,0);
  chk=new FXCheckButton(hframe, _("Backup files every "), prefs, Settings::ID_TOGGLE_AUTOSAVE);
  chk->setCheck(prefs->Autosave, FALSE);
  chk->setPadBottom(12);
  spin=new FXSpinner(hframe, 3, prefs, Settings::ID_SET_AUTOSAVE_INT,SPIN_OPTS);
  spin->setRange(15,900);
  spin->setValue(prefs->AutosaveInterval);

  new FXLabel(hframe, _(" seconds."));
  if (!chk->getCheck()) { spin->disable(); }
  chk->setUserData(spin);

  new FXLabel(left_column,  _("Ask before closing multiple files:"));
  chk=new FXCheckButton(left_column, _("From menu"), prefs, Settings::ID_TOGGLE_ASK_CLOSE_MULTI_MENU);
  chk->setCheck(prefs->PromptCloseMultiMenu, FALSE);
  chk->setPadLeft(12);
  chk=new FXCheckButton(left_column, _("On exit"), prefs, Settings::ID_TOGGLE_ASK_CLOSE_MULTI_EXIT);
  chk->setCheck(prefs->PromptCloseMultiExit, FALSE);
  chk->setPadLeft(12);
  chk->setPadBottom(12);

  hframe=new FXHorizontalFrame(left_column);
  hframe->setPadBottom(8);
  tf=new ClipTextField(hframe, 3, prefs, Settings::ID_SET_MAX_FILES,TEXTFIELD_LIMITED|TEXTFIELD_INTEGER|TEXTFIELD_NORMAL);
  char maxfiles[8]="\0\0\0\0\0\0\0";
  snprintf(maxfiles, sizeof(maxfiles)-1, "%d", prefs->MaxFiles);
  tf->setText(maxfiles);
  new FXLabel(hframe, _("Maximum number of open files."));

  (new FXHorizontalSeparator(left_column,LAYOUT_SIDE_TOP|LAYOUT_FILL_X|SEPARATOR_GROOVE))->setPadBottom(12);
  hframe=new FXHorizontalFrame(left_column);
  hframe->setPadLeft(12);
  new FXButton(hframe, _(" File dialog filters... "), NULL, this, ID_FILTERS_EDIT);

  hframe=new FXHorizontalFrame(left_column, LAYOUT_FILL_X);
  (new FXLabel(hframe, _("Remember\nlast-used filter :")))->setJustify(JUSTIFY_LEFT);
  FXListBox* list=new FXListBox(hframe,prefs,Settings::ID_SET_KEEP_FILE_FILTER,LIST_BOX_OPTS|LAYOUT_BOTTOM);
  list->appendItem(_("never"),NULL,NULL);
  list->appendItem(_("during session"),NULL,NULL);
  list->appendItem(_("across sessions"),NULL,NULL);
  list->setNumVisible(list->getNumItems());
  list->setCurrentItem(prefs->KeepFileFilter);

  new FXLabel(right_column,_("Save open files before executing:"));
  chk=new FXCheckButton(right_column, _("Tools->Filter Selection"), prefs, Settings::ID_SAVE_ON_FILTER_SEL);
  chk->setCheck(prefs->SaveBeforeFilterSel, FALSE);
  chk->setPadLeft(12);

  chk=new FXCheckButton(right_column, _("Tools->Insert Command"), prefs, Settings::ID_SAVE_ON_INS_CMD);
  chk->setCheck(prefs->SaveBeforeInsCmd, FALSE);
  chk->setPadLeft(12);

  chk=new FXCheckButton(right_column, _("Tools->Execute Command"), prefs, Settings::ID_SAVE_ON_EXEC_CMD);
  chk->setCheck(prefs->SaveBeforeExecCmd, FALSE);
  chk->setPadLeft(12);

#ifndef WIN32
  hframe=new FXHorizontalFrame(right_column);
  hframe->setPadTop(16);
  new FXLabel(hframe, _("Shell command: "));
  tf=new ClipTextField(hframe,18,prefs,Settings::ID_SET_SHELL_CMD,TEXTFIELD_NORMAL);
  tf->setText(prefs->ShellCommand);
#endif

  new FXHorizontalSeparator(right_column,LAYOUT_SIDE_TOP|LAYOUT_FILL_X|SEPARATOR_GROOVE);

  hframe=new FXHorizontalFrame(right_column);
  hframe->setPadTop(12);
  spin=new FXSpinner(hframe, 2, prefs, Settings::ID_SET_WHEEL_LINES,SPIN_OPTS);
  spin->setRange(1,32);
  spin->setValue(prefs->WheelLines);
  new FXLabel(hframe, _("Mouse wheel acceleration."));

  hframe=new FXHorizontalFrame(right_column);
  hframe->setPadTop(12);
  spin=new FXSpinner(hframe, 4, prefs, Settings::ID_SET_TAB_TITLE_MAX_WIDTH,SPIN_OPTS);
  spin->setRange(0,prefs->ScreenWidth);
  spin->setValue(prefs->TabTitleMaxWidth);
  new FXLabel(hframe, _("Max width for tab titles.\n( in pixels;  0 = no limit. )"));

  right_column=new FXVerticalFrame(right_column,FRAME_NONE|PACK_UNIFORM_WIDTH|LAYOUT_CENTER_X|LAYOUT_BOTTOM);
  right_column->setVSpacing(8);
  new FXButton(right_column, _("System header locations... "),NULL,this,ID_SYSINCS_EDIT,BUTTON_NORMAL|LAYOUT_CENTER_X);
  new FXButton(right_column, _("Output pane line matching... "),NULL,this,ID_ERRPATS_EDIT,BUTTON_NORMAL|LAYOUT_CENTER_X);
}



void PrefsDialog::MakeEditorTab()
{
  new FXTabItem(tabs,_("editor"));
  FXHorizontalFrame *frame;
  FXVerticalFrame *column;
  FXHorizontalFrame *spinframe;
  FXCheckButton*chk;
  FXSpinner *spin;
  FXListBox*list;

  frame=new FXHorizontalFrame(tabs,FRAME_RAISED|LAYOUT_FILL);
  column=new FXVerticalFrame(frame,FRAME_SUNKEN|LAYOUT_FILL|PACK_UNIFORM_HEIGHT);

  spinframe=new FXHorizontalFrame(column);
  new FXLabel(spinframe, _("Auto indent:"));
  list=new FXListBox(spinframe,prefs,Settings::ID_SET_AUTO_INDENT,LIST_BOX_OPTS);
  list->appendItem(_("off"),NULL,NULL);
  list->appendItem(_("basic"),NULL,NULL);
  list->appendItem(_("smart"),NULL,NULL);
  list->setNumVisible(list->getNumItems());
  list->setCurrentItem(prefs->AutoIndent);

  chk=new FXCheckButton(column, _("Smart home key"), prefs, Settings::ID_TOGGLE_SMART_HOME);
  chk->setCheck(prefs->SmartHome, FALSE);

  chk=new FXCheckButton(column, _("Smooth scrolling"), prefs, Settings::ID_TOGGLE_SMOOTH_SCROLL);
  chk->setCheck(prefs->SmoothScroll, FALSE);

  chk=new FXCheckButton(column, _("Allow caret beyond end of line"), prefs, Settings::ID_TOGGLE_CARET_PAST_EOL);
  chk->setCheck(prefs->CaretPastEOL, FALSE);

  spinframe=new FXHorizontalFrame(column);
  spin=new FXSpinner(spinframe, 2, prefs, Settings::ID_SET_CARET_WIDTH, SPIN_OPTS);
  spin->setRange(1,3);
  spin->setValue(prefs->CaretWidth);
  new FXLabel(spinframe, _("Caret width"));


  spinframe=new FXHorizontalFrame(column);
  spin=new FXSpinner(spinframe, 4, prefs, Settings::ID_SET_RIGHT_EDGE, SPIN_OPTS);
  spin->setRange(1,1024);
  spin->setValue(prefs->RightEdgeColumn);
  new FXLabel(spinframe, _("Right Margin Indicator"));

  chk=new FXCheckButton(column,_("Turn line wrapping on by default"),prefs,Settings::ID_TOGGLE_WORD_WRAP);
  chk->setCheck(prefs->WordWrap);

  spinframe=new FXHorizontalFrame(column);
  new FXLabel(spinframe, _("Split views:"));
  list=new FXListBox(spinframe,prefs,Settings::ID_SET_SPLIT_VIEW,LIST_BOX_OPTS);
  list->appendItem(_("cycle"),NULL,NULL);
  list->appendItem(_("below"),NULL,NULL);
  list->appendItem(_("beside"),NULL,NULL);
  list->setNumVisible(list->getNumItems());
  list->setCurrentItem(prefs->SplitView);

  new FXLabel(column, " ");
  new FXButton(column, _(" Editor font... "), NULL, prefs, Settings::ID_CHOOSE_FONT, BUTTON_NORMAL|LAYOUT_CENTER_X);

  column=new FXVerticalFrame(frame,FRAME_SUNKEN|LAYOUT_FILL);
  chk=new FXCheckButton(column, _("Highlight matching braces"), prefs, Settings::ID_TOGGLE_BRACE_MATCH);
  chk->setCheck(prefs->BraceMatch, FALSE);

  chk=new FXCheckButton(column, _("Use tabs for indent"), prefs, Settings::ID_TOGGLE_USE_TABS);

  spinframe=new FXHorizontalFrame(column);
  spin=new FXSpinner(spinframe, 2, prefs, Settings::ID_SET_TAB_WIDTH, SPIN_OPTS);
  spin->setRange(1,16);
  spin->setValue(prefs->TabWidth);
  (new FXLabel(spinframe, _("Tab width")))->setPadRight(16);

  spin=new FXSpinner(spinframe, 2, prefs, Settings::ID_SET_INDENT_WIDTH, SPIN_OPTS);
  spin->setRange(1,16);
  spin->setValue(prefs->IndentWidth);
  new FXLabel(spinframe, _("Indent width"));

  chk->setUserData((void*)spin);
  chk->setCheck(prefs->UseTabs, TRUE);
  
  new FXHorizontalSeparator(column,LAYOUT_SIDE_TOP|LAYOUT_FILL_X|SEPARATOR_GROOVE);
  new FXLabel(column, _("Default search options:"));
  srchopts=new SciSearchOptions(column, prefs,Settings::ID_SET_SEARCH_OPTS);
  srchopts->SetSciFlags(prefs->SearchOptions);
  chk=new FXCheckButton(column, _("Verbose search messages"), prefs, Settings::ID_TOGGLE_SEARCH_VERBOSE);
  chk->setCheck(prefs->SearchVerbose, FALSE);
  chk->setPadLeft(6);
  spinframe=new FXHorizontalFrame(column);
  new FXLabel(spinframe, _("Search wrap:"));
  list=new FXListBox(spinframe,prefs,Settings::ID_SET_SEARCH_WRAP,LIST_BOX_OPTS);
  list->appendItem(_("never"),NULL,NULL);
  list->appendItem(_("always"),NULL,NULL);
  list->appendItem(_("prompt"),NULL,NULL);
  list->setNumVisible(list->getNumItems());
  list->setCurrentItem(prefs->SearchWrap);
  new FXHorizontalSeparator(column,LAYOUT_SIDE_TOP|LAYOUT_FILL_X|SEPARATOR_GROOVE);
  chk=new FXCheckButton(column, _("Open 7-bit ASCII files as UTF-8"), prefs, Settings::ID_TOGGLE_ASCII_DEFAULT);
  chk->setCheck(!prefs->DefaultToAscii, FALSE);
  chk->setPadBottom(4);
  chk=new FXCheckButton(column, _("Treat other files as broken UTF-8"), prefs, Settings::ID_TOGGLE_SBCS_DEFAULT);
  chk->setCheck(!prefs->DefaultToSbcs, FALSE);
  chk->setPadBottom(8);
  spinframe=new FXHorizontalFrame(column);
  new FXLabel(spinframe, _("Default file format:"));
  list=new FXListBox(spinframe,prefs,Settings::ID_SET_FILE_FORMAT,LIST_BOX_OPTS);
  list->appendItem(_("DOS  [CRLF]"),NULL,NULL);
  list->appendItem(_("Mac  [CR]"),NULL,NULL);
  list->appendItem(_("Unix [LF]"),NULL,NULL);
  list->setNumVisible(list->getNumItems());
  list->setCurrentItem(prefs->DefaultFileFormat);

  chk=new FXCheckButton(column, _("Show line endings with \"View Whitespace\""),
                          prefs, Settings::ID_TOGGLE_VIEW_WHITESPACE_EOL);
  chk->setCheck(prefs->WhitespaceShowsEOL, FALSE);
  chk->setPadLeft(6);
}



void PrefsDialog::MakeThemeTab()
{
  new FXTabItem(tabs,_("theme"));
  Theme::MakeThemeGUI(tabs);
}



long PrefsDialog::onTabSwitch(FXObject*o,FXSelector sel,void*p)
{
  whichtab=(FXival)p;
  hint_lab->setText(hint_list[whichtab]);
  return 0;
}



#ifndef WIN32
extern void SetupXAtoms(FXTopWindow*win, const char*class_name);
#else
#define SetupXAtoms(win,class_name)
#endif



void PrefsDialog::create()
{
  FXDialogBox::create();
  SetupXAtoms(this, "prefs");
  show(PLACEMENT_DEFAULT);
  acclist->setWidth((acclist->getParent()->getWidth()/3)*2);
  acclist->setHeaderSize(0,(acclist->getWidth()/2)-8);
  acclist->setHeaderSize(1,(acclist->getWidth()/2)-8);
#ifndef FOX_1_6
  acclist->hide();
  acclist->show();
#endif
}



PrefsDialog::PrefsDialog(FXMainWindow* w, Settings* aprefs):FXDialogBox(w->getApp(), "Preferences")
{
  prefs=aprefs;
  main_win=w;
  maxw=0;
  setX(w->getX()+16);
  setY(w->getY()+24);
  setWidth(620);
  changed_toolbar=ToolbarUnchanged;
  scifont=new FXFont(w->getApp(), prefs->FontName, 10);
  if (!scifont->isFontMono()) {
    delete scifont;
    scifont=new FXFont(w->getApp(), "fixed", 10);
  }
  FXHorizontalFrame* buttons=new FXHorizontalFrame(this,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X);
  new FXButton(buttons,_("  &Close  "), NULL,this,ID_ACCEPT,FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_Y);

  hint_lab=new FXLabel(buttons, hint_list[0], NULL, LAYOUT_FILL_X);

  new FXHorizontalSeparator(this,SEPARATOR_GROOVE|LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X);
  tabs=new FXTabBook(this, this,ID_TAB_SWITCHED,FRAME_NONE|PACK_UNIFORM|LAYOUT_FILL);
  MakeGeneralTab();
  MakeEditorTab();
  MakeSyntaxTab();
  MakeKeybindingsTab();
  MakeToolbarTab();
  MakeThemeTab();
  tabs->setCurrent(whichtab,true);
  tabs->childAtIndex(whichtab*2)->setFocus();
  setIcon(w->getIcon());
}

