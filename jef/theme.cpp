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


#include <fx.h>
#include "setcolors.h"
#include "theme.h"

#define RegBackColor        "backcolor"
#define RegBaseColor        "basecolor"
#define RegBorderColor      "bordercolor"
#define RegForeColor        "forecolor"
#define RegHiliteColor      "hilitecolor"
#define RegSelbackColor     "selbackcolor"
#define RegSelforeColor     "selforecolor"
#define RegSelMenuBackColor "selmenubackcolor"
#define RegSelMenuTextColor "selmenutextcolor"
#define RegShadowColor      "shadowcolor"
#define RegTipbackColor     "tipbackcolor"
#define RegTipforeColor     "tipforecolor"


static const AppColors ColorThemes[]={
//|        Name    |        Base         |       Border        |       Back          |      Fore           |      Selback        |      Selfore        |      Tipback        |     Tipfore         |      Menuback       |      Menufore       |
//|----------------+---------------------+---------------------+---------------------+---------------------+---------------------+---------------------+---------------------+---------------------+---------------------+---------------------+
  {"FOX",           FXRGB(0xD4,0xD0,0xC8),FXRGB(0x00,0x00,0x00),FXRGB(0xFF,0xFF,0xFF),FXRGB(0x00,0x00,0x00),FXRGB(0x0A,0x24,0x6A),FXRGB(0xFF,0xFF,0xFF),FXRGB(0xFF,0xFF,0xE1),FXRGB(0x00,0x00,0x00),FXRGB(0x0A,0x24,0x6A),FXRGB(0xFF,0xFF,0xFF)},
  {"Dark Blue",     FXRGB(0x42,0x67,0x94),FXRGB(0x00,0x00,0x00),FXRGB(0x00,0x2A,0x4E),FXRGB(0xFF,0xFF,0xFF),FXRGB(0x5C,0xB3,0xFF),FXRGB(0x00,0x00,0x00),FXRGB(0xFF,0xFF,0xE1),FXRGB(0x00,0x00,0x00),FXRGB(0x5C,0xB3,0xFF),FXRGB(0x00,0x00,0x00)},
  {"Desert FOX",    FXRGB(0xD6,0xCD,0xBB),FXRGB(0x00,0x00,0x00),FXRGB(0xFF,0xFF,0xFF),FXRGB(0x00,0x00,0x00),FXRGB(0x80,0x00,0x00),FXRGB(0xFF,0xFF,0xFF),FXRGB(0xFF,0xFF,0xE1),FXRGB(0x00,0x00,0x00),FXRGB(0x80,0x00,0x00),FXRGB(0xFF,0xFF,0xFF)},
  {"Plastik",       FXRGB(0xEF,0xEF,0xEF),FXRGB(0x00,0x00,0x00),FXRGB(0xFF,0xFF,0xFF),FXRGB(0x00,0x00,0x00),FXRGB(0x67,0x8D,0xB2),FXRGB(0xFF,0xFF,0xFF),FXRGB(0xFF,0xFF,0xE1),FXRGB(0x00,0x00,0x00),FXRGB(0x67,0x8D,0xB2),FXRGB(0xFF,0xFF,0xFF)},
  {"Pumpkin",       FXRGB(0xEE,0xD8,0xAE),FXRGB(0x00,0x00,0x00),FXRGB(0xFF,0xFF,0xFF),FXRGB(0x00,0x00,0x00),FXRGB(0xCD,0x85,0x3F),FXRGB(0xFF,0xFF,0xFF),FXRGB(0xFF,0xFF,0xE1),FXRGB(0x00,0x00,0x00),FXRGB(0xCD,0x85,0x3F),FXRGB(0xFF,0xFF,0xFF)},
  {"Storm",         FXRGB(0xC0,0xC0,0xC0),FXRGB(0x00,0x00,0x00),FXRGB(0xFF,0xFF,0xFF),FXRGB(0x00,0x00,0x00),FXRGB(0x8B,0x00,0x8B),FXRGB(0xFF,0xFF,0xFF),FXRGB(0xFF,0xFF,0xE1),FXRGB(0x00,0x00,0x00),FXRGB(0x8B,0x00,0x8B),FXRGB(0xFF,0xFF,0xFF)},
  {"Sea Sky",       FXRGB(0xA5,0xB2,0xC6),FXRGB(0x00,0x00,0x00),FXRGB(0xFF,0xFF,0xFF),FXRGB(0x00,0x00,0x00),FXRGB(0x31,0x65,0x9C),FXRGB(0xFF,0xFF,0xFF),FXRGB(0xFF,0xFF,0xE1),FXRGB(0x00,0x00,0x00),FXRGB(0x31,0x65,0x9C),FXRGB(0xFF,0xFF,0xFF)},
};


static const FXint numthemes=ARRAYNUMBER(ColorThemes);
static const char*colors_sect="SETTINGS";
static bool use_system_colors;
static AppColors system_colors={"System"};
static AppColors current_colors={"User Defined"};
static AppColors backup_colors;
static FXString system_font;
static FXString current_font;
static FXString backup_font;

AppColors*Theme::SystemColors() { return &system_colors; }
AppColors*Theme::CurrentColors() { return &current_colors; }

const AppColors* Theme::PresetThemes() { return ColorThemes; }
FXint Theme::NumPresets() { return numthemes; }
const FXString Theme::GetCurrentFontName() { return current_font; }
void Theme::SetCurrentFontName(const FXString&name) { current_font=name; }
bool Theme::GetUseSystemColors() { return use_system_colors; }


// Size of an AppColors struct minus the size of its name field
#define SizeOfColors (sizeof(AppColors)-sizeof(char*))


static void CopyColors(AppColors*dst,AppColors*src)
{
  memcpy(&(dst->BaseColor),&(src->BaseColor),SizeOfColors);
}



// Copy color fields only, preserve existing name field of destination.
void Theme::CopyColors(AppColors*dst,AppColors*src) {
  ::CopyColors(dst,src);
}



// Restore previously saved backup_colors
// Returns a bitmask of ChangedFont,ChangedColors
FXuint Theme::Restore()
{
  FXuint what_changed=0;
  if (!SameColors(&current_colors,&backup_colors)) { 
    CopyColors(&current_colors, &backup_colors);
    what_changed|=ChangedColors;
  }
  if (current_font!=backup_font) {
    current_font=backup_font;
    what_changed|=ChangedFont;
  }
  return what_changed;
}



// Save an in-memory copy of current state
void Theme::Backup()
{
  CopyColors(&backup_colors, &current_colors);
  backup_font=current_font;
}



static bool SameColors(AppColors*a,AppColors*b)
{
  return memcmp(&(a->BaseColor),&(b->BaseColor),SizeOfColors)==0;
}



// Return true if both structs have the same color scheme
bool Theme::SameColors(AppColors*a,AppColors*b)
{
  return ::SameColors(a,b);
}



// Compare current colors and font to last backup
// Returns a bitmask of ChangedFont,ChangedColors
FXuint Theme::Modified()
{
  FXuint what_changed=Unchanged;
  if (!SameColors(&current_colors,&backup_colors)) { what_changed|=ChangedColors; }
  if (current_font!=backup_font) { what_changed|=ChangedFont; }
  return what_changed;
}



static const FXString GetActualFont(FXFont*fnt)
{
  FXString dst=fnt->getFont().section(',',1,5);
  dst.prepend(',');
  FXString name=fnt->getActualName();
  if (name.empty()) { name=fnt->getName(); }
  if (name.empty()) { name="Sans"; }
  dst.prepend(name);
  return dst;
}



#define AppColorToReg(c) a->reg().writeColorEntry(colors_sect, Reg##c, a->get##c());

// Store current application colors to in-memory registry
static void AppToReg()
{
  FXApp*a=FXApp::instance();
  AppColorToReg(BackColor);
  AppColorToReg(BackColor);
  AppColorToReg(BaseColor);
  AppColorToReg(BorderColor);
  AppColorToReg(ForeColor);
  AppColorToReg(HiliteColor);
  AppColorToReg(SelMenuBackColor);
  AppColorToReg(SelMenuTextColor);
  AppColorToReg(SelbackColor);
  AppColorToReg(SelforeColor);
  AppColorToReg(ShadowColor);
  AppColorToReg(TipbackColor);
  AppColorToReg(TipforeColor);
  a->reg().writeStringEntry(colors_sect,"Font", GetActualFont(a->getNormalFont()).text());
}



#define RegColorToApp(c) a->set##c(r->readColorEntry(colors_sect, Reg##c));

// Apply registry color and font settings to application object
static void RegToApp()
{
  FXApp*a=FXApp::instance();
  use_system_colors=a->reg().readBoolEntry(colors_sect, "UseSystemColors",use_system_colors);
  FXRegistry*r=use_system_colors?(new FXRegistry()):&(a->reg());
  if (use_system_colors) { r->read(); }
  RegColorToApp(BackColor);
  RegColorToApp(BaseColor);
  RegColorToApp(BorderColor);
  RegColorToApp(ForeColor);
  RegColorToApp(HiliteColor);
  RegColorToApp(SelMenuBackColor);
  RegColorToApp(SelMenuTextColor);
  RegColorToApp(SelbackColor);
  RegColorToApp(SelforeColor);
  RegColorToApp(ShadowColor);
  RegColorToApp(TipbackColor);
  RegColorToApp(TipforeColor);
  if (use_system_colors) { delete r; }
  current_font=a->reg().readStringEntry(colors_sect,"Font",system_font.text());
  if (current_font!=system_font) {
    a->getNormalFont()->destroy();
    a->getNormalFont()->setFont(current_font);
    a->getNormalFont()->create();
    current_font=GetActualFont(a->getNormalFont());
  }
}



static bool BadTheme(AppColors*ac)
{
  AppColors AllBlack={"",0,0,0,0,0,0,0,0,0,0};
  return SameColors(ac,&AllBlack);
}



#define GetSystemColor(f) system_colors.f=r.readColorEntry(colors_sect,Reg##f);

// Populate system_colors struct with system-wide FOX color settings
static bool GetSystemColors()
{
  FXRegistry r(FXString::null,FXString::null);
  r.read();
  GetSystemColor(BaseColor);
  GetSystemColor(BorderColor);
  GetSystemColor(BackColor);
  GetSystemColor(ForeColor);
  GetSystemColor(SelbackColor);
  GetSystemColor(SelforeColor);
  GetSystemColor(TipforeColor);
  GetSystemColor(TipbackColor);
  GetSystemColor(SelMenuBackColor);
  GetSystemColor(SelMenuTextColor);
  return !BadTheme(&system_colors);
}



// Delete any existing color settings from the application's registry.
// This will cause the application to fallback to the system-wide FOX settings.
static void RemoveAppSettings()
{
  FXApp*a=FXApp::instance();
  static const char* settings[]={
    "hilitecolor",
    "selmenutextcolor",
    "bordercolor",
    "tipforecolor",
    "shadowcolor",
    "forecolor",
    "basecolor",
    "tipbackcolor",
    "backcolor",
    "selforecolor",
    "selmenubackcolor",
    "selbackcolor",
    NULL
  };
  for (const char**setting=settings; *setting; setting++) {
    a->reg().deleteEntry("SETTINGS",*setting);
  }
}



// Setup application to use system-wide FOX color settings
// Returns a bitmask of ChangedFont,ChangedColors
FXuint Theme::SetUseSystemColors(bool use)
{
  FXuint what_changed=0;
  use_system_colors=use;
  if (use) {
    RemoveAppSettings();
    RegToApp();
    if (!GetSystemColors()) { CopyColors(&system_colors, (AppColors*)&ColorThemes[0]); }
    use_system_colors=true;
    if (!SameColors(&current_colors,&system_colors)) { what_changed|=ChangedColors; }
    if (current_font!=system_font) {
      current_font=system_font;
      what_changed|=ChangedFont;
    }
  }
  return what_changed;
}



#define ThemeColorToReg(f) a->reg().writeColorEntry(colors_sect,Reg##f,ac->f);

static void ThemeColorsToReg(AppColors*ac) {
  FXApp*a=FXApp::instance();
  ThemeColorToReg(BaseColor);
  ThemeColorToReg(BorderColor);
  ThemeColorToReg(BackColor);
  ThemeColorToReg(ForeColor);
  ThemeColorToReg(SelbackColor);
  ThemeColorToReg(SelforeColor);
  ThemeColorToReg(TipbackColor);
  ThemeColorToReg(TipforeColor);
  ThemeColorToReg(SelMenuBackColor);
  ThemeColorToReg(SelMenuTextColor);
  a->reg().writeColorEntry(colors_sect, "shadowcolor", makeShadowColor(ac->BaseColor));
  a->reg().writeColorEntry(colors_sect, "hilitecolor", makeHiliteColor(ac->BaseColor));
  a->reg().write();
  a->reg().read();
}



#define AppColorIsBlack(f) ((FXApp::instance()->get##f())==0)

bool AppIsBlack()
{
  return (
    AppColorIsBlack(BaseColor) &&
    AppColorIsBlack(BorderColor) &&
    AppColorIsBlack(BackColor) &&
    AppColorIsBlack(ForeColor) &&
    AppColorIsBlack(SelbackColor) &&
    AppColorIsBlack(SelforeColor) &&
    AppColorIsBlack(TipbackColor) &&
    AppColorIsBlack(TipforeColor) &&
    AppColorIsBlack(SelMenuBackColor) &&
    AppColorIsBlack(SelMenuTextColor)
  );
}


// Initialize the theme manager, this should be called after FXApp::init(), 
// but before any windows are created
void Theme::init()
{
  if (AppIsBlack()) { ThemeColorsToReg((AppColors*)&ColorThemes[0]); }
  if (!GetSystemColors()) {
    use_system_colors=false;
    FXApp::instance()->reg().writeBoolEntry(colors_sect, "UseSystemColors",use_system_colors);
  }
  FXFont *fnt=FXApp::instance()->getNormalFont();
  fnt->create();
  system_font=GetActualFont(fnt);
  current_font=system_font;
  CopyColors(&current_colors,&system_colors);
  RegToApp();
  if (AppIsBlack()) {
    ThemeColorsToReg((AppColors*)&ColorThemes[0]);
    RegToApp();
  }
}



// Make sure our settings get written out to the application registry,
// this should be called just before the program terminates.
void Theme::done()
{
  FXApp*a=FXApp::instance();
  a->reg().writeBoolEntry(colors_sect, "UseSystemColors",use_system_colors);
  if (use_system_colors) { RemoveAppSettings(); } else { AppToReg(); }
  a->reg().write();
}



// Recursively apply application color settings to an existing
// window and all of its children.
void Theme::apply(FXWindow*win)
{
  ApplyColorsToWindow(win);
}

