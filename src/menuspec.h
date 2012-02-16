/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2009 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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



class MenuMgr {
private:
  friend class UsrMnuCmd;
  static void ValidateUsrTBarCmd(FXMenuCommand*mc);
  static void InvalidateUsrTBarCmd(FXMenuCommand*mc);
public:
  static FXMenuCommand*MakeMenuCommand(FXComposite*p, FXObject*tgt, FXSelector sel, char type, bool checked=false);
  static void ShowPopupMenu(FXPoint*pt);
  static MenuSpec*MenuSpecs();
  static FXint*TBarBtns();
  static MenuSpec*LookupMenu(FXint sel);
  static MenuSpec*LookupMenuByPref(const char*pref);
  static const char*TBarColors(FXint i);
  static const char*GetUsrCmdPath(MenuSpec*spec);
  static void GetTBarBtnTip(MenuSpec*spec, FXString &tip);
  static void GetTipFromFilename(const char*filename, FXString &tip);
  static MenuSpec*AddTBarUsrCmd(FXMenuCommand*mc);
  static void RemoveTBarUsrCmd(MenuSpec*spec);
  static void PurgeTBarCmds();
  static void WriteToolbarButtons(FXRegistry*reg, const char*tbar_section);
  static void ReadToolbarButtons(FXRegistry*reg, const char*tbar_section);
  static void ReadMenuSpecs(FXRegistry*reg, const char* keys_sect);
  static void WriteMenuSpecs(FXRegistry*reg, const char* keys_sect);
  static void ReadPopupMenu(FXRegistry*reg, const char* popup_sect);
  static void WritePopupMenu(FXRegistry*reg, const char* popup_sect);
  static char**GetPopupCommands();
  static void FreePopupCommands();
};

