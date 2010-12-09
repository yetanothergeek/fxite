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


/*
Steps to adding a new preference:
  1. Add the variable to the public interface of the Settings class.
  2. Add a corresponding ID_* entry to the class's selectors enumeration.
  3. Be sure the ID_* is picked up by the SettingsMap macros in prefs.cpp
  4. Add a case to the switch statement in Settings::onChangeSetting to catch the new selector.
  5. Add a call in Settings::Settings to read your setting from the registry (or default value).
  6. Add a call in Settings::~Settings to save your setting to the registry.
  7. Add the variable name (as a string) to the appropriate *_keys[] array in prefs.cpp.
  8. Add an adjustment widget to one of the tabs in PrefsDialog::Make*Tab() and set its target and selector, or...
  9. In lieu of steps 2,3,4,8 you could add an item to the View menu and control the preference from there.
*/




typedef struct _StyleDef StyleDef;

typedef enum { 
  AUTO_INDENT_NONE=0,
  AUTO_INDENT_BASIC,
  AUTO_INDENT_SMART
} AutoIndentPolicy;

typedef enum { 
  REMEMBER_NEVER=0,
  REMEMBER_SESSION,
  REMEMBER_ALWAYS
} FileFilterPolicy;


class Settings: public FXObject {
  FXDECLARE(Settings);
  static FXbool loaded;
  bool packtabs;
  FXuchar tabside;
  FXApp*app;
  FXRegistry*reg;
  FXSettings*style_reg;
  FXString style_file;
  StyleDef *styles;
  Settings() {}
public:
  Settings(FXMainWindow*w);
  ~Settings();

  FXulong ShowStatusBar:1;
  FXulong ShowLineNumbers:1;
  FXulong ShowToolbar:1;
  FXulong ShowWhiteSpace:1;

  FXulong ShowOutputPane:1;
  FXulong InvertColors:1;
  FXulong ShowRightEdge:1;
  FXulong ShowIndentGuides:1;

  FXulong DocTabsPacked:1;
  FXulong Maximize:1;
  FXulong SmartHome:1;

  FXulong BraceMatch:1;
  FXulong UseTabs:1;
  FXulong CaretPastEOL:1;
  FXulong WhitespaceShowsEOL:1;

  FXulong PromptCloseMultiMenu:1;
  FXulong PromptCloseMultiExit:1;
  FXulong WatchExternChanges:1;
  FXulong Autosave:1;

  FXulong SaveBeforeFilterSel:1;
  FXulong SaveBeforeInsCmd:1;
  FXulong SaveBeforeExecCmd:1;
  FXulong WrapToolbar:1;

  FXulong DefaultToAscii:1;
  FXulong DefaultToSbcs:1;

  FXint AutoIndent;
  FXint FileFilterIndex;
  FXint KeepFileFilter;

  FXFontDesc fontdesc;
  FXint ToolbarButtonSize;
  FXint SplitView;
  FXint OutputPaneHeight;
  FXString FontName;
  FXint FontSize;
  FXint Left,Top,Width,Height;
  FXint SearchWrap;
  FXbool SearchVerbose;
  FXint SearchOptions;
  FXlong ZoomFactor;
  FXint TabWidth;
  FXint IndentWidth;
  FXint CaretWidth;
  FXint SmoothScroll;
  FXint WheelLines;
  FXint placement;
  FXString FileFilters;
  FXint RightEdgeColumn;
  FXString ShellCommand;
  FXchar DocTabPosition;
  FXint MaxFiles;
  FXint AutosaveInterval;
  FXint DefaultFileFormat;

  StyleDef *Styles() { return styles; }

  static StyleDef *globalStyle();
  static StyleDef *whitespaceStyle();
  static StyleDef *caretlineStyle();
  static StyleDef *rightmarginStyle();
  static StyleDef *selectionStyle();
  static StyleDef *caretStyle();
  static const FXchar *defaultFileFilters();

  long onChangeSetting(FXObject*o, FXSelector sel, void*p);

  enum {
    ID_TOGGLE_SMART_HOME,
    ID_TOGGLE_USE_TABS,
    ID_TOGGLE_BRACE_MATCH,
    ID_TOGGLE_ASK_CLOSE_MULTI_EXIT,
    ID_TOGGLE_ASK_CLOSE_MULTI_MENU,
    ID_TOGGLE_WATCH_EXTERN,
    ID_TOGGLE_SMOOTH_SCROLL,
    ID_TOGGLE_POSIX_SEARCH,
    ID_TOGGLE_SEARCH_VERBOSE,
    ID_TOGGLE_CARET_PAST_EOL,
    ID_TOGGLE_AUTOSAVE,
    ID_TOGGLE_VIEW_WHITESPACE_EOL,
    ID_TOGGLE_WRAP_TOOLBAR,
    ID_TOGGLE_ASCII_DEFAULT,
    ID_TOGGLE_SBCS_DEFAULT,
    ID_SAVE_ON_FILTER_SEL,
    ID_SAVE_ON_INS_CMD,
    ID_SAVE_ON_EXEC_CMD,
    ID_CHOOSE_FONT,
    ID_SET_FILETYPES,
    ID_SET_SHABANGS,
    ID_SET_MAX_FILES,
    ID_SET_TAB_WIDTH,
    ID_SET_INDENT_WIDTH,
    ID_SET_CARET_WIDTH,
    ID_SET_WHEEL_LINES,
    ID_SET_SPLIT_VIEW,
    ID_SET_SEARCH_WRAP,
    ID_SET_RIGHT_EDGE,
    ID_SET_SHELL_CMD,
    ID_SET_AUTOSAVE_INT,
    ID_SET_FILE_FORMAT,
    ID_SET_TOOLBAR_BTN_SIZE,
    ID_SET_SEARCH_OPTS,
    ID_SET_KEEP_FILE_FILTER,
    ID_SET_AUTO_INDENT,
    ID_LAST
  };
  static Settings*instance();
};

enum {
  ToolbarUnchanged     = 0,
  ToolbarChangedLayout = 1<<0,
  ToolbarChangedFont   = 1<<1,
  ToolbarChangedWrap   = 1<<2
};

