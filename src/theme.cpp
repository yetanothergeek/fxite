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
#include "intl.h"

#include "compat.h"

#include "theme.h"


typedef struct _AppColors {
  const FXchar* name;
  FXColor base;
  FXColor border;
  FXColor back;
  FXColor fore;
  FXColor selback;
  FXColor selfore;
  FXColor tipback;
  FXColor tipfore;
  FXColor selmenuback;
  FXColor selmenutext;
} AppColors;


static const AppColors ColorThemes[]={
//|        Name    |        Base         |       Border        |       Back          |      Fore           |      Selback        |      Selfore        |      Tipback        |     Tipfore         |      Menuback       |      Menufore       |
//|----------------+---------------------+---------------------+---------------------+---------------------+---------------------+---------------------+---------------------+---------------------+---------------------+---------------------+
  {"FOX",           FXRGB(0xD4,0xD0,0xC8),FXRGB(0x00,0x00,0x00),FXRGB(0xFF,0xFF,0xFF),FXRGB(0x00,0x00,0x00),FXRGB(0x0A,0x24,0x6A),FXRGB(0xFF,0xFF,0xFF),FXRGB(0xFF,0xFF,0xE1),FXRGB(0x00,0x00,0x00),FXRGB(0x0A,0x24,0x6A),FXRGB(0xFF,0xFF,0xFF)},
  {"Dark Blue",     FXRGB(0x42,0x67,0x94),FXRGB(0x00,0x00,0x00),FXRGB(0x00,0x2A,0x4E),FXRGB(0xFF,0xFF,0xFF),FXRGB(0x5C,0xB3,0xFF),FXRGB(0x00,0x00,0x00),FXRGB(0xFF,0xFF,0xE1),FXRGB(0x00,0x00,0x00),FXRGB(0x5C,0xB3,0xFF),FXRGB(0x00,0x00,0x00)},
  {"Desert FOX",    FXRGB(0xD6,0xCD,0xBB),FXRGB(0x00,0x00,0x00),FXRGB(0xFF,0xFF,0xFF),FXRGB(0x00,0x00,0x00),FXRGB(0x80,0x00,0x00),FXRGB(0xFF,0xFF,0xFF),FXRGB(0xFF,0xFF,0xE1),FXRGB(0x00,0x00,0x00),FXRGB(0x80,0x00,0x00),FXRGB(0xFF,0xFF,0xFF)},
  {"Plastik",       FXRGB(0xEF,0xEF,0xEF),FXRGB(0x00,0x00,0x00),FXRGB(0xFF,0xFF,0xFF),FXRGB(0x00,0x00,0x00),FXRGB(0x67,0x8D,0xB2),FXRGB(0xFF,0xFF,0xFF),FXRGB(0xFF,0xFF,0xE1),FXRGB(0x00,0x00,0x00),FXRGB(0x67,0x8D,0xB2),FXRGB(0xFF,0xFF,0xFF)},
  {"Pumpkin",       FXRGB(0xEE,0xD8,0xAE),FXRGB(0x00,0x00,0x00),FXRGB(0xFF,0xFF,0xFF),FXRGB(0x00,0x00,0x00),FXRGB(0xCD,0x85,0x3F),FXRGB(0xFF,0xFF,0xFF),FXRGB(0xFF,0xFF,0xE1),FXRGB(0x00,0x00,0x00),FXRGB(0xCD,0x85,0x3F),FXRGB(0xFF,0xFF,0xFF)},
  {"Storm",         FXRGB(0xC0,0xC0,0xC0),FXRGB(0x00,0x00,0x00),FXRGB(0xFF,0xFF,0xFF),FXRGB(0x00,0x00,0x00),FXRGB(0x8B,0x00,0x8B),FXRGB(0xFF,0xFF,0xFF),FXRGB(0xFF,0xFF,0xE1),FXRGB(0x00,0x00,0x00),FXRGB(0x8B,0x00,0x8B),FXRGB(0xFF,0xFF,0xFF)},
  {"Sea Sky",       FXRGB(0xA5,0xB2,0xC6),FXRGB(0x00,0x00,0x00),FXRGB(0xFF,0xFF,0xFF),FXRGB(0x00,0x00,0x00),FXRGB(0x31,0x65,0x9C),FXRGB(0xFF,0xFF,0xFF),FXRGB(0xFF,0xFF,0xE1),FXRGB(0x00,0x00,0x00),FXRGB(0x31,0x65,0x9C),FXRGB(0xFF,0xFF,0xFF)},
};

static const FXint numthemes=ARRAYNUMBER(ColorThemes);

static AppColors system_colors={"System"};
static const char*colors_sect="CustomColors";
static AppColors custom_colors={"User Defined"};

static FXString current_theme="System";
static bool use_system_colors=false;
static FXuint what_changed=ThemeUnchanged;

static FXString system_font="Sans,90";
static FXString current_font="Sans,90";

static void RegToApp()
{
  FXApp*a=FXApp::instance();
  current_theme=a->reg().readStringEntry(colors_sect, "CurrentTheme", current_theme.text());
  use_system_colors=a->reg().readBoolEntry(colors_sect, "UseSystemColors",use_system_colors);
  if (use_system_colors) { return; }
  a->setBackColor(a->reg().readColorEntry(colors_sect,"Back",a->getBackColor()));
  a->setBaseColor(a->reg().readColorEntry(colors_sect,"Base",a->getBaseColor()));
  a->setBorderColor(a->reg().readColorEntry(colors_sect,"Border",a->getBorderColor()));
  a->setForeColor(a->reg().readColorEntry(colors_sect,"Fore",a->getForeColor()));
  a->setHiliteColor(a->reg().readColorEntry(colors_sect,"Hilite",a->getHiliteColor()));
  a->setSelMenuBackColor(a->reg().readColorEntry(colors_sect,"SelMenuBack",a->getSelMenuBackColor()));
  a->setSelMenuTextColor(a->reg().readColorEntry(colors_sect,"SelMenuText",a->getSelMenuTextColor()));
  a->setSelbackColor(a->reg().readColorEntry(colors_sect,"Selback",a->getSelbackColor()));
  a->setSelforeColor(a->reg().readColorEntry(colors_sect,"Selfore",a->getSelforeColor()));
  a->setShadowColor(a->reg().readColorEntry(colors_sect,"Shadow",a->getShadowColor()));
  a->setTipbackColor(a->reg().readColorEntry(colors_sect,"Tipback",a->getTipbackColor()));
  a->setTipforeColor(a->reg().readColorEntry(colors_sect,"Tipfore",a->getTipforeColor()));
  current_font=a->reg().readStringEntry(colors_sect,"Font",a->getNormalFont()->getFont().text());
  if (current_font!=system_font) {
    a->getNormalFont()->destroy();
    a->getNormalFont()->setFont(current_font);
    a->getNormalFont()->create();
  }
}



static void AppToReg()
{
  FXApp*a=FXApp::instance();
  a->reg().writeColorEntry(colors_sect,"Back",a->getBackColor());
  a->reg().writeColorEntry(colors_sect,"Base",a->getBaseColor());
  a->reg().writeColorEntry(colors_sect,"Border",a->getBorderColor());
  a->reg().writeColorEntry(colors_sect,"Fore",a->getForeColor());
  a->reg().writeColorEntry(colors_sect,"Hilite",a->getHiliteColor());
  a->reg().writeColorEntry(colors_sect,"SelMenuBack",a->getSelMenuBackColor());
  a->reg().writeColorEntry(colors_sect,"SelMenuText",a->getSelMenuTextColor());
  a->reg().writeColorEntry(colors_sect,"Selback",a->getSelbackColor());
  a->reg().writeColorEntry(colors_sect,"Selfore",a->getSelforeColor());
  a->reg().writeColorEntry(colors_sect,"Shadow",a->getShadowColor());
  a->reg().writeColorEntry(colors_sect,"Tipback",a->getTipbackColor());
  a->reg().writeColorEntry(colors_sect,"Tipfore",a->getTipforeColor());
  a->reg().writeStringEntry(colors_sect,"Font", a->getNormalFont()->getFont().text());
}



static void GetColorsFromApp(AppColors *c)
{
  FXApp*a=FXApp::instance();
  c->base=a->getBaseColor();
  c->border=a->getBorderColor();
  c->back=a->getBackColor();
  c->fore=a->getForeColor();
  c->selback=a->getSelbackColor();
  c->selfore=a->getSelforeColor();
  c->tipfore=a->getTipforeColor();
  c->tipback=a->getTipbackColor();
  c->selmenuback=a->getSelMenuBackColor();
  c->selmenutext=a->getSelMenuTextColor();
}



static void ApplyColorsToApp(const AppColors *c)
{
  FXApp*a=FXApp::instance();
  a->setBaseColor(c->base);
  a->setBorderColor(c->border);
  a->setBackColor(c->back);
  a->setForeColor(c->fore);
  a->setSelbackColor(c->selback);
  a->setSelforeColor(c->selfore);
  a->setTipbackColor(c->tipback);
  a->setTipforeColor(c->tipfore);
  a->setSelMenuBackColor(c->selmenuback);
  a->setSelMenuTextColor(c->selmenutext);
  a->setHiliteColor(makeHiliteColor(c->base));
  a->setShadowColor(makeShadowColor(c->base));
}



#define IfIs(t,dst,src) t*dst=dynamic_cast<t*>(src); if (dst)

#define SetColor(t,f) IfIs(t,a##t,w) { a##t->f(c); return; }

#define BackColor(t) SetColor(t,setBackColor)
static void SetBackColor(FXWindow*w)
{
  FXColor c=w->getApp()->getBackColor();
  BackColor(FXComboBox);
  BackColor(FXListBox);
  BackColor(FXList);
  BackColor(FXTextField);
  c=w->getApp()->getBaseColor();
  BackColor(FXWindow);
}



#define BaseColor(t) SetColor(t,setBaseColor)
static void SetBaseColor(FXWindow*w)
{
  FXColor c=w->getApp()->getBaseColor();
  BaseColor(FXFrame);
  BaseColor(FXMDIChild);
  BaseColor(FXPacker);
  BaseColor(FXPopup);
  BaseColor(FXTable);
  BaseColor(FXToolBarShell);
}


#define TextColor(t) SetColor(t,setTextColor)
static void SetTextColor(FXWindow*w)
{
  FXColor c=w->getApp()->getForeColor();
  TextColor(FXComboBox);
  TextColor(FXFoldingList);
  TextColor(FXGroupBox);
  TextColor(FXHeader);
  TextColor(FXIconList);
  TextColor(FXLabel);
  TextColor(FXList);
  TextColor(FXListBox);
  TextColor(FXMenuCaption);
  TextColor(FXProgressBar);
  TextColor(FXRealSpinner);
  TextColor(FXRuler);
  TextColor(FXSpinner);
  TextColor(FXText);
  TextColor(FXTextField);
  TextColor(FXToolTip);
  TextColor(FXTreeList);
}



#define SelTextColor(t) SetColor(t,setSelTextColor)
static void SetSelTextColor(FXWindow*w)
{
  FXColor c=w->getApp()->getSelforeColor();
  SelTextColor(FXComboBox);
  SelTextColor(FXFoldingList);
  SelTextColor(FXIconList);
  SelTextColor(FXList);
  SelTextColor(FXListBox);
  SelTextColor(FXMenuCaption);
  SelTextColor(FXOption);
  SelTextColor(FXRealSpinner);
  SelTextColor(FXSpinner);
  SelTextColor(FXTable);
  SelTextColor(FXText);
  SelTextColor(FXTextField);
  SelTextColor(FXTreeList);
}


#define SelBackColor(t) SetColor(t,setSelBackColor)
static void SetSelBackColor(FXWindow*w)
{
  FXColor c=w->getApp()->getSelbackColor();
  SelBackColor(FXComboBox);
  SelBackColor(FXFoldingList);
  SelBackColor(FXIconList);
  SelBackColor(FXList);
  SelBackColor(FXListBox);
  SelBackColor(FXMenuCaption);
  SelBackColor(FXOption);
  SelBackColor(FXRealSpinner);
  SelBackColor(FXSpinner);
  SelBackColor(FXTable);
  SelBackColor(FXText);
  SelBackColor(FXTextField);
  SelBackColor(FXTreeList);
}


#define ShadowColor(t) SetColor(t,setShadowColor)
static void SetShadowColor(FXWindow*w)
{
  FXColor c=w->getApp()->getShadowColor();
  ShadowColor(FXDragCorner);
  ShadowColor(FXFrame);
  ShadowColor(FXMDIChild);
  ShadowColor(FXMenuCaption);
  ShadowColor(FXMenuSeparator);
  ShadowColor(FXPacker);
  ShadowColor(FXPopup);
  ShadowColor(FXScrollBar);
  ShadowColor(FXTable);
  ShadowColor(FXToolBarShell);
}



#define HiliteColor(t) SetColor(t,setHiliteColor)
static void SetHiliteColor(FXWindow*w)
{
  FXColor c=w->getApp()->getHiliteColor();
  HiliteColor(FXDragCorner);
  HiliteColor(FXFrame);
  HiliteColor(FXMDIChild);
  HiliteColor(FXMenuCaption);
  HiliteColor(FXMenuSeparator);
  HiliteColor(FXPacker);
  HiliteColor(FXPopup);
  HiliteColor(FXScrollBar);
  HiliteColor(FXTable);
  HiliteColor(FXToolBarShell);
}



#define BorderColor(t) SetColor(t,setBorderColor)
static void SetBorderColor(FXWindow*w)
{
  FXColor c=w->getApp()->getBorderColor();
  BorderColor(FXFrame);
  BorderColor(FXMDIChild);
  BorderColor(FXPacker);
  BorderColor(FXPopup);
  BorderColor(FXScrollBar);
  BorderColor(FXTable);
  BorderColor(FXToolBarShell);
}



#define BoxColor(t) SetColor(t,setBoxColor)
static void SetBoxColor(FXWindow*w)
{
  FXColor c=w->getApp()->getBackColor();
  BoxColor(FXCheckButton);
  BoxColor(FXMenuCheck);
}



#define ArrowColor(t) SetColor(t,setArrowColor)
static void SetArrowColor(FXWindow*w)
{
  FXColor c=w->getApp()->getForeColor();
  ArrowColor(FXArrowButton);
  ArrowColor(FXScrollBar);
}



#define CheckColor(t) SetColor(t,setCheckColor)
static void SetCheckColor(FXWindow*w)
{
  FXColor c=w->getApp()->getForeColor();
  CheckColor(FXCheckButton);
}



#define DiskColor(t) SetColor(t,setDiskColor)
static void SetDiskColor(FXWindow*w)
{
  FXColor c=w->getApp()->getBackColor();
  DiskColor(FXRadioButton);
}



#define CursorColor(t) SetColor(t,setCursorColor)
static void SetCursorColor(FXWindow*w)
{
  FXColor c=w->getApp()->getForeColor();
  CursorColor(FXRealSpinner);
  CursorColor(FXSpinner);
  CursorColor(FXText);
  CursorColor(FXTextField);
}



void Theme::init()
{
  GetColorsFromApp(&system_colors);
  FXApp*a=FXApp::instance();
  a->getNormalFont()->create();
  system_font=a->getNormalFont()->getFont();
  memcpy(&custom_colors.base,&system_colors.base,sizeof(AppColors)-sizeof(char*));
  RegToApp();
}



void Theme::done()
{
  FXApp*a=FXApp::instance();
  a->reg().writeBoolEntry(colors_sect, "UseSystemColors",use_system_colors);
  a->reg().writeStringEntry(colors_sect, "CurrentTheme",current_theme.text());
  if (!use_system_colors) { AppToReg(); }
  ApplyColorsToApp(&system_colors);
}



FXuint Theme::changed()
{
  return what_changed;
}



void Theme::apply(FXWindow*win)
{
  if (win) {
    for (FXWindow*w=win; w; w=w->getNext()) {
      SetBackColor(w);
      SetBaseColor(w);
      SetTextColor(w);
      SetSelBackColor(w);
      SetSelTextColor(w);
      SetHiliteColor(w);
      SetShadowColor(w);
      SetBorderColor(w);
      SetBoxColor(w);
      SetArrowColor(w);
      SetCheckColor(w);
      SetDiskColor(w);
      SetCursorColor(w);
      w->update();
      apply(w->getFirst());
    }
  }
}



class ThemeGUI: public FXHorizontalFrame {
private:
  FXDECLARE(ThemeGUI)
  ThemeGUI() {}
protected:
  FXLabel*list_hdr;
  FXList*list;
  FXMatrix* mtx;
  FXButton*font_btn;
  FXColorWell *cw_base;
  FXColorWell *cw_bord;
  FXColorWell *cw_fore;
  FXColorWell *cw_back;
  FXColorWell *cw_self;
  FXColorWell *cw_selb;
  FXColorWell *cw_menf;
  FXColorWell *cw_menb;
  FXColorWell *cw_tipf;
  FXColorWell *cw_tipb;

  FXTabBook         *demo_tabs;
  FXTabItem         *demo_tab;
  FXVerticalFrame   *demo_frame;
  FXVerticalFrame   *menuframe;
  FXHorizontalFrame *frm_focused;
  FXHorizontalFrame *frm_unfocus;
  FXHorizontalFrame *demo_top_frm;
  FXHorizontalFrame *demo_btm_frm;
  FXGroupBox        *mnu_grp;
  FXGroupBox        *tip_grp;
  FXLabel           *label1;
  FXLabel           *tip_txt;
  FXLabel           *txt_focused;
  FXLabel           *txt_unfocus;
  FXLabel           *menulabels[5];
  FXTextField       *textfield1;
  FXSeparator       *demo_sep;
  FXSeparator       *menu_sep;
  void SetDemoColors();
  void SetFont(const FXString &fontname);
  void FillThemeList();
public:
  ThemeGUI(FXComposite*p);
  ~ThemeGUI();
  long onToggleEnabled(FXObject*o, FXSelector sel, void*p);
  long onSetGUIColor(FXObject*o, FXSelector sel, void*p);
  long onSelectTheme(FXObject*o, FXSelector sel, void*p);
  long onChooseFont(FXObject*o, FXSelector sel, void*p);
  enum {
    ID_TOGGLE_ENABLED=FXTabItem::ID_LAST,
    ID_COLOR_BASE,
    ID_COLOR_BORD,
    ID_COLOR_FORE,
    ID_COLOR_BACK,
    ID_COLOR_SELF,
    ID_COLOR_SELB,
    ID_COLOR_MENF,
    ID_COLOR_MENB,
    ID_COLOR_TIPF,
    ID_COLOR_TIPB,
    ID_SELECT_THEME,
    ID_CHOOSE_FONT,
    ID_LAST
  };
};



FXDEFMAP(ThemeGUI) ThemeTabMap[]={
  FXMAPFUNC(SEL_COMMAND, ThemeGUI::ID_TOGGLE_ENABLED, ThemeGUI::onToggleEnabled),
  FXMAPFUNCS(SEL_CHANGED, ThemeGUI::ID_COLOR_BASE, ThemeGUI::ID_COLOR_TIPB, ThemeGUI::onSetGUIColor),
  FXMAPFUNCS(SEL_COMMAND, ThemeGUI::ID_COLOR_BASE, ThemeGUI::ID_COLOR_TIPB, ThemeGUI::onSetGUIColor),
  FXMAPFUNC(SEL_COMMAND, ThemeGUI::ID_SELECT_THEME, ThemeGUI::onSelectTheme),
  FXMAPFUNC(SEL_COMMAND, ThemeGUI::ID_CHOOSE_FONT, ThemeGUI::onChooseFont),
};


FXIMPLEMENT(ThemeGUI,FXHorizontalFrame,ThemeTabMap,ARRAYNUMBER(ThemeTabMap));



void ThemeGUI::SetDemoColors()
{
  AppColors*ac=&custom_colors;

  FXColor shadow=makeShadowColor(ac->base);
  FXColor hilite=makeHiliteColor(ac->base);

  demo_tabs->setBorderColor(ac->border);
  demo_tabs->setBaseColor(ac->base);
  demo_tabs->setBackColor(ac->base);
  demo_tabs->setShadowColor(shadow);
  demo_tabs->setHiliteColor(hilite);

  demo_tab->setBorderColor(ac->border);
  demo_tab->setBaseColor(ac->base);
  demo_tab->setBackColor(ac->base);
  demo_tab->setTextColor(ac->fore);
  demo_tab->setShadowColor(shadow);
  demo_tab->setHiliteColor(hilite);

  demo_frame->setBorderColor(ac->border);
  demo_frame->setBaseColor(ac->base);
  demo_frame->setBackColor(ac->base);
  demo_frame->setShadowColor(shadow);
  demo_frame->setHiliteColor(hilite);

  demo_top_frm->setBorderColor(ac->border);
  demo_top_frm->setBaseColor(ac->base);
  demo_top_frm->setBackColor(ac->base);
  demo_top_frm->setShadowColor(shadow);
  demo_top_frm->setHiliteColor(hilite);

  demo_btm_frm->setBorderColor(ac->border);
  demo_btm_frm->setBaseColor(ac->base);
  demo_btm_frm->setBackColor(ac->base);
  demo_btm_frm->setShadowColor(shadow);
  demo_btm_frm->setHiliteColor(hilite);

  menuframe->setBorderColor(ac->border);
  menuframe->setBaseColor(ac->base);
  menuframe->setBackColor(ac->base);
  menuframe->setShadowColor(shadow);
  menuframe->setHiliteColor(hilite);

  mnu_grp->setBorderColor(ac->border);
  mnu_grp->setBaseColor(ac->base);
  mnu_grp->setBackColor(ac->base);
  mnu_grp->setShadowColor(shadow);
  mnu_grp->setHiliteColor(hilite);
  mnu_grp->setTextColor(ac->fore);

  tip_grp->setBorderColor(ac->border);
  tip_grp->setBaseColor(ac->base);
  tip_grp->setBackColor(ac->base);
  tip_grp->setShadowColor(shadow);
  tip_grp->setHiliteColor(hilite);
  tip_grp->setTextColor(ac->fore);

  demo_sep->setBorderColor(ac->border);
  demo_sep->setBaseColor(ac->base);
  demo_sep->setBackColor(ac->base);
  demo_sep->setShadowColor(shadow);
  demo_sep->setHiliteColor(hilite);

  menu_sep->setBorderColor(ac->border);
  menu_sep->setBaseColor(ac->base);
  menu_sep->setBackColor(ac->base);
  menu_sep->setShadowColor(shadow);
  menu_sep->setHiliteColor(hilite);

  frm_focused->setBorderColor(ac->border);
  frm_focused->setBaseColor(ac->base);
  frm_focused->setBackColor(ac->base);
  frm_focused->setShadowColor(shadow);
  frm_focused->setHiliteColor(hilite);

  frm_unfocus->setBorderColor(ac->border);
  frm_unfocus->setBaseColor(ac->base);
  frm_unfocus->setBackColor(ac->base);
  frm_unfocus->setShadowColor(shadow);
  frm_unfocus->setHiliteColor(hilite);

  label1->setBorderColor(ac->border);
  label1->setBaseColor(ac->base);
  label1->setBackColor(ac->base);
  label1->setTextColor(ac->fore);
  label1->setShadowColor(shadow);
  label1->setHiliteColor(hilite);

  txt_focused->setBorderColor(ac->border);
  txt_focused->setBaseColor(ac->base);
  txt_focused->setBackColor(ac->selback);
  txt_focused->setTextColor(ac->selfore);
  txt_focused->setShadowColor(shadow);
  txt_focused->setHiliteColor(hilite);

  txt_unfocus->setBorderColor(ac->border);
  txt_unfocus->setBaseColor(ac->base);
  txt_unfocus->setBackColor(ac->base);
  txt_unfocus->setTextColor(ac->fore);
  txt_unfocus->setShadowColor(shadow);
  txt_unfocus->setHiliteColor(hilite);

  for (FXint i=0; i<5; i++) {
    menulabels[i]->setBorderColor(ac->border);
    menulabels[i]->setBaseColor(ac->base);
    menulabels[i]->setBackColor(ac->base);
    menulabels[i]->setTextColor(ac->fore);
    menulabels[i]->setShadowColor(shadow);
    menulabels[i]->setHiliteColor(hilite);
  }

  menulabels[3]->setBorderColor(ac->border);
  menulabels[3]->setBaseColor(ac->selmenuback);
  menulabels[3]->setBackColor(ac->selmenuback);
  menulabels[3]->setTextColor(ac->selmenutext);
  menulabels[3]->setShadowColor(shadow);
  menulabels[3]->setHiliteColor(hilite);

  frm_focused->setBorderColor(ac->border);
  frm_focused->setBaseColor(ac->base);
  frm_focused->setBackColor(ac->back);
  frm_focused->setShadowColor(shadow);
  frm_focused->setHiliteColor(hilite);

  frm_unfocus->setBorderColor(ac->border);
  frm_unfocus->setBaseColor(ac->base);
  frm_unfocus->setBackColor(ac->back);
  frm_unfocus->setShadowColor(shadow);
  frm_unfocus->setHiliteColor(hilite);

  textfield1->setBorderColor(ac->border);
  textfield1->setBackColor(ac->back);
  textfield1->setBaseColor(ac->base);
  textfield1->setTextColor(ac->fore);
  textfield1->setSelTextColor(ac->selfore);
  textfield1->setSelBackColor(ac->selback);
  textfield1->setCursorColor(ac->fore);
  textfield1->setShadowColor(shadow);
  textfield1->setHiliteColor(hilite);

  tip_txt->setBackColor(ac->tipback);
  tip_txt->setTextColor(ac->tipfore);

}



long ThemeGUI::onSelectTheme(FXObject*o, FXSelector sel, void*p)
{
  AppColors*ac=(AppColors*)(list->getItemData((FXint)((FXival)p)));
  cw_base->setRGBA(ac->base,TRUE);
  cw_bord->setRGBA(ac->border,TRUE);
  cw_fore->setRGBA(ac->fore,TRUE);
  cw_back->setRGBA(ac->back,TRUE);
  cw_self->setRGBA(ac->selfore,TRUE);
  cw_selb->setRGBA(ac->selback,TRUE);
  cw_menf->setRGBA(ac->selmenutext,TRUE);
  cw_menb->setRGBA(ac->selmenuback,TRUE);
  cw_tipf->setRGBA(ac->tipfore,TRUE);
  cw_tipb->setRGBA(ac->tipback,TRUE);
  if (&custom_colors!=ac) {
    memcpy(&custom_colors.base,&ac->base,sizeof(AppColors)-sizeof(char*));
  }
  current_theme=ac->name;
  SetDemoColors();
  if (id()) {
    what_changed |= ThemeChangedColors;
  }
  return 1;
}



long ThemeGUI::onSetGUIColor(FXObject*o, FXSelector sel, void*p)
{
  FXColor c=(FXColor)((FXival)p);
  switch (FXSELID(sel)) {
    case ID_COLOR_BASE: {
      custom_colors.base=c;
      break;
    }
    case ID_COLOR_BORD: {
      custom_colors.border=c;
      break;
    }
    case ID_COLOR_FORE: {
      custom_colors.fore=c;
      break;
    }
    case ID_COLOR_BACK: {
      custom_colors.back=c;
      break;
    }
    case ID_COLOR_SELF: {
      custom_colors.selfore=c;
      break;
    }
    case ID_COLOR_SELB: {
      custom_colors.selback=c;
      break;
    }
    case ID_COLOR_MENF: {
      custom_colors.selmenutext=c;
      break;
    }
    case ID_COLOR_MENB: {
      custom_colors.selmenuback=c;
      break;
    }
    case ID_COLOR_TIPF: {
      custom_colors.tipfore=c;
      break;
    }
    case ID_COLOR_TIPB: {
      custom_colors.tipback=c;
      break;
    }
  }
  // Select "User defined" theme if any colors changed
  if ((FXSELTYPE(sel)==SEL_CHANGED) && shown()) {
    what_changed |= ThemeChangedColors;
    if (list->getCurrentItem()!=numthemes+1) {
      list->setCurrentItem(numthemes+1);
      list->selectItem(numthemes+1);
      list->makeItemVisible(numthemes+1);
      current_theme=list->getItem(numthemes+1)->getText();
    }
  }
  SetDemoColors();
  return 1;
}



void ThemeGUI::SetFont(const FXString &fontname)
{
  if (getApp()->getNormalFont()->getFont()!=fontname) {
    what_changed |= ThemeChangedFont;
    getApp()->getNormalFont()->destroy();
    getApp()->getNormalFont()->setFont(fontname);
    getApp()->getNormalFont()->create();
    FXint i=list->getCurrentItem();
    list->destroy();
    FillThemeList();
    list->setCurrentItem(i);
    list->makeItemVisible(i);
    if (id()) { list->create(); }
    recalc();
    update();
    layout();
  }
}



#ifdef FOX_1_6
static void DlgGetFont(const FXFontDialog &dlg, FXString&dst)
{
  FXFontDesc fd;
  dlg.getFontSelection(fd);
  FXFont f(FXApp::instance(),fd);
  f.create();
  dst=f.getFont();
  f.destroy();
}



static void DlgSetFont(FXFontDialog &dlg, const FXString&src)
{
  FXFont f(FXApp::instance(),src);
  f.create();
  FXFontDesc fd;
  f.getFontDesc(fd);
  dlg.setFontSelection(fd);
  f.destroy();
}
#else

# define DlgSetFont(d,f) d.setFont(f)
# define DlgGetFont(d,f) f=d.getFont()

#endif



long ThemeGUI::onChooseFont(FXObject*o, FXSelector sel, void*p)
{
  FXFontDialog dlg(((FXWindow*)o)->getShell(), _("Select Font"), 0);
  DlgSetFont(dlg,current_font);
  if (dlg.execute(PLACEMENT_SCREEN)) {
    DlgGetFont(dlg,current_font);
    SetFont(current_font);
  }
  return 1;
}



long ThemeGUI::onToggleEnabled(FXObject*o, FXSelector sel, void*p)
{
  use_system_colors=(bool)((FXival)p);
  if (use_system_colors) {
    SetFont(system_font);
    for (FXWindow*w=mtx->getFirst(); w; w=w->getNext()) {
      w->disable();
    }
    font_btn->disable();
    list->disable();
    list_hdr->disable();
    list->selectItem(numthemes, true);
    list->makeItemVisible(numthemes);
    onSelectTheme(NULL,0,(void*)(FXival)numthemes);
  } else {
    for (FXWindow*w=mtx->getFirst(); w; w=w->getNext()) {
      w->enable();
    }
    font_btn->enable();
    list->enable();
    list_hdr->enable();
  }
  return 1;
}



void ThemeGUI::FillThemeList()
{
  list->clearItems();
  for (FXint i=0; i<numthemes; i++) {
    list->appendItem(ColorThemes[i].name, NULL, (void*)&ColorThemes[i]);
  }
  list->appendItem(system_colors.name, NULL, (void*)&system_colors);
  list->appendItem(custom_colors.name, NULL, (void*)&custom_colors);
  list->setNumVisible(3);

}

#define CW_STYLE COLORWELL_OPAQUEONLY|COLORWELL_NORMAL|LAYOUT_FIX_WIDTH

ThemeGUI::ThemeGUI(FXComposite*p):FXHorizontalFrame(p,FRAME_RAISED|LAYOUT_FILL) {
  GetColorsFromApp(&custom_colors);
  what_changed=ThemeUnchanged;
  FXVerticalFrame* left_column=new FXVerticalFrame(this,FRAME_SUNKEN|LAYOUT_FILL);
  FXVerticalFrame* right_column=new FXVerticalFrame(this,FRAME_SUNKEN|LAYOUT_FILL,0,0,0,0,8,8,8,8);
  right_column->setBackColor(FXRGB(0xA0,0xC8,0xB0));
  FXCheckButton *who_chk=new FXCheckButton(left_column,_("Let FOX manage these settings"),this,ID_TOGGLE_ENABLED);
  list_hdr=new FXLabel(left_column,_("Preset themes:"),NULL,LAYOUT_CENTER_Y);
  FXHorizontalFrame*list_frm=new FXHorizontalFrame(left_column,FRAME_SUNKEN|LAYOUT_FILL,0,0,0,0,0,0,0,0);
  list=new FXList(list_frm,this,ID_SELECT_THEME,LAYOUT_FILL|LIST_BROWSESELECT);
  FillThemeList();

  mtx=new FXMatrix(left_column,3,LAYOUT_FILL|MATRIX_BY_COLUMNS,0,0,0,0,DEFAULT_SPACING,DEFAULT_SPACING,DEFAULT_SPACING,DEFAULT_SPACING,1,1);

  new FXLabel(mtx,_("Custom Colors:"),NULL,LAYOUT_CENTER_Y);
  new FXLabel(mtx,_("FG"),NULL,LAYOUT_CENTER_X|LAYOUT_CENTER_Y);
  new FXLabel(mtx,_("BG"),NULL,LAYOUT_CENTER_X|LAYOUT_CENTER_Y);

  new FXLabel(mtx,_("Border/Base"),NULL,LAYOUT_CENTER_Y);
  cw_bord=new FXColorWell(mtx,FXRGB(0,0,255),this,ID_COLOR_BORD,CW_STYLE,0,0,32);
  cw_base=new FXColorWell(mtx,FXRGB(0,0,255),this,ID_COLOR_BASE,CW_STYLE,0,0,32);

  new FXLabel(mtx,_("Default"),NULL,LAYOUT_CENTER_Y);
  cw_fore=new FXColorWell(mtx,FXRGB(0,0,255),this,ID_COLOR_FORE,CW_STYLE,0,0,32);
  cw_back=new FXColorWell(mtx,FXRGB(0,0,255),this,ID_COLOR_BACK,CW_STYLE,0,0,32);

  new FXLabel(mtx,_("Selection"),NULL,LAYOUT_CENTER_Y);
  cw_self=new FXColorWell(mtx,FXRGB(0,0,255),this,ID_COLOR_SELF,CW_STYLE,0,0,32);
  cw_selb=new FXColorWell(mtx,FXRGB(0,0,255),this,ID_COLOR_SELB,CW_STYLE,0,0,32);

  new FXLabel(mtx,_("Active Menu"),NULL,LAYOUT_CENTER_Y);
  cw_menf=new FXColorWell(mtx,FXRGB(0,0,255),this,ID_COLOR_MENF,CW_STYLE,0,0,32);
  cw_menb=new FXColorWell(mtx,FXRGB(0,0,255),this,ID_COLOR_MENB,CW_STYLE,0,0,32);

  new FXLabel(mtx,_("Tooltip"),NULL,LAYOUT_CENTER_Y);
  cw_tipf=new FXColorWell(mtx,FXRGB(0,0,255),this,ID_COLOR_TIPF,CW_STYLE,0,0,32);
  cw_tipb=new FXColorWell(mtx,FXRGB(0,0,255),this,ID_COLOR_TIPB,CW_STYLE,0,0,32);

  font_btn=new FXButton(left_column,"  Normal &Font...  ",NULL,this,ID_CHOOSE_FONT);


  demo_tabs=new FXTabBook(right_column,NULL,0,LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0,0,0,0,0);
  demo_tab=new FXTabItem(demo_tabs,_(" Preview "));
  demo_frame=new FXVerticalFrame(demo_tabs,LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_THICK|FRAME_RAISED);

  textfield1=new FXTextField(demo_frame,30,NULL,0,LAYOUT_FILL_X|FRAME_THICK|FRAME_SUNKEN);
  textfield1->setText(_("Select this text, to see selection colors"));

  demo_top_frm=new FXHorizontalFrame(demo_frame,FRAME_NONE|LAYOUT_FILL,0,0,0,0,0,0,0,0);
  frm_focused=new FXHorizontalFrame(demo_top_frm,FRAME_THICK|FRAME_SUNKEN,0,0,0,0,2,2,2,2,0,0);
  txt_focused=new FXLabel(frm_focused,_("Selected Text (focused)"),NULL,LAYOUT_FILL_X,0,0,0,0,1,1,1,1);

  label1=new FXLabel(demo_top_frm,_("  Label with Text"),NULL);

  frm_unfocus=new FXHorizontalFrame(demo_frame,FRAME_THICK|FRAME_SUNKEN,0,0,0,0,2,2,2,2,0,0);
  txt_unfocus=new FXLabel(frm_unfocus,_("Selected Text (no focus)"),NULL,LAYOUT_FILL_X,0,0,0,0,1,1,1,1);

  demo_sep=new FXSeparator(demo_frame,LAYOUT_FILL_X|SEPARATOR_LINE);
  demo_btm_frm=new FXHorizontalFrame(demo_frame,LAYOUT_FILL_X|LAYOUT_FILL_Y);
  mnu_grp=new FXGroupBox(demo_btm_frm,_("Sample Menu"),FRAME_GROOVE|LAYOUT_FILL_Y|LAYOUT_FILL_X);
  menuframe=new FXVerticalFrame(mnu_grp,FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_X|LAYOUT_CENTER_Y,0,0,0,0,0,0,0,0,0,0);
  menulabels[0]=new FXLabel(menuframe,_("&Open"),NULL,LABEL_NORMAL,0,0,0,0,16,4);
  menulabels[1]=new FXLabel(menuframe,_("S&ave"),NULL,LABEL_NORMAL,0,0,0,0,16,4);
  menu_sep=new FXSeparator(menuframe,LAYOUT_FILL_X|SEPARATOR_GROOVE);
  menulabels[2]=new FXLabel(menuframe,_("I&mport"),NULL,LABEL_NORMAL,0,0,0,0,16,4);
  menulabels[3]=new FXLabel(menuframe,_("Active Menu Item  "),NULL,LABEL_NORMAL,0,0,0,0,16,4);
  menulabels[4]=new FXLabel(menuframe,_("&Quit"),NULL,LABEL_NORMAL,0,0,0,0,16,4);

  tip_grp=new FXGroupBox(demo_btm_frm,_("Tooltips"),FRAME_GROOVE|LAYOUT_FILL_Y|LAYOUT_FILL_X);
  tip_txt=new FXLabel(tip_grp,_(" Sample Tooltip "),NULL,FRAME_LINE|LAYOUT_CENTER_X);
  int i=0;
  while (i<=numthemes+1) {
    if (strcmp(list->getItem(i)->getText().text(),current_theme.text())==0) {
      list->selectItem(i,true);
      list->setCurrentItem(i,true);
      list->makeItemVisible(i);
      onSelectTheme(NULL,0,(void*)((FXival)i));
      break;
    }
    i++;
  }
  who_chk->setCheck(use_system_colors,true);
}


ThemeGUI::~ThemeGUI()
{
  ApplyColorsToApp(&custom_colors);
}



FXHorizontalFrame* Theme::MakeThemeGUI(FXComposite*o)
{
  return (FXHorizontalFrame*)(new ThemeGUI(o));
}


