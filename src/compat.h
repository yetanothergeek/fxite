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
# define FXRexError     FXRex::Error
# define FXIntVal(s,b) ((FXint)strtol(s.text(),NULL,b))
# define LocaleIsUTF8() FXSystem::localeIsUTF8()
#endif


#ifdef WIN32
# define NULL_FILE "NUL"
  bool IsWin9x();
# if 1
  bool IsDir(const FXString &dn);
# else
# define IsDir(dn) (FXStat::isDirectory(dn))
# endif
#else
# define NULL_FILE "/dev/null"
# define IsDir(dn) (FXStat::isDirectory(dn))
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

