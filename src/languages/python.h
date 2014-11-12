/* lexname=python lexpfx=SCE_P_ lextag=SCLEX_PYTHON */

static StyleDef python_style[] = {
  { "character", SCE_P_CHARACTER, STRING_FG, _DEFLT_BG, Normal },
  { "classname", SCE_P_CLASSNAME, _WORD3_FG, _DEFLT_BG, Bold },
  { "commentblock", SCE_P_COMMENTBLOCK, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentline", SCE_P_COMMENTLINE, COMMNT_FG, _DEFLT_BG, Italic },
  { "decorator", SCE_P_DECORATOR, _DEFLT_FG, _DEFLT_BG, Normal },
  { "default", SCE_P_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "defname", SCE_P_DEFNAME, _DEFLT_FG, _DEFLT_BG, Normal },
  { "identifier", SCE_P_IDENTIFIER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "number", SCE_P_NUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "operator", SCE_P_OPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { "string", SCE_P_STRING, STRING_FG, _DEFLT_BG, Normal },
  { "stringeol", SCE_P_STRINGEOL, STREOL_FG, _DEFLT_BG, Normal },
  { "triple", SCE_P_TRIPLE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "tripledouble", SCE_P_TRIPLEDOUBLE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "word", SCE_P_WORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "word2", SCE_P_WORD2, _WORD2_FG, _DEFLT_BG, Bold },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* python_words[]= {
  "and as assert break class continue def del elif else except exec finally for from global if import in is lambda None not or pass print raise return try while with yield",
  "",
  NULL
};


static const char* python_mask = "*.py";


static const char* python_apps = "python";


static LangStyle LangPython = {
  "python",
  SCLEX_PYTHON,
  python_style,
  (char**)python_words,
  (char*)python_mask,
  (char*)python_apps,
  0,
  TABS_DEFAULT,
  0
};


