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
#include "shady_tabs.h"

FXDEFMAP(ShadyTabs) ShadyTabsMap[]={};

FXIMPLEMENT(ShadyTabs,FXTabBook,ShadyTabsMap,ARRAYNUMBER(ShadyTabsMap));


ShadyTabs::ShadyTabs( FXComposite* p, FXObject* tgt, FXSelector sel ,FXuint opts, FXint x, FXint y,
                      FXint w, FXint h,FXint pl, FXint pr,FXint pt,FXint pb
                    ):FXTabBook(p,tgt,sel,opts,x,y,w,h,pl,pr,pt,pb)
{

}

#define brightness(c) ( \
  ( (299*FXREDVAL(c)) + (587*FXGREENVAL(c)) + (114*FXBLUEVAL(c)) ) / 1000 )



static FXColor shade(FXColor c, bool brighten, bool istext, FXColor other)
{
  FXfloat h=0, s=0, v=0; // H is 0..360; S is 0..1; V is 0..255
  FXfloat r=FXREDVAL(c), g=FXGREENVAL(c), b=FXBLUEVAL(c);
  FXfloat d=.07;
  fxrgb_to_hsv(h,s,v,r,g,b);
  if (brighten) {
    if (istext) {
      if ( brighten && (brightness(c)>brightness(other))) { d=.10; }
    } else {
      if (v>240) {
        if (s>0.50) { s=0.50; }
      } else {
        if (v<84) { v=84; }
        if (v<160) { d=.10; }
      }
    }
  } else if (istext && (v>240)) { v=240; }
  v=brighten?v*(1.0+d):v*(1.0-d);
  if (v>255) { v=255; } else if (v<0) { v=0; }
  fxhsv_to_rgb(r,g,b,h,s,v);
  return FXRGB(r,g,b);
}



void ShadyTabs::setTabColor(FXTabItem*t, bool active)
{
  t->setBackColor(shade(getApp()->getBaseColor(),active,false,getApp()->getForeColor()));
  t->setTextColor(shade(getApp()->getForeColor(),active,true, getApp()->getBaseColor()));
  if (active) {
    if (brightness(t->getTextColor())>brightness(t->getBackColor())) {
      t->setHiliteColor(makeHiliteColor(getApp()->getHiliteColor()));
    } else {
      t->setHiliteColor(getShadowColor());
    }
  } else {
    t->setHiliteColor(getApp()->getHiliteColor());
  }
  return;
}



void ShadyTabs::UpdateTabs()
{
  bool istab;
  FXWindow*w;
  for (w=getFirst(),istab=true; w; w=w->getNext(),istab=!istab) {
    if (istab) {
      setTabColor((FXTabItem*)w,false);
    }
  }
  if ((w=childAtIndex(getCurrent()*2))) {
    setTabColor((FXTabItem*)w,true);
    if ( (w=w->getNext()) && (w=w->getFirst()) ) { w->setFocus(); }
  }
}



void ShadyTabs::setCurrent(FXint i, FXbool notify)
{
  FXTabBook::setCurrent(i, notify);
  UpdateTabs();
}



long ShadyTabs::onCmdOpenItem(FXObject* sender, FXSelector sel, void* p)
{
  long rv=FXTabBook::onCmdOpenItem(sender,sel,p);
  UpdateTabs();
  return rv;
}

