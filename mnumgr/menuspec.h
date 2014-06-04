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

#ifndef FXITE_MENUSPEC_H
#define FXITE_MENUSPEC_H


typedef struct _MenuSpec {
  FXint sel;
  char pref[32];
  const char* mnu_txt;
  const char* btn_txt;
  char accel[32];
  char type;
  union {
    FXMenuCommand*ms_mc;
    char *ms_fn;
  };
} MenuSpec;



#define TBAR_MAX_BTNS 24
#define POPUP_MAX_CMDS 24


class UsrMnuCmd;
class ToolBarFrame;

class MenuMgr {
private:
  friend class UsrMnuCmd;
  static void ValidateUsrTBarCmd(FXMenuCommand*mc);
  static void InvalidateUsrTBarCmd(FXMenuCommand*mc);
  void RemoveToolbarButton(FXint index);
  MenuSpec* RegTBarUsrCmd(FXint index, const char*pref, const char*filename);
  MenuSpec*menu_specs;
  FXint* toolbar_buttons;
  FXint last_id;
  const char** DefaultPopupCommands;
  char** PopupCommands;
  FXString config_dir;
public:
  MenuMgr(MenuSpec*specs, FXint* tbar_btns, FXint id_last, const char** def_pop_cmnds, char** pop_cmnds, const char*cfg_dir);
  FXMenuCommand*MakeMenuCommand(FXComposite*p, FXObject*tgt, FXSelector sel, char type, bool checked=false);
  MenuSpec*MenuSpecs();
  FXint*TBarBtns();
  MenuSpec*LookupMenu(FXint sel);
  MenuSpec*LookupMenuByPref(const char*pref);
  static const char*TBarColors(FXint i);
  static const char*GetUsrCmdPath(MenuSpec*spec);
  static void SetAccelerator(MenuSpec*spec, const FXString &accel);
  void GetTBarBtnTip(MenuSpec*spec, FXString &tip);
  void GetTipFromFilename(const char*filename, FXString &tip);
  static MenuSpec*AddTBarUsrCmd(FXMenuCommand*mc);
  static void RemoveTBarUsrCmd(ToolBarFrame*toolbar, MenuSpec*spec);
  void PurgeTBarCmds(ToolBarFrame*toolbar);
  void WriteToolbarButtons(FXRegistry*reg, const char*tbar_section);
  void ReadToolbarButtons(FXRegistry*reg, const char*tbar_section);
  void ReadMenuSpecs(FXRegistry*reg, const char* keys_sect);
  void WriteMenuSpecs(FXRegistry*reg, const char* keys_sect);
  void ReadPopupMenu(FXRegistry*reg, const char* popup_sect);
  void WritePopupMenu(FXRegistry*reg, const char* popup_sect);
  char**GetPopupCommands();
  void FreePopupCommands();
  void RadioUpdate(FXSelector curr, FXSelector min, FXSelector max);
  FXint LastID();
};

#endif

