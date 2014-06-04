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

#ifndef MNUMGR_CFG_SHORTCUT_H
#define MNUMGR_CFG_SHORTCUT_H


class ShortcutEditor: public FXGroupBox {
  FXDECLARE(ShortcutEditor)
protected:
  FXCheckButton* ctrl_chk;
  FXCheckButton* alt_chk;
  FXCheckButton* shift_chk;
  FXTextField* accel_field;
  ShortcutEditor() {}
  void SetState(FXint state);
  void SetKey(FXint code);
  FXuint GetState();
  void notify(FXSelector sel=SEL_CHANGED);
public:
  ShortcutEditor(FXComposite*p, FXObject*trg=NULL, FXSelector sel=0);
  void setShortcut(const FXString &sc);
  void setShortcut(FXHotKey hk);
  const FXString getShortcut();
  FXHotKey getChord();
  bool ConfirmOverwrite(FXAccelTable*table);
  virtual bool verify();
  long onAccelField(FXObject*o, FXSelector sel, void*p);
  long onModified(FXObject*o, FXSelector sel, void*p);
  long onFocusIn(FXObject*o, FXSelector sel, void*p);
  enum {
    ID_ACCELFIELD=FXGroupBox::ID_LAST,
    ID_MODIFIED,
    ID_LAST
  };
};

#endif
