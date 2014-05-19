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

#ifndef FXITE_APPWIN_BASE_H
#define FXITE_APPWIN_BASE_H


#include "histbox.h"

class SciDoc;
class DocTab;
class DocTabs;
class SearchDialogs;
class FileDialogs;
class SettingsBase;
class Settings;
class PrefsDialog;
class UserMenu;
class MacroRunner;
class MacroRecorder;
class BackupMgr;
class ToolsDialog;
class ToolBarFrame;
class OutputList;
class StatusBar;
class MainMenu;
class AutoCompleter;
class CommandUtils;

class TopWindowBase: public MainWinWithClipBrd {
  FXDECLARE(TopWindowBase)
protected:
  TopWindowBase(){}
  SciDoc* recording;
  FXToolTip* tips;
  FXVerticalFrame* outerbox;
  FXVerticalFrame* innerbox;
  MainMenu* menubar;
  StatusBar* statusbar;
  DocTabs* tabbook;
  FXSplitter* hsplit;
  OutputList* outlist;
  ToolBarFrame* toolbar;
  SearchDialogs* srchdlgs;
  FileDialogs* filedlgs;
  AutoCompleter* completions;
  SettingsBase* prefs;
  MacroRecorder* recorder;
  MacroRunner* macros;
  BackupMgr* backups;
  CommandUtils* cmdutils;
  FXint stale_ticks;
  FXint save_ticks;
  FXString session_data;
  FXID active_widget;
  bool destroying;
  bool close_all_confirmed;
  bool kill_commands_confirmed;
  bool command_timeout;
  FXint need_status;
  FXString save_hook;
  FXString bookmarked_file;
  DocTab* bookmarked_tab;
  long bookmarked_pos;
  bool skipfocus;
  void Closing(bool is_closing);
  void SetTabDirty(SciDoc* sci, bool dirty);
  void SetTabLocked(SciDoc* sci, bool locked);
  void RescanUserMenu();
  void ClosedDialog();
  bool ShowSaveAsDlg(SciDoc* sci);
  void ShowInsertFileDlg();
  void ShowFilterDialog(bool is_filter);
  void ShowCommandDialog();
  void ShowToolManagerDialog();
  void ShowPrefsDialog();
  void ToggleRecorder();
  bool FilterSelection(SciDoc* sci, const FXString &cmd, const FXString &input);
  bool RunCommand(SciDoc* sci, const FXString &cmd);
  bool RunMacro(const FXString &script, bool isfilename);
  bool RunHookScript(const char* hookname);
  void FileSaved(SciDoc* saved);
  void AskReload();
  bool SaveAll(bool break_on_fail);
  bool CloseAll(bool close_last);
  const FXString &SessionFile();
  bool SetLanguage(FXMenuRadio* mnu);
  void SetBookmark();
  void EnableUserFilters(bool enabled);
  void OpenSelected();
  bool NewFile(bool hooked);
  void ChangeCase(bool to_upper);
  void FindTag();
  void GoToBookmark();
  void UpdateToolbar();
  void UpdateTitle(long line, long col);
  void InvertColors(bool inverted);
public:
  TopWindowBase(FXApp* a);
  virtual ~TopWindowBase();
  static TopWindowBase* instance();
  virtual void create();
  virtual FXbool close(FXbool notify=false);
  void ParseCommands(FXString &commands);
  bool Closing();
  bool IsMacroCancelled();
  static const FXString& ConfigDir();
  static const FXString& Connector();
  Settings* Prefs() { return (Settings*)prefs; }
  SciDoc* ControlDoc();
  SciDoc* FocusedDoc();
  bool OpenFile(const char* caption, const char* rowcol, bool readonly, bool hooked);
  bool CloseFile(bool close_last, bool hooked);
  int IsFileOpen(const FXString &filename, bool activate);
  FXString* NamedFiles() const;
  void Cut();
  void Copy();
  void Paste();
  DocTabs* Tabs() {return tabbook; }
  FileDialogs* FileDlgs() { return filedlgs; }
  FXMenuCaption* TagFiles();
  void AddFileToTagsMenu(const FXString &filename);
  bool RemoveFileFromTagsMenu(const FXString &filename);
  bool SetLanguage(const FXString &name);
  void SetKillCommandAccelKey(FXHotKey acckey);
  void RemoveTBarBtnData(void* p);
  bool SetReadOnly(SciDoc* sci, bool rdonly);
  void AddOutput(const FXString&line);
  void ClearOutput();
  UserMenu** UserMenus() const;
  void FindText(const char* searchstring, FXuint searchmode, bool forward);
  void FindAndReplace(const char*searchfor, const char*replacewith, FXuint searchmode, bool forward);
  void ReplaceAllInSelection(const char*searchfor, const char*replacewith, FXuint searchmode);
  void ReplaceAllInDocument(const char*searchfor, const char*replacewith, FXuint searchmode);
  void AdjustIndent(SciDoc* sci,char ch);
  bool FoundBookmarkedTab(DocTab* tab);
  bool Destroying() { return destroying; }
  void ActiveWidget(FXID aw) { active_widget=aw; }
  void SetWordWrap(SciDoc* sci, bool wrapped);
  void ShowLineNumbers(bool showit);
  void ShowStatusBar(bool showit);
  void ShowOutputPane(bool showit);
  void ShowWhiteSpace(bool showit);
  void ShowToolbar(bool showit);
  void ShowMargin(bool showit);
  void ShowIndent(bool showit);
  void ShowCaretLine(bool showit);
  long onTimer(     FXObject* o, FXSelector sel, void* p );
  long onCloseWait( FXObject* o, FXSelector sel, void* p );
  long CheckStale(  FXObject* o, FXSelector sel, void* p );
  long CheckStyle(  FXObject* o, FXSelector sel, void* p );
  long onFocusIn(   FXObject* o, FXSelector sel, void* p );
  long onFocusDoc(  FXObject* o, FXSelector sel, void* p );
  long onFocusSci(  FXObject* o, FXSelector sel, void* p );
  enum {
    ID_TIMER = MainWinWithClipBrd::ID_LAST,
    ID_CLOSEWAIT,
    ID_CHECK_STALE,
    ID_CHECK_STYLE,
    ID_FOCUS_DOC,
    ID_SCINTILLA,
    ID_LAST
  };
};

#endif

