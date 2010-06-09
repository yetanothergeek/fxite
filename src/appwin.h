/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2009-2010 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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


//class AppClass;
class SciDoc;
class DocTab;
class DocTabs;
class SearchDialogs;
class FileDialogs;
class Settings;
class PrefsDialog;
class UserMenu;
class MacroRunner;
class MacroRecorder;
class HistMenu;
class BackupMgr;
class ToolsDialog;
class TopMenuPane;


class TopWindow: public FXMainWindow {
private:
  FXDECLARE(TopWindow)
  TopWindow(){}
  static void TranslatorCB(const char*text, void*user_data);
  static bool ZoomStepCB(FXint index, DocTab*tab, FXWindow*page, void*user_data);
  static bool ZoomSpecCB(FXint index, DocTab*tab, FXWindow*page, void*user_data);
  static bool LineNumsCB(FXint index, DocTab*tab, FXWindow*page, void*user_data);
  static bool WhiteSpaceCB(FXint index, DocTab*tab, FXWindow*page, void*user_data);
  static bool ShowMarginCB(FXint index, DocTab*tab, FXWindow*page, void*user_data);
  static bool ShowIndentCB(FXint index, DocTab*tab, FXWindow*page, void*user_data);
  static bool FileAlreadyOpenCB(FXint index, DocTab*tab, FXWindow*page, void*user_data);
  static bool ResetUndoLevelCB(FXint index, DocTab*tab, FXWindow*page, void*user_data);
  static bool PrefsCB(FXint index, DocTab*tab, FXWindow*page, void*user_data);
  static bool AutoSaveCB(FXint index, DocTab*tab, FXWindow*page, void*user_data);
  static bool BookmarkCB(FXint index, DocTab*tab, FXWindow*page, void*user_data);
  static void SetSciDocPrefs(SciDoc*sci, Settings*prefs);
  static bool FileStillOpenCB(FXint index, DocTab*tab, FXWindow*page, void*user_data);
  static bool SetGlobalClipboardCB(FXint index, DocTab*tab, FXWindow*page, void*user_data);
  static bool StyleNextDocCB(FXint index, DocTab*tab, FXWindow*page, void*user_data);
  static void EnableFilterBtnCB(FXButton*btn, void*user_data);
  static void SetTBarBtnFontCB(FXButton*btn, void*user_data);
  static void ClearTBarBtnDataCB(FXButton*btn, void*user_data);
  void UpdateToolbarWrap();
  bool IsDocValid(SciDoc*sci);
  void SetTabTag(SciDoc*sci, char mark, bool set);
  void Freeze(FXWindow*win, bool frozen);
  void ClosedDialog();
  static const char* DontFreezeMe();
  bool CheckKillCommand(FXWindow*w, FXAccelTable*tbl);
  void RadioUpdate(FXSelector curr, FXSelector min, FXSelector max);
  void SyncToggleBtn(FXObject*o, FXSelector sel);
  void UpdateEolMenu(SciDoc*sci);
  static void SetMenuEnabled(FXMenuCommand*mnu, bool enabled);
  SciDoc*clipsci;
  SciDoc*recording;
  FXMenuBar*  menubar;
  TopMenuPane* filemenu;
  TopMenuPane* editmenu;
  TopMenuPane* searchmenu;
  TopMenuPane* viewmenu;
  TopMenuPane* toolsmenu;
  TopMenuPane* docmenu;
  TopMenuPane* helpmenu;
  FXMenuPane* editindentmenu;
  FXMenuCheck* linenums_chk;
  FXMenuCheck* toolbar_chk;
  FXMenuCheck* white_chk;
  FXMenuCheck* margin_chk;
  FXMenuCheck* guides_chk;
  FXMenuCheck* invert_chk;
  FXMenuCheck* status_chk;
  FXMenuPane* viewzoommenu;
  FXMenuPane* searchselectmenu;
  FXMenuPane* searchmarkmenu;
  FXMenuPane* tabmenu;
  FXMenuPane* tabordermenu;
  FXMenuPane* tabsidemenu;
  FXMenuPane* tabwidthmenu;
  FXMenuPane* fileexportmenu;
  FXMenuPane* fileformatmenu;
  FXMenuCascade* fileformatcasc;

  FXMenuRadio*fmt_dos_mnu;
  FXMenuRadio*fmt_mac_mnu;
  FXMenuRadio*fmt_unx_mnu;

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
  FXScrollPane* doclistmenu;
  FXMenuCascade* unloadtagsmenu;
  FXMenuCommand* findtagmenu;
  FXMenuCommand* showtipmenu;
  FXMenuCommand* filterselmenu;
  FXMenuCommand* openselmenu;
  FXMenuPane*recordermenu;
  FXMenuCommand *recorderstartmenu;
  FXMenuCommand* playbackmenu;
  FXMenuCommand* showmacromenu;
  FXMenuCheck* readonlymenu;
  FXVerticalFrame* vbox;
  FXHorizontalFrame* statusbar;
  FXTextField* coords;
  FXTextField* docname;
  FXTextField* encname;
  FXLabel* general_info;
  DocTabs*    tabbook;
  FXGroupBox* outputpane;
  FXSplitter* hsplit;
  FXList*outlist;
  FXPacker*toolbar_frm;
  FXFont*toolbar_font;
  FXMenuCheck*outpane_chk;
  FXMenuPane *outpop;
  UserMenu* usercmdmenu;
  UserMenu* usersnipmenu;
  UserMenu* userfiltermenu;
  UserMenu* usermacromenu;
  SearchDialogs* srchdlgs;
  FileDialogs*   filedlgs;
  Settings* prefs;
  PrefsDialog *prefdlg;
  ToolsDialog *tooldlg;
  MacroRunner* macros;
  MacroRecorder* recorder;
  BackupMgr* backups;
  FXint StaleTicks;
  FXint SaveTicks;
  FXAccelTable*saved_accels;
  FXAccelTable*temp_accels;
  FXString session_data;
  HistMenu*recent_files;
  FXWindow* active_widget;
  bool destroying;
  bool close_all_confirmed;
  bool kill_commands_confirmed;
  bool command_timeout;
  FXint need_status;
  void CreateToolbar();
  void CreateMenus();
  void DeleteMenus();
  void CreateLanguageMenu();
  void CreateTabsMenu();
  void CreateZoomMenu();
  void UpdateToolbar();
  void UpdateTitle(long line, long col);
  bool AskReload(SciDoc*sci);
  bool FilterSelection(SciDoc *sci, const FXString &cmd, const FXString &input);
  bool RunCommand(SciDoc *sci, const FXString &cmd);
  bool InsertFile(SciDoc *sci, const FXString &filename);
  bool RunMacro(const FXString &script, bool isfilename);
  bool RunHookScript(const char*hookname);
  void SetTabDirty(SciDoc*sci, bool dirty);
  void SetTabLocked(SciDoc*sci, bool locked);
  bool SaveAll(bool break_on_fail);
  void SetInfo(const char*msg, bool hide_docname=false);
  void DisableUI(bool disabled);
  void SetSrchDlgsPrefs();
  void DoneSci(SciDoc*sci);
  void EnableUserFilters(bool enabled);
  void CharAdded(SciDoc*sci, long line, long pos, int ch);
  FXString save_hook;
  FXHotKey killkey;
  FXString bookmarked_file;
  DocTab* bookmarked_tab;
  long bookmarked_pos;
  bool skipfocus;
public:
  long CheckStale(       FXObject*o, FXSelector sel, void*p );
  long CheckStyle(       FXObject*o, FXSelector sel, void*p );
  long onTabOrient(      FXObject*o, FXSelector sel, void*p );
  long onPackTabWidth(   FXObject*o, FXSelector sel, void*p );
  long onSwitchTabs(     FXObject*o, FXSelector sel, void*p );
  long onNextTab(        FXObject*o, FXSelector sel, void*p );
  long onTabActivate(    FXObject*o, FXSelector sel, void*p );
  long onMoveTab(        FXObject*o, FXSelector sel, void*p );
  long onFileOpen(       FXObject*o, FXSelector sel, void*p );
  long onFileSave(       FXObject*o, FXSelector sel, void*p );
  long onFileSaveAs(     FXObject*o, FXSelector sel, void*p );
  long onFileSaveAll(    FXObject*o, FXSelector sel, void*p );
  long onFileSaveACopy(  FXObject*o, FXSelector sel, void*p );
  long onFileNew(        FXObject*o, FXSelector sel, void*p );
  long onQuit(           FXObject*o, FXSelector sel, void*p );
  long onScintilla(      FXObject*o, FXSelector sel, void*p );
  long onCloseTab(       FXObject*o, FXSelector sel, void*p );
  long onCloseAll(       FXObject*o, FXSelector sel, void*p );
  long onUndo(           FXObject*o, FXSelector sel, void*p );
  long onRedo(           FXObject*o, FXSelector sel, void*p );
  long onCut(            FXObject*o, FXSelector sel, void*p );
  long onCopy(           FXObject*o, FXSelector sel, void*p );
  long onPaste(          FXObject*o, FXSelector sel, void*p );
  long onFind(           FXObject*o, FXSelector sel, void*p );
  long onFindNext(       FXObject*o, FXSelector sel, void*p );
  long onFindPrev(       FXObject*o, FXSelector sel, void*p );
  long onReplace(        FXObject*o, FXSelector sel, void*p );
  long onGoto(           FXObject*o, FXSelector sel, void*p );
  long onGotoSelected(   FXObject*o, FXSelector sel, void*p );
  long onFindSelected(   FXObject*o, FXSelector sel, void*p );
  long onZoom(           FXObject*o, FXSelector sel, void*p );
  long onShowStatusBar(  FXObject*o, FXSelector sel, void*p );
  long onShowMargin(     FXObject*o, FXSelector sel, void*p );
  long onShowIndent(     FXObject*o, FXSelector sel, void*p );
  long onShowLineNums(   FXObject*o, FXSelector sel, void*p );
  long onShowToolbar(    FXObject*o, FXSelector sel, void*p );
  long onShowWhiteSpace( FXObject*o, FXSelector sel, void*p );
  long onShowOutputPane( FXObject*o, FXSelector sel, void*p );
  long onInvertColors(   FXObject*o, FXSelector sel, void*p );
  long onPrefsDialog(    FXObject*o, FXSelector sel, void*p );
  long onCtrlTab(        FXObject*o, FXSelector sel, void*p );
  long onIndent(         FXObject*o, FXSelector sel, void*p );
  long onFilterSel(      FXObject*o, FXSelector sel, void*p );
  long onKillCommand(    FXObject*o, FXSelector sel, void*p );
  long onRunCommand(     FXObject*o, FXSelector sel, void*p );
  long onOutlistClick(   FXObject*o, FXSelector sel, void*p );
  long onGoToError(      FXObject*o, FXSelector sel, void*p );
  long onBookmark(       FXObject*o, FXSelector sel, void*p );
  long onSplitChanged(   FXObject*o, FXSelector sel, void*p );
  long onOutlistFocus(   FXObject*o, FXSelector sel, void*p );
  long onOutlistPopup(   FXObject*o, FXSelector sel, void*p );
  long onChangeCase(     FXObject*o, FXSelector sel, void*p );
  long onSetLanguage(    FXObject*o, FXSelector sel, void*p );
  long onReload(         FXObject*o, FXSelector sel, void*p );
  long onTimer(          FXObject*o, FXSelector sel, void*p );
  long onReadOnly(       FXObject*o, FXSelector sel, void*p );
  long onLoadTags(       FXObject*o, FXSelector sel, void*p );
  long onUnloadTags(     FXObject*o, FXSelector sel, void*p );
  long onFindTag(        FXObject*o, FXSelector sel, void*p );
  long onShowCallTip(    FXObject*o, FXSelector sel, void*p );
  long onUserCmd(        FXObject*o, FXSelector sel, void*p );
  long onRescanUserMenu( FXObject*o, FXSelector sel, void*p );
  long onConfigureTools( FXObject*o, FXSelector sel, void*p );
  long onInsertFile(     FXObject*o, FXSelector sel, void*p );
  long onCloseWait(      FXObject*o, FXSelector sel, void*p );
  long onMacroRecord(    FXObject*o, FXSelector sel, void*p );
  long onMacroPlayback(  FXObject*o, FXSelector sel, void*p );
  long onMacroShow(      FXObject*o, FXSelector sel, void*p );
  long onHelpAbout(      FXObject*o, FXSelector sel, void*p );
  long onFileSaved(      FXObject*o, FXSelector sel, void*p );
  long onOpenPrevious(   FXObject*o, FXSelector sel, void*p );
  long onOpenSelected(   FXObject*o, FXSelector sel, void*p );
  long onCycleSplitter(  FXObject*o, FXSelector sel, void*p );
  long onClearOutput(    FXObject*o, FXSelector sel, void*p );
  long onFileFormat(     FXObject*o, FXSelector sel, void*p );
  long onFileExport(     FXObject*o, FXSelector sel, void*p );
  long onShowHelp(       FXObject*o, FXSelector sel, void*p );
  long onTBarCustomCmd(  FXObject*o, FXSelector sel, void*p );
  long onFocusIn(        FXObject*o, FXSelector sel, void*p );
  long onFocusDoc(       FXObject*o, FXSelector sel, void*p );
  long onTestSomething(  FXObject*o, FXSelector sel, void*p );
  enum {
    ID_TABS_TOP=FXMainWindow::ID_LAST,
    ID_TABS_BOTTOM,
    ID_TABS_LEFT,
    ID_TABS_RIGHT,
    ID_TABS_UNIFORM,
    ID_TABS_COMPACT,
    ID_TAB_NEXT,
    ID_TAB_PREV,
    ID_TAB_SWITCHED,
    ID_TAB_TOFIRST,
    ID_TAB_TOLAST,
    ID_TAB_UP,
    ID_TAB_DOWN,
    ID_TAB_ACTIVATE,
    ID_SCINTILLA,
    ID_OPEN_FILE,
    ID_OPEN_FILES,
    ID_OPEN_PREVIOUS,
    ID_OPEN_SELECTED,
    ID_CLOSE,
    ID_CLOSEALL,
    ID_NEW,
    ID_SAVE,
    ID_SAVEAS,
    ID_SAVEALL,
    ID_SAVECOPY,
    ID_QUIT,
    ID_UNDO,
    ID_REDO,
    ID_CUT,
    ID_COPY,
    ID_PASTE,
    ID_SHOW_STATUSBAR,
    ID_FIND,
    ID_FINDNEXT,
    ID_FINDPREV,
    ID_REPLACE_IN_DOC,
    ID_GOTO,
    ID_NEXT_SELECTED,
    ID_PREV_SELECTED,
    ID_GOTO_SELECTED,
    ID_GOTO_ERROR,
    ID_BOOKMARK_SET,
    ID_BOOKMARK_RETURN,
    ID_ZOOM_IN,
    ID_ZOOM_OUT,
    ID_ZOOM_FAR,
    ID_ZOOM_NEAR,
    ID_ZOOM_NONE,
    ID_SHOW_LINENUMS,
    ID_SHOW_TOOLBAR,
    ID_SHOW_WHITESPACE,
    ID_SHOW_OUTLIST,
    ID_INVERT_COLORS,
    ID_SHOW_MARGIN,
    ID_SHOW_INDENT,
    ID_PREFS_DIALOG,
    ID_CTRL_TAB,
    ID_INDENT_STEP,
    ID_INDENT_FULL,
    ID_UNINDENT_STEP,
    ID_UNINDENT_FULL,
    ID_KILL_COMMAND,
    ID_FILTER_SEL,
    ID_INSERT_CMD_OUT,
    ID_RUN_COMMAND,
    ID_OUTLIST_CLICK,
    ID_SPLIT_CHANGED,
    ID_FOCUS_OUTLIST,
    ID_OUTLIST_ASEL,
    ID_OUTLIST_COPY,
    ID_TOUPPER,
    ID_TOLOWER,
    ID_SET_LANGUAGE,
    ID_RELOAD,
    ID_TIMER,
    ID_READONLY,
    ID_INSERT_FILE,
    ID_LOAD_TAGS,
    ID_UNLOAD_TAGS,
    ID_FIND_TAG,
    ID_SHOW_CALLTIP,
    ID_USER_COMMAND,
    ID_USER_FILTER,
    ID_USER_SNIPPET,
    ID_USER_MACRO,
    ID_RESCAN_USER_MENU,
    ID_CONFIGURE_TOOLS,
    ID_MACRO_RECORD,
    ID_MACRO_PLAYBACK,
    ID_MACRO_TRANSLATE,
    ID_CLOSEWAIT,
    ID_HELP_ABOUT,
    ID_FILE_SAVED,
    ID_CYCLE_SPLITTER,
    ID_CLEAR_OUTPUT,
    ID_EXPORT_PDF,
    ID_EXPORT_HTML,
    ID_SHOW_HELP,
    ID_SHOW_LUA_HELP,
    ID_CHECK_STALE,
    ID_CHECK_STYLE,
    ID_FMT_DOS,
    ID_FMT_MAC,
    ID_FMT_UNIX,
    ID_TBAR_CUSTOM_CMD,
    ID_FOCUS_DOC,
    ID_TEST_SOMETHING,
    ID_LAST
  };
  TopWindow(FXApp* a);
  virtual ~TopWindow();
  virtual void create();
  virtual FXbool close(FXbool notify=FALSE);

  FXApp* getApp() const { return (FXApp*)FXMainWindow::getApp(); }
  bool OpenFile(const char*caption, const char*rowcol, bool readonly, bool hooked);
  bool NewFile(bool hooked);
  void ParseCommands(const FXString &commands);
  bool CloseFile(bool close_last, bool hooked);
  bool CloseAll(bool close_last);
  bool IsMacroCancelled();
  bool Destroying() { return destroying; }
  bool Closing();
  bool SetReadOnly(SciDoc*sci, bool rdonly);
  SciDoc*ControlDoc();
  SciDoc*FocusedDoc();
  DocTabs* Tabs() {return tabbook; }
  FileDialogs* FileDlgs() { return filedlgs; }
  bool SetLanguage(const FXString &name);
  void ShowLineNumbers(bool showit);
  bool ShowLineNumbers();
  void ShowStatusBar(bool showit);
  bool ShowStatusBar();
  void ShowOutputPane(bool showit);
  bool ShowOutputPane();
  void ShowWhiteSpace(bool showit);
  bool ShowWhiteSpace();
  void ShowToolbar(bool showit);
  bool ShowToolbar();
  void AddFileToTagsMenu(const FXString &filename);
  bool RemoveFileFromTagsMenu(const FXString &filename);
  Settings* GetPrefs() { return prefs; }
  void SetKillCommandAccelKey(FXHotKey acckey);
  FXHotKey GetKillCommandAccelKey() { return killkey; }
  static void DumpLexers();
  void AddOutput(const FXString&line) {
    outlist->appendItem(line,NULL,NULL,true);
    outlist->makeItemVisible(outlist->getNumItems()-1);
  };
  void ClearOutput() { outlist->clearItems(); };
  void SaveClipboard();
  UserMenu**UserMenus() const;
  void About();
  static void VersionInfo();
  void ForEachToolbarButton(void (*cb)(FXButton*btn, void*user_data), void*user_data);
  bool FindText(const char*searchstring, FXuint searchmode, bool forward);
  FXID GetActiveWindow();
};

