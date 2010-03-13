/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2010 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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


// Dialog box to modify preferences for the file dialog's filter list.

class FileFiltersDlg: public FXDialogBox {
  FXDECLARE(FileFiltersDlg)
protected:
  FileFiltersDlg(){}
  FXString before;
  FXString after;
  FXIconList*filters;
  FXButton*raise_btn;
  FXButton*lower_btn;
  FXButton*edit_btn;
  FXButton*delete_btn;
  FXButton*new_btn;
  void enableButtons();
  void setText(const FXString str);
public:
  bool editItem();
  long onCommand(FXObject* sender,FXSelector sel,void *ptr);
  long onClose(FXObject* sender,FXSelector sel,void *ptr);
  enum{
    ID_LIST_SEL=FXDialogBox::ID_LAST,
    ID_DEFAULTS_CMD,
    ID_REVERT_CMD,
    ID_RAISE_CMD,
    ID_LOWER_CMD,
    ID_EDIT_CMD,
    ID_DELETE_CMD,
    ID_NEW_CMD,
    ID_LAST
  };
  FileFiltersDlg(FXWindow* w, const FXString init);
  virtual void create();
  const FXString& getText();
};

