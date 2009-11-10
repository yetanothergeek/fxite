/* lexname=COBOL lexpfx=UNKNOWN lextag=UNKNOWN */

static StyleDef cobol_style[] = {
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* cobol_words[]= {
  "",
  "",
  "",
  NULL
};


static const char* cobol_mask = "";


static const char* cobol_apps = "";


static LangStyle LangCOBOL = {
  "cobol",
  SCLEX_COBOL,
  cobol_style,
  (char**)cobol_words,
  (char*)cobol_mask,
  (char*)cobol_apps,
  0,
  TABS_DEFAULT
};


