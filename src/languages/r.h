/* lexname=r lexpfx=SCE_R_ lextag=SCLEX_R */

static StyleDef r_style[] = {
  { "default", SCE_R_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "comment", SCE_R_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "kword", SCE_R_KWORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "basekword", SCE_R_BASEKWORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "otherkword", SCE_R_OTHERKWORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "number", SCE_R_NUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "string", SCE_R_STRING, STRING_FG, _DEFLT_BG, Normal },
  { "string2", SCE_R_STRING2, STRING_FG, _DEFLT_BG, Normal },
  { "operator", SCE_R_OPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { "identifier", SCE_R_IDENTIFIER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "infix", SCE_R_INFIX, _DEFLT_FG, _DEFLT_BG, Normal },
  { "infixeol", SCE_R_INFIXEOL, STREOL_FG, _DEFLT_BG, Normal },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* r_words[]= {
  "if else repeat while function for in next break TRUE FALSE NULL NA Inf NaN",
  "",
  "",
  "",
  "",
  NULL
};


static const char* r_mask = "*.R|*.rsource|*.S|";


static const char* r_apps = "";


static LangStyle LangR = {
  "r",
  SCLEX_R,
  r_style,
  (char**)r_words,
  (char*)r_mask,
  (char*)r_apps,
  0,
  TABS_DEFAULT
};


