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


#ifndef SHADY_TABS_H
#define SHADY_TABS_H
class ShadyTabs: public FXTabBook {
private:
  FXDECLARE(ShadyTabs);
protected:
  ShadyTabs(){}
  void setTabColor(FXTabItem*t, bool active);
  void UpdateTabs();
public:
  long onCmdOpenItem  ( FXObject* sender, FXSelector sel, void* p );
  void setCurrent(FXint i, FXbool notify=false);
  ShadyTabs(FXComposite* p,FXObject* tgt=NULL, FXSelector sel=0, FXuint opts=TABBOOK_NORMAL,
              FXint x=0, FXint y=0, FXint w=0, FXint h=0, FXint pl=DEFAULT_SPACING,
              FXint pr=DEFAULT_SPACING, FXint pt=DEFAULT_SPACING, FXint pb=DEFAULT_SPACING);
};
#endif

