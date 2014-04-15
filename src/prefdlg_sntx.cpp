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
#include <Scintilla.h>
#include <FXScintilla.h>
#include <ILexer.h>
#include <LexerModule.h>
#include <Catalogue.h>

#include "intl.h"
#include "appwin_pub.h"
#include "compat.h"
#include "histbox.h"
#include "lang.h"
#include "prefs.h"
#include "shady_tabs.h"

#include "prefdlg_sntx.h"

#define LIST_BOX_OPTS  ( FRAME_SUNKEN | FRAME_THICK | LISTBOX_NORMAL )
#define SPIN_OPTS      ( SPIN_NORMAL | FRAME_SUNKEN | FRAME_THICK )
#define PACK_UNIFORM   ( PACK_UNIFORM_WIDTH | PACK_UNIFORM_HEIGHT )

#define SetPad(padwin, padsize) \
(padwin)->setPadLeft(padsize); \
(padwin)->setPadTop(padsize); \
(padwin)->setPadRight(padsize); \
(padwin)->setPadBottom(padsize);


#define COLORWELL_OPTS ( COLORWELL_OPAQUEONLY|LAYOUT_FIX_WIDTH|FRAME_SUNKEN|FRAME_THICK|LAYOUT_CENTER_Y )
#define STYLE_BTN_OPTS ( TOGGLEBUTTON_NORMAL | TOGGLEBUTTON_KEEPSTATE | LAYOUT_FILL_Y )


static int whichlang=0;


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



class StyleEdit: public FXObject {
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
  FXFontDesc fontdesc;
  FXFont* labelfont;
  void conceal(FXFrame*w);
public:
  long onStyleBtn(FXObject*o,FXSelector sel,void*p);
  long onColorBtn(FXObject*o,FXSelector sel,void*p);
  StyleEdit(FXComposite *p, StyleDef*def, FXFont*scifont, bool bgonly=false);
  ~StyleEdit(){ delete labelfont; }
  void create();
  enum {
    ID_STYLE_BTN=FXHorizontalFrame::ID_LAST,
    ID_COLOR_BTN,
    ID_LAST
  };
};



class StylePan: public FXMatrix {
private:
  FXFont*bold_btn_font;
  FXFont*italic_btn_font;
  friend class StyleEdit;
public:
  ~StylePan();
  StylePan(FXComposite*o);
};



FXDEFMAP(StyleEdit) StyleEditMap[]={
  FXMAPFUNC(SEL_COMMAND,StyleEdit::ID_STYLE_BTN,StyleEdit::onStyleBtn),
  FXMAPFUNC(SEL_COMMAND,StyleEdit::ID_COLOR_BTN,StyleEdit::onColorBtn)
};

FXIMPLEMENT(StyleEdit,FXObject,StyleEditMap,ARRAYNUMBER(StyleEditMap));



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
  FXFont*tmpfont=new FXFont(caption->getApp(), fontdesc);
  tmpfont->create();
  caption->setFont(tmpfont);
  delete labelfont;
  labelfont=tmpfont;
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
    RgbToHex(rgb,styledef->bg);
    if (!fg_btn->isEnabled()) {
      caption->setTextColor(rgb>FXRGB(0x80,0x80,0x80)?FXRGB(0,0,0):FXRGB(0xFF,0xFF,0xFF));
    }
  }
  caption->update();
  return 1;
}



void StyleEdit::conceal(FXFrame*w) {
  FXColor c=w->getShell()->getBackColor();
  w->disable();
  w->setShadowColor(c);
  w->setHiliteColor(c);
  w->setBaseColor(c);
  w->setBorderColor(c);
}



void StyleEdit::create()
{
  caption->create();
  bold_btn->getParent()->create();
  bold_btn->create();
  italic_btn->create();
  fg_btn->getParent()->create();
  fg_btn->create();
  bg_btn->create();
}



StyleEdit::StyleEdit(FXComposite *p, StyleDef*sd, FXFont*scifont, bool bgonly)
{
  Settings*prefs=Settings::instance();
  styledef=sd;
  FXColor bg=HexToRGB(sd->bg[0]?sd->bg:prefs->globalStyle()->bg);
  FXColor fg=HexToRGB(sd->fg[0]?sd->fg:prefs->globalStyle()->fg);
  FXHorizontalFrame*stylebtns=new FXHorizontalFrame(p,LAYOUT_FILL_X|PACK_UNIFORM_WIDTH,0,0,0,0,0,0,0,0,0,0);
  stylebtns->setBackColor(bg);
  stylebtns->setUserData((void*)this);

  caption=new FXLabel(p,sd->key,NULL,LABEL_NORMAL|LAYOUT_FILL|LAYOUT_FILL_COLUMN);
  caption->setBackColor(bg);
  caption->setTextColor(fg);
  caption->setPadLeft(4);
  caption->setPadRight(4);

  FXHorizontalFrame*colorbtns=new FXHorizontalFrame(p,LAYOUT_FILL_X|LAYOUT_CENTER_Y,0,0,0,0,0,0,0,0,0,0);
  fg_btn=new MyColorWell(colorbtns, fg, this, ID_COLOR_BTN,COLORWELL_OPTS,0,0,32,32,0,0,0,0);
  bg_btn=new MyColorWell(colorbtns, bg, this, ID_COLOR_BTN,COLORWELL_OPTS,0,0,32,32,0,0,0,0);


  italic_btn=new FXToggleButton(stylebtns, "I", "I", NULL, NULL, this, ID_STYLE_BTN, STYLE_BTN_OPTS);
  italic_btn->setFont((dynamic_cast<StylePan*>(p))->italic_btn_font);
  italic_btn->setState(sd->style&Italic?STATE_DOWN:STATE_UP);

  bold_btn = new FXToggleButton(stylebtns, "B", "B", NULL, NULL, this, ID_STYLE_BTN, STYLE_BTN_OPTS);
  bold_btn->setFont((dynamic_cast<StylePan*>(p))->bold_btn_font);
  bold_btn->setState(sd->style&Bold?STATE_DOWN:STATE_UP);

  caption->setJustify(JUSTIFY_LEFT|JUSTIFY_CENTER_Y);
  GetFontDescription(fontdesc,scifont);
  fontdesc.weight=sd->style&Bold?FXFont::Bold:FXFont::Normal;
  fontdesc.slant=sd->style&Italic?FXFont::Italic:FXFont::Straight;
  labelfont=new FXFont(p->getApp(), fontdesc);
  caption->setFont(labelfont);

  if (bgonly) {
    caption->setTextColor(bg>FXRGB(0x80,0x80,0x80)?FXRGB(0,0,0):FXRGB(0xFF,0xFF,0xFF));
    fg_btn->setRGBA(fg_btn->getShell()->getBackColor());
    conceal(fg_btn);
    fg_btn->setWidth(1);
    conceal(italic_btn);
    conceal(bold_btn);
  }
}



StylePan::~StylePan() {
  for (FXWindow*w=getFirst(); w; w=w->getNext()) {
    if (colOfChild(w)==0) { delete dynamic_cast<StyleEdit*>((FXObject*)(w->getUserData())); }
  }
  delete bold_btn_font;
  delete italic_btn_font;
}



StylePan::StylePan(FXComposite*o):FXMatrix(o,3,MATRIX_BY_COLUMNS|LAYOUT_FILL,0,0,0,0,0,0,0,0,0,0) {
  FXFontDesc fd;
  GetFontDescription(fd,getApp()->getNormalFont());
  strncpy(fd.face,"serif",sizeof(fd.face)-1);
  fd.weight=FXFont::ExtraBold;
  bold_btn_font  = new FXFont(getApp(),fd);
  fd.weight=FXFont::Light;
  fd.slant=FXFont::Italic;
  italic_btn_font= new FXFont(getApp(),fd);
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



FXDEFMAP(LangGUI) LangGUIMap[]={
  FXMAPFUNC(SEL_COMMAND,LangGUI::ID_LANG_SWITCH,LangGUI::onLangSwitch),
  FXMAPFUNC(SEL_COMMAND,LangGUI::ID_KWORD_EDIT,LangGUI::onKeywordEdit),
  FXMAPFUNC(SEL_COMMAND,LangGUI::ID_TABOPTS_SWITCH,LangGUI::onTabOptsSwitch),
  FXMAPFUNCS(SEL_COMMAND,LangGUI::ID_EDIT_FILETYPES,LangGUI::ID_EDIT_SHABANGS,LangGUI::onSyntaxFiletypeEdit),
};

FXIMPLEMENT(LangGUI,FXVerticalFrame,LangGUIMap,ARRAYNUMBER(LangGUIMap))



long LangGUI::onLangSwitch(FXObject*o,FXSelector sel,void*p)
{
  FXival index=(FXival)p;
  if (index==-1) { index=langlist->getCurrentItem(); }
  whichlang=index;
  StyleDef*sd0,*sdN;
  wordlist->clearItems();
  LangStyle* ls=(LangStyle*)langlist->getItemData(index);
  syntabs->setCurrent(0);
  if (ls) {
    opts_tab->enable();
    wildcardbtn->enable();
    wildcardbtn->setUserData(ls);
    shabangbtn->enable();
    shabangbtn->setUserData(ls);
    kwordsgrp->enable();
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
      kwordsgrp->enable();
      wordlist->enable();
      wordbtn->enable();
      wordlist->setNumVisible(wordlist->getNumItems());
    } else {
      kwordsgrp->disable();
      wordlist->disable();
      wordbtn->disable();
    }
    sd0=ls->styles;
  } else {
    opts_tab->disable();
    kwordsgrp->disable();
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
  if (scifont) { delete scifont; }
  scifont=new FXFont(langlist->getApp(), prefs->FontName, prefs->FontSize/10);
  FXWindow* w=style_pan->getParent();
  delete style_pan;
  style_pan=new StylePan((FXComposite*)w);
  if (sel) { style_pan->create(); }
  StyleEdit*se;
  for (sdN=sd0; sdN->key; sdN++) {
    se=new StyleEdit(style_pan,sdN,scifont);
    if (sel) { se->create(); }
  }
  if (sd0==prefs->globalStyle()) {

    se=new StyleEdit(style_pan,prefs->selectionStyle(),scifont,true);
    if (sel) { se->create(); }

    se=new StyleEdit(style_pan,prefs->caretStyle(),scifont,true);
    if (sel) { se->create(); }

    se=new StyleEdit(style_pan,prefs->caretlineStyle(),scifont,true);
    if (sel) { se->create(); }

    se=new StyleEdit(style_pan,prefs->rightmarginStyle(),scifont,true);
    if (sel) { se->create(); }

    se=new StyleEdit(style_pan,prefs->whitespaceStyle(),scifont,true);
    if (sel) { se->create(); }

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



long LangGUI::onKeywordEdit(FXObject*o,FXSelector sel,void*p)
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
  sc->sendMessage(SCI_STYLESETFONT, STYLE_DEFAULT, reinterpret_cast<long>(scifont->getName().text()));
  sc->sendMessage(SCI_STYLESETSIZE, STYLE_DEFAULT, 10);
  sc->sendMessage(SCI_SETWRAPMODE,SC_WRAP_WORD,0);
  sc->sendMessage(SCI_SETMARGINWIDTHN,1,0);
  sc->sendMessage(SCI_SETMARGINLEFT,0,4);
  sc->sendMessage(SCI_SETMARGINRIGHT,0,4);
  sc->sendMessage(SCI_APPENDTEXT,strlen(words),reinterpret_cast<long>(words));
  FXHorizontalFrame *btns=new FXHorizontalFrame(vframe, PACK_UNIFORM|LAYOUT_FILL_X|LAYOUT_SIDE_BOTTOM);
  new FXButton(btns,_(" &OK "), NULL, &kwdlg, FXDialogBox::ID_ACCEPT);
  new FXButton(btns,_(" &Cancel "), NULL, &kwdlg, FXDialogBox::ID_CANCEL);
  kwdlg.changeFocus(sc);
  if (kwdlg.execute(PLACEMENT_SCREEN)) {
    FXString newwords;
    newwords.length(sc->sendMessage(SCI_GETLENGTH,0,0)+1);
    sc->sendMessage(SCI_GETTEXT,newwords.length(), reinterpret_cast<long>(newwords.text()));
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
    TopWinPub::SaveClipboard();
  }
  return 1;
}



long LangGUI::onTabOptsSwitch(FXObject*o,FXSelector sel,void*p)
{
  LangStyle*ls=(LangStyle*)langlist->getItemData(langlist->getCurrentItem());
  ls->tabs=(TabPolicy)tabopts->getCurrentItem();
  return 1;
}



long LangGUI::onSyntaxFiletypeEdit(FXObject*o,FXSelector sel,void*p)
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



void LangGUI::MakeStyleTab()
{
  FXVerticalFrame *style_gui;
  FXHorizontalFrame *frame;
  new FXTabItem(syntabs,_(" Styles   "));
  style_gui=new FXVerticalFrame(syntabs,FRAME_RAISED|LAYOUT_FILL,0,0,0,0,0,0,0,0,0,0);
  style_hdr=new FXHorizontalFrame(style_gui,LAYOUT_FILL_X|FRAME_GROOVE,0,0,0,0,0,0,0,0,0,0);
  new FXLabel(style_hdr, _("Font"),NULL,FRAME_SUNKEN);
  new FXLabel(style_hdr, _("Style Name"), NULL, LAYOUT_FILL_X|FRAME_SUNKEN);
  new FXLabel(style_hdr, "  FG  /  BG  ",NULL,FRAME_SUNKEN);
  scroll=new FXScrollWindow(style_gui,LAYOUT_FILL|HSCROLLING_OFF|VSCROLLING_ON|VSCROLLER_ALWAYS);
  style_pan=new StylePan(scroll);
  frame=new FXHorizontalFrame(style_gui,FRAME_RAISED|LAYOUT_FILL_X,0,0,0,0,0,0,0,0,0,0);
  SetPad(frame,4);
  frame=new FXHorizontalFrame(frame,FRAME_NONE|LAYOUT_CENTER_X,0,0,0,0,0,0,0,0,0,0);
  for (FXint i=0; rainbow[i][0]; i++) {
    new FXColorWell(frame,
    HexToRGB(rainbow[i]),NULL,0,FRAME_NONE|COLORWELL_OPAQUEONLY|COLORWELL_SOURCEONLY|COLORWELL_NORMAL,0,0,0,0,0,0,0,0);
  }
}



void LangGUI::MakeOptsTab()
{
  FXHorizontalFrame *hframe;
  FXGroupBox*grp;
  FXVerticalFrame *opts_gui;
  opts_tab=new FXTabItem(syntabs,_(" Options  "));
  opts_gui=new FXVerticalFrame(syntabs,FRAME_RAISED|LAYOUT_FILL|PACK_UNIFORM);

  hframe=new FXHorizontalFrame(opts_gui,FRAME_NONE);
  grp=new FXGroupBox(hframe,_("Recognition"),FRAME_GROOVE);
  hframe=new FXHorizontalFrame(grp,FRAME_NONE|PACK_UNIFORM);
  hframe->setHSpacing(8);
  wildcardbtn=new FXButton(hframe,_("File types..."),NULL,this,ID_EDIT_FILETYPES,BUTTON_NORMAL|LAYOUT_FILL_X);
  shabangbtn=new FXButton(hframe,_("#! programs..."),NULL,this,ID_EDIT_SHABANGS,BUTTON_NORMAL|LAYOUT_FILL_X);

  hframe=new FXHorizontalFrame(opts_gui,FRAME_NONE);
  kwordsgrp=new FXGroupBox(hframe,_("Keyword lists"),FRAME_GROOVE);
  hframe=new FXHorizontalFrame(kwordsgrp,FRAME_NONE);

  wordlist=new FXListBox(hframe,NULL,0,LIST_BOX_OPTS|LAYOUT_FILL_X );
  wordbtn=new FXButton(hframe, "...", NULL, this, ID_KWORD_EDIT);

  grp=new FXGroupBox(opts_gui,_("Tab settings"),FRAME_GROOVE);
  hframe=new FXHorizontalFrame(grp,FRAME_NONE);
  taboptlab=new FXLabel(hframe,_("Use tabs:"));
  tabopts=new FXListBox(hframe,this,ID_TABOPTS_SWITCH,LIST_BOX_OPTS|LAYOUT_FILL_X);
  tabopts->appendItem(_("use default setting"));
  tabopts->appendItem(_("always use tabs"));
  tabopts->appendItem(_("never use tabs"));
  tabopts->appendItem(_("detect from content"));
  tabopts->setNumVisible(4);

  hframe=new FXHorizontalFrame(grp,FRAME_NONE);
  tabwidthspin=new FXSpinner(hframe, 3, prefs, Settings::ID_SET_TAB_WIDTH_FOR_LANG,SPIN_OPTS);
  tabwidthspin->setRange(0,16);
  tabwidthspin->setValue(0);
  tabwidthlab=new FXLabel(hframe,"Tab width (zero to use default)");
}



LangGUI::~LangGUI()
{
  delete scifont;
}




LangGUI::LangGUI(FXComposite*o,Settings*aprefs,FXObject*trg, FXSelector sel):FXVerticalFrame(o,FRAME_RAISED|FRAME_THICK|LAYOUT_FILL)
{
  FXHorizontalFrame *frame;
  prefs=aprefs;
  scifont=NULL;
  frame=new FXHorizontalFrame(this,FRAME_GROOVE|LAYOUT_FILL_X);
  new FXLabel(frame,_("Language:"));
  langlist=new FXListBox(frame,this,ID_LANG_SWITCH,LIST_BOX_OPTS);
  langlist->appendItem(_("global"),NULL,NULL);
  LangStyle*ls;
  for (ls=languages; ls->name; ls++) { langlist->appendItem(ls->name,NULL,ls); }
  langlist->setNumVisible(langlist->getNumItems()>12?12:langlist->getNumItems());
  langlist->setCurrentItem(whichlang);

  syntabs=new ShadyTabs(this,trg,sel,PACK_UNIFORM_WIDTH|LAYOUT_FILL);
  SetPad(syntabs,0);
  MakeStyleTab();
  MakeOptsTab();
  onLangSwitch(langlist, 0, (void*)(FXival)whichlang);
}

