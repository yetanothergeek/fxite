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


#include <cctype>
#include <cerrno>
#include <fx.h>
#include <Scintilla.h>

#include "compat.h"
#include "lang.h"
#include "search.h"
#include "appwin_pub.h"
#include "menuspec.h"


#include "intl.h"
#include "prefs.h"


#ifdef WIN32
# define DEFAULT_EOL_FORMAT   SC_EOL_CRLF
# define STYLE_FILE "styles.ini"
#else
# define STYLE_FILE "styles"
# define  DEFAULT_EOL_FORMAT   SC_EOL_LF
#endif


FXDEFMAP(Settings) SettingsMap[]={
  FXMAPFUNCS(SEL_COMMAND,Settings::ID_TOGGLE_SMART_HOME,Settings::ID_SET_AUTO_INDENT, Settings::onChangeSetting)
};

FXIMPLEMENT(Settings, FXObject, SettingsMap, ARRAYNUMBER(SettingsMap));

#define LIMIT_RANGE(n,lo,hi) if (n<lo) { n=lo; } else { if (n>hi) { n=hi; } }

#define SHELL_COMMAND "/bin/sh -c"

#define FILE_FILTERS _("\
All Files (*)|\
C/C++ files (*.[ch],*.[ch]pp,*.[ch]xx,*.[CH],*.cc,*.hh)|\
Script files (*.sh,*.lua,*.rb,*.pl,*.py)|\
Config files (config*,*cfg,*conf,*.ini,*rc,.*)|\
Makefiles ([Mm]akefile*,*.mk,CMake*,*.cmake*,Jam*)|\
Web files (*html,*.htm,*.php*)|\
Text files (*.txt)|\
")

static const char* default_file_filters = FILE_FILTERS;


class EditorFontDlg: public FXFontDialog {
private:
  FXSpinner*ascent;
  FXSpinner*descent;
  class EditorFontSel: public FXFontSelector {
    public:
    FXHorizontalFrame* ActionArea() {
      return static_cast<FXHorizontalFrame*>(accept->getParent());
    }
  };
public:
  EditorFontDlg(FXWindow*o):FXFontDialog(o, _("Select Font"), 0) {
    FXHorizontalFrame*frm=((EditorFontSel*)fontbox)->ActionArea();
    (new FXLabel(frm, _("Line Spacing:\n(in pixels)")))->setPadTop(0);
    new FXLabel(frm, _("  Above:"));
    ascent=new FXSpinner(frm,2,NULL,0,SPIN_NORMAL|FRAME_SUNKEN|FRAME_THICK);
    new FXLabel(frm, _("  Below:"));
    descent=new FXSpinner(frm,2,NULL,0,SPIN_NORMAL|FRAME_SUNKEN|FRAME_THICK);
    ascent->setRange(0,16);
    descent->setRange(0,16);
  }
  FXint getAscent() { return ascent->getValue(); }
  FXint getDescent() { return descent->getValue(); }
  void setAscent(FXint a) { ascent->setValue(a); }
  void setDescent(FXint d) { descent->setValue(d); }
};




long Settings::onChangeSetting(FXObject*o, FXSelector sel, void*p)
{
  switch FXSELID(sel) {
    case ID_TOGGLE_SMART_HOME:  { SmartHome = !SmartHome; break; }
    case ID_TOGGLE_WRAP_AWARE:  { WrapAwareHomeEnd = !WrapAwareHomeEnd; break; }
    case ID_TOGGLE_USE_TABS:    {
      FXSpinner*spin=(FXSpinner*)(((FXCheckButton*)o)->getUserData());
      UseTabs = (bool)((FXival)p);
      if (UseTabs) {
        spin->disable();
        spin->getNext()->disable();
        spin->setTextColor(spin->getApp()->getBaseColor());
      } else {
        spin->enable();
        spin->getNext()->enable();
        spin->setTextColor(spin->getApp()->getForeColor());
      }
      break;
    }
    case ID_SET_BRACE_MATCHING: {
      BraceMatch = (FXival)p;
      LIMIT_RANGE(BraceMatch,BRACEMATCH_NONE,BRACEMATCH_AFTER);
      break;
    }
    case ID_TOGGLE_ASK_CLOSE_MULTI_MENU: { PromptCloseMultiMenu = !PromptCloseMultiMenu; break; }
    case ID_TOGGLE_ASK_CLOSE_MULTI_EXIT: { PromptCloseMultiExit = !PromptCloseMultiExit; break; }
    case ID_TOGGLE_WATCH_EXTERN:    { WatchExternChanges = !WatchExternChanges; break; }
    case ID_TOGGLE_SMOOTH_SCROLL:   { SmoothScroll  = !SmoothScroll;  break; }
    case ID_TOGGLE_SEARCH_VERBOSE:  { SearchVerbose = !SearchVerbose; break; }
    case ID_TOGGLE_CARET_PAST_EOL:  { CaretPastEOL  = !CaretPastEOL;  break; }
    case ID_TOGGLE_VIEW_WHITESPACE_EOL: { WhitespaceShowsEOL = !WhitespaceShowsEOL; break; }
    case ID_TOGGLE_ASCII_DEFAULT: { DefaultToAscii = !DefaultToAscii; break; }
    case ID_TOGGLE_SBCS_DEFAULT: { DefaultToSbcs = !DefaultToSbcs; break; }
    case ID_TOGGLE_WORD_WRAP: { WordWrap = !WordWrap; break; }
    case ID_TOGGLE_WRAP_TOOLBAR: {
      WrapToolbar = !WrapToolbar;
      break;
    }
    case ID_TOGGLE_AUTOSAVE:        {
      FXWindow*w=(FXWindow*)o;
      w=(FXWindow*)w->getUserData();
      Autosave = !Autosave;
      if (Autosave) { w->enable(); } else { w->disable(); }
      break;
    }
    case ID_SAVE_ON_FILTER_SEL: { SaveBeforeFilterSel = !SaveBeforeFilterSel; break; }
    case ID_SAVE_ON_INS_CMD:    { SaveBeforeInsCmd    = !SaveBeforeInsCmd;    break; }
    case ID_SAVE_ON_EXEC_CMD:   { SaveBeforeExecCmd   = !SaveBeforeExecCmd;   break; }
    case ID_CHOOSE_FONT: {
      EditorFontDlg dlg(((FXWindow*)o)->getShell());
      dlg.setAscent(FontAscent);
      dlg.setDescent(FontDescent);
#ifdef WIN32 // Windows font dialog is empty, unless setwidth is zero.
      FXushort setwidth=fontdesc.setwidth;
      fontdesc.setwidth=0;
#endif
      SetDialogFromFont(dlg,fontdesc);
      if (dlg.execute(PLACEMENT_SCREEN)) {
        SetFontFromDialog(fontdesc,dlg);
        FontName=(FXchar*)(fontdesc.face);
        FontSize=fontdesc.size;
        FontAscent=dlg.getAscent();
        FontDescent=dlg.getDescent();
      }
#ifdef WIN32
      else { fontdesc.setwidth=setwidth; }
#endif
      break;
    }
    case ID_SET_MAX_FILES: {
      FXTextField*tf=(FXTextField*)o;
      MaxFiles=FXIntVal(tf->getText(),10);
      LIMIT_RANGE(MaxFiles,1,999);
      char maxfiles[8]="\0\0\0\0\0\0\0";
      snprintf(maxfiles, sizeof(maxfiles)-1, "%d", MaxFiles);
      tf->setText(maxfiles);
      break;
    }
    case ID_SET_TAB_WIDTH: {
      FXSpinner*spin=(FXSpinner*)o;
      TabWidth=spin->getValue();
      LIMIT_RANGE(TabWidth,1,16);
      break;
    }
    case ID_SET_TAB_WIDTH_FOR_LANG: {
      FXSpinner*spin=(FXSpinner*)o;
      LangStyle*ls=(LangStyle*)spin->getUserData();
      ls->tabwidth=spin->getValue();
      LIMIT_RANGE(ls->tabwidth,0,16);
      break;
    }
    case ID_SET_INDENT_WIDTH: {
      FXSpinner*spin=(FXSpinner*)o;
      IndentWidth=spin->getValue();
      LIMIT_RANGE(IndentWidth,1,16);
      break;
    }
    case ID_SET_CARET_WIDTH: {
      FXSpinner*spin=(FXSpinner*)o;
      CaretWidth=spin->getValue();
      LIMIT_RANGE(CaretWidth,1,3);
      break;
    }
    case ID_SET_WHEEL_LINES: {
      FXSpinner*spin=(FXSpinner*)o;
      WheelLines=spin->getValue();
      LIMIT_RANGE(WheelLines,1,32);
      break;
    }
    case ID_SET_TAB_TITLE_MAX_WIDTH: {
      FXSpinner*spin=(FXSpinner*)o;
      TabTitleMaxWidth=spin->getValue();
      LIMIT_RANGE(TabTitleMaxWidth,0,ScreenWidth);
    }
    case ID_SET_SEARCH_WRAP: {
      SearchWrap=(FXival)p;
      LIMIT_RANGE(SearchWrap,SEARCH_WRAP_NEVER,SEARCH_WRAP_ASK);
      break;
    }

    case ID_SET_AUTO_INDENT: {
      AutoIndent=(FXival)p;
      LIMIT_RANGE(AutoIndent,AUTO_INDENT_NONE,AUTO_INDENT_SMART);
      break;
    }

    case ID_SET_SPLIT_VIEW: {
      SplitView=(FXival)p;
      LIMIT_RANGE(SplitView,SPLIT_NONE,SPLIT_BESIDE);
      break;
    }
    case ID_SET_KEEP_FILE_FILTER: {
      KeepFileFilter=(FXival)p;
      LIMIT_RANGE(KeepFileFilter,REMEMBER_NEVER,REMEMBER_ALWAYS);
      break;
    }
    case ID_SET_RIGHT_EDGE: {
      FXSpinner*spin=(FXSpinner*)o;
      RightEdgeColumn=spin->getValue();
      LIMIT_RANGE(RightEdgeColumn,1,1024);
      break;
    }
    case ID_SET_SHELL_CMD: {
      FXTextField*tf=(FXTextField*)o;
      ShellCommand=tf->getText().text();
      break;
    }
    case ID_SET_AUTOSAVE_INT: {
      FXSpinner*spin=(FXSpinner*)o;
      AutosaveInterval=spin->getValue();
      LIMIT_RANGE(AutosaveInterval,15,900);
      break;
    }
    case ID_SET_SEARCH_OPTS: {
      SearchOptions=(FXival)p;
      break;
    }
    case ID_SET_FILETYPES: {
      FXInputDialog*dlg=(FXInputDialog*)o;
      LangStyle* ls=(LangStyle*)dlg->getUserData();
      if (ls) {
        if (ls->mask) {
          if (strcmp(ls->mask,dlg->getText().text())==0) { break; }
          if ( ls->mallocs & (1<<30) ) { free(ls->mask); }
        }
        ls->mask=strdup(dlg->getText().text());
      }
      break;
    }
    case ID_SET_SHABANGS: {
      FXInputDialog*dlg=(FXInputDialog*)o;
      LangStyle* ls=(LangStyle*)dlg->getUserData();
      if (ls) {
        if (ls->apps) {
          if (strcmp(ls->apps,dlg->getText().text())==0) { break; }
          if ( ls->mallocs & (1<<29) ) { free(ls->apps); }
        }
        ls->apps=strdup(dlg->getText().text());
      }
      break;
    }
    case ID_SET_FILE_FORMAT: {
      DefaultFileFormat=(FXival)p;
      LIMIT_RANGE(DefaultFileFormat,0,2);
      break;
    }
    case ID_SET_TOOLBAR_BTN_SIZE: {
      ToolbarButtonSize=(FXival)p;
      LIMIT_RANGE(ToolbarButtonSize,0,2);
      break;
    }
  }
  return 1;
}



static StyleDef GlobalStyle[] = {
  { "default",     STYLE_DEFAULT,     "#000000", "#ffffff", Normal },
  { "linenumber",  STYLE_LINENUMBER,  "#0000c0", "#d0d0d0", Normal },
  { "bracelight",  STYLE_BRACELIGHT,  "#ffff00", "#00d000", Bold  },
  { "bracebad",    STYLE_BRACEBAD,    "#ffff00", "#ff0000", Bold  },
  { "controlchar", STYLE_CONTROLCHAR, "#0000c0", "#d0d0d0", Normal },
  { "indentguide", STYLE_INDENTGUIDE, "#0000c0", "#d0d0d0", Normal },
  { "calltip",     STYLE_CALLTIP,     "#808000", "#e0e0d0", Normal },
  { NULL,          0,                        "",        "", Normal }
};


static StyleDef WhiteSpaceStyle =  { "whitespace",  0,     "#000000", "#ffffdd", Normal };
static StyleDef CaretLineStyle =   { "caretline",   0,     "#000000", "#f8f8f8", Normal };
static StyleDef RightMarginStyle = { "rightmargin", 0,     "#000000", "#ff0000", Normal };
static StyleDef SelectionStyle =   { "selection",   0,     "#c0c0c0", "#c0c0c0", Normal };
static StyleDef CaretStyle =       { "caret",       0,     "#000000", "#000000", Normal };



const FXchar* Settings::CaretFG()       { return CaretStyle.fg; }
const FXchar* Settings::CaretLineBG()   { return CaretLineStyle.bg; }
const FXchar* Settings::RightMarginBG() { return RightMarginStyle.bg; }
const FXchar* Settings::WhiteSpaceBG()  { return WhiteSpaceStyle.bg; }
const FXchar* Settings::WhiteSpaceFG()  { return WhiteSpaceStyle.fg; }
const FXchar* Settings::SelectionBG()   { return SelectionStyle.bg; }



static ErrorPattern DefaultErrPats[8] = {
#ifdef __minix
  { "^\"([^\\s\"]+)\",[^(]+(\\d+)",  "MINIX ACK" },
#endif
  { "([^\\s:]+):(\\d+)",             "GCC" },
  { "([^\\s(]+)\\((\\d+)",           "FreePascal" },
  {"",""},
  {"",""},
  {"",""},
  {"",""},
  {"",""},
#ifndef __minix
  {"",""},
#endif
};

#define MAX_ERROR_PATTERNS ARRAYNUMBER(DefaultErrPats)

static ErrorPattern UserErrorPatterns[MAX_ERROR_PATTERNS];

static const char* errpats_sect="ErrorPatterns";

ErrorPattern* Settings::ErrorPatterns() { return UserErrorPatterns; }

ErrorPattern* Settings::DefaultErrorPatterns() { return DefaultErrPats; }

int Settings::MaxErrorPatterns() { return (MAX_ERROR_PATTERNS); }


static void ReadErrorPatterns(FXSettings*reg)
{
  char pat_key[]="Pattern_\0\0";
  char id_key[]="Comment_\0\0";
  memset(UserErrorPatterns,0,sizeof(ErrorPattern)*MAX_ERROR_PATTERNS);
  if (reg->existingSection(errpats_sect)) {
    FXRex *rx=new FXRex();
    for (FXuint n=0;n<MAX_ERROR_PATTERNS; n++) {
      pat_key[8] = id_key[8] = n+65;
      FXString pat=reg->readStringEntry(errpats_sect,pat_key,"");
      FXString id=reg->readStringEntry(errpats_sect,id_key,"");
      if (!(id.empty()||pat.empty())) {
        FXRexError err=rx->parse(pat,REX_CAPTURE|REX_SYNTAX);
        if (err!=REGERR_OK) {
          fxwarning("%s: %s\n", _("Error parsing regular expression"), FXRex::getError(err));
        }
        strncpy(UserErrorPatterns[n].pat, pat.text(), sizeof(UserErrorPatterns[n].pat)-1);
        strncpy(UserErrorPatterns[n].id,  id.text(),  sizeof(UserErrorPatterns[n].id)-1);
      }
    }
    delete rx;
  } else {
    memcpy(UserErrorPatterns,DefaultErrPats,sizeof(ErrorPattern)*MAX_ERROR_PATTERNS);
  }
}



static void SaveErrorPatterns(FXSettings*reg)
{
  char pat_key[]="Pattern_\0\0";
  char id_key[]="Comment_\0\0";
  reg->deleteSection(errpats_sect);
  for (int n=0;n<=9&&UserErrorPatterns[n].pat[0]; n++) {
    pat_key[8] = id_key[8] = n+65;
    reg->writeStringEntry(errpats_sect,pat_key,UserErrorPatterns[n].pat);
    reg->writeStringEntry(errpats_sect,id_key,UserErrorPatterns[n].id);
  }
}



int Settings::ErrorPatternCount()
{
  FXuint rv=0;
  for (rv=0; rv<MAX_ERROR_PATTERNS && UserErrorPatterns[rv].pat[0]; rv++) { }
  return rv;
}



static const char* sysincs_sect="SystemIncludes";

#define MAX_SYSINC_PATHS 255


static const FXchar* DefaultSysIncs =
  "/usr/include\n"
#ifdef __minix
  "/usr/pkg/include\n"
  "/usr/pkg/X11R6\n"
#endif
  "/usr/local/include\n"
;


static FXString SysIncs = FXString::null;



const FXString Settings::defaultSystemIncludePaths()
{
  return DefaultSysIncs;
}



const FXString Settings::SystemIncludePaths()
{
  return SysIncs;
}



void Settings::SystemIncludePaths(const FXString paths)
{
  SysIncs=paths;
}



static void SaveSysIncPaths(FXSettings*reg)
{
  reg->deleteSection(sysincs_sect);
  int n=SysIncs.contains('\n');
  if (n>MAX_SYSINC_PATHS) { n=MAX_SYSINC_PATHS; }
  for (int i=0; i<n; i++) {
    const FXString value=SysIncs.section('\n',i);
    if (!value.empty()) {
      char key[16];
      snprintf(key, sizeof(key)-1 ,"Path_%d", i+1);
      reg->writeStringEntry(sysincs_sect,key,value.text());
    }
  }
}



static void ReadSysIncPaths(FXSettings*reg)
{
  SysIncs=FXString::null;
  if (reg->existingSection(sysincs_sect)) {
    for (int i=0; i<MAX_SYSINC_PATHS; i++) {
      char key[16];
      snprintf(key, sizeof(key)-1 ,"Path_%d", i+1);
      if (reg->existingEntry(sysincs_sect,key)) {
        const char* value=reg->readStringEntry(sysincs_sect,key,NULL);
        if (value && *value) {
          SysIncs+=value;
          SysIncs+='\n';
        }
      }
    }
  }
  if (SysIncs.empty()) { SysIncs=DefaultSysIncs; }
}



static FXbool IsColor(const char*clr)
{
  const char*p;
  if ( (clr) && (clr[0]=='#') && (strlen(clr)==7) ) {
    for (p=clr+1; *p; p++) { if (!isxdigit(*p)) { return false; } }
    return true;
  } else { return false; }
}



static void SaveStyle(FXSettings*reg, const char*section, StyleDef *style)
{
  FXString tmp="";
  tmp.format("%s,%s", style->fg[0]?style->fg:"default",style->bg[0]?style->bg:"default");
  if (style->style==Normal) {
    tmp.append(",Normal");
  } else {
    if (style->style & Bold) {
      tmp.append(",Bold");
    }
    if (style->style & Italic) {
      tmp.append(",Italic");
    }
    if (style->style & Underline) {
      tmp.append(",Underline");
    }
    if (style->style & EOLFill) {
      tmp.append(",EOLFill");
    }
  }
  reg->writeStringEntry(section,style->key,tmp.text());
}



static void SaveStyles(FXSettings*reg, const char*section, StyleDef *style)
{
  int i;
  for (i=0; style[i].key; i++) {
    SaveStyle(reg,section,&style[i]);
  }
}



static void ParseStyle(FXSettings*reg, const char*section, StyleDef *style)
{
  if (style->key) {
    FXString tmp=reg->readStringEntry(section, style->key, "");
    tmp.lower();
    tmp.substitute('\t', ' ', true);
    tmp.trim();
    tmp.simplify();
    tmp.substitute(" ", "", true);
    if (!tmp.empty()) {
      char buf[256];
      char *fg=buf;
      char *bg=NULL;
      char *fs=NULL;
      int flags=Normal;
      strncpy(buf, tmp.text(), sizeof(buf)-1);
      bg=strchr(buf,',');
      if (bg) {
        *bg='\0';
        bg++;
        fs=strchr(bg,',');
        if (fs) {
          *fs='\0';
          fs++;
          if (strstr(fs,"italic")) { flags|=Italic; }
          if (strstr(fs,"bold")) { flags|=Bold; }
          if (strstr(fs,"underline")) { flags|=Underline; }
          if (strstr(fs,"eolfill")) { flags|=EOLFill; }
        }
        if (IsColor(bg)) { strncpy(style->bg, bg, 7); }
      }
      if (IsColor(fg)) { strncpy(style->fg, fg, 7); }
      style->style=(SciDocFontStyle)(flags);
    }
  }
}



static void ParseStyles(FXSettings*reg, const char*section, StyleDef *style)
{
  int i;
  for (i=0; style[i].key; i++) {
    ParseStyle(reg,section,&style[i]);
  }
}


static Settings*global_settings_instance=NULL;


Settings* Settings::instance()         { return global_settings_instance; }
StyleDef* Settings::globalStyle()      { return GlobalStyle; }
StyleDef* Settings::whitespaceStyle()  { return &WhiteSpaceStyle; }
StyleDef* Settings::caretlineStyle()   { return &CaretLineStyle; }
StyleDef* Settings::rightmarginStyle() { return &RightMarginStyle; }
StyleDef* Settings::selectionStyle()   { return &SelectionStyle; }
StyleDef* Settings::caretStyle()       { return &CaretStyle; }
const FXchar *Settings::defaultFileFilters() { return default_file_filters; }



// List of possible font names - we check if any system
// font name *begins* with any of these phrases...
static const char* tryfonts[] = {
  "Liberation Mono ",
  "DejaVu Sans Mono ",
  "FreeMono ",
  "Courier New ",
  "Nimbus Mono ",
  "Courier ",
  "LucidaTypewriter ",
  "Fixed ",
  "Terminal ",
  "Clean ",
  "Anonymous ",
  NULL
};


// Try to guess at a suitable font - we only do this when
// there is no "FontName" entry in the registry.
static void FindFont(FXString &FontName) {
  FXFontDesc*fonts=NULL;
  FXuint numfonts=0;
  if (FXFont::listFonts(fonts,numfonts,FXString::null)) {
    // First, try each of the items in our "tryfonts" list...
    for (const char**tryfont=tryfonts; *tryfont; tryfont++) {
      for (FXuint i=0; i<numfonts; i++) {
        FXFontDesc fd=fonts[i];
        if (strncasecmp(fd.face, *tryfont, strlen(*tryfont))==0) {
           FontName=fd.face;
           freeElms(fonts);
           return;
        }
      }
    } // keep looking...
    // Try to grab the first thing that is fixed-width and scalable.
    for (FXuint i=0; i<numfonts; i++) {
      FXFontDesc fd=fonts[i];
      if ((fd.flags&FXFont::Fixed) && (fd.flags&FXFont::Scalable)) {
        FontName=fd.face;
        freeElms(fonts);
        return;
      }
    } // keep looking...
    // Try to grab the first thing that is fixed-width and reasonably sized.
    for (FXuint i=0; i<numfonts; i++) {
      FXFontDesc fd=fonts[i];
      if ((fd.flags&FXFont::Fixed)&&(fd.size>=100)&&(fd.size<200)) {
        FontName=fd.face;
        freeElms(fonts);
        return;
      }
    }
    freeElms(fonts);
  }
  FontName="fixed"; // I give up!
}


/*
  Break some of the preference settings up into to categories, each category represents a
  separate section header in the config file. Note that in order for the Read/Write macros
  to work correctly, these strings must *exactly* match their respective variable names!
*/

static const char* view_keys[] = {
  "ShowStatusBar",
  "ShowLineNumbers",
  "ShowToolbar",
  "ShowWhiteSpace",
  "ShowOutputPane",
  "InvertColors",
  "ShowRightEdge",
  "ShowIndentGuides",
  "DocTabsPacked",
  "ZoomFactor",
  "DocTabPosition",
  "ShowCaretLine",
  NULL
};

static const char* edit_keys[] = {
  "SmartHome",
  "WrapAwareHomeEnd",
  "AutoIndent",
  "BraceMatch",
  "UseTabs",
  "CaretPastEOL",
  "WhitespaceShowsEOL",
  "SearchWrap",
  "SearchVerbose",
  "SearchOptions",
  "TabWidth",
  "IndentWidth",
  "CaretWidth",
  "SmoothScroll",
  "WheelLines",
  "FontName",
  "FontSize",
  "fontdesc",
  "FontAscent",
  "FontDescent",
  NULL
};

static const char* geom_keys[] = {
  "Maximize",
  "Left",
  "Top",
  "Width",
  "Height",
  "OutputPaneHeight",
  "placement",
  "LastFocused",
  "TabTitleMaxWidth",
  NULL
};

static const char* main_keys[] = {
  "PromptCloseMultiMenu",
  "PromptCloseMultiExit",
  "WatchExternChanges",
  "Autosave",
  "SaveBeforeFilterSel",
  "SaveBeforeInsCmd",
  "SaveBeforeExecCmd",
  "SplitView",
  "FileFilters",
  "RightEdgeColumn",
  "ShellCommand",
  "MaxFiles",
  "AutosaveInterval",
  "DefaultFileFormat",
  "DefaultToAscii",
  "DefaultToSbcs",
  "KeepFileFilter",
  "FileFilterIndex",
  "FileOpenMulti",
  "WordWrap",
  NULL
};

static const char* tbar_keys[] = {
  "WrapToolbar",
  "ToolbarButtonSize",
  NULL
};


static const char* global_sect="GlobalStyle";

static const char* main_sect = "Settings";
static const char* geom_sect = "Geometry";
static const char* edit_sect = "Editor";
static const char* view_sect = "ViewMenu";
static const char* tbar_sect = "Toolbar";
static const char* keys_sect = "Keybindings";
static const char* popup_sect = "PopupMenu";


static const char*  sectnames[] = {  main_sect,  geom_sect,  edit_sect,  view_sect,  tbar_sect,  NULL};
static const char** keynames[] =  {  main_keys,  geom_keys,  edit_keys,  view_keys,  tbar_keys,  NULL};



static const char *GetSectForKey(const char*key)
{
  for (FXint i=0; keynames[i]; i++) {
    for (FXint j=0; keynames[i][j]; j++) {
      if (strcmp(keynames[i][j],key)==0) { return sectnames[i]; }
    }
  }
  fxerror("FATAL: Section for key \"%s\" not found.\n", key);
  FXASSERT(!key);
  return NULL;
}



static FXint ReadRegInt(FXRegistry *reg, const char*key, FXint def, FXint min=0, FXint max=0)
{
  FXint rv=reg->readIntEntry(GetSectForKey(key),key,def);
  if (min||max) {
    if (rv<min) { rv=min; } else if (rv>max) { rv=max; }
  }
  return rv;
}


#define ReadIntRng(k,df,mn,mx) { k=ReadRegInt(reg,""#k,df,mn,mx); }
#define ReadInt(k,df) { k=ReadRegInt(reg,""#k,df); }
#define WriteInt(k) {reg->writeIntEntry(GetSectForKey(""#k),""#k,k); }


#define ReadStr(k,df) { k=reg->readStringEntry(GetSectForKey(""#k),""#k,df); }
#define WriteStr(k) {reg->writeStringEntry(GetSectForKey(""#k),""#k,k.text()); }

#ifdef FOX_1_6
  FXbool LocaleIsUTF8(){
# ifdef WIN32
    return GetACP()==CP_UTF8;
# else
    const FXchar* str;
    if((str=getenv("LC_CTYPE"))!=NULL || (str=getenv("LC_ALL"))!=NULL || (str=getenv("LANG"))!=NULL){
      return (strstr(str,"utf")!=NULL || strstr(str,"UTF")!=NULL);
    }
    return false;
# endif
  }
#endif


Settings::Settings(FXMainWindow*w, const FXString &configdir)
{
  FXASSERT(!global_settings_instance);
  global_settings_instance=this;
  FXString tmp;
  reg=&(w->getApp()->reg());
  style_reg=new FXSettings();
  app=w->getApp();
  style_file=configdir+STYLE_FILE;
  if (use_xdg_config()) { style_file.append(".rc"); }

#ifdef FOX_1_6
  // Fox-1.6 will choke reading string entries > 2000 chars, so rewrite
  // the styles file, in case it was written by a newer version of Fox.
  if (FXStat::exists(style_file)) {
    FXFile style_fh(style_file,FXIO::Reading);
    if (style_fh.isOpen()) {
      FXString style_data('\0', style_fh.size()+1);
      style_fh.readBlock((char*)style_data.text(), style_fh.size());
      style_fh.close();
      FXint n=style_data.contains(ENDLINE);
      bool toolong=false;
      for (FXint i=0; i<=n; i++) {
        if (style_data.section(ENDLINE,i).length()>1952) {
          toolong=true;
          break;
        }
      }
      if (toolong && style_fh.open(style_file,FXIO::Writing)) {
        FXint eoln=strlen(ENDLINE);
        for (FXint i=0; i<=n; i++) {
          FXString line=style_data.section(ENDLINE,i);
          line.trunc(1952);
          if (strlen(line.text())) { style_fh.writeBlock(line.text(),line.length()); }
          style_fh.writeBlock(ENDLINE,eoln);
        }
        style_fh.close();
      }
    }
  }
#endif

  if ( (FXStat::exists(style_file)) && (!style_reg->parseFile(style_file, true))) {
    FXMessageBox::error(app,MBOX_OK,
      _("Configuration error"), "%s \n%s\n%s", _("Failed to read settings from"),
      style_file.text(), SystemErrorStr()
    );
  }
  ReadInt(ShowStatusBar,true);
  ReadInt(ShowLineNumbers,false);
  ReadInt(ShowToolbar,true);
  ReadInt(ShowWhiteSpace,false);
  ReadInt(ShowOutputPane,false);
  ReadInt(InvertColors,false);
  ReadIntRng(SplitView,0,SPLIT_NONE,SPLIT_BESIDE);
  ReadInt(OutputPaneHeight,64);
  ReadInt(SmartHome,false);
  ReadInt(WrapAwareHomeEnd,false);
  ReadIntRng(BraceMatch,BRACEMATCH_EITHER,BRACEMATCH_NONE,BRACEMATCH_AFTER);
  ReadInt(UseTabs,true);
  ReadInt(CaretPastEOL,false);
  ReadIntRng(TabWidth,4,1,16);
  ReadIntRng(IndentWidth,TabWidth,1,16);
  ReadIntRng(CaretWidth,1,1,3);
  ReadIntRng(RightEdgeColumn,80,1,1024);
  ReadInt(ShowRightEdge,false);
  ReadInt(ShowIndentGuides,false);
  ReadInt(ShowCaretLine,true);
  ReadInt(SmoothScroll,true);
  ReadIntRng(WheelLines,3,1,32);
  ReadIntRng(SearchWrap,SEARCH_WRAP_ASK,SEARCH_WRAP_NEVER,SEARCH_WRAP_ASK);
  ReadIntRng(AutoIndent,AUTO_INDENT_NONE,AUTO_INDENT_NONE,AUTO_INDENT_SMART);
  ReadInt(SearchVerbose,true);
  ReadInt(SearchOptions,0);
  ReadInt(ZoomFactor,0);
  ReadInt(DocTabsPacked,true);
  ReadIntRng(MaxFiles,128,1,999);
  ReadInt(PromptCloseMultiMenu,true);
  ReadInt(PromptCloseMultiExit,true);
  ReadInt(WatchExternChanges,true);
  ReadInt(Autosave,false);
  ReadIntRng(AutosaveInterval,60,15,900);
  ReadInt(SaveBeforeFilterSel,false);
  ReadInt(SaveBeforeInsCmd,false);
  ReadInt(SaveBeforeExecCmd,true);
  ReadInt(WhitespaceShowsEOL,true);
  ReadIntRng(DefaultFileFormat,DEFAULT_EOL_FORMAT,0,2);
  ReadInt(DefaultToAscii,!LocaleIsUTF8());
  ReadInt(DefaultToSbcs,true);
  ReadIntRng(KeepFileFilter,REMEMBER_NEVER,REMEMBER_NEVER,REMEMBER_ALWAYS);
  if (KeepFileFilter==REMEMBER_ALWAYS) {
    ReadInt(FileFilterIndex,0);
  } else {
    FileFilterIndex=0;
  }
  ReadInt(FileOpenMulti,false);
  ReadInt(WordWrap,false);
  ReadInt(WrapToolbar,true);
  ReadIntRng(ToolbarButtonSize,1,0,2);// 0=small;  1=medium;  2=large

  placement=reg->existingEntry(geom_sect,"Top")?PLACEMENT_DEFAULT:PLACEMENT_SCREEN;
  ReadInt(Left,0);
  ReadInt(Top,0);
  ReadInt(Width,0);
  ReadInt(Height,0);

  ScreenWidth=w->getApp()->getRootWindow()->getDefaultWidth();
  ScreenHeight=w->getApp()->getRootWindow()->getDefaultHeight();
  if ((Width==0)&&(Height==0)) { // First run, size based on screen dimensions
    Width=ScreenWidth*(4.0/5.0);
    Height=ScreenHeight*(3.0/4.0);
    LIMIT_RANGE(Width,600,Width);
    LIMIT_RANGE(Height,400,Height);
  }

  LIMIT_RANGE(Left,0,Left);
  LIMIT_RANGE(Top,0,Top);
  LIMIT_RANGE(Width,160,Width);
  LIMIT_RANGE(Height,120,Height);
  ReadInt(Maximize,false);
  ReadIntRng(TabTitleMaxWidth,ScreenWidth/6,0,ScreenWidth);
  LastFocused=reg->readStringEntry("LastFocused",FXPath::title(TopWinPub::Connector()).text(),"");
  ReadInt(FontSize,120);
  ReadIntRng(FontAscent,2,0,16);
  ReadIntRng(FontDescent,0,0,16);

  if (reg->existingEntry(edit_sect,"FontName")) {
    ReadStr(FontName,"Fixed [Misc]");
  } else {
    FindFont(FontName);
  }


  tmp=reg->readStringEntry(view_sect,"DocTabPosition","T");
  if (tmp.empty() || !strchr("TBLR",tmp.text()[0])) {
    DocTabPosition='T';
  } else {
    DocTabPosition=tmp.text()[0];
  }

  ReadStr(FileFilters, default_file_filters);
  FileFilters.substitute("|", "\n", true);
  ReadStr(ShellCommand,SHELL_COMMAND);

  ParseStyles(style_reg, global_sect, GlobalStyle);
  ParseStyle(style_reg, global_sect, &WhiteSpaceStyle);
  ParseStyle(style_reg, global_sect, &CaretLineStyle);
  ParseStyle(style_reg, global_sect, &RightMarginStyle);
  for (LangStyle*ls=languages; ls->name; ls++) {
    if (ls->words) {
      int i;
      for (i=0; ls->words[i]; i++) {
        char buf[256];
        snprintf(buf,sizeof(buf)-1, "words_%d", i+1);
        tmp=style_reg->readStringEntry(ls->name,buf,ls->words[i]);
        SetKeywordList(ls,i,tmp);
      }
    }
    ParseStyles(style_reg, ls->name, ls->styles);
    tmp=style_reg->readStringEntry(ls->name,"FileMask",ls->mask?ls->mask:"");
    if ( (ls->mask && (strcmp(ls->mask,tmp.text())!=0))||((ls->mask==NULL)&&(!tmp.empty())) ) {
      ls->mask=strdup(tmp.text());
      ls->mallocs |= LANGSTYLE_MASK_ALLOCD;
    }
    tmp=style_reg->readStringEntry(ls->name,"ShebangApps",ls->apps?ls->apps:"");
    if ( (ls->apps && (strcmp(ls->apps,tmp.text())!=0))||((ls->apps==NULL)&&(!tmp.empty())) ){
      ls->apps=strdup(tmp.text());
      ls->mallocs |= LANGSTYLE_APPS_ALLOCD;
    }
    ls->tabs=(TabPolicy)(style_reg->readIntEntry(ls->name,"TabPolicy", ls->tabs));
    ls->tabwidth=style_reg->readIntEntry(ls->name,"TabWidth", 0);
    LIMIT_RANGE(ls->tabwidth,0,16);
  }
  styles=GlobalStyle;
  MenuMgr::ReadMenuSpecs(reg,keys_sect);
  MenuMgr::ReadToolbarButtons(reg,tbar_sect);
  MenuMgr::ReadPopupMenu(reg,popup_sect);
  ReadErrorPatterns(reg);
  ReadSysIncPaths(reg);
}



extern "C" { void ini_sort(const char *filename); }


Settings::~Settings()
{
  LangStyle*ls;
  for (ls=languages; ls->name; ls++) {
    int i;
    SaveStyles(style_reg, ls->name, ls->styles);
    style_reg->writeStringEntry(ls->name,"FileMask",ls->mask?ls->mask:"");
    style_reg->writeStringEntry(ls->name,"ShebangApps",ls->apps?ls->apps:"");
    style_reg->writeIntEntry(ls->name,"TabPolicy", ls->tabs);
    style_reg->writeIntEntry(ls->name,"TabWidth", ls->tabwidth);
    for (i=0; ls->words[i]; i++) {
      char key[32];
      snprintf(key, sizeof(key)-1, "words_%d", i+1);
#ifdef FOX_1_6
    // fox 1.6 will choke reading string entries > 2000 chars
    char buf[1952];
    memset(buf,0,sizeof(buf));
    strncpy(buf, ls->words[i], sizeof(buf)-1);
    style_reg->writeStringEntry(ls->name,key,buf);
#else
    style_reg->writeStringEntry(ls->name,key,ls->words[i]);
#endif
    }
  }
  SaveStyles(style_reg, global_sect, GlobalStyle);
  SaveStyle(style_reg, global_sect,&WhiteSpaceStyle);
  SaveStyle(style_reg, global_sect,&CaretLineStyle);
  SaveStyle(style_reg, global_sect,&RightMarginStyle);

  reg->deleteSection(keys_sect);
  reg->deleteSection(tbar_sect);

  WriteInt(ShowStatusBar);
  WriteInt(ShowWhiteSpace);
  WriteInt(ShowOutputPane);
  WriteInt(InvertColors);
  WriteInt(SplitView);
  WriteInt(OutputPaneHeight);
  WriteInt(ShowLineNumbers);
  WriteInt(ShowToolbar);
  WriteInt(SmartHome);
  WriteInt(WrapAwareHomeEnd);
  WriteInt(AutoIndent);
  WriteInt(BraceMatch);
  WriteInt(UseTabs);
  WriteInt(CaretPastEOL);
  WriteInt(TabWidth);
  WriteInt(IndentWidth);
  WriteInt(CaretWidth);
  WriteInt(RightEdgeColumn);
  WriteInt(ShowRightEdge);
  WriteInt(ShowIndentGuides);
  WriteInt(ShowCaretLine);
  WriteInt(SmoothScroll);
  WriteInt(WheelLines);
  WriteInt(SearchWrap);
  WriteInt(SearchVerbose);
  WriteInt(SearchOptions);
  WriteInt(ZoomFactor);
  WriteInt(DocTabsPacked);
  WriteInt(MaxFiles);
  WriteInt(PromptCloseMultiMenu);
  WriteInt(PromptCloseMultiExit);
  WriteInt(WatchExternChanges);
  WriteInt(Autosave);
  WriteInt(AutosaveInterval);
  WriteInt(SaveBeforeFilterSel);
  WriteInt(SaveBeforeInsCmd);
  WriteInt(SaveBeforeExecCmd);
  WriteInt(WhitespaceShowsEOL);
  WriteInt(DefaultFileFormat);
  WriteInt(WrapToolbar);
  WriteInt(WordWrap);
  WriteInt(ToolbarButtonSize);
  WriteInt(Left);
  WriteInt(Top);
  WriteInt(Width);
  WriteInt(Height);
  WriteInt(Maximize);
  reg->writeStringEntry("LastFocused",FXPath::title(TopWinPub::Connector()).text(),LastFocused.text());
  WriteInt(FontSize);
  WriteInt(FontAscent);
  WriteInt(FontDescent);
  WriteInt(DefaultToAscii);
  WriteInt(DefaultToSbcs);
  WriteInt(KeepFileFilter);
  WriteInt(FileFilterIndex);
  WriteInt(FileOpenMulti);
  WriteInt(TabTitleMaxWidth);

  if (!(DocTabPosition && strchr("TBLR",DocTabPosition))) { DocTabPosition='T'; }
  char dtp[2]={DocTabPosition,'\0'};
  reg->writeStringEntry(view_sect,"DocTabPosition",dtp);
  WriteStr(FontName);
  WriteStr(FileFilters);
  WriteStr(ShellCommand);

  FreeAllKeywordLists();

  MenuMgr::WriteMenuSpecs(reg,keys_sect);
  MenuMgr::WriteToolbarButtons(reg,tbar_sect);
  MenuMgr::WritePopupMenu(reg,popup_sect);
  SaveErrorPatterns(reg);
  SaveSysIncPaths(reg);

  if (!style_reg->unparseFile(style_file)) {
    FXMessageBox::error(app,MBOX_OK,
      _("Configuration error"), "%s \n%s\n%s", _("Failed to save settings to"),
      style_file.text(), SystemErrorStr()
    );
  }
  delete style_reg;
  ini_sort(style_file.text());
  global_settings_instance=NULL;
}

