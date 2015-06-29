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

#ifndef FXITE_MAINMENU_H
#define FXITE_MAINMENU_H


class TopWindow;
class Settings;
class RecentFilesMenu;
class UserMenu;
class MenuMgr;

class MainMenu: public FXMenuBar {
private:
  TopWindow* tw;
  SettingsBase* prefs;
  void CreateMenus();
  void CreateLanguageMenu();
  void CreateTabsMenu();
  void CreateZoomMenu();

  FXMenuPane* filemenu;
  FXMenuPane* editmenu;
  FXMenuPane* searchmenu;
  FXMenuPane* viewmenu;
  FXMenuPane* toolsmenu;
  FXMenuPane* docmenu;
  FXMenuPane* helpmenu;

  FXMenuPane* recordermenu;
  FXMenuCheck* recorderstartmenu;
  FXMenuCommand* playbackmenu;
  FXMenuCommand* showmacromenu;
  FXMenuCommand* filterselmenu;
  FXMenuCommand* openselmenu;


  FXMenuPane* editdeletemenu;
  FXMenuPane* editindentmenu;
  FXMenuPane* viewzoommenu;
  FXMenuPane* searchselectmenu;
  FXMenuPane* searchmarkmenu;
  FXMenuPane* tabmenu;
  FXMenuPane* tabordermenu;
  FXMenuPane* tabsidemenu;
  FXMenuPane* tabwidthmenu;
  FXMenuPane* fileexportmenu;
  FXMenuPane* fileformatmenu;
  FXMenuPane* langmenu;
  FXMenuCascade* langcasc;
  FXMenuPane* cpp_langmenu;
  FXMenuCascade* cpp_langcasc;
  FXMenuPane* html_langmenu;
  FXMenuCascade* html_langcasc;
  FXMenuPane* lgcy_langmenu;
  FXMenuCascade* lgcy_langcasc;
  FXMenuPane* scr_langmenu;
  FXMenuCascade* scr_langcasc;
  FXMenuPane* db_langmenu;
  FXMenuCascade* db_langcasc;
  FXMenuPane* cfg_langmenu;
  FXMenuCascade* cfg_langcasc;
  FXMenuPane* inf_langmenu;
  FXMenuCascade* inf_langcasc;
  FXMenuPane* tex_langmenu;
  FXMenuCascade* tex_langcasc;
  FXMenuPane* asm_langmenu;
  FXMenuCascade* asm_langcasc;
  FXMenuPane* misc_langmenu;
  FXMenuCascade* misc_langcasc;

  FXMenuCascade* fileformatcasc;
  FXMenuRadio* fmt_dos_mnu;
  FXMenuRadio* fmt_mac_mnu;
  FXMenuRadio* fmt_unx_mnu;

  FXMenuCascade* unloadtagsmenu;
  FXMenuCommand* autocompmenu;
  FXMenuCommand* findtagmenu;
  FXMenuCommand* showtipmenu;

  FXScrollPane* doclistmenu;
  RecentFilesMenu* recent_files;

  UserMenu* usercmdmenu;
  UserMenu* userfiltermenu;
  UserMenu* usermacromenu;
  UserMenu* usersnipmenu;

  FXMenuCheck* readonly_chk;
  FXMenuCheck* caretline_chk;
  FXMenuCheck* guides_chk;
  FXMenuCheck* linenums_chk;
  FXMenuCheck* margin_chk;
  FXMenuCheck* outpane_chk;
  FXMenuCheck* status_chk;
  FXMenuCheck* toolbar_chk;
  FXMenuCheck* white_chk;
  FXMenuCheck* wordwrap_chk;
  FXMenuCheck* invert_chk;
  MenuMgr*mnumgr;
  FXRegistry*reg;
  static void SetMenuEnabled(FXMenuCommand*mnu, bool enabled);
  void EnableTagMenus(bool enabled);

public:
  MainMenu(FXComposite* p, MenuMgr* mmgr, FXRegistry*r);
  ~MainMenu();
  void Recording(bool recording, bool recorded);
  void RescanUserMenus();
  void SyncPrefsCheckmarks();
  void SetWordWrapCheckmark(bool wrapped);
  void SetReadOnlyCheckmark(bool rdonly);
  void SetLanguageCheckmark(LangStyle*ls);
  FXMenuRadio*GetMenuForLanguage(const FXString &name) const;
  void UnloadTagFile(FXMenuCommand*mc);
  void AddFileToTagsList(const FXString &filename);
  bool RemoveFileFromTagsList(const FXString &filename);
  void AppendDocList(const FXString &filename, FXTabItem*tab);
  void PrependRecentFile(const FXString &filename);
  void RemoveRecentFile(const FXString &filename);
  void EnableFileFormats();
  void EnableFilterMenu(bool enabled);
  void UpdateDocTabSettings();
  UserMenu**UserMenus() const;
  FXMenuCascade*TagsMenu() const;
};


#endif

