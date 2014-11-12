/* lexname=freebasic lexpfx=SCE_B_ lextag=SCLEX_FREEBASIC */

static StyleDef freebasic_style[] = {
  { "asm", SCE_B_ASM, _DEFLT_FG, _DEFLT_BG, Normal },
  { "binnumber", SCE_B_BINNUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "comment", SCE_B_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentblock", SCE_B_COMMENTBLOCK, COMMNT_FG, _DEFLT_BG, Italic },
  { "constant", SCE_B_CONSTANT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "date", SCE_B_DATE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "default", SCE_B_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "docblock", SCE_B_DOCBLOCK, _DEFLT_FG, _DEFLT_BG, Normal },
  { "dockeyword", SCE_B_DOCKEYWORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "docline", SCE_B_DOCLINE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "error", SCE_B_ERROR, _DEFLT_FG, _DEFLT_BG, Normal },
  { "hexnumber", SCE_B_HEXNUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "identifier", SCE_B_IDENTIFIER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "keyword", SCE_B_KEYWORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "keyword2", SCE_B_KEYWORD2, _WORD2_FG, _DEFLT_BG, Bold },
  { "keyword3", SCE_B_KEYWORD3, _WORD1_FG, _DEFLT_BG, Bold },
  { "keyword4", SCE_B_KEYWORD4, _WORD1_FG, _DEFLT_BG, Bold },
  { "label", SCE_B_LABEL, _DEFLT_FG, _DEFLT_BG, Normal },
  { "number", SCE_B_NUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "operator", SCE_B_OPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { "preprocessor", SCE_B_PREPROCESSOR, PREPRC_FG, _DEFLT_BG, Normal },
  { "string", SCE_B_STRING, STRING_FG, _DEFLT_BG, Normal },
  { "stringeol", SCE_B_STRINGEOL, STREOL_FG, _DEFLT_BG, Normal },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* freebasic_words[]= {
  "append as asc asin asm atan2 atn beep bin binary bit bitreset bitset bload bsave byref byte byval call callocate case cbyte cdbl cdecl chain chdir chr cint circle clear clng clngint close cls color command common cons const continue cos cshort csign csng csrlin cubyte cuint culngint cunsg curdir cushort custom cvd cvi cvl cvlongint cvs cvshort data date deallocate declare defbyte defdbl defined defint deflng deflngint defshort defsng defstr defubyte defuint defulngint defushort dim dir do double draw dylibload dylibsymbol else elseif end enum environ environ$ eof eqv erase err error exec exepath exit exp export extern field fix flip for fre freefile function get getjoystick getkey getmouse gosub goto hex hibyte hiword if iif imagecreate imagedestroy imp inkey inp input instr int integer is kill lbound lcase left len let lib line lobyte loc local locate lock lof log long longint loop loword lset ltrim mid mkd mkdir mki mkl mklongint mks mkshort mod multikey mutexcreate mutexdestroy mutexlock mutexunlock name next not oct on once open option or out output overload paint palette pascal pcopy peek peeki peeks pipe pmap point pointer poke pokei pokes pos preserve preset print private procptr pset ptr public put random randomize read reallocate redim rem reset restore resume resume next return rgb rgba right rmdir rnd rset rtrim run sadd screen screencopy screeninfo screenlock screenptr screenres screenset screensync screenunlock seek statement seek function selectcase setdate setenviron setmouse settime sgn shared shell shl short shr sin single sizeof sleep space spc sqr static stdcall step stop str string string strptr sub swap system tab tan then threadcreate threadwait time time timer to trans trim type ubound ubyte ucase uinteger ulongint union unlock unsigned until ushort using va_arg va_first va_next val val64 valint varptr view viewprint wait wend while width window windowtitle with write xor zstring",
  "#define #dynamic #else #endif #error #if #ifdef #ifndef #inclib #include #print #static #undef",
  "",
  "",
  NULL
};


static const char* freebasic_mask = "*.bas|*.bi";


static const char* freebasic_apps = "";


static LangStyle LangFreebasic = {
  "freebasic",
  SCLEX_FREEBASIC,
  freebasic_style,
  (char**)freebasic_words,
  (char*)freebasic_mask,
  (char*)freebasic_apps,
  0,
  TABS_DEFAULT,
  0
};


