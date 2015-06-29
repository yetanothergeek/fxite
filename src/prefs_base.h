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

#ifndef FXITE_PREFS_BASE_H
#define FXITE_PREFS_BASE_H


typedef struct _StyleDef StyleDef;

class SettingsBase: public FXObject {
protected:
  StyleDef *styles;
public:
  SettingsBase();
  ~SettingsBase();

  FXulong ShowStatusBar:1;
  FXulong ShowLineNumbers:1;
  FXulong ShowToolbar:1;
  FXulong ShowWhiteSpace:1;

  FXulong ShowOutputPane:1;
  FXulong ShowRightEdge:1;
  FXulong ShowIndentGuides:1;

  FXulong ShowCaretLine:1;
  FXulong Maximize:1;
  FXulong SmartHome:1;
  FXulong WrapAwareHomeEnd:1;

  FXulong BraceMatch:3;
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
  FXulong FileOpenMulti:1;
  FXulong WordWrap:1;



  FXint AutoIndent;
  FXint FileFilterIndex;
  FXint KeepFileFilter;

  FXFontDesc fontdesc;
  FXint ToolbarButtonSize;
  FXint SplitView;
  FXint OutputPaneHeight;
  FXString FontName;
  FXint FontSize;
  FXuchar FontAscent;
  FXuchar FontDescent;
  FXint Left,Top,Width,Height;
  FXint SearchWrap;
  FXint SearchGui;
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
  FXchar DocTabsPacked;
  FXint MaxFiles;
  FXint AutosaveInterval;
  FXint DefaultFileFormat;
  FXint ScreenWidth;
  FXint ScreenHeight;
  FXint TabTitleMaxWidth;
  StyleDef *Styles() { return styles; }
  static SettingsBase*instance();
};


enum {
  SPLIT_NONE,
  SPLIT_BELOW,
  SPLIT_BESIDE
};


typedef enum {
  BRACEMATCH_NONE=0,
  BRACEMATCH_INSIDE,
  BRACEMATCH_OUTSIDE,
  BRACEMATCH_EITHER,
  BRACEMATCH_AFTER,
} BraceMatchPolicy;

#endif

