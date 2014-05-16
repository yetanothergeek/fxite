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

#include <fx.h>

#include "mru_menu.h"


class RecentFilesMnuCmd: public FXMenuCommand {
public:
  RecentFilesMnuCmd( FXComposite *p,
              const FXString &text, FXObject *tgt=NULL, FXSelector sel=0):FXMenuCommand(p,text,NULL,tgt,sel){}
  ~RecentFilesMnuCmd() { free(getUserData()); }
};



FXDEFMAP(RecentFilesMenu) RecentFilesMenuMap[] = {
  FXMAPFUNC(SEL_COMMAND,RecentFilesMenu::ID_ITEM_CLICK,RecentFilesMenu::onItemClick),
};

FXIMPLEMENT(RecentFilesMenu,FXMenuPane,RecentFilesMenuMap,ARRAYNUMBER(RecentFilesMenuMap))



long RecentFilesMenu::onItemClick(FXObject*o,FXSelector sel,void*p)
{
  RecentFilesMnuCmd*mc = (RecentFilesMnuCmd*)o;
  if (target&&message) { target->handle(this,FXSEL(SEL_COMMAND, message), mc->getUserData()); }
  return 1;
}



void RecentFilesMenu::add_item(const FXString &txt, bool prepended)
{
  if ((!prepended) && (numChildren()>=26)) { return; }
  if (!txt.empty()) {
    if (prepended) {
      remove(txt);
    } else {
      if (find(txt)) { return; }
    }
    FXStat info;
    if ( FXStat::statFile(txt,info) && info.isFile() && info.isReadable() ) {
      RecentFilesMnuCmd*mc=new RecentFilesMnuCmd(this,txt,this,ID_ITEM_CLICK);
      mc->setUserData((void*)(strdup(  FXPath::simplify(FXPath::absolute(txt)).text()  )));
      if (created) { mc->create(); }
      if (prepended) {
        mc->reparent(this, this->getFirst());
        while (numChildren()>26) { delete getLast(); }
      }
    }
  }
}



void RecentFilesMenu::prepend(const FXString &txt)
{
  add_item(txt, true);
}



void RecentFilesMenu::append(const FXString &txt)
{
  add_item(txt, false);
}



void RecentFilesMenu::remove(const FXString &txt)
{
  if (!txt.empty()) {
    FXWindow*w;
    FXString s=FXPath::simplify(FXPath::absolute(txt)).text();
    for (w=getFirst(); w; ) {
      if (strcmp(s.text(), (char*)w->getUserData())==0) {
        FXWindow*w2=w->getNext();
        delete (RecentFilesMnuCmd*) w;
        w=w2;
      } else {
        w=w->getNext();
      }
    }
  }
}



FXMenuCommand*RecentFilesMenu::find(const FXString &txt)
{
  if (!txt.empty()) {
    FXWindow*w;
    FXString s=FXPath::simplify(FXPath::absolute(txt)).text();
    for (w=getFirst(); w; w=w->getNext()) {
      if (strcmp(s.text(), (char*)w->getUserData())==0) {
        return (FXMenuCommand*)w;
      }
    }
  }
  return NULL;
}



RecentFilesMenu::RecentFilesMenu(FXWindow *p,
  const FXString &caption, const FXString &groupname, FXObject *tgt, FXSelector sel):FXMenuPane(p)
{
  target=tgt;
  message=sel;
  created=false;
  FXchar key[2]="\0";
  casc=new FXMenuCascade((FXComposite*)p,caption, NULL, this);
  group=groupname.text();
  app=p->getApp();
  for (key[0]='a'; key[0]<='z'; key[0]++) { prepend(app->reg().readStringEntry(group.text(),key, "")); }
}



RecentFilesMenu::~RecentFilesMenu()
{
  FXchar key[2]="\0";
  FXWindow*w;
  for (key[0]='a'; key[0]<='z'; key[0]++) { append(app->reg().readStringEntry(group.text(),key, "")); }
  app->reg().deleteSection(group.text());
  for (w=getFirst(), key[0]='z' ; w && (key[0]>='a'); w=w->getNext(), key[0]--) {
    app->reg().writeStringEntry(group.text(),key,(char*)w->getUserData());
  }
}



void RecentFilesMenu::create()
{
  if (created) return;
  created=true;
  FXMenuPane::create();
  casc->create();
}

