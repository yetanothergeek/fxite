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

#ifndef FXITE_TOOLBAR_H
#define FXITE_TOOLBAR_H


class MenuMgr;

// This thing behaves something like an FXHorizontalFrame that can "wrap" its contents across two rows.
class ToolBarFrame: public FXMatrix {
private:
  FXDECLARE(ToolBarFrame);
  bool wraptoolbar;
  bool hidden;
  FXFont*toolbar_font;
  FXuchar button_size;
  static void SetTBarBtnColorCB(FXButton*btn, void*user_data);
  static void SetTBarBtnFontCB(FXButton*btn, void*user_data);
  static void NullifyButtonDataCB(FXButton*btn, void*user_data);
  static void ClearTBarBtnDataCB(FXButton*btn, void*user_data);
  void reconf();
protected:
  void ForEachToolbarButton(void (*cb)(FXButton*btn, void*user_data), void*user_data);
  ToolBarFrame(){}
public:
  long onConfigure(FXObject*o,FXSelector sel,void*p);
  void NullifyButtonData(void*user_data);
  void SetToolbarColors();
  void SetTBarFont();
  void CreateButtons(FXMainWindow*tw, FXuchar btn_size, bool wrapit, FXSelector custom_cmd_id, MenuMgr*mmgr);
  bool Wrapped() { return wraptoolbar; }
  void Wrapped(bool wrapit) { wraptoolbar=wrapit; }
  FXuchar ButtonSize() { return button_size; }
  void ButtonSize(FXuchar bs) { button_size = bs>=2 ? 2 : bs<=0 ? 0 : bs; }
  virtual void show();
  virtual void hide();
  virtual void create();
  ToolBarFrame(FXComposite *o, bool hideit);
  ~ToolBarFrame();
};


#endif

