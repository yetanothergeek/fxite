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

#include "color_funcs.h"
#include "menuspec.h"
#include "compat.h"
#include "toolbar.h"


#define TBarOpts (FRAME_NONE|LAYOUT_FILL_X|PACK_UNIFORM_HEIGHT|PACK_UNIFORM_WIDTH|MATRIX_BY_COLUMNS)
#define TogBtnOpts  (FRAME_RAISED|FRAME_THICK|JUSTIFY_NORMAL|TOGGLEBUTTON_KEEPSTATE)

#define UsedWidthOf(f) ((f&&f->getLast())?(f->getLast()->getX()+f->getLast()->getWidth()):0)


//  Override (ignore) setFocus method: toolbar buttons are for mice,
//  and should never gain keyboard focus.
class ToolBarBtn: public FXButton {
public:
  virtual void setFocus() { update(); }
  ToolBarBtn(FXComposite*p, const FXString&txt, FXObject*trg, FXSelector sel):
    FXButton(p,txt,NULL,trg,sel) {}
};


class ToolBarTogBtn: public FXToggleButton {
public:
  virtual void setFocus() { update(); }
  ToolBarTogBtn(FXComposite*p, const FXString&txt, FXObject*trg, FXSelector sel):
    FXToggleButton(p,txt,txt,NULL,NULL,trg,sel,TogBtnOpts) {}
};



FXDEFMAP(ToolBarFrame)ToolBarFrameMap[]={
  FXMAPFUNC(SEL_CONFIGURE, 0, ToolBarFrame::onConfigure)
};

FXIMPLEMENT(ToolBarFrame,FXMatrix,ToolBarFrameMap,ARRAYNUMBER(ToolBarFrameMap));


long ToolBarFrame::onConfigure(FXObject*o,FXSelector sel,void*p)
{
  FXMatrix::onConfigure(o,sel,p);
  reconf();
  return 1;
}


// Wrap or unwrap buttons as needed.
void ToolBarFrame::reconf()
{  
  if (numChildren()<1) { return; }
  setNumColumns(numChildren());
  setNumRows(1);
  if (wraptoolbar && (getParent()->getWidth()<getDefaultWidth())) {
    setNumColumns((numChildren()/2)+(numChildren()%2));
    setNumRows(2);
  }
}



void ToolBarFrame::SetTBarBtnColorCB(FXButton*btn, void*user_data)
{
  FXint*i=(FXint*)user_data;
  btn->setBackColor(ColorFuncs::HexToRGB(MenuMgr::TBarColors(*i)));
  btn->setTextColor(ColorFuncs::HexToRGB("#000000"));
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
    switch (tbf->button_size) {
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
}



void ToolBarFrame::SetTBarFont()
{
  FXFont *old_font=toolbar_font;
  toolbar_font=NULL;
  ForEachToolbarButton(SetTBarBtnFontCB,this);
  delete old_font;
  reconf();
  getParent()->layout();
}



ToolBarFrame::~ToolBarFrame()
{
  delete toolbar_font;
}



ToolBarFrame::ToolBarFrame(FXComposite *o, bool hideit):FXMatrix(o,1,TBarOpts,0,0,0,0,0,0,0,0,1,1)
{
  toolbar_font = NULL;
  hidden=hideit;
}



void ToolBarFrame::ForEachToolbarButton(void (*cb)(FXButton*btn, void*user_data), void*user_data) {
  for (FXWindow*btn=getFirst(); btn; btn=btn->getNext()) { cb((FXButton*)btn, user_data); }
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



void ToolBarFrame::create()
{
  FXMatrix::create();
  if (hidden) { hide(); }
}



void ToolBarFrame::show()
{
  FXMatrix::show();
  getParent()->layout();
}



void ToolBarFrame::hide()
{
  FXMatrix::hide();
  getParent()->layout();
}



#define EngageBtn(s,t) ((t*)btn)->setState(s?((t*)btn)->getState()|STATE_ENGAGED:((t*)btn)->getState()&~STATE_ENGAGED)

void ToolBarFrame::CreateButtons(FXMainWindow*tw, FXuchar btn_size, bool wrapit, FXSelector custom_cmd_id, MenuMgr*mmgr)
{
  button_size=btn_size;
  wraptoolbar=wrapit;
  ForEachToolbarButton(ClearTBarBtnDataCB,NULL);
  while (numChildren()) { delete getFirst(); }
  for (FXint i=0; i<TBAR_MAX_BTNS; i++) {
    MenuSpec* spec=mmgr->LookupMenu(mmgr->TBarBtns()[i]);
    if (spec && spec->sel<mmgr->LastID()) {
      FXString txt=spec->btn_txt;
      txt.substitute(' ','\n',true);
      FXLabel*btn;
      if (spec->type=='k') {
        btn=new ToolBarTogBtn(this,txt,tw,spec->sel);
      } else {
        btn=new ToolBarBtn(this,txt,tw,(spec->type=='u')?custom_cmd_id:spec->sel);
      }
      if (spec->ms_mc) {
        if (spec->type!='u') { 
          spec->ms_mc->setUserData((void*)btn);
          if (!spec->ms_mc->isEnabled()) { btn->disable(); }
        }
        btn->setUserData(spec);
        switch(spec->type) {
          case 'k': { EngageBtn(((FXMenuCheck*)(spec->ms_mc))->getCheck(),FXToggleButton); break; }
          case 'r': { EngageBtn(((FXMenuRadio*)(spec->ms_mc))->getCheck(),FXButton); break; }
        }
      }
      FXString tip;
      mmgr->GetTBarBtnTip(spec,tip);
      btn->setTipText(tip);
    } else {
      break;
    }
  }
  SetTBarFont();
  SetToolbarColors();
  layout();
}

