/* lexname=freebasic lexpfx=UNKNOWN lextag=UNKNOWN */

static StyleDef freebasic_style[] = {
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* freebasic_words[]= {
  "append as asc asin asm atan2 atn beep bin binary bit bitreset bitset bload bsave byref byte byval call callocate case cbyte cdbl cdecl chain chdir chr cint circle clear clng clngint close cls color command common cons const continue cos cshort csign csng csrlin cubyte cuint culngint cunsg curdir cushort custom cvd cvi cvl cvlongint cvs cvshort data date deallocate declare defbyte defdbl defined defint deflng deflngint defshort defsng defstr defubyte defuint defulngint defushort dim dir do double draw dylibload dylibsymbol else elseif end enum environ environ$ eof eqv erase err error exec exepath exit exp export extern field fix flip for fre freefile function get getjoystick getkey getmouse gosub goto hex hibyte hiword if iif imagecreate imagedestroy imp inkey inp input instr int integer is kill lbound lcase left len let lib line lobyte loc local locate lock lof log long longint loop loword lset ltrim mid mkd mkdir mki mkl mklongint mks mkshort mod multikey mutexcreate mutexdestroy mutexlock mutexunlock name next not oct on once open option or out output overload paint palette pascal pcopy peek peeki peeks pipe pmap point pointer poke pokei pokes pos preserve preset print private procptr pset ptr public put random randomize read reallocate redim rem reset restore resume resume next return rgb rgba right rmdir rnd rset rtrim run sadd screen screencopy screeninfo screenlock screenptr screenres screenset screensync screenunlock seek statement seek function selectcase setdate setenviron setmouse settime sgn shared shell shl short shr sin single sizeof sleep space spc sqr static stdcall step stop str string string strptr sub swap system tab tan then threadcreate threadwait time time timer to trans trim type ubound ubyte ucase uinteger ulongint union unlock unsigned until ushort using va_arg va_first va_next val val64 valint varptr view viewprint wait wend while width window windowtitle with write xor zstring",
  "",
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


