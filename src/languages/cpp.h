/* lexname=cpp lexpfx=SCE_C_ lextag=SCLEX_CPP */

static StyleDef cpp_style[] = {
  { "default", SCE_C_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "comment", SCE_C_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentline", SCE_C_COMMENTLINE, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentdoc", SCE_C_COMMENTDOC, COMMNT_FG, _DEFLT_BG, Italic },
  { "number", SCE_C_NUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "word", SCE_C_WORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "string", SCE_C_STRING, STRING_FG, _DEFLT_BG, Normal },
  { "character", SCE_C_CHARACTER, STRING_FG, _DEFLT_BG, Normal },
  { "uuid", SCE_C_UUID, _DEFLT_FG, _DEFLT_BG, Normal },
  { "preprocessor", SCE_C_PREPROCESSOR, PREPRC_FG, _DEFLT_BG, Normal },
  { "operator", SCE_C_OPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { "identifier", SCE_C_IDENTIFIER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "stringeol", SCE_C_STRINGEOL, STREOL_FG, _DEFLT_BG, Normal },
  { "verbatim", SCE_C_VERBATIM, _DEFLT_FG, _DEFLT_BG, Normal },
  { "regex", SCE_C_REGEX, _DEFLT_FG, _DEFLT_BG, Normal },
  { "commentlinedoc", SCE_C_COMMENTLINEDOC, COMMNT_FG, _DEFLT_BG, Italic },
  { "word2", SCE_C_WORD2, _WORD2_FG, _DEFLT_BG, Bold },
  { "commentdockeyword", SCE_C_COMMENTDOCKEYWORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "commentdockeyworderror", SCE_C_COMMENTDOCKEYWORDERROR, _WORD1_FG, _DEFLT_BG, Bold },
  { "globalclass", SCE_C_GLOBALCLASS, _WORD3_FG, _DEFLT_BG, Bold },
  { "stringraw", SCE_C_STRINGRAW, STRING_FG, _DEFLT_BG, Normal },
  { "tripleverbatim", SCE_C_TRIPLEVERBATIM, _DEFLT_FG, _DEFLT_BG, Normal },
  { "hashquotedstring", SCE_C_HASHQUOTEDSTRING, STRING_FG, _DEFLT_BG, Normal },
  { "preprocessorcomment", SCE_C_PREPROCESSORCOMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "preprocessorcommentdoc", SCE_C_PREPROCESSORCOMMENTDOC, COMMNT_FG, _DEFLT_BG, Italic },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* cpp_words[]= {
  "and and_eq asm auto bitand bitor bool break case catch char class compl const const_cast continue default delete do double dynamic_cast else enum explicit export extern false float for friend goto if inline int long mutable namespace new not not_eq operator or or_eq private protected public register reinterpret_cast return short signed sizeof static static_cast struct switch template this throw true try typedef typeid typename union unsigned using virtual void volatile wchar_t while xor xor_eq@class @defs @protocol @required @optional @end @interface @public @package @protected @private @property @implementation @synthesize @dynamic @throw @try @catch @finally @synchronized @autoreleasepool @selector @encode @compatibility_alias",
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


