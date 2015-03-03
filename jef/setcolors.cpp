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
#include "setcolors.h"

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
  BackColor(FXIconList);
  BackColor(FXTreeList);
  BackColor(FXText);
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



void ApplyColorsToWindow(FXWindow* win) {
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
      ApplyColorsToWindow(w->getFirst());
    }
  }
}

