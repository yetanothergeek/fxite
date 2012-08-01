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

#include <fx.h>
#include <fxkeys.h>
#include <Scintilla.h>
#include <FXScintilla.h>

#include "scidoc.h"
#include "appwin.h"
#include "compat.h"
#include "prefs.h"

#include "intl.h"
#include "outpane.h"



FXDEFMAP(OutputList) OutputListMap[]={
  FXMAPFUNC(SEL_COMMAND,            0,                            OutputList::onUserInput),
  FXMAPFUNC(SEL_DOUBLECLICKED,      0,                            OutputList::onUserInput),
  FXMAPFUNC(SEL_KEYPRESS,           0,                            OutputList::onUserInput),
  FXMAPFUNC(SEL_RIGHTBUTTONRELEASE, 0,                            OutputList::onUserInput),
  FXMAPFUNC(SEL_FOCUSIN,            0,                            OutputList::onUserInput),
  FXMAPFUNC(SEL_COMMAND,            OutputList::ID_SELECT_ALL,    OutputList::onPopup),
  FXMAPFUNC(SEL_COMMAND,            OutputList::ID_COPY_SELECTED, OutputList::onPopup),
};

FXIMPLEMENT(OutputList,FXList,OutputListMap,ARRAYNUMBER(OutputListMap));


long OutputList::onUserInput(FXObject*o, FXSelector sel, void*p)
{
  FXEvent* ev=(FXEvent*)p;
  switch (FXSELTYPE(sel)) {
    case SEL_RIGHTBUTTONRELEASE: {
      if(!ev->moved){
        outpop->create();
        outpop->popup(NULL,ev->root_x,ev->root_y);
        getApp()->runModalWhileShown(outpop);
        outpop->destroy();
      }
      return 1;
    }
    case SEL_DOUBLECLICKED: { break; }
    case SEL_KEYPRESS: {
      FXint code=ev->code;
      if ((code==KEY_Return)||(code==KEY_KP_Enter)) { break; } else {
        if (code==KEY_Tab) {
          killFocus();
          tw->ControlDoc()->setFocus();
          return 1;
        } else {
          if (ev->state&CONTROLMASK) {
            switch (code) {
              case KEY_a: { return handle(this,FXSEL(SEL_COMMAND,ID_SELECT_ALL),p); }
              case KEY_c: { return handle(this,FXSEL(SEL_COMMAND,ID_COPY_SELECTED),p); }
            }
          }
        }
        return FXList::handle(o,sel,p);
      }
    }
    case SEL_FOCUSIN: {
      tw->ActiveWidget(this);
      return FXList::handle(o,sel,p);
    }
    default: { return FXList::handle(o,sel,p); }
  }
  GoToError();
  return 1;
}



void OutputList::GoToError()
{
  FXint n=getCurrentItem();
  if (n>=0) {
    FXListItem*item=getItem(n);
    if (item) {
      FXString txt=item->getText();
      if (!txt.empty()) {
        ErrorPattern*pats=prefs->ErrorPatterns();
        for (FXint i=0; i<prefs->ErrorPatternCount(); i++) {
          FXint begs[4]={0,0,0,0};
          FXint ends[4]={0,0,0,0};
          FXRex rx(pats[i].pat, REX_CAPTURE);
          if (rx.match(txt,begs,ends,REX_FORWARD,3)) {
            FXString filename = txt.mid(begs[1],ends[1]-begs[1]);
            FXString linenum =  txt.mid(begs[2],ends[2]-begs[2]);
            if (FXStat::isFile(filename)) {
              tw->OpenFile(filename.text(), linenum.text(),false,true);
              break;
            } else {
              SciDoc*sci=tw->ControlDoc();
              if (sci && (!sci->Filename().empty()) && (!FXPath::isAbsolute(filename))) {
                filename=FXPath::name(filename);
                filename.prepend(PATHSEP);
                filename.prepend(FXPath::directory(sci->Filename()));
                if (FXStat::isFile(filename)) {
                  tw->OpenFile(filename.text(), linenum.text(),false,true);
                  break;
                }
              }
            }
          }
        }
      }
    }
  }
}



bool OutputList::Focus()
{
  static bool isfocused=false;
  if (getNumItems()<=0) { return 1; }
  isfocused=!isfocused;
  if (!isfocused) {
    killFocus();
    tw->ControlDoc()->setFocus();
    return false;
  } else {
    if (!prefs->ShowOutputPane) { tw->ShowOutputPane(true); }
    tw->FocusedDoc()->killFocus();
    setFocus();
    if (getCurrentItem()<0) { setCurrentItem(0); }
    if (!isItemSelected(getCurrentItem())) {
      selectItem(getCurrentItem());
    }
    makeItemVisible(getCurrentItem());
    return true;
  }
}



long OutputList::onPopup(FXObject*o, FXSelector sel, void*p)
{
  FXint count=getNumItems();
  if (count==0) { return 1; }
  switch (FXSELID(sel))
  {
    case ID_SELECT_ALL: {
      setAnchorItem(0);
      selectItem(0);
      extendSelection(count-1);
      break;
    }
    case ID_COPY_SELECTED: {
      FXint i;
      FXString outclip="";
      FXString newline="\n";
      switch (prefs->DefaultFileFormat) {
        case 0: { newline="\r\n"; break; }
        case 1: { newline="\r";   break; }
        case 2: { newline="\n";   break; }
      }
      for (i=0; i<count; i++) {
        if (isItemSelected(i)) {
          outclip.append(getItemText(i));
          outclip.append(newline);
        }
      }
      tw->ControlDoc()->sendString(SCI_COPYTEXT,outclip.length(), outclip.text());
    }
    default: { return 0; }
  }
  return 1;
}



FXint OutputList::fillItems(const FXString& strings)
{
  FXString s=strings;
  s.substitute('\t', ' ');
  s.substitute('\r', ' ');
  FXint rv=FXList::fillItems(s);
  makeItemVisible(getNumItems()-1);
  return rv;
}



FXint OutputList::appendItem(const FXString& text)
{
  FXint rv=FXList::appendItem(text);
  makeItemVisible(getNumItems()-1);
  return rv; 
}



void OutputList::SelectFirstError()
{
  FXRex rx(_(": [Ee]rror: "));
  for (FXint i=0; i<getNumItems(); i++) {
    FXListItem *item=getItem(i);
    if (rx.match(item->getText())) {
      selectItem(i);
      setCurrentItem(i);
      makeItemVisible(i);
      break;
    }
  }
}



OutputList::OutputList(FXComposite*p,FXObject*tgt,FXSelector sel,FXuint opts,FXint x,FXint y,FXint w,FXint h):
FXList(p,tgt,sel,opts,x,y,w,h)
{
  tw=TopWindow::instance();
  prefs=Settings::instance();
  outpop=new FXMenuPane(this);
  new FXMenuCommand(outpop,_("Select &All"),NULL,this,ID_SELECT_ALL);
  new FXMenuCommand(outpop,_("&Copy"),NULL,this,ID_COPY_SELECTED);
}



OutputList::~OutputList()
{
  delete outpop; 
}

