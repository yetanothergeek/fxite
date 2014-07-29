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
#include "intl.h"
#include "theme.h"
#include "cfg_theme.h"


class ThemeControl: public FXVerticalFrame {
  FXDECLARE(ThemeControl)
protected:
  ThemeControl() {}
  ThemeSample*sample;
  AppColors notified_colors;
  FXLabel*list_hdr;
  FXList*list;
  FXMatrix* mtx;
  FXButton*font_btn;
  FXButton*revert_btn;

  FXColorWell *cwBaseColor;
  FXColorWell *cwBorderColor;
  FXColorWell *cwForeColor;
  FXColorWell *cwBackColor;
  FXColorWell *cwSelforeColor;
  FXColorWell *cwSelbackColor;
  FXColorWell *cwSelMenuTextColor;
  FXColorWell *cwSelMenuBackColor;
  FXColorWell *cwTipforeColor;
  FXColorWell *cwTipbackColor;
  void GetColorsFromApp();
  void ApplyColorsToApp(const AppColors *c);
  void SetDemoColors();
  void SetWellColors(AppColors*ac);
  void SetFont(const FXString &fontname);
  FXuint SetAppFont(const FXString &fontname);
  void SetCanRevert();
  void FillThemeList();
  void SelectTheme(FXint idx);
  void SelectThemeByName(const FXString &name);
  FXint notify(FXuint what);
  void CreateControls();
public:
  ThemeControl(FXComposite*o,FXObject*trg, FXSelector sel, FXuint opts);
  void SetSample(ThemeSample*ts);
  long onToggleEnabled(FXObject*o, FXSelector sel, void*p);
  long onSetGUIColor(FXObject*o, FXSelector sel, void*p);
  long onSelectTheme(FXObject*o, FXSelector sel, void*p);
  long onChooseFont(FXObject*o, FXSelector sel, void*p);
  long onRevertTheme(FXObject*o, FXSelector sel, void*p);
  enum {
    ID_TOGGLE_ENABLED=FXHorizontalFrame::ID_LAST,
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
    ID_REVERT,
    ID_LAST
  };
};


FXDEFMAP(ThemeControl) ThemeControlsMap[]={
  FXMAPFUNC(SEL_COMMAND, ThemeControl::ID_TOGGLE_ENABLED, ThemeControl::onToggleEnabled),
  FXMAPFUNCS(SEL_CHANGED, ThemeControl::ID_COLOR_BASE, ThemeControl::ID_COLOR_TIPB, ThemeControl::onSetGUIColor),
  FXMAPFUNCS(SEL_COMMAND, ThemeControl::ID_COLOR_BASE, ThemeControl::ID_COLOR_TIPB, ThemeControl::onSetGUIColor),
  FXMAPFUNC(SEL_COMMAND, ThemeControl::ID_SELECT_THEME, ThemeControl::onSelectTheme),
  FXMAPFUNC(SEL_COMMAND, ThemeControl::ID_CHOOSE_FONT, ThemeControl::onChooseFont),
  FXMAPFUNC(SEL_COMMAND, ThemeControl::ID_REVERT, ThemeControl::onRevertTheme),
};

FXIMPLEMENT(ThemeControl,FXVerticalFrame,ThemeControlsMap,ARRAYNUMBER(ThemeControlsMap));


ThemeControl::ThemeControl(FXComposite*o,FXObject*trg, FXSelector sel, FXuint opts):FXVerticalFrame(o,opts)
{
  target=trg;
  message=sel;
  sample=NULL;
  CreateControls();
}



FXDEFMAP(ThemeSample) ThemeSampleMap[] = {
  FXMAPFUNC(SEL_CHANGED,ThemeSample::ID_CHANGED,ThemeSample::onChanged),
};

FXIMPLEMENT(ThemeSample,FXVerticalFrame,ThemeSampleMap,ARRAYNUMBER(ThemeSampleMap));



class TypicalThemeSample: public ThemeSample {
private:
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
protected:
  virtual void SetColors(AppColors*ac);
public:
  TypicalThemeSample(FXComposite*o);
};



TypicalThemeSample::TypicalThemeSample(FXComposite*o):ThemeSample(o)
{
  setBackColor(FXRGB(0xA0,0xC8,0xB0));
  demo_tabs=new FXTabBook(this,NULL,0,LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0,0,0,0,0);
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
}



#define SetBevelColors(w) \
  w->setShadowColor(shadow); \
  w->setHiliteColor(hilite);

#define SetPlainColors(w) \
  w->setBorderColor(ac->BorderColor); \
  w->setBaseColor(ac->BaseColor); \
  w->setBackColor(ac->BaseColor); \
  SetBevelColors(w);

#define SetPlainColorsAndText(w) \
  SetPlainColors(w); \
  w->setTextColor(ac->ForeColor);


void TypicalThemeSample::SetColors(AppColors*ac)
{
  FXColor shadow=makeShadowColor(ac->BaseColor);
  FXColor hilite=makeHiliteColor(ac->BaseColor);
  SetPlainColors(demo_tabs);
  SetPlainColorsAndText(demo_tab);
  SetPlainColors(demo_frame);
  SetPlainColors(demo_top_frm);
  SetPlainColors(demo_btm_frm);
  SetPlainColors(menuframe);
  SetPlainColorsAndText(mnu_grp);
  SetPlainColorsAndText(tip_grp);
  SetPlainColors(demo_sep);
  SetPlainColors(menu_sep);
  SetPlainColors(frm_focused);
  SetPlainColors(frm_unfocus);
  SetPlainColorsAndText(label1);
  SetPlainColorsAndText(txt_unfocus);

  txt_focused->setBorderColor(ac->BorderColor);
  txt_focused->setBaseColor(ac->BaseColor);
  txt_focused->setBackColor(ac->SelbackColor);
  txt_focused->setTextColor(ac->SelforeColor);
  SetBevelColors(txt_focused);

  for (FXint i=0; i<5; i++) { SetPlainColorsAndText(menulabels[i]); }

  menulabels[3]->setBorderColor(ac->BorderColor);
  menulabels[3]->setBaseColor(ac->SelMenuBackColor);
  menulabels[3]->setBackColor(ac->SelMenuBackColor);
  menulabels[3]->setTextColor(ac->SelMenuTextColor);
  SetBevelColors(menulabels[3]);

  frm_focused->setBorderColor(ac->BorderColor);
  frm_focused->setBaseColor(ac->BaseColor);
  frm_focused->setBackColor(ac->BackColor);
  SetBevelColors(frm_focused);

  frm_unfocus->setBorderColor(ac->BorderColor);
  frm_unfocus->setBaseColor(ac->BaseColor);
  frm_unfocus->setBackColor(ac->BackColor);
  SetBevelColors(frm_unfocus);

  textfield1->setBorderColor(ac->BorderColor);
  textfield1->setBackColor(ac->BackColor);
  textfield1->setBaseColor(ac->BaseColor);
  textfield1->setTextColor(ac->ForeColor);
  textfield1->setSelTextColor(ac->SelforeColor);
  textfield1->setSelBackColor(ac->SelbackColor);
  textfield1->setCursorColor(ac->ForeColor);
  SetBevelColors(textfield1);

  tip_txt->setBackColor(ac->TipbackColor);
  tip_txt->setTextColor(ac->TipforeColor);

}



void ThemeControl::SetCanRevert()
{
  if (Theme::Modified()) { revert_btn->enable(); } else { revert_btn->disable(); }
}



long ThemeControl::onRevertTheme(FXObject*o, FXSelector sel, void*p)
{
  FXuint changed=Theme::Restore();
  SetWellColors(Theme::CurrentColors());
  SetDemoColors();
  SetCanRevert();
  SelectThemeByName("");
  if (changed & Theme::ChangedFont) { SetFont(Theme::GetCurrentFontName()); }
  if (changed & Theme::ChangedColors) { notify(Theme::ChangedColors); }
  return 1;
}



void ThemeControl::SetDemoColors()
{
  if (!sample) { return; }
  sample->handle(this,FXSEL(SEL_CHANGED,ThemeSample::ID_CHANGED),(void*)Theme::CurrentColors());
}



void ThemeControl::SetSample(ThemeSample*ts)
{
  sample=ts;
  SetDemoColors();
}



#define SetColorWell(f) cw##f->setRGBA(ac->f,false);
void ThemeControl::SetWellColors(AppColors*ac)
{
  SetColorWell(BaseColor);
  SetColorWell(BorderColor);
  SetColorWell(ForeColor);
  SetColorWell(BackColor);
  SetColorWell(SelforeColor);
  SetColorWell(SelbackColor);
  SetColorWell(SelMenuTextColor);
  SetColorWell(SelMenuBackColor);
  SetColorWell(TipforeColor);
  SetColorWell(TipbackColor);
}

#define GetColorFromApp(f) current_colors->f=a->get##f();
// Populate current_colors struct with application colors
void ThemeControl::GetColorsFromApp()
{
  FXApp*a=getApp();
  AppColors*current_colors=Theme::CurrentColors();
  GetColorFromApp(BaseColor);
  GetColorFromApp(BorderColor);
  GetColorFromApp(BackColor);
  GetColorFromApp(ForeColor);
  GetColorFromApp(SelbackColor);
  GetColorFromApp(SelforeColor);
  GetColorFromApp(TipforeColor);
  GetColorFromApp(TipbackColor);
  GetColorFromApp(SelMenuBackColor);
  GetColorFromApp(SelMenuTextColor);
}



#define ApplyColorToApp(f) a->set##f(c->f)
// Apply colors from an AppColors struct to the application object
void ThemeControl::ApplyColorsToApp(const AppColors *c)
{
  FXApp*a=getApp();
  ApplyColorToApp(BaseColor);
  ApplyColorToApp(BaseColor);
  ApplyColorToApp(BorderColor);
  ApplyColorToApp(BackColor);
  ApplyColorToApp(ForeColor);
  ApplyColorToApp(SelbackColor);
  ApplyColorToApp(SelforeColor);
  ApplyColorToApp(TipbackColor);
  ApplyColorToApp(TipforeColor);
  ApplyColorToApp(SelMenuBackColor);
  ApplyColorToApp(SelMenuTextColor);
  a->setHiliteColor(makeHiliteColor(c->BaseColor));
  a->setShadowColor(makeShadowColor(c->BaseColor));
}



void ThemeControl::SelectTheme(FXint idx)
{
  AppColors*ac=(AppColors*)(list->getItemData(idx));
  SetWellColors(ac);
  if (Theme::CurrentColors()!=ac) { Theme::CopyColors(Theme::CurrentColors(),ac); }
  SetDemoColors();
  ApplyColorsToApp(ac);
}



FXint ThemeControl::notify(FXuint what)
{
  if (!target) { return 0; }
  if (what==Theme::ChangedColors) {
    if (Theme::SameColors(&notified_colors,Theme::CurrentColors())) { return 0; }
    Theme::CopyColors(&notified_colors,Theme::CurrentColors());
  }
  return target->handle(this,FXSEL(SEL_CHANGED,message),(void*)(FXuval)what);
}



long ThemeControl::onSelectTheme(FXObject*o, FXSelector sel, void*p)
{
  SelectTheme((FXint)((FXival)p));
  notify(Theme::ChangedColors);
  SetCanRevert();
  return 1;
}



long ThemeControl::onSetGUIColor(FXObject*o, FXSelector sel, void*p)
{
  FXColor c=(FXColor)((FXival)p);
  AppColors*ac=Theme::CurrentColors();
  switch (FXSELID(sel)) {
    case ID_COLOR_BASE: { ac->BaseColor=c;         break; }
    case ID_COLOR_BORD: { ac->BorderColor=c;       break; }
    case ID_COLOR_FORE: { ac->ForeColor=c;         break; }
    case ID_COLOR_BACK: { ac->BackColor=c;         break; }
    case ID_COLOR_SELF: { ac->SelforeColor=c;      break; }
    case ID_COLOR_SELB: { ac->SelbackColor=c;      break; }
    case ID_COLOR_MENF: { ac->SelMenuTextColor=c;  break; }
    case ID_COLOR_MENB: { ac->SelMenuBackColor=c;  break; }
    case ID_COLOR_TIPF: { ac->TipforeColor=c;      break; }
    case ID_COLOR_TIPB: { ac->TipbackColor=c;      break; }
  }
  if ((FXSELTYPE(sel)==SEL_CHANGED)) { // Select "User defined" theme if any color changed
    FXint last_item=list->getNumItems()-1;
    if (list->getCurrentItem()!=last_item) {
      list->setCurrentItem(last_item);
      list->selectItem(last_item);
      list->makeItemVisible(last_item);
    }
  }
  ApplyColorsToApp(ac);
  SetCanRevert();
  if (FXSELTYPE(sel)==SEL_COMMAND) { notify(Theme::ChangedColors); }
  SetDemoColors();
  return 1;
}


FXuint ThemeControl::SetAppFont(const FXString &fontname)
{
  FXApp*a=FXApp::instance();
  if (a->getNormalFont()->getFont()!=fontname) {
    a->getNormalFont()->destroy();
    a->getNormalFont()->setFont(fontname);
    a->getNormalFont()->create();
    return Theme::ChangedFont;
  }
  return Theme::Unchanged;
}




void ThemeControl::SetFont(const FXString &fontname)
{
  if (SetAppFont(fontname)==Theme::ChangedFont) {
    FXint i=list->getCurrentItem();
    list->destroy();
    FillThemeList();
    list->setCurrentItem(i);
    list->makeItemVisible(i);
    if (id()) { list->create(); }
    recalc();
    update();
    layout();
    notify(Theme::ChangedFont);
  }
}

#if (FOX_MAJOR<2) && (FOX_MINOR<7) && !defined(FOX_1_6)
# define FOX_1_6
#endif


#ifdef FOX_1_6
static void DlgGetFont(const FXFontDialog &dlg, FXString&dst)
{
  FXFontDesc fd;
  dlg.getFontSelection(fd);
  FXFont f(FXApp::instance(),fd);
  f.create();
  dst=f.getActualName();
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



long ThemeControl::onChooseFont(FXObject*o, FXSelector sel, void*p)
{
  FXFontDialog dlg(((FXWindow*)o)->getShell(), _("Select Font"), 0);
  DlgSetFont(dlg,Theme::GetCurrentFontName());
  if (dlg.execute(PLACEMENT_SCREEN)) {
    FXString current_font;
    DlgGetFont(dlg,current_font);
    Theme::SetCurrentFontName(current_font);
    SetFont(Theme::GetCurrentFontName());
    SetCanRevert();
  }
  return 1;
}


#define SelectInList(n) \
  list->selectItem((n),true); \
  list->setCurrentItem((n),true); \
  list->makeItemVisible((n)); \
  SelectTheme((n));


#define SetEditable(e) \
  for (FXWindow*w=mtx->getFirst(); w; w=w->getNext()) { w->e(); } \
  list->e(); \
  list_hdr->e(); \
  font_btn->e(); \
  revert_btn->e();

long ThemeControl::onToggleEnabled(FXObject*o, FXSelector sel, void*p)
{
  if (p) {
    FXuint changed=Theme::SetUseSystemColors(true);
    if (changed & Theme::ChangedFont) {
      notify(Theme::ChangedFont);
      SetFont(Theme::GetCurrentFontName());
    }
    SelectInList(list->getNumItems()-2);
    if (changed & Theme::ChangedColors) { notify(Theme::ChangedColors); }
    SetEditable(disable);
  } else {
    SetEditable(enable);
    Theme::SetUseSystemColors(false);
  }
  return 1;
}



void ThemeControl::FillThemeList()
{
  list->clearItems();
  for (FXint i=0; i<Theme::NumPresets(); i++) {
    list->appendItem(Theme::PresetThemes()[i].name, NULL, (void*)&Theme::PresetThemes()[i]);
  }
  list->appendItem(Theme::SystemColors()->name, NULL, (void*)Theme::SystemColors());
  list->appendItem(Theme::CurrentColors()->name, NULL, (void*)Theme::CurrentColors());
  list->setNumVisible(3);
}



void ThemeControl::SelectThemeByName(const FXString &name)
{
  for (FXint i=0; i<list->getNumItems(); i++) {
    AppColors*ac=(AppColors*)(list->getItemData(i));
    if (Theme::SameColors(ac,Theme::CurrentColors())) {
      SelectInList(i);
      return;
    }
  }
  SelectInList(list->getNumItems()-1);
}



#define CW_STYLE COLORWELL_OPAQUEONLY|COLORWELL_NORMAL|LAYOUT_FIX_WIDTH

#define MakeColorRow(cpn, cw1, id1, cw2, id2) \
  cw1=new FXColorWell(mtx,FXRGB(0,0,255),this,id1,CW_STYLE,0,0,32); \
  cw2=new FXColorWell(mtx,FXRGB(0,0,255),this,id2,CW_STYLE,0,0,32); \
  new FXLabel(mtx,cpn,NULL,LAYOUT_CENTER_Y); 

void ThemeControl::CreateControls()
{
  GetColorsFromApp();
  Theme::Backup();

  FXCheckButton *who_chk=new FXCheckButton(this,_("Let FOX manage these settings"),this,ID_TOGGLE_ENABLED);
  list_hdr=new FXLabel(this,_("Preset themes:"),NULL,LAYOUT_CENTER_Y);
  FXHorizontalFrame*list_frm=new FXHorizontalFrame(this,FRAME_SUNKEN|LAYOUT_FILL,0,0,0,0,0,0,0,0);
  list=new FXList(list_frm,this,ID_SELECT_THEME,LAYOUT_FILL|LIST_BROWSESELECT);
  FillThemeList();

  mtx=new FXMatrix(this,3,LAYOUT_FILL|MATRIX_BY_COLUMNS,0,0,0,0,DEFAULT_SPACING,DEFAULT_SPACING,DEFAULT_SPACING,DEFAULT_SPACING,1,1);

  new FXLabel(mtx,_("FG"),NULL,LAYOUT_CENTER_X|LAYOUT_CENTER_Y);
  new FXLabel(mtx,_("BG"),NULL,LAYOUT_CENTER_X|LAYOUT_CENTER_Y);
  new FXLabel(mtx,_("Custom Colors:"),NULL,LAYOUT_CENTER_Y);

  MakeColorRow(_("Border/Base"), cwBorderColor, ID_COLOR_BORD, cwBaseColor, ID_COLOR_BASE);
  MakeColorRow(_("Default"), cwForeColor, ID_COLOR_FORE, cwBackColor, ID_COLOR_BACK);
  MakeColorRow(_("Selection"), cwSelforeColor, ID_COLOR_SELF, cwSelbackColor, ID_COLOR_SELB);
  MakeColorRow(_("Active Menu"), cwSelMenuTextColor, ID_COLOR_MENF, cwSelMenuBackColor, ID_COLOR_MENB);
  MakeColorRow(_("Tooltip"), cwTipforeColor, ID_COLOR_TIPF, cwTipbackColor, ID_COLOR_TIPB);

  font_btn=new FXButton(this,"Normal &Font...",NULL,this,ID_CHOOSE_FONT);
  revert_btn=new FXButton(this,"&Revert",NULL,this,ID_REVERT);

  SelectThemeByName("");
  who_chk->setCheck(Theme::GetUseSystemColors(),true);
  SetCanRevert();
  Theme::CopyColors(&notified_colors,Theme::CurrentColors());  
}



FXDEFMAP(ThemeGUI) ThemeTabMap[]={};
FXIMPLEMENT(ThemeGUI,FXHorizontalFrame,ThemeTabMap,ARRAYNUMBER(ThemeTabMap));



ThemeSample* ThemeGUI::MakeSample()
{
  return new TypicalThemeSample(this);
}


ThemeGUI::ThemeGUI(FXComposite*p,FXObject*trg,FXSelector sel):FXHorizontalFrame(p,FRAME_RAISED|LAYOUT_FILL) {
  ThemeControl* control=new ThemeControl(this,trg,sel,FRAME_SUNKEN|LAYOUT_FILL);
  control->SetSample(MakeSample());
}

