/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2010-2011 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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


// Dialog box that implements an editable list of items and descriptions.
// Entries can be edited, re-ordered, reverted to previous state, or reverted
// to application defaults. Each subclass should provide its own definition
// of the Verify(), RestoreAppDefaults(), setText() and getText() methods.
class DescListDlg: public FXDialogBox {
  FXDECLARE(DescListDlg)
protected:
  int desc_max_len;
  int item_max_len;
  int items_max;
  bool has_browse_btn;
  DescListDlg(){}
  FXString before;
  FXString after;
  FXIconList*items;
  FXButton*raise_btn;
  FXButton*lower_btn;
  FXButton*edit_btn;
  FXButton*delete_btn;
  FXButton*new_btn;
  FXLabel* intro_label;
  FXString item_comment;
  void enableButtons();
  virtual void setText(const FXString str) {}
  virtual const FXString& getText() { return after; }
  virtual bool Verify(FXString&item) { return true; }
  virtual void RestoreAppDefaults() {}
  virtual bool Browse(FXString &text) { return false; }
public:
  bool editItem(const FXString &desc, const FXString &item, bool focus_item=false);
  long onCommand(FXObject* sender,FXSelector sel,void *ptr);
  long onClose(FXObject* sender,FXSelector sel,void *ptr);
  long onBrowse(FXObject* sender,FXSelector sel,void *ptr);
  enum{
    ID_LIST_SEL=FXDialogBox::ID_LAST,
    ID_DEFAULTS_CMD,
    ID_REVERT_CMD,
    ID_RAISE_CMD,
    ID_LOWER_CMD,
    ID_EDIT_CMD,
    ID_DELETE_CMD,
    ID_NEW_CMD,
    ID_BROWSE,
    ID_LAST
  };
  // For max_desc_len, max_item_len, max_items: zero means unlimited.
  // For max_desc_len: (-1) creates an item list only, no descriptions.
  DescListDlg( FXWindow* w, const char*name, const char*hdr2,
               const char*howto, const char*intro=NULL,
               int max_desc_len=0, int max_item_len=0, int max_items=0, bool browse_btn=false);
  virtual void create();
};

