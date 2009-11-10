/* lexname=makefile lexpfx=SCE_MAKE_ lextag=SCLEX_MAKEFILE */

static StyleDef makefile_style[] = {
  { "default", SCE_MAKE_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "comment", SCE_MAKE_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "preprocessor", SCE_MAKE_PREPROCESSOR, PREPRC_FG, _DEFLT_BG, Normal },
  { "identifier", SCE_MAKE_IDENTIFIER, SCALAR_FG, _DEFLT_BG, Normal },
  { "operator", SCE_MAKE_OPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { "target", SCE_MAKE_TARGET, _WORD1_FG, _DEFLT_BG, Bold },
  { "ideol", SCE_MAKE_IDEOL, STREOL_FG, _DEFLT_BG, Normal },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* makefile_words[]= {NULL};


static const char* makefile_mask = "[Mm]akefile*";


static const char* makefile_apps = "";


static LangStyle LangMakeFile = {
  "makefile",
  SCLEX_MAKEFILE,
  makefile_style,
  (char**)makefile_words,
  (char*)makefile_mask,
  (char*)makefile_apps,
  0,
  TABS_ALWAYS
};


