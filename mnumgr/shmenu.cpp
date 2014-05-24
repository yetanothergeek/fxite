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


#include <cctype>
#include <fxkeys.h>
#include <fx.h>
#include "sl.h"

#include "menuspec.h"
#include "compat.h"

#include "shmenu.h"

#define MAX_DEPTH 16         // Maximum recursion level
#define MAX_ENTRIES_PER 32   // Maximum entries per menu pane
#define MAX_ENTRIES_ALL 512  // Maximum total entries per UserMenu object

FXDEFMAP(UserMenu) UserMenuMap[] = { };

FXIMPLEMENT(UserMenu,FXObject,UserMenuMap,ARRAYNUMBER(UserMenuMap));



class UsrMnuCmd: public FXMenuCommand {
  FXDECLARE(UsrMnuCmd);
  UsrMnuCmd(){}
public:
  UsrMnuCmd( FXComposite *p,
             const FXString &text, FXObject *tgt=NULL, FXSelector sel=0):FXMenuCommand(p,text,NULL,tgt,sel){}

  long onButtonRelease(FXObject*o,FXSelector sel,void*p) {
    FXbool active=isActive();
    if(!isEnabled()) { return 0; }
    getParent()->handle(this,FXSEL(SEL_COMMAND,ID_UNPOST),NULL);
    if(active && target){ target->tryHandle(this,FXSEL(SEL_COMMAND,message),(void*)(FXuval)2); }
    return 1;
  }
  long onKeyRelease(FXObject*o,FXSelector sel,void* p){
    FXEvent* ev=(FXEvent*)p;
    if(isEnabled() && (flags&FLAG_PRESSED)){
      if(ev->code==KEY_space || ev->code==KEY_KP_Space || ev->code==KEY_Return || ev->code==KEY_KP_Enter) {
        flags&=~FLAG_PRESSED;
        getParent()->handle(this,FXSEL(SEL_COMMAND,ID_UNPOST),NULL);
        if(target) target->tryHandle(this,FXSEL(SEL_COMMAND,message),(void*)(FXuval)((ev->state&CONTROLMASK)?2:1));
        return 1;
        }
      }
    return 0;
  }
  void ValidateTBarCmd() { MenuMgr::ValidateUsrTBarCmd(this); }
  ~UsrMnuCmd() {
    MenuMgr::InvalidateUsrTBarCmd(this);
    free(getUserData());
  }
};



FXDEFMAP(UsrMnuCmd) MyCmdMap[] = {
  FXMAPFUNC(SEL_KEYRELEASE,0,UsrMnuCmd::onKeyRelease),
  FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,0,UsrMnuCmd::onButtonRelease)
};

FXIMPLEMENT(UsrMnuCmd,FXMenuCommand,MyCmdMap,ARRAYNUMBER(MyCmdMap))



UserMenu::UserMenu(FXMenuPane *owner,
  const FXString &label, const FXString &dirname, char tag, FXObject *tgt, FXSelector sel, const char**flags)
{
  target=(FXWindow*)tgt;
  selector=sel;
  _flags=flags;
  _tag=tag;
  topcasc=NULL;
  toplabel=label.text();
  topdir=dirname.text();
  topowner=owner;
  rescan();
}



void UserMenu::DeletePanes()
{
  for (FXint i=0; i<panes.no(); i++) {
    FXMenuPane*pane=panes[i];
    if (pane->getUserData()) { free(pane->getUserData()); }
    delete pane;
  }
  panes.clear();
}



UserMenu::~UserMenu()
{
  DeletePanes();
  if (topcasc) {
    delete(topcasc);
    topcasc=NULL;
  }
}



void UserMenu::create()
{
  for (FXint i=0; i<panes.no(); i++) {
    panes[i]->create();
  }
  topcasc->create();
}



void UserMenu::rescan()
{
  bool created=false;
  level=1;
  count=0;
  hasitems=false;
  DeletePanes();
  if (topcasc) {
    created=true;
    delete(topcasc);
    topcasc=NULL;
  }
  ScanDir(topowner,topdir.text());
  if (created) { create(); }
  if (topcasc) {
    if (hasitems) {
      if (panes[0]->numChildren()==1) {
        FXMenuCascade*mc=(FXMenuCascade*)panes[0]->getFirst();
        if (mc && (strcmp(mc->getClassName(),"FXMenuCascade")==0)) {
          if (mc->getText()=="Hidden") {
            topcasc->hide();
          }
        }
      }
    } else { topcasc->hide(); }
  }
}



void UserMenu::enable()
{
  if (topcasc) { topcasc->enable(); }
}



void UserMenu::disable()
{
  if (topcasc) { topcasc->disable(); }
}



FXString UserMenu::getText()
{
  if (topcasc) {return topcasc->getText(); } else {
    FXString rv=FXPath::name(topdir);
    rv[0]=toupper(rv[0]);
    return rv;
  }
}



void UserMenu::setText(const FXString &s)
{
  if (topcasc) { topcasc->setText(s); }
}



bool UserMenu::MakeLabelFromPath(const char*path, FXString &label)
{
  label=FXPath::name(path);

  if ( (label.length()>=3) && isdigit(label[0]) && isdigit(label[0]) && (label[2]=='.')) {
    label.erase(0,3);
  } else {
    return false;
  }

  label=FXPath::stripExtension(label);

  if (!(FXPath::extension(label)).empty()) {
    label=FXPath::stripExtension(label);
  }
  FXString accel="";
  FXint at=label.find('@');
  if (at>=0) {
    accel=label.text();
    accel.erase(0,at+1);
    label.trunc(at);
  }
  if (isdigit(label[0])&&isdigit(label[1]&&(label[2]=='.'))) { label.erase(0,3); }
  label.lower();
  label.substitute('_', '&', false);
  label.substitute('-', ' ', true);
  label.substitute('\t', ' ', true);
  label.trim();
  label.simplify();
  if (!accel.empty()) {
    label.append("\t");
    label.append(accel);
  }
  if (label[0]=='&') {
    label[1]=toupper(label[1]);
  } else {
    label[0]=toupper(label[0]);
  }
  return true;
}



typedef struct _StrNode {
  struct _StrNode*next;
  char* data;
} StrNode;



static int NodeCmp(StrNode*n1, StrNode*n2)
{
  return strcasecmp(n1->data, n2->data);
}


typedef int (*SlFunc)(void*,void*);

static void NodeFree(void*p)
{
  StrNode*n=(StrNode*)p;
  free(n);
}



static StrNode*NodeNew(const char*s)
{
  StrNode*n=(StrNode*)malloc(sizeof(StrNode));
  n->next=NULL;
  n->data=strdup(s);
  return n;
}



class UserMenuHelper {
public:
  static void IncCount(UserMenu*um) { um->count++; um->hasitems=true; }
  static void ScanDir(UserMenu*um, FXMenuPane*parent, const char *directory) { um->ScanDir(parent, directory); }
};



int NodeCB(StrNode*n, FXMenuPane*mp)
{
  UserMenu*um=(UserMenu*)mp->getUserData();
  if ( (mp->numChildren()<MAX_ENTRIES_PER) && (um->getCount()<MAX_ENTRIES_ALL)) {
    if ( FXStat::isDirectory(n->data) ) {
     UserMenuHelper::ScanDir(um, mp, n->data);
     free(n->data);
    } else {
      FXString label;
      if ( UserMenu::MakeLabelFromPath(n->data, label) ) {
        UserMenuHelper::IncCount(um);
        UsrMnuCmd*cmd=new UsrMnuCmd(mp,label,um->getTarget(),um->getSelector());
        cmd->setUserData(n->data);
        cmd->ValidateTBarCmd();
      }
    }
  }
  return 0;
}



void UserMenu::ScanDir(FXMenuPane*parent, const char *directory)
{
  FXint matchmode=FILEMATCH_FILE_NAME|FILEMATCH_NOESCAPE;
  FXString fn;
  FXString pathname;
  FXString pattern="*";
  FXDir dir;
  FXStat info;
  level++;
  if (( level<=MAX_DEPTH ) && dir.open(directory)) {
    void*list=NULL;

#ifdef FOX_1_6
    while (dir.next()) {
      fn=dir.name();
#else
    FXString name;
    while (dir.next(name)) {
      fn=name.text();
#endif
      if ( fn[0]=='.') { continue; }
      pathname=directory;
      if(!ISPATHSEP(pathname[pathname.length()-1])) { pathname.append(PATHSEPSTRING); }
      pathname.append(fn);
      if (FXStat::statFile(pathname,info)) {
        if ((info.isFile() && PathMatch(pattern,fn,matchmode))||info.isDirectory()) {
          list=sl_push(list,NodeNew(pathname.text()));
        }
      }
    }
    dir.close();
    FXString label;
    if (MakeLabelFromPath(directory, label)||(!topcasc)) {
      count++;
      if (list) { list=sl_mergesort(list,(SlFunc)NodeCmp); }
      FXMenuPane*mp=new FXMenuPane(target);
      panes.append(mp);
      FXMenuCascade* mc;
      if (topcasc) {
        mc=new FXMenuCascade(parent,label,NULL,mp);
        if (label=="Hidden") { mc->hide(); }
      } else {
        mc=new FXMenuCascade(parent,toplabel,NULL,mp);
        topcasc=mc;
      }
      mp->setUserData(this);
      if (list) { sl_map(list,(SlFunc)NodeCB, mp); } else { mc->hide(); }
      mp->setUserData(strdup(directory));
    }
    if (list) { sl_free(list,NodeFree); }
  }
  level--;
}

