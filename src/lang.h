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

#ifndef FXITE_LANG_H
#define FXITE_LANG_H

#include "color_funcs.h"

typedef enum {
  Normal=0,
  Bold=1,
  Italic=2,
  Underline=4,
  EOLFill=8
} SciDocFontStyle;



typedef struct _StyleDef {
  const char* key;   // Human-readable key name for config file
  int id;            // Numeric SCE_* identifier
  ColorName fg;      // Foreground color, in #RRGGBB format
  ColorName bg;      // Background color, in #RRGGBB format
  SciDocFontStyle style; // Bitmask of Normal | Bold | Italic
} StyleDef;

typedef enum {
  TABS_DEFAULT,  // Decide by preferences setting
  TABS_ALWAYS,   // Always use tabs
  TABS_NEVER,    // Always use spaces
  TABS_AUTO      // Decide by document content, else fallback to TABS_DEFAULT
} TabPolicy;


#define LANGSTYLE_MASK_ALLOCD 1<<30
#define LANGSTYLE_APPS_ALLOCD 1<<29

typedef struct _LangStyle
{
  const char *name;  // Human-readable language name
  int id;            // Scintilla SCLEX_* identifier
  StyleDef*styles;   // Array of style definitions
  char **words;      // Array of space-delimited keyword lists
  char *mask;        // Pipe-delimited list of wildcards { mallocs bit is (1<<30) }
  char *apps;        // Pipe-delimited list of applications { mallocs bit is (1<<29) }
  FXuint mallocs;    // Bitmask to track which keyword lists are malloc'd
  TabPolicy tabs;   // How to determine use of tabs or spaces.
  unsigned char tabwidth;  // Language-specific tab width ( or zero to use global setting. )
} LangStyle;


extern LangStyle languages[];


LangStyle*GetLangByName(const char*name);
LangStyle*GetLangFromDocName(const char*filename);
LangStyle*GetLangFromAppName(const char*app);
void SetKeywordList(LangStyle*lang, int index, const FXString &keywords);
void FreeAllKeywordLists();


StyleDef* GetStyleFromId(StyleDef*styles, int id);

#endif

