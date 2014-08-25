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

#if (FOX_MAJOR==1) && (FOX_MINOR==6) && !defined(FOX_1_6)
# define FOX_1_6
#endif

#ifdef FOX_1_6
# define ONE_SECOND ((FXuint)1000)
# define PathMatch(pattern,file,flags) FXPath::match(pattern,file,flags)
# define SetFontFromDialog(dsc,dlg) { dlg.getFontSelection(dsc); }
# define SetDialogFromFont(dlg,dsc)  { dlg.setFontSelection(dsc); }
# define GetFontDescription(dsc,fnt) { (fnt)->getFontDesc(dsc); }
# define UnEscape(s) { s=unescape(s); }
# define AddInput(file,mode,tgt,sel) addInput(file,mode,tgt,sel)
# define freeElms(e) FXFREE(&e)
FXbool LocaleIsUTF8();
#else
# define ONE_SECOND ((FXTime)1000000000)
# define PathMatch(pattern,file,flags) FXPath::match(file,pattern,flags)
# define SetFontFromDialog(dsc,dlg) { dsc=dlg.getFontDesc(); }
# define SetDialogFromFont(dlg,dsc) { dlg.setFontDesc(dsc); }
# define GetFontDescription(dsc,fnt) { dsc=(fnt)->getFontDesc(); }
# define UnEscape(s) { s.unescape(); }
# define AddInput(file,mode,tgt,sel) addInput(tgt,sel,file,mode)
# define ID_COLORDIALOG FXFrame::ID_LAST
# define FILEMATCH_NOESCAPE FXPath::NoEscape
# define FILEMATCH_FILE_NAME FXPath::PathName
# define FILEMATCH_CASEFOLD FXPath::CaseFold
# define REX_FORWARD    FXRex::Forward
# define REX_BACKWARD   FXRex::Backward
# define REX_CAPTURE    FXRex::Capture
# define REX_NEWLINE    FXRex::Newline
# define REX_NORMAL     FXRex::Normal
# define REX_ICASE      FXRex::IgnoreCase
# define REX_NOT_EMPTY  FXRex::NotEmpty
# define REX_SYNTAX     FXRex::Syntax
# define REGERR_OK      FXRex::ErrOK
# define FXRexError     FXRex::Error
# define FXIntVal(s,b) ((FXint)strtol(s.text(),NULL,b))
# define LocaleIsUTF8() FXSystem::localeIsUTF8()
# if (FOX_MAJOR>1)||(FOX_MINOR>7)||(FOX_LEVEL>26)
#  define fxgetpid() (FXProcess::current())
# endif
# if (FOX_MAJOR>1) || (FOX_MINOR>7) || (FOX_LEVEL>44)
#  define FOX_1_7_45_OR_NEWER
# endif
#endif


#ifdef WIN32
# define SetupXAtoms(win,class_name,app_name)
# define IsDesktopCurrent(tw) (true)
# define NULL_FILE "NUL"
  bool IsWin9x();
  const char* SystemErrorStr();
  bool IsDir(const FXString &dn);
  void GetAppDataDir(FXString &AppDataDir);
  FXString GetShellFolder(const char*shellfolder);
#else
# define NULL_FILE "/dev/null"
# define IsDir(dn) (FXStat::isDirectory(dn))
# define SystemErrorStr() (strerror(errno))
void SetupXAtoms(FXTopWindow*win, const char*class_name, const char* app_name);
bool IsDesktopCurrent(FXMainWindow*tw);
#endif


#if defined(FOX_1_6) || defined(WIN32)
# define use_xdg_config() (0)
#else
# define use_xdg_config() \
            ((fxversion[0]>1) || (fxversion[1]>7) || ((fxversion[1]==7) && (fxversion[2]>=23)))
#endif

#ifdef WIN32
# if (FOX_MAJOR<2) && ( (FOX_MINOR<7) || ((FOX_MINOR==7)&&(FOX_LEVEL<24)) )
#  error *** Building FXiTe for Windows requires at least FOX-1.7.24 ***
# endif
#endif

#ifdef FOX_1_7_45_OR_NEWER
# define Dictionary FXDictionary
# define DictKeyName(d,n)     ((d).key(n).text())
# define TotalSlotsInDict(d)  ((d)->no())
# define UsedSlotsInDict(d)   ((d)->used())
# define ReplaceInDict(d,k,v) ((d)->at(k)=(v))
# define LookupInDict(d,k)    ((d)->at(k))
#else
# define Dictionary FXDict
# define DictKeyName(d,n)     ((d).key(n))
# define TotalSlotsInDict(d)  ((d)->size())
# define UsedSlotsInDict(d)   ((d)->no())
# define ReplaceInDict(d,k,v) ((d)->replace((k),(v)))
# define LookupInDict(d,k)    ((d)->find(k))
#endif

FXID GetNetActiveWindow();

void WaitForWindowFocus(FXTopWindow*w, FXuint msecs=1000);

#define SetPadLRTB(o,l,r,t,b) \
  (o)->setPadLeft(l); \
  (o)->setPadRight(r); \
  (o)->setPadTop(t); \
  (o)->setPadBottom(b);

#define SetPad(o,p) SetPadLRTB(o,p,p,p,p)

