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
#include <fxkeys.h>

#include "intl.h"
#include "cfg_keybind.h"

FXDEFMAP(KeyBindingList) KeyBindingListMap[] = {
  FXMAPFUNC(SEL_QUERY_TIP,0,KeyBindingList::onQueryTip),
  FXMAPFUNC(SEL_DOUBLECLICKED,KeyBindingList::ID_ACCEL_EDIT,KeyBindingList::onAccelEdit),
  FXMAPFUNC(SEL_KEYPRESS,KeyBindingList::ID_ACCEL_EDIT,KeyBindingList::onAccelEdit),
};

FXIMPLEMENT(KeyBindingList,FXIconList,KeyBindingListMap,ARRAYNUMBER(KeyBindingListMap))


static bool AccelSanity(FXWindow*w, FXHotKey acckey)
{
  FXushort key=FXSELID(acckey);
  FXushort mod=FXSELTYPE(acckey);
  if (key==0) {
    FXMessageBox::error(w, MBOX_OK, _("Invalid keybinding"), _("That keybinding does not end with a valid key name"));
    return false;
  }
  if ((key>=KEY_F1)&&(key<=KEY_F12)) { return true; }
  if ((mod&CONTROLMASK)||(mod&ALTMASK)||(mod&METAMASK)) { return true; }
  return (FXMessageBox::question(w, MBOX_YES_NO, _("Weak keybinding"), "%s\n\n%s",
    _("That key binding doesn't contain any [Ctrl] or [Alt]\n"
      "modifiers, which might cause you some problems."),
    _("Are you sure you want to continue?")
   )==MBOX_CLICKED_YES);
}



bool KeyBindingList::AccelUnique(FXHotKey acckey, MenuSpec*spec)
{
  FXAccelTable*table=win->getAccelTable();
  if (!table->hasAccel(acckey)) { return true; }

  if (FXMessageBox::question(getShell(), MBOX_YES_NO, _("Conflicting keybinding"), "%s\n\n%s",
    _("This keybinding appears to conflict with an existing one."),
    _("Are you sure you want to continue?")
  )==MBOX_CLICKED_YES) {
    table->removeAccel(acckey);
    return true;
  } else {
    return false;
  }

}



bool KeyBindingList::AccelDelete(MenuSpec*spec)
{
  FXHotKey acckey=parseAccel(spec->accel);
  FXAccelTable*table=win->getAccelTable();
  if (!table->hasAccel(acckey)) {
    return true;
  } else {
    if (FXMessageBox::question(getShell(), MBOX_YES_NO, _("Confirm delete"),
      _("Are you sure you want to remove this keybinding?")
    )==MBOX_CLICKED_YES) {
     table->removeAccel(acckey);
      return true;
    } else {
      return false;
    }
  }
}



static bool EditAccel(FXString&acctxt, FXWindow*w, MenuSpec*spec, FXHotKey &acckey)
{
  FXInputDialog dlg(w,"","");
  FXint maxlen=sizeof(spec->accel)-1;
  dlg.setNumColumns(maxlen);
  FXString msg;
  msg.format(
    "%s:\n"
    "   Ctrl+Shift+F12\n"
    "   F3\n"
    "   Alt+G\n\n"
    "%s \"%s\"",
    _("Examples"), _("Keybinding for"), spec->pref);
  FXString orig=acctxt.text();
  while (true) {
    acckey=0;
    if (dlg.getString(acctxt, w->getShell(), _("Edit keybinding"), msg )) {
      if (acctxt.empty()) { return true; }
      acckey=parseAccel(acctxt);
      if (acckey) {
        acctxt=unparseAccel(acckey);
        if ((acctxt.length())<maxlen) {
          if (AccelSanity(w,acckey)) { return strcmp(spec->accel, acctxt.text())!=0; }
        } else {
          FXMessageBox::error(w->getShell(), MBOX_OK,
            _("Keybinding too long"), _("Text of keybinding specification must not exceed %d bytes\n"), maxlen);
        }
      } else {
        FXMessageBox::error(w->getShell(), MBOX_OK,
          _("Invalid keybinding"), "%s:\n%s", _("Failed to parse accelerator"), acctxt.text());
        acctxt=orig.text();
      }
    } else {
      return false;
    }
  }
}

void KeyBindingList::AccelInsert(FXHotKey acckey, MenuSpec*spec)
{
   FXAccelTable*table=win->getAccelTable();
   if (spec->ms_mc) {
     spec->ms_mc->setAccelText(spec->accel);
     table->addAccel(acckey,spec->ms_mc->getTarget(),FXSEL(SEL_COMMAND,spec->sel));
   } else {
     table->addAccel(acckey,win,FXSEL(SEL_COMMAND,spec->sel));
   }
}

long KeyBindingList::onAccelEdit(FXObject*o, FXSelector s, void*p)
{
  if (o!=this) { return 0; }
    switch ( FXSELTYPE(s) ) {
    case SEL_DOUBLECLICKED: {  break;  }
    case SEL_KEYPRESS: {
      FXint code=((FXEvent*)p)->code;
      switch (code) {
        case KEY_Return: { break; }
        case KEY_KP_Enter: { break; }
        case KEY_F2: { break; }
        case KEY_space: { break; }
        default: { return 0; }
      }
      break;
    }
    default: { return 0; }
  }
  MenuSpec*spec=(MenuSpec*)(getItemData(getCurrentItem()));
  FXAccelTable*table=win->getAccelTable();
  if (spec) {
    FXString acctxt=spec->accel;
    FXHotKey acckey;
    if ( EditAccel(acctxt,this,spec,acckey) ) {
      if (acctxt.empty()) {
        if (AccelDelete(spec)) {
          memset(spec->accel,0,sizeof(spec->accel));
          if (spec->ms_mc) { spec->ms_mc->setAccelText(spec->accel); }
          FXString txt;
          txt.format("%s\t",spec->pref);
          setItemText(getCurrentItem(),txt);
        }
      } else {
        if (AccelUnique(acckey, spec)) {
          FXHotKey oldkey=parseAccel(spec->accel);
          memset(spec->accel,0,sizeof(spec->accel));
          strncpy(spec->accel, acctxt.text(),sizeof(spec->accel)-1);
          if (oldkey && table->hasAccel(oldkey)) { table->removeAccel(oldkey); }
          AccelInsert(acckey,spec);
          FXString txt;
          txt.format("%s\t%s",spec->pref,spec->accel);
          setItemText(getCurrentItem(),txt);
        }
      }
    }
  } else {
    FXMessageBox::error(getShell(), MBOX_OK, _("Internal error"), _("Failed to retrieve keybinding information"));
  }
  return 1;
}



#define LIST_OPTS FRAME_SUNKEN|FRAME_THICK|LAYOUT_SIDE_TOP|LAYOUT_FILL_Y|LAYOUT_FIX_WIDTH|ICONLIST_BROWSESELECT

KeyBindingList::KeyBindingList(FXComposite*o, MenuMgr*mmgr, FXWindow*w):FXIconList(o,this,ID_ACCEL_EDIT,LIST_OPTS)
{
  mnumgr=mmgr;
  win=w;
  appendHeader(_("action"));
  appendHeader(_("keybinding"));
  FXString spaces;
  for (MenuSpec*spec=mnumgr->MenuSpecs(); spec->sel!=mnumgr->LastID(); spec++) {
    FXString txt;
    txt.format("%s\t%s", spec->pref, spec->accel);
    appendItem(new FXIconItem(txt, NULL, NULL, (void*)spec));
  }
  selectItem(0);
}



long KeyBindingList::onQueryTip(FXObject* sender,FXSelector sel,void* ptr)
{
  FXint index,cx,cy;
  FXuint btns;
  if (flags&FLAG_TIP) {
    if (getCursorPosition(cx,cy,btns) && (index=getItemAt(cx,cy))>=0) {
      FXString tip;
      MenuSpec*spec=(MenuSpec*)(getItem(index)->getData());
      mnumgr->GetTBarBtnTip(spec,tip);
      sender->handle(this,FXSEL(SEL_COMMAND,ID_SETSTRINGVALUE),(void*)&tip);
      return 1;
    }
  }
  return 0;
}


