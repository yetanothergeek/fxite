/* lexname=d lexpfx=SCE_D_ lextag=SCLEX_D */

static StyleDef d_style[] = {
  { "default", SCE_D_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "comment", SCE_D_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentline", SCE_D_COMMENTLINE, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentdoc", SCE_D_COMMENTDOC, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentnested", SCE_D_COMMENTNESTED, COMMNT_FG, _DEFLT_BG, Italic },
  { "number", SCE_D_NUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "word", SCE_D_WORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "word2", SCE_D_WORD2, _WORD2_FG, _DEFLT_BG, Bold },
  { "word3", SCE_D_WORD3, _WORD1_FG, _DEFLT_BG, Bold },
  { "typedef", SCE_D_TYPEDEF, _DEFLT_FG, _DEFLT_BG, Normal },
  { "string", SCE_D_STRING, STRING_FG, _DEFLT_BG, Normal },
  { "stringeol", SCE_D_STRINGEOL, STREOL_FG, _DEFLT_BG, Normal },
  { "character", SCE_D_CHARACTER, STRING_FG, _DEFLT_BG, Normal },
  { "operator", SCE_D_OPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { "identifier", SCE_D_IDENTIFIER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "commentlinedoc", SCE_D_COMMENTLINEDOC, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentdockeyword", SCE_D_COMMENTDOCKEYWORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "commentdockeyworderror", SCE_D_COMMENTDOCKEYWORDERROR, _WORD1_FG, _DEFLT_BG, Bold },
  { "stringb", SCE_D_STRINGB, STRING_FG, _DEFLT_BG, Normal },
  { "stringr", SCE_D_STRINGR, STRING_FG, _DEFLT_BG, Normal },
  { "word5", SCE_D_WORD5, _WORD1_FG, _DEFLT_BG, Bold },
  { "word6", SCE_D_WORD6, _WORD1_FG, _DEFLT_BG, Bold },
  { "word7", SCE_D_WORD7, _WORD1_FG, _DEFLT_BG, Bold },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* d_words[]= {
  "abstract alias align asm assert auto body bool break byte case cast catch cdouble cent cfloat char class const continue creal dchar debug default delegate delete deprecated do double else enum export extern false final finally float for foreach foreach_reverse function goto idouble if ifloat import in inout int interface invariant ireal is lazy long mixin module new null out override package pragma private protected public real return scope short static struct super switch synchronized template this throw true try typedef typeid typeof ubyte ucent uint ulong union unittest ushort version void volatile wchar while with",
  "",
  "a addindex addtogroup anchor arg attention author b brief bug c class code date def defgroup deprecated dontinclude e em endcode endhtmlonly endif endlatexonly endlink endverbatim enum example exception f$ f[ f] file fn hideinitializer htmlinclude htmlonly if image include ingroup internal invariant interface latexonly li line link mainpage name namespace nosubgrouping note overload p page par param post pre ref relates remarks return retval sa section see showinitializer since skip skipline struct subsection test throw todo typedef union until var verbatim verbinclude version warning weakgroup $ @  & < > # { }",
  "",
  "",
  "",
  "",
  NULL
};


static const char* d_mask = "*.d";


static const char* d_apps = "";


static LangStyle LangD = {
  "d",
  SCLEX_D,
  d_style,
  (char**)d_words,
  (char*)d_mask,
  (char*)d_apps,
  0,
  TABS_DEFAULT,
  0
};


