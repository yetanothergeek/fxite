/* lexname=haskell lexpfx=SCE_HA_ lextag=SCLEX_HASKELL */

static StyleDef haskell_style[] = {
  { "capital", SCE_HA_CAPITAL, _DEFLT_FG, _DEFLT_BG, Normal },
  { "character", SCE_HA_CHARACTER, STRING_FG, _DEFLT_BG, Normal },
  { "class", SCE_HA_CLASS, _WORD3_FG, _DEFLT_BG, Bold },
  { "commentblock", SCE_HA_COMMENTBLOCK, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentblock2", SCE_HA_COMMENTBLOCK2, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentblock3", SCE_HA_COMMENTBLOCK3, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentline", SCE_HA_COMMENTLINE, COMMNT_FG, _DEFLT_BG, Italic },
  { "data", SCE_HA_DATA, _DEFLT_FG, _DEFLT_BG, Normal },
  { "default", SCE_HA_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "identifier", SCE_HA_IDENTIFIER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "import", SCE_HA_IMPORT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "instance", SCE_HA_INSTANCE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "keyword", SCE_HA_KEYWORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "literatecodedelim", SCE_HA_LITERATE_CODEDELIM, _DEFLT_FG, _DEFLT_BG, Normal },
  { "literatecomment", SCE_HA_LITERATE_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "module", SCE_HA_MODULE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "number", SCE_HA_NUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "operator", SCE_HA_OPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { "pragma", SCE_HA_PRAGMA, _DEFLT_FG, _DEFLT_BG, Normal },
  { "preprocessor", SCE_HA_PREPROCESSOR, PREPRC_FG, _DEFLT_BG, Normal },
  { "reservedoperator", SCE_HA_RESERVED_OPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { "string", SCE_HA_STRING, STRING_FG, _DEFLT_BG, Normal },
  { "stringeol", SCE_HA_STRINGEOL, STREOL_FG, _DEFLT_BG, Normal },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* haskell_words[]= {
  "case class data default deriving do else hiding if import in infix infixl infixr instance let module newtype of then type where forall foreign",
  "export label dynamic safe threadsafe unsafe stdcall ccall prim",
  "",
  NULL
};


static const char* haskell_mask = "";


static const char* haskell_apps = "";


static LangStyle LangHaskell = {
  "haskell",
  SCLEX_HASKELL,
  haskell_style,
  (char**)haskell_words,
  (char*)haskell_mask,
  (char*)haskell_apps,
  0,
  TABS_DEFAULT,
  0
};


