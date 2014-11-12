/* lexname=cpp lexpfx=SCE_C_ lextag=SCLEX_CPP */

static StyleDef cpp_style[] = {
  { "character", SCE_C_CHARACTER, STRING_FG, _DEFLT_BG, Normal },
  { "comment", SCE_C_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentdoc", SCE_C_COMMENTDOC, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentdockeyword", SCE_C_COMMENTDOCKEYWORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "commentdockeyworderror", SCE_C_COMMENTDOCKEYWORDERROR, _WORD1_FG, _DEFLT_BG, Bold },
  { "commentline", SCE_C_COMMENTLINE, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentlinedoc", SCE_C_COMMENTLINEDOC, COMMNT_FG, _DEFLT_BG, Italic },
  { "default", SCE_C_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "escapesequence", SCE_C_ESCAPESEQUENCE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "globalclass", SCE_C_GLOBALCLASS, _WORD3_FG, _DEFLT_BG, Bold },
  { "hashquotedstring", SCE_C_HASHQUOTEDSTRING, STRING_FG, _DEFLT_BG, Normal },
  { "identifier", SCE_C_IDENTIFIER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "number", SCE_C_NUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "operator", SCE_C_OPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { "preprocessor", SCE_C_PREPROCESSOR, PREPRC_FG, _DEFLT_BG, Normal },
  { "preprocessorcomment", SCE_C_PREPROCESSORCOMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "preprocessorcommentdoc", SCE_C_PREPROCESSORCOMMENTDOC, COMMNT_FG, _DEFLT_BG, Italic },
  { "regex", SCE_C_REGEX, _DEFLT_FG, _DEFLT_BG, Normal },
  { "string", SCE_C_STRING, STRING_FG, _DEFLT_BG, Normal },
  { "stringeol", SCE_C_STRINGEOL, STREOL_FG, _DEFLT_BG, Normal },
  { "stringraw", SCE_C_STRINGRAW, STRING_FG, _DEFLT_BG, Normal },
  { "taskmarker", SCE_C_TASKMARKER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "tripleverbatim", SCE_C_TRIPLEVERBATIM, _DEFLT_FG, _DEFLT_BG, Normal },
  { "userliteral", SCE_C_USERLITERAL, _DEFLT_FG, _DEFLT_BG, Normal },
  { "uuid", SCE_C_UUID, _DEFLT_FG, _DEFLT_BG, Normal },
  { "verbatim", SCE_C_VERBATIM, _DEFLT_FG, _DEFLT_BG, Normal },
  { "word", SCE_C_WORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "word2", SCE_C_WORD2, _WORD2_FG, _DEFLT_BG, Bold },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* cpp_words[]= {
  "and and_eq asm auto bitand bitor bool break case catch char class compl const const_cast continue default delete do double dynamic_cast else enum explicit export extern false float for friend goto if inline int long mutable namespace new not not_eq operator or or_eq private protected public register reinterpret_cast return short signed sizeof static static_cast struct switch template this throw true try typedef typeid typename union unsigned using virtual void volatile wchar_t while xor xor_eq",
  "alignas alignof char16_t char32_t constexpr decltype noexcept nullptr static_assert thread_local",
  "",
  "",
  "",
  "",
  NULL
};


static const char* cpp_mask = "*.cc|*.cpp|*.cxx|*.hh|*.hpp|*.hxx|*.ipp|*.sma";


static const char* cpp_apps = "";


static LangStyle LangCpp = {
  "cpp",
  SCLEX_CPP,
  cpp_style,
  (char**)cpp_words,
  (char*)cpp_mask,
  (char*)cpp_apps,
  0,
  TABS_DEFAULT,
  0
};


