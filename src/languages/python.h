/* lexname=python lexpfx=SCE_P_ lextag=SCLEX_PYTHON */

static StyleDef python_style[] = {
  { "default", SCE_P_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "commentline", SCE_P_COMMENTLINE, COMMNT_FG, _DEFLT_BG, Italic },
  { "number", SCE_P_NUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "string", SCE_P_STRING, STRING_FG, _DEFLT_BG, Normal },
  { "character", SCE_P_CHARACTER, STRING_FG, _DEFLT_BG, Normal },
  { "word", SCE_P_WORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "triple", SCE_P_TRIPLE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "tripledouble", SCE_P_TRIPLEDOUBLE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "classname", SCE_P_CLASSNAME, _WORD3_FG, _DEFLT_BG, Bold },
  { "defname", SCE_P_DEFNAME, _DEFLT_FG, _DEFLT_BG, Normal },
  { "operator", SCE_P_OPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { "identifier", SCE_P_IDENTIFIER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "commentblock", SCE_P_COMMENTBLOCK, COMMNT_FG, _DEFLT_BG, Italic },
  { "stringeol", SCE_P_STRINGEOL, STREOL_FG, _DEFLT_BG, Normal },
  { "word2", SCE_P_WORD2, _WORD2_FG, _DEFLT_BG, Bold },
  { "decorator", SCE_P_DECORATOR, _DEFLT_FG, _DEFLT_BG, Normal },
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


