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


#ifndef FXITE_CFG_THEME_H
#define FXITE_CFG_THEME_H

#include "theme.h"

/*
  The ThemeGUI class provides a user interface to configure an application's 
  colors and default font, somewhat similar to FOX's "ControlPanel" program.

  Whenever one of the colors is reconfigured, it sends a SEL_CHANGED message
  to its target, the data pointer is a typecast of Theme::ChangedColors.
  Likewise, if a new default font is chosen, it sends a SEL_CHANGED message
  to its target, the data pointer is a typecast of Theme::ChangedFont.

  The GUI consists of two parts: a group of "control" widgets used to modify
  the settings, and  a group of "sample" widgets that are updated to provide
  an example of the settings. The control widgets are a private part of the
  ThemeGUI class, but the ThemeSample class can be subclassed in case you want
  to create your own application-specific sample widgets. Applications that do
  this should also subclass ThemeGUI, overriding ThemeGUI::MakeSample() to 
  return an instance of your customized sample.
*/


class ThemeSample: public FXVerticalFrame {
  ThemeSample(){}
protected:
  FXDECLARE(ThemeSample);
  virtual void SetColors(AppColors*ac) {}
public:
  ThemeSample(FXComposite*o):FXVerticalFrame(o,FRAME_SUNKEN|LAYOUT_FILL,0,0,0,0,8,8,8,8) {}
  long onChanged(FXObject*o, FXSelector sel, void*p) {
    SetColors((AppColors*)p);
    return 1;    
  }
  enum {
    ID_CHANGED=FXVerticalFrame::ID_LAST,
  };
};



class ThemeGUI: public FXHorizontalFrame {
  FXDECLARE(ThemeGUI)
protected:
  ThemeGUI() {}
  virtual ThemeSample* MakeSample();
public:
  ThemeGUI(FXComposite*p, FXObject*trg=NULL, FXSelector sel=0);
};

#endif

