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


#ifndef FXITE_THEME_H
#define FXITE_THEME_H

typedef struct _AppColors {
  const FXchar* name;
  FXColor BaseColor;
  FXColor BorderColor;
  FXColor BackColor;
  FXColor ForeColor;
  FXColor SelbackColor;
  FXColor SelforeColor;
  FXColor TipbackColor;
  FXColor TipforeColor;
  FXColor SelMenuBackColor;
  FXColor SelMenuTextColor;
} AppColors;

class ThemeControl;

class Theme:public FXObject {
private:
  friend class ThemeControl;
  static AppColors*CurrentColors();
  static AppColors*SystemColors();
  static const AppColors*PresetThemes();
  static FXint NumPresets();
  static const FXString GetCurrentFontName();
  static void SetCurrentFontName(const FXString&name);
  static bool SameColors(AppColors*a,AppColors*b);
  static void CopyColors(AppColors*dst,AppColors*src);
  static bool GetUseSystemColors();
  static FXuint SetUseSystemColors(bool use);
  static void Backup();
  static FXuint Restore();
  static FXuint Modified();
public:
  static void init();
  static void done();
  static void apply(FXWindow*win);
  enum {
    Unchanged     = 0,
    ChangedColors = 1<<0,
    ChangedFont   = 1<<1
  };
};

#endif

