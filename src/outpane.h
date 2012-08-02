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

#ifndef OUTPANE_H
#define OUTPANE_H

class TopWindow;

class OutputList: public FXList {
  FXDECLARE(OutputList)
  OutputList() {}
protected:
  TopWindow*tw;
  FXMenuPane *outpop;
  Settings* prefs;
public:
  OutputList(FXComposite *p,FXObject* tgt=NULL,FXSelector sel=0,FXuint opts=LIST_NORMAL,FXint x=0,FXint y=0,FXint w=0,FXint h=0);
  ~OutputList();
  long onUserInput(     FXObject*o, FXSelector sel, void*p );
  long onPopup(  FXObject*o, FXSelector sel, void*p );
  void GoToError();
  bool Focus();
  FXint fillItems(const FXString& strings);
  FXint appendItem(const FXString& text);
  void SelectFirstError();
  virtual void show();
  virtual void hide();
  enum {
    ID_SOMETHING=FXList::ID_LAST,
    ID_SELECT_ALL,
    ID_COPY_SELECTED,
    ID_LAST
  };
};

#endif

