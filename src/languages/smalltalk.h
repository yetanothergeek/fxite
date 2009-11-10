/* lexname=smalltalk lexpfx=SCE_ST_ lextag=SCLEX_SMALLTALK */

static StyleDef smalltalk_style[] = {
  { "default", SCE_ST_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "string", SCE_ST_STRING, STRING_FG, _DEFLT_BG, Normal },
  { "number", SCE_ST_NUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "comment", SCE_ST_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "symbol", SCE_ST_SYMBOL, _DEFLT_FG, _DEFLT_BG, Normal },
  { "binary", SCE_ST_BINARY, _DEFLT_FG, _DEFLT_BG, Normal },
  { "bool", SCE_ST_BOOL, _DEFLT_FG, _DEFLT_BG, Normal },
  { "self", SCE_ST_SELF, _DEFLT_FG, _DEFLT_BG, Normal },
  { "super", SCE_ST_SUPER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "nil", SCE_ST_NIL, _DEFLT_FG, _DEFLT_BG, Normal },
  { "global", SCE_ST_GLOBAL, _DEFLT_FG, _DEFLT_BG, Normal },
  { "return", SCE_ST_RETURN, _DEFLT_FG, _DEFLT_BG, Normal },
  { "special", SCE_ST_SPECIAL, _DEFLT_FG, _DEFLT_BG, Normal },
  { "kwsend", SCE_ST_KWSEND, _DEFLT_FG, _DEFLT_BG, Normal },
  { "assign", SCE_ST_ASSIGN, _DEFLT_FG, _DEFLT_BG, Normal },
  { "character", SCE_ST_CHARACTER, STRING_FG, _DEFLT_BG, Normal },
  { "specsel", SCE_ST_SPEC_SEL, _DEFLT_FG, _DEFLT_BG, Normal },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* smalltalk_words[]= {
  "ifTrue: ifFalse: whileTrue: whileFalse: ifNil: ifNotNil: whileTrue whileFalse repeat isNil notNil",
  NULL
};


static const char* smalltalk_mask = "*.st";


static const char* smalltalk_apps = "";


static LangStyle LangSmalltalk = {
  "smalltalk",
  SCLEX_SMALLTALK,
  smalltalk_style,
  (char**)smalltalk_words,
  (char*)smalltalk_mask,
  (char*)smalltalk_apps,
  0,
  TABS_DEFAULT
};


