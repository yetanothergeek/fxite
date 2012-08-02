/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2009-2012 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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

#include "lang.h"
#include "menuspec.h"
#include "prefs.h"
#include "appwin.h"
#include "compat.h"
#include "toolbar.h"


#define TBarOpts (FRAME_NONE|LAYOUT_FILL_X|PACK_UNIFORM_HEIGHT)
#define TogBtnOpts  (FRAME_RAISED|FRAME_THICK|JUSTIFY_NORMAL|TOGGLEBUTTON_KEEPSTATE)

#define UsedWidthOf(f) ((f&&f->getLast())?(f->getLast()->getX()+f->getLast()->getWidth()):0)


/* Class that makes tool bar buttons restore document focus after they are clicked */
class ToolBarBtn: public FXButton {
  FXDECLARE(ToolBarBtn)
  ToolBarBtn(){}
public:
  ToolBarBtn(FXComposite* p, const FXString& text, FXObject*tgt, FXSelector sel);
  long onLeftBtnRelease(FXObject*o, FXSelector sel, void*p );
};


/* Class that makes tool bar toggle buttons restore document focus after they are clicked */
class ToolBarTogBtn: public FXToggleButton {
  FXDECLARE(ToolBarTogBtn)
  ToolBarTogBtn(){}
public:
  ToolBarTogBtn(FXComposite* p, const FXString& text, FXObject*tgt, FXSelector sel);
  long onLeftBtnRelease(FXObject*o, FXSelector sel, void*p );
};



FXDEFMAP(ToolBarBtn) ToolBarBtnMap[] = {
  FXMAPFUNC(SEL_LEFTBUTTONRELEASE,0,ToolBarBtn::onLeftBtnRelease),
};

FXIMPLEMENT(ToolBarBtn,FXButton,ToolBarBtnMap,ARRAYNUMBER(ToolBarBtnMap))


ToolBarBtn::ToolBarBtn(FXComposite* p, const FXString& text, FXObject*tgt, FXSelector sel):
    FXButton(p,text,NULL,tgt,sel,FRAME_NONE|JUSTIFY_NORMAL,0,0,24,24,2,2,0,0)
{

}


long ToolBarBtn::onLeftBtnRelease(FXObject*o, FXSelector sel, void*p ) {
  long rv=FXButton::onLeftBtnRelease(o,sel,p);
  getApp()->addChore(TopWindow::instance(),TopWindow::ID_FOCUS_DOC,NULL);
  return rv;
}

FXDEFMAP(ToolBarTogBtn) ToolBarTogBtnMap[] = {
  FXMAPFUNC(SEL_LEFTBUTTONRELEASE,0,ToolBarTogBtn::onLeftBtnRelease),
};

FXIMPLEMENT(ToolBarTogBtn,FXToggleButton,ToolBarTogBtnMap,ARRAYNUMBER(ToolBarTogBtnMap))



ToolBarTogBtn::ToolBarTogBtn(FXComposite* p, const FXString& text, FXObject*tgt, FXSelector sel):
    FXToggleButton(p,text,text,NULL,NULL,tgt,sel,TogBtnOpts,0,0,24,24,2,2,0,0)
{

}



long ToolBarTogBtn::onLeftBtnRelease(FXObject*o, FXSelector sel, void*p )
{
  long rv=FXToggleButton::onLeftBtnRelease(o,sel,p);
  getApp()->addChore(TopWindow::instance(),TopWindow::ID_FOCUS_DOC,NULL);
  return rv;
}



FXDEFMAP(ToolBarFrame)ToolBarFrameMap[]={
  FXMAPFUNC(SEL_CONFIGURE, 0, ToolBarFrame::onConfigure)
};

FXIMPLEMENT(ToolBarFrame,FXVerticalFrame,ToolBarFrameMap,ARRAYNUMBER(ToolBarFrameMap));



long ToolBarFrame::onConfigure(FXObject*o,FXSelector sel,void*p)
{
  FXVerticalFrame::onConfigure(o,sel,p);
  reconf();
  return 1;
}


// Wrap or unwrap buttons as needed.
void ToolBarFrame::reconf()
{
  bool prefwrap=Settings::instance()->WrapToolbar;
  if ((!prefwrap)&&(!wraptoolbar)) { return; }
  FXint kids1=rows[0]->numChildren();
  FXint kids2=rows[1]->numChildren();
  if ((prefwrap)&&(kids1>0)&&(kids2==0)&&(width<UsedWidthOf(rows[0]))) {
    FXWindow *topright=rows[0]->childAtIndex((kids1/2));
    if (topright && (kids1%2==0)) { topright=topright->getPrev(); }
    while ( rows[0]->getLast() && (rows[0]->getLast() != topright) ) {
      rows[0]->getLast()->reparent(rows[1], rows[1]->getFirst());
    }
  } else if ( (kids2>0) && ((!prefwrap)||(width>(UsedWidthOf(rows[0])+UsedWidthOf(rows[1])))) ) {
    while (rows[1]->getFirst()) {
      rows[1]->getFirst()->reparent(rows[0],NULL);
    }
  }
  if (rows[1]->numChildren()) { rows[1]->show(); } else { rows[1]->hide(); }
  wraptoolbar=prefwrap;
  normalize();
}



// Make all buttons the same width
void ToolBarFrame::normalize()
{
  if (rows[0]->numChildren()==0) { return; }
  FXint wdt=0;
  for (FXWindow*row=getFirst(); row; row=row->getNext()) {
    for (FXWindow*btn=row->getFirst(); btn; btn=btn->getNext()) {
      btn->setLayoutHints(btn->getLayoutHints()&~LAYOUT_FIX_WIDTH);
    }
  }
  layout();
  for (FXWindow*row=getFirst(); row; row=row->getNext()) {
    for (FXWindow*btn=row->getFirst(); btn; btn=btn->getNext()) {
      FXint w=btn->getWidth();
      if (w>wdt) { wdt=w; }
    }
  }
  for (FXWindow*row=getFirst(); row; row=row->getNext()) {
    for (FXWindow*btn=row->getFirst(); btn; btn=btn->getNext()) {
      btn->setLayoutHints(btn->getLayoutHints()|LAYOUT_FIX_WIDTH);
      btn->setWidth(wdt);
    }
  }
  layout();
}



void ToolBarFrame::SetTBarBtnColorCB(FXButton*btn, void*user_data)
{
  FXint*i=(FXint*)user_data;
  btn->setBackColor(HexToRGB(MenuMgr::TBarColors(*i)));
  btn->setTextColor(HexToRGB("#000000"));
  btn->setHiliteColor(makeHiliteColor(btn->getBackColor()));
  btn->setShadowColor(makeShadowColor(btn->getBackColor()));
  btn->setBorderColor(btn->getShadowColor());
  (*i)++;
}



void ToolBarFrame::SetToolbarColors()
{
  FXint i=0;
  ForEachToolbarButton(SetTBarBtnColorCB,&i);
}



void ToolBarFrame::SetTBarBtnFontCB(FXButton*btn, void*user_data)
{
  ToolBarFrame*tbf=(ToolBarFrame*)user_data;
  if (!tbf->toolbar_font) {
    FXFontDesc dsc;
    GetFontDescription(dsc, tbf->getApp()->getNormalFont());
    FXfloat scale=1.0;
    switch (Settings::instance()->ToolbarButtonSize) {
      case 0:{ scale=0.75; break; }
      case 1:{ scale=0.90; break; }
      case 2:{ scale=1.00; break; }
    }
    dsc.size=FXushort(((FXfloat)dsc.size)*scale);
    tbf->toolbar_font=new FXFont(tbf->getApp(),dsc);
  }
  btn->setFont(tbf->toolbar_font);
  btn->destroy();
  if (tbf->getShell()->shown()) {
    btn->create();
  }
  if (btn->getNext()==NULL) {
    tbf->normalize();
    tbf->reconf();
  }
}



void ToolBarFrame::SetTBarFont()
{
  delete toolbar_font;
  toolbar_font=NULL;
  ForEachToolbarButton(SetTBarBtnFontCB,this);
  getParent()->layout();
}



ToolBarFrame::~ToolBarFrame()
{
  delete toolbar_font;
}



ToolBarFrame::ToolBarFrame(FXComposite *o):FXVerticalFrame(o,TBarOpts,0,0,0,0,0,0,0,0,1,1)
{
  toolbar_font = NULL;
  wraptoolbar=Settings::instance()->WrapToolbar;
  rows[0]=new FXHorizontalFrame(this,TBarOpts,0,0,0,0,0,0,0,0,1,1);
  rows[1]=new FXHorizontalFrame(this,TBarOpts,0,0,0,0,0,0,0,0,1,1);
  rows[1]->hide();
}


void ToolBarFrame::ForEachToolbarButton(void (*cb)(FXButton*btn, void*user_data), void*user_data) {
  for (FXWindow*hfrm=getFirst(); hfrm; hfrm=hfrm->getNext()) {
    for (FXWindow*btn=hfrm->getFirst(); btn; btn=btn->getNext()) {
      cb((FXButton*)btn, user_data);
    }
  }
}



void ToolBarFrame::ClearTBarBtnDataCB(FXButton*btn, void*user_data)
{
  MenuSpec*spec=(MenuSpec*)btn->getUserData();
  if (spec&&spec->ms_mc&&(spec->type!='u')) { spec->ms_mc->setUserData(NULL); }
}




void ToolBarFrame::NullifyButtonDataCB(FXButton*btn, void*user_data)
{
  if (btn->getUserData()==user_data) { btn->setUserData(NULL); }
}



void ToolBarFrame::NullifyButtonData(void*user_data)
{
  ForEachToolbarButton(NullifyButtonDataCB,user_data);
}



void ToolBarFrame::EnableFilterBtnCB(FXButton*btn, void*user_data)
{
  MenuSpec*spec=(MenuSpec*)btn->getUserData();
  if (spec && (spec->type=='u') && spec->ms_mc && (spec->ms_mc->getSelector()==TopWindow::ID_USER_FILTER)) {
    if ((bool)user_data) { btn->enable(); } else { btn->disable(); }
  }
}



void ToolBarFrame::EnableFilterBtn(bool enabled)
{
  ForEachToolbarButton(EnableFilterBtnCB,(void*)enabled);
}



void ToolBarFrame::create()
{
  FXVerticalFrame::create();
  normalize();
  if (!Settings::instance()->ShowToolbar) { hide(); }
}



void ToolBarFrame::show()
{
  FXVerticalFrame::show();
  getParent()->layout();
}



void ToolBarFrame::hide()
{
  FXVerticalFrame::hide();
  getParent()->layout();
}



#define EngageBtn(s,t) ((t*)btn)->setState(s?((t*)btn)->getState()|STATE_ENGAGED:((t*)btn)->getState()&~STATE_ENGAGED)

void ToolBarFrame::CreateButtons(TopWindow*tw)
{
  ForEachToolbarButton(ClearTBarBtnDataCB,NULL);
  for (FXWindow*w=getFirst(); w; w=w->getNext()) {
    while (w->numChildren()) { delete w->getFirst(); }
  }
  for (FXint i=0; i<TBAR_MAX_BTNS; i++) {
    MenuSpec* spec=MenuMgr::LookupMenu(MenuMgr::TBarBtns()[i]);
    if (spec && spec->sel<TopWindow::ID_LAST) {
      FXString txt=spec->btn_txt;
      txt.substitute(' ','\n',true);
      FXLabel*btn;
      if ((spec->type=='k')||(spec->sel==TopWindow::ID_MACRO_RECORD)) {
        btn=new ToolBarTogBtn((FXComposite*)(getFirst()),txt,tw,spec->sel);
      } else {
        btn=new ToolBarBtn((FXComposite*)(getFirst()),txt,tw,(spec->type=='u')?TopWindow::ID_TBAR_CUSTOM_CMD:spec->sel);
        switch (spec->sel) { // These menus might be in a disabled state...
          case TopWindow::ID_MACRO_PLAYBACK:
          case TopWindow::ID_MACRO_TRANSLATE:
          case TopWindow::ID_FILTER_SEL:
          case TopWindow::ID_FIND_TAG:
          case TopWindow::ID_SHOW_CALLTIP:
          case TopWindow::ID_FMT_DOS:
          case TopWindow::ID_FMT_MAC:
          case TopWindow::ID_FMT_UNIX: {
            if (!spec->ms_mc->isEnabled()) { btn->disable(); }
            break;
          }
        }
      }
      if (spec->ms_mc) {
        if (spec->type!='u') { spec->ms_mc->setUserData((void*)btn); }
        btn->setUserData(spec);
        switch(spec->type) {
          case 'k': { EngageBtn(((FXMenuCheck*)(spec->ms_mc))->getCheck(),FXToggleButton); break; }
          case 'r': { EngageBtn(((FXMenuRadio*)(spec->ms_mc))->getCheck(),FXButton); break; }
        }
      }
      FXString tip;
      MenuMgr::GetTBarBtnTip(spec,tip);
      btn->setTipText(tip);
    } else {
      break;
    }
  }
  SetTBarFont();
  SetToolbarColors();
  layout();
}

