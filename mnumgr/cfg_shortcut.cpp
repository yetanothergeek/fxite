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
#include "cfg_shortcut.h"


FXDEFMAP(ShortcutEditor) ShortcutEditorMap[]={
  FXMAPFUNC(SEL_KEYPRESS,            ShortcutEditor::ID_ACCELFIELD, ShortcutEditor::onAccelField),
  FXMAPFUNC(SEL_MIDDLEBUTTONRELEASE, ShortcutEditor::ID_ACCELFIELD, ShortcutEditor::onAccelField),
  FXMAPFUNC(SEL_COMMAND,             ShortcutEditor::ID_MODIFIED,   ShortcutEditor::onModified),
  FXMAPFUNC(SEL_FOCUSIN, 0, ShortcutEditor::onFocusIn),
};

FXIMPLEMENT(ShortcutEditor,FXGroupBox,ShortcutEditorMap,ARRAYNUMBER(ShortcutEditorMap));



long ShortcutEditor::onFocusIn(FXObject*o, FXSelector sel, void*p)
{
  FXGroupBox::onFocusIn(o,sel,p);
  accel_field->setFocus();
  return 1;
}



ShortcutEditor::ShortcutEditor(FXComposite* p, FXObject*trg, FXSelector sel):
  FXGroupBox(p,_("Shortcut"),GROUPBOX_NORMAL|FRAME_SUNKEN|FRAME_RAISED|FRAME_THICK)
{
  target=trg;
  message=sel;
  FXVerticalFrame*vframe=new FXVerticalFrame(this,LAYOUT_FILL);
  FXHorizontalFrame*hframe;
  hframe = new FXHorizontalFrame(vframe,LAYOUT_FILL_X);
  new FXLabel(hframe,_("&Key:"));
  accel_field=new FXTextField(hframe,6,this,ID_ACCELFIELD,TEXTFIELD_LIMITED|FRAME_SUNKEN|FRAME_THICK);
  hframe = new FXHorizontalFrame(vframe,LAYOUT_FILL_X);
  ctrl_chk  = new FXCheckButton(hframe,_("Ctrl"), this, ID_MODIFIED);
  alt_chk   = new FXCheckButton(hframe,_("Alt"),  this, ID_MODIFIED);
  shift_chk = new FXCheckButton(hframe,_("Shift"),this, ID_MODIFIED);
}



#define IsPrintingKey(c) ((c>=KEY_exclam)&&(c<=KEY_asciitilde))
#define IsFunctionKey(c) ((c>=KEY_F1) && (c<=KEY_F12))
#define AcceptKey(c) (IsPrintingKey(c)||IsFunctionKey(c))


long ShortcutEditor::onAccelField(FXObject*o, FXSelector sel, void*p)
{  
  if (FXSELTYPE(sel)==SEL_MIDDLEBUTTONRELEASE) { return 1; }

  FXEvent*ev=(FXEvent*)p;
  if (AcceptKey(ev->code)) {
    SetKey(ev->code);
    if (ev->state) { SetState(ev->state); }
    notify();
  } else {
    switch (ev->code) {
      case KEY_Tab: {
        if ((ev->state==0) || (ev->state==SHIFTMASK)) {
          return 0;
        } else {
          SetKey(ev->code);
          SetState(ev->state);
          notify();
          return 1;
        }
      }
      case KEY_Return:
      case KEY_KP_Enter:
      case KEY_Escape:
      case KEY_Left:
      case KEY_Right:
      case KEY_Up:
      case KEY_Down:
      case KEY_Page_Up:
      case KEY_Page_Down:
      {
        if (ev->state) {
          SetKey(ev->code);
          SetState(ev->state);
          notify();
          return 1;
        } else { 
          return 0;
        }
      }
      case KEY_Delete:
      case KEY_BackSpace: {
        if (ev->state) {
          SetKey(ev->code);
          SetState(ev->state);
        } else {
          accel_field->setText(FXString::null);
          SetState(0);
        }
        notify();
        return 1;
      }
    }
  }
  return 1;
}



void ShortcutEditor::SetState(FXint state)
{
  ctrl_chk->setCheck(state&CONTROLMASK);
  alt_chk->setCheck(state&ALTMASK);
  shift_chk->setCheck(state&SHIFTMASK);
}



void ShortcutEditor::SetKey(FXint code)
{
  FXString key=code?unparseAccel(code):FXString::null;
  if (key.length()==1) { key=key.upper(); }
  accel_field->setText(key);
}



long ShortcutEditor::onModified(FXObject*o, FXSelector sel, void*p)
{
  notify();
  return 0;
}



void ShortcutEditor::setShortcut(const FXString &sc)
{
  setShortcut(sc.empty()?0:parseAccel(sc));
}



void ShortcutEditor::setShortcut(FXHotKey hk)
{
  SetKey(hk&0xffff);
  SetState((hk&0xffff0000)>>16);
}



bool ShortcutEditor::verify()
{
  FXint state=GetState();
  FXint key=parseAccel(accel_field->getText());
  if ((state==0)&&accel_field->getText().empty()) { return true; }
  if (IsFunctionKey(key)) { return true; }
  if (((state&ALTMASK)||(state&CONTROLMASK)) && (!accel_field->getText().empty())) { return true; }
  if ((state==SHIFTMASK)&&!IsPrintingKey(key)) { return true; }
  FXMessageBox::error(getShell(), MBOX_OK, _("Invalid accelerator"), "%s\n\n%s",
    _("Only the function keys [F1] - [F12] can be used without a modifier."),
    _("Other key combinations must have a [Ctrl] and/or [Alt] modifier.")
  );
  accel_field->setFocus();
  accel_field->selectAll();
  return false;
}



FXuint ShortcutEditor::GetState()
{
  return 
    (ctrl_chk->getCheck()?CONTROLMASK:0) | 
      (alt_chk->getCheck()?ALTMASK:0) | 
        (shift_chk->getCheck()?SHIFTMASK:0);
}



FXHotKey ShortcutEditor::getChord()
{
  return (accel_field->getText().empty()) ? 0 : MKUINT(parseAccel(accel_field->getText()),GetState());
}



const FXString ShortcutEditor::getShortcut()
{
  return (accel_field->getText().empty()) ? FXString::null : unparseAccel(getChord());
}



void ShortcutEditor::notify(FXSelector sel)
{
  if (target) {
    const FXString sc=getShortcut();
    target->handle(this,FXSEL(sel,message),(void*)&sc);
  }
}



bool ShortcutEditor::ConfirmOverwrite(FXAccelTable*table)
{
  FXHotKey acckey=getChord();
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

