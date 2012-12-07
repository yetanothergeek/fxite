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


#ifndef DUAL_LIST_H

#define DUAL_LIST_H

/*
DualListForm is a composite control consisting of a list of "available" items
on the left-hand side and a list of "used" items on the right-hand side, and
two buttons between the lists to move items from one side to the other.
The right-hand list also has two buttons below the list to arrange the order
of an item up or down.

Subclasses should implement their own version of the PopulateAvail() and
PopulateUsed() methods, these will be called by the base class's constructor.

The control generates a SEL_CHANGED event whenever an item is moved from
one list to the other or reordered in the right-hand list. Subclasses can
assign whatever value they want to the "udata" variable and it will be passed
to the message handler.

*/

class DualListForm: public FXHorizontalFrame {
private:
  FXDECLARE(DualListForm);
protected:
  DualListForm(){}
  FXVerticalFrame* left_column;
  FXVerticalFrame* mid_column;
  FXVerticalFrame* right_column;
  FXButton* ins_btn;
  FXButton* rem_btn;
  FXButton* raise_btn;
  FXButton* lower_btn;
  FXint max_items; // Maximum number of used items ( zero = unlimited )
  FXList* avail_items;
  FXList* used_items;
  FXListItemList orig_avail;
  void*udata;
  void InsertItem(FXListItem*item);
  void CheckIndex();
  virtual void CheckCount();
  virtual void PopulateAvail(); // Add *all* possible items here, whether they are used or not!
  virtual void PopulateUsed();  // Move items from the "available" list to the "used" list here.
  void NotifyChanged(void*cbdata);
public:
  long onHiliteItem(  FXObject*o, FXSelector sel, void*p);
  long onInsertItem(  FXObject*o, FXSelector sel, void*p);
  long onRemoveItem(  FXObject*o, FXSelector sel, void*p);
  long onArrangeItem( FXObject*o, FXSelector sel, void*p);
  DualListForm(FXComposite*p, FXObject*tgt=NULL, FXSelector sel=0, FXint max=0);
  virtual void create();
  enum {
    ID_ITEM_INSERT = FXHorizontalFrame::ID_LAST,
    ID_ITEM_REMOVE,
    ID_AVAIL_ITEMS,
    ID_USED_ITEMS,
    ID_ITEM_RAISE,
    ID_ITEM_LOWER,
    ID_LAST
  };
};

#endif

