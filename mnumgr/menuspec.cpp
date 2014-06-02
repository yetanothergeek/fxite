/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2009-2013 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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

#include <fx.h>
#include "toolbar.h"
#include "color_funcs.h"
#include "shmenu.h"
#include "popmenu.h"

#include "intl.h"
#include "menuspec.h"

/*
MenuSpec types:
  m: FXMenuCommand (default)
  r: FXMenuRadio
  k: FXMenuCheck
  u: User-defined menu item from Tools menu item, used to create a toolbar button.
  x: User-defined menu item is in a "transitional" state:
      The mc field is overloaded and points to a filename, instead of a menu item.
*/



#define EXE_NAME FXPath::name(FXApp::instance()->getArgv()[0]).text()

MenuMgr::MenuMgr(MenuSpec*specs,FXint* tbar_btns,FXint id_last,const char** def_pop_cmnds,char** pop_cmnds, const char*cfg_dir)
{
  menu_specs=specs;
  toolbar_buttons=tbar_btns;
  last_id=id_last;
  DefaultPopupCommands=def_pop_cmnds;
  PopupCommands=pop_cmnds;
  config_dir=cfg_dir;
}



// This array holds information about toolbar buttons that the user created from
// his own user-defined menu items from the Tools menu.
static MenuSpec* custom_commands[TBAR_MAX_BTNS+1] = {
  NULL,NULL,NULL,NULL,
  NULL,NULL,NULL,NULL,
  NULL,NULL,NULL,NULL,
  NULL,NULL,NULL,NULL,
  NULL,NULL,NULL,NULL,
  NULL,NULL,NULL,NULL,
  NULL };



// We aren't using any fancy icons, but we can at least make each button a different color...
static ColorName tbar_colors[TBAR_MAX_BTNS] = {
  "#BBFFFF",
  "#FFEECC",
  "#CCEEFF",
  "#CCFFEE",
  "#DDDDFF",
  "#DDEEEE",
  "#FFBBFF",
  "#DDFFDD",
  "#ECECEC",
  "#EECCFF",
  "#EEEEDD",
  "#EEDDEE",
  "#EEFFCC",
  "#FFCCEE",
  "#FFFFBB",
  "#FFDDDD",
  "#BBFFFF",
  "#FFEECC",
  "#CCEEFF",
  "#CCFFEE",
  "#DDDDFF",
  "#DDEEEE",
  "#FFBBFF",
  "#DDFFDD",
};



const char*MenuMgr::TBarColors(FXint i) { return (const char*)(tbar_colors[i]); }


void MenuMgr::RemoveToolbarButton(FXint index)
{
  for (FXint i=index; i<TBAR_MAX_BTNS; i++) {
    toolbar_buttons[i]=toolbar_buttons[i+1];
    if (toolbar_buttons[i]==last_id) { break; }
  }
}



// The text for a button created from a user-defined menu uses
// the first two words from the menu item's label. Each of the
// two words is then truncated to a maximum of five characters.
static void SetSpecTBarBtnText(MenuSpec*spec)
{
  FXString btn_txt=spec->ms_mc->getText();
  if (btn_txt.contains(' ')) {
    FXString btn_txt2=btn_txt.section(' ',1);
    btn_txt=btn_txt.section(' ',0);
    btn_txt.trunc(5);
    btn_txt2.trunc(5);
    btn_txt.append(" ");
    btn_txt.append(btn_txt2);
  } else {
    btn_txt.trunc(5);
  }
  btn_txt.lower();
  spec->btn_txt=strdup(btn_txt.text());
}



//  When UserMenu objects are re-scanned, the menu commands they contain become invalid.
//  If any of our toolbar buttons reference these menu commands, we need to invalidate the
//  menu commands, while at the same time saving the filename that the item pointed to.
//  That way, when the menu is rebuilt, we can compare the filenames referenced by the new
//  menu item to our saved filenames and point the toolbar button to the new menu command.
// ( This method is called by the menu item's destructor. )
void MenuMgr::InvalidateUsrTBarCmd(FXMenuCommand*mc)
{
  for (FXint i=0; i<TBAR_MAX_BTNS; i++) {
    MenuSpec*spec=custom_commands[i];
    if (spec && (spec->type=='u') && (spec->ms_mc==mc)) {
      spec->type='x';
      char*tmp=strdup((const char*)(spec->ms_mc->getUserData()));
      spec->ms_fn=tmp;
    }
  }
}



//  When UserMenu objects are re-scanned, a completely new set of menu items is created.
//  Most of these simply replace existing items, so any of the custom toolbar button specs
//  that point to the old copy of the menu item will need to be updated so they will point
//  to the new instance of the item.
// ( This method is called by the menu item's constructor. )
void MenuMgr::ValidateUsrTBarCmd(FXMenuCommand *mc)
{
  if (mc) {
    const char*tmp=(const char*)(mc->getUserData());
    if (tmp) {
      for (FXint i=0; i<TBAR_MAX_BTNS; i++) {
        MenuSpec*spec=custom_commands[i];
        if (spec && spec->ms_fn && (spec->type=='x') && (strcmp(spec->ms_fn,tmp)==0)) {
          free(spec->ms_fn);
          spec->ms_mc=mc;
          spec->type='u';
          if (spec->btn_txt==NULL) { SetSpecTBarBtnText(spec); }
          return;
        }
      }
    }
  }
}



// After we have completed re-scanning the UserMenu object, there might
// still be some items that were deleted and never re-created. This procedure
// cleans up any remaining "orphaned" items.
void MenuMgr::PurgeTBarCmds(ToolBarFrame*toolbar)
{
  for (FXint i=0; i<TBAR_MAX_BTNS; i++) {
    MenuSpec*spec=custom_commands[i];
    if (spec && (spec->type=='x')) {
      if (spec->ms_fn) { free(spec->ms_fn); }
      if (spec->btn_txt) { free((char*)(spec->btn_txt)); }
      for (FXint j=0; j<TBAR_MAX_BTNS; j++) {
        if (toolbar_buttons[j]==spec->sel) { toolbar_buttons[j]=0; }
      }
      // If any toolbar button holds a reference to this spec, make sure we set it to NULL...
      toolbar->NullifyButtonData(spec);
      delete spec;
      custom_commands[i]=NULL;
    }
  }
  bool found;
  do {
    found=false;
    for (FXint i=0; i<TBAR_MAX_BTNS; i++) {
      if (toolbar_buttons[i]==0) {
        RemoveToolbarButton(i);
        found=true;
        break;
      }
    }
  } while (found);
  // Somewhat unrelated, but now is also a good time to clear any old
  // button references from the builtin commands...
  for (MenuSpec*spec=menu_specs; spec && spec->ms_mc; spec++) {
    spec->ms_mc->setUserData(NULL);
  }
}



#define _GetCaption(p) ((FXMenuCaption*)((p)->getParent()->getUserData()))


void MenuMgr::GetTipFromFilename(const char*filename, FXString &tip)
{
  tip=filename;
  tip.erase(0,config_dir.length());
  FXString path=FXPath::directory(tip);
#ifdef WIN32
  path.substitute(PATHSEP,'/');
#endif
  tip=FXPath::title(tip);
  tip.erase(0,3);
  if (tip.find('.')>=0) { tip=FXPath::title(tip); }
  tip=tip.section('@',0);
  tip.prepend(path+"/");
  tip.substitute("_","");
  tip.substitute('-',' ');
  FXint n=0;
  do {
    n=tip.find('/',n);
    if (n>=0) {
      if (isdigit(tip[n+1])&&isdigit(tip[n+2])&&(tip[n+3]=='.')) { tip.erase(n+1,3); }
      n++;
    } else {
      break;
    }
  } while (1);
  tip.substitute("/"," -> ");
  tip.at(0)=toupper(tip.text()[0]);
  for (char*c=&(tip.at(0)); *c; c++) { if (c[0]==' ') { c[1]=toupper(c[1]); } }
}



// Construct a tooltip string based on a menu item's path.
void MenuMgr::GetTBarBtnTip(MenuSpec*spec, FXString &tip)
{
  if (spec&&spec->ms_mc) {
    switch (spec->type) {
      case 'u': {
        GetTipFromFilename((const char*)spec->ms_mc->getUserData(),tip);
        break;
      }
      case 'x': {
        GetTipFromFilename(spec->ms_fn,tip);
        break;
      }
      default: {
        tip=spec->ms_mc->getText();
        for (FXMenuCaption*cpn=_GetCaption(spec->ms_mc); cpn; cpn=_GetCaption(cpn)) {
          tip.prepend(cpn->getText()+" -> ");
        }
        break;
      }
    }
  } else {
    tip=spec?spec->mnu_txt:FXString::null;
    tip.substitute("&", "",true);
  }
}



// Returns a unique negative ID number for a new user-defined toolbar button.
static FXint GetUniqueID()
{
  FXint unique=0;
  bool exists=true;
  do {
    unique--;
    exists=false;
    for (FXint i=0; i<TBAR_MAX_BTNS; i++) {
      if (custom_commands[i] && (custom_commands[i]->sel==unique)) {
        exists=true;
        break;
      }
    }
  } while (exists);
  return unique;
}



//  Create a new MenuSpec for a toolbar button from a user-defined menu item.
MenuSpec* MenuMgr::AddTBarUsrCmd(FXMenuCommand*mc)
{
  FXint unique=GetUniqueID();
  for (FXint i=0; i<TBAR_MAX_BTNS; i++) {
    if (custom_commands[i]==NULL) {
      MenuSpec*spec=new MenuSpec;
      spec->type='u';
      spec->ms_mc=mc;
      snprintf(spec->pref,sizeof(spec->pref)-1,"Custom_%d",abs(unique));
      SetSpecTBarBtnText(spec);
      spec->sel=GetUniqueID();
      custom_commands[i]=spec;
      return spec;
    }
  }
  return NULL;
}



//  When we read a user-defined toolbar item from the registry, its menu item has not
//  yet been created, so we set up a "transitional" menu spec containing the path to
//  the script file. This information will be used later to create a toolbar button,
//  after the user-defined menus are in place.
MenuSpec* MenuMgr::RegTBarUsrCmd(FXint index, const char*pref, const char*filename)
{
  FXint unique=0;
  for (FXint i=0; i<TBAR_MAX_BTNS; i++) {
    if (custom_commands[i]==NULL) {
      MenuSpec*spec=new MenuSpec;
      spec->type='x';
      spec->ms_fn=strdup(filename);
      sscanf(pref, "Custom_%d",&unique);
      spec->sel = -(unique);
      strncpy(spec->pref,pref,sizeof(spec->pref)-1);
      spec->btn_txt=NULL;
      custom_commands[i]=spec;
      toolbar_buttons[index]=spec->sel;
      return spec;
    }
  }
  return NULL;
}


// The user has decided to remove a toolbar button for a user-defined menu item.
void MenuMgr::RemoveTBarUsrCmd(ToolBarFrame*toolbar, MenuSpec*spec)
{
 if (spec==NULL) { return; }
 for (FXint i=0; i<TBAR_MAX_BTNS; i++) {
    if (custom_commands[i]==spec) {
      custom_commands[i]=NULL;
      // If any toolbar button holds a reference to this spec, make sure we set it to NULL...
      toolbar->NullifyButtonData(spec);
    }
  }
  if (spec->btn_txt) free((char*)spec->btn_txt);
  delete(spec);
}



// Free up memory allocated to the custom_commands[] array when the program exits.
static void FreeTBarUsrCmds() {
  for (FXint i=0; i<TBAR_MAX_BTNS; i++) {
   MenuSpec*spec=custom_commands[i];
   if (spec) {
     custom_commands[i]=NULL;
     if (spec->btn_txt) free((char*)spec->btn_txt);
     delete spec;
   }
  }
}



MenuSpec* MenuMgr::LookupMenu(FXint sel)
{
  if (sel>0) {
    MenuSpec*spec;
    for (spec=menu_specs; spec->sel!=last_id; spec++) {
      if (spec->sel==sel) { return spec; }
    }
  } else {
    for (FXint i=0; i<TBAR_MAX_BTNS; i++) {
      if (custom_commands[i] && (custom_commands[i]->sel==sel)) { return custom_commands[i]; }
    }
  }
  return NULL;
}



MenuSpec* MenuMgr::LookupMenuByPref(const char*pref)
{
  if (pref) {
    MenuSpec*spec;
    for (spec=menu_specs; spec->sel!=last_id; spec++) {
      if (strcmp(spec->pref, pref)==0) { return spec; }
    }
  }
  return NULL;
}



FXint*MenuMgr::TBarBtns() { return toolbar_buttons; }



MenuSpec*MenuMgr::MenuSpecs() { return menu_specs; }



const char* MenuMgr::GetUsrCmdPath(MenuSpec*spec) {
  if (spec) {
    switch (spec->type) {
      case 'x': { return spec->ms_fn; }
      case 'u': { return spec->ms_mc?((const char*)(spec->ms_mc->getUserData())):NULL; }
    }
  }
  return NULL;
}





void MenuMgr::FreePopupCommands()
{
  for (FXint i=0; PopupCommands[i]; i++) {
    free(PopupCommands[i]);
    PopupCommands[i]=NULL;
  }
}



char**MenuMgr::GetPopupCommands()
{
  return PopupCommands;
}


static const char*popup_section=NULL;


void MenuMgr::ReadPopupMenu(FXRegistry*reg, const char* popup_sect) {
  popup_section=popup_sect;
  FreePopupCommands();
  if (reg->existingSection(popup_sect)) {
    for (FXint i=0; i<POPUP_MAX_CMDS; i++) {
      char keyname[32];
      memset(keyname,0, sizeof(keyname));
      snprintf(keyname,sizeof(keyname)-1,"Command_%d", i+1);
      if (reg->existingEntry(popup_sect,keyname)) {
        const char*tmp=reg->readStringEntry(popup_sect,keyname);
        PopupCommands[i]=strdup(tmp?tmp:"");
      }
    }
  } else {
    for (FXint i=0; DefaultPopupCommands[i]; i++) {
      PopupCommands[i]=strdup(DefaultPopupCommands[i]);
    }
  }
}



void MenuMgr::WritePopupMenu(FXRegistry*reg, const char* popup_sect) {
  reg->deleteSection(popup_sect);
  for (FXint i=0; PopupCommands[i]; i++) {
    char keyname[32];
    memset(keyname,0, sizeof(keyname));
    snprintf(keyname,sizeof(keyname)-1,"Command_%d", i+1);
    reg->writeStringEntry(popup_sect,keyname,PopupCommands[i]);
  }
  FreePopupCommands();
}





FXMenuCommand*MenuMgr::MakeMenuCommand(FXComposite*p, FXObject*tgt, FXSelector sel, char type, bool checked)
{
  MenuSpec*spec=MenuMgr::LookupMenu(sel);

  if (spec) {
    FXWindow*own;
    FXAccelTable *table;
    switch(type) {
      case 'm': {
        spec->ms_mc = new FXMenuCommand(p,spec->mnu_txt,NULL,tgt,sel);
        break;
      }
      case 'k': {
        spec->ms_mc = (FXMenuCommand*) new FXMenuCheck(p,spec->mnu_txt,tgt,sel);
        ((FXMenuCheck*)spec->ms_mc)->setCheck(checked);
        break;
      }
      case 'r': {
        spec->ms_mc = (FXMenuCommand*) new FXMenuRadio(p,spec->mnu_txt,tgt,sel);
        break;
      }
      default: {
        fxwarning(_("%s: Warning: unknown menu type: '%c'.\n"), EXE_NAME, type);
        spec->ms_mc = new FXMenuCommand(p,spec->mnu_txt,NULL,tgt,sel);
      }
    }
    FXHotKey acckey=parseAccel(spec->accel);
    if (acckey) {
      spec->ms_mc->setAccelText(spec->accel);
      own=p->getShell()->getOwner();
      if (own) {
        table=own->getAccelTable();
        if (table) {
          if (table->hasAccel(acckey)) {
            fxwarning(_("%s: Warning: action \"%s\" overrides existing accelerator.\n"), EXE_NAME, spec->pref);
          }
          table->addAccel(acckey,tgt,FXSEL(SEL_COMMAND,sel));
        }
      }
    } else {
      if (spec->accel[0]) {
        fxwarning(_("%s: Warning: Failed to parse accelerator for \"%s\"\n"), EXE_NAME, spec->pref);
      }
    }
    spec->type=type;
    return spec->ms_mc;
  } else {
    fxwarning(_("%s: Warning: Could not build menu for selector #%d\n"), EXE_NAME, sel);
    return NULL;
  }
}



void MenuMgr::SetAccelerator(MenuSpec*spec, const FXString &accel)
{
  memset(spec->accel,0,sizeof(spec->accel));
  strncpy(spec->accel,accel.text(),sizeof(spec->accel)-1);
  char*plus=strrchr(spec->accel,'+');
  if (plus && (strlen(plus)==2)) { plus[1]=Ascii::toUpper(plus[1]); }
}



void MenuMgr::ReadMenuSpecs(FXRegistry*reg, const char* keys_sect)
{
  for (MenuSpec*spec=menu_specs; spec->sel!=last_id; spec++) {
   const char*fallback=reg->existingEntry(keys_sect,spec->pref) ? "" : spec->accel;
    FXString acc=reg->readStringEntry(keys_sect,spec->pref,fallback);
#ifdef WIN32
    if (acc.contains("Shift")) {
      acc.substitute(')','0');
      acc.substitute('(','9');
    }
#endif
    SetAccelerator(spec,acc);
  }
}



void MenuMgr::WriteMenuSpecs(FXRegistry*reg, const char* keys_sect)
{
  for (MenuSpec*spec=menu_specs; spec->sel!=last_id; spec++) {
    reg->writeStringEntry(keys_sect,spec->pref,spec->accel);
  }
}



void MenuMgr::ReadToolbarButtons(FXRegistry*reg, const char* tbar_sect)
{
  if (!reg->existingSection(tbar_sect)) { return; }
  for (FXint i=0; i<TBAR_MAX_BTNS; i++) {
    char keyname[32];
    memset(keyname,0, sizeof(keyname));
    snprintf(keyname,sizeof(keyname)-1,"Button_%d", i+1);
    const FXchar *keyval=reg->readStringEntry(tbar_sect,keyname,"");
    MenuSpec*spec=LookupMenuByPref(keyval);
    if (spec) {
      toolbar_buttons[i]=spec->sel;
    } else {
      toolbar_buttons[i]=last_id;
      if ( keyval && (strncmp(keyval,"Custom_",7)==0) && isdigit(keyval[7]) ) {
        const FXchar *filename=reg->readStringEntry(tbar_sect,keyval,NULL);
        if (filename && FXStat::isFile(filename)) {
           RegTBarUsrCmd(i,keyval,filename);
        }
      }
    }
  }
}



void MenuMgr::WriteToolbarButtons(FXRegistry*reg, const char* tbar_section)
{
  for (FXint i=0; i<TBAR_MAX_BTNS; i++) {
    char keyname[32];
    memset(keyname,0, sizeof(keyname));
    snprintf(keyname,sizeof(keyname)-1,"Button_%d", i+1);
    MenuSpec*spec=MenuMgr::LookupMenu(toolbar_buttons[i]);
    if (spec) {
      reg->writeStringEntry(tbar_section,keyname,spec->pref);
      if (spec->type=='u') {
        reg->writeStringEntry(tbar_section,spec->pref,(const char*)(spec->ms_mc->getUserData()));
      }
    } else {
      reg->writeStringEntry(tbar_section,keyname,"");
    }
  }
  FreeTBarUsrCmds();
}



// Update a set of radio buttons
void MenuMgr::RadioUpdate(FXSelector curr, FXSelector min, FXSelector max)
{
  for (FXSelector i=min; i<=max; i++) {
    MenuSpec*spec=MenuMgr::LookupMenu(i);
    if (spec && spec->ms_mc) {
      ((FXMenuRadio*)(spec->ms_mc))->setCheck(curr==i);
      FXButton*btn=(FXButton*)spec->ms_mc->getUserData();
      if (btn) {
        if (curr==i) {
          btn->setFrameStyle(btn->getFrameStyle()|FRAME_THICK|FRAME_RAISED);
          btn->setState(btn->getState()|STATE_ENGAGED);
        } else {
          btn->setFrameStyle(FRAME_NONE);
          btn->setState(btn->getState()&~STATE_ENGAGED);
        }
      }
    }
  }
}


FXint MenuMgr::LastID()
{
  return last_id;
}

