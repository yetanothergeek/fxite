/* lexname=pascal lexpfx=SCE_PAS_ lextag=SCLEX_PASCAL */

static StyleDef pascal_style[] = {
  { "default", SCE_PAS_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "identifier", SCE_PAS_IDENTIFIER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "comment", SCE_PAS_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "comment2", SCE_PAS_COMMENT2, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentline", SCE_PAS_COMMENTLINE, COMMNT_FG, _DEFLT_BG, Italic },
  { "preprocessor", SCE_PAS_PREPROCESSOR, PREPRC_FG, _DEFLT_BG, Normal },
  { "preprocessor2", SCE_PAS_PREPROCESSOR2, PREPRC_FG, _DEFLT_BG, Normal },
  { "number", SCE_PAS_NUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "hexnumber", SCE_PAS_HEXNUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "word", SCE_PAS_WORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "string", SCE_PAS_STRING, STRING_FG, _DEFLT_BG, Normal },
  { "stringeol", SCE_PAS_STRINGEOL, STREOL_FG, _DEFLT_BG, Normal },
  { "character", SCE_PAS_CHARACTER, STRING_FG, _DEFLT_BG, Normal },
  { "operator", SCE_PAS_OPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { "asm", SCE_PAS_ASM, _DEFLT_FG, _DEFLT_BG, Normal },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* pascal_words[]= {
  "absolute abstract and array as asm assembler automated begin case cdecl class const constructor delayed deprecated destructor dispid dispinterface div do downto dynamic else end except experimental export exports external far file final finalization finally for forward function goto helper if implementation in inherited initialization inline interface is label library message mod near nil not object of on operator or out overload override packed pascal platform private procedure program property protected public published raise record reference register reintroduce repeat resourcestring safecall sealed set shl shr static stdcall strict string then threadvar to try type unit unsafe until uses var varargs virtual while winapi with xoradd default implements index name nodefault read readonly remove stored write writeonlyabsolute abstract and array as asm assembler automated begin case cdecl class const constructor delayed deprecated destructor dispid dispinterface div do downto dynamic else end except experimental export exports external far file final finalization finally for forward function goto helper if implementation in inherited initialization inline interface is label library message mod near nil not object of on operator or out overload override packed pascal platform private procedure program property protected public published raise record reference register reintroduce repeat resourcestring safecall sealed set shl shr static stdcall strict string then threadvar to try type unit unsafe until uses var varargs virtual while winapi with xoradd default implements index name nodefault read readonly remove stored write writeonlypackage contains requires",
  NULL
};


static const char* pascal_mask = "*.dpr|*.pas|*.dfm|*.inc|*.pp";


static const char* pascal_apps = "";


static LangStyle LangPascal = {
  "pascal",
  SCLEX_PASCAL,
  pascal_style,
  (char**)pascal_words,
  (char*)pascal_mask,
  (char*)pascal_apps,
  0,
  TABS_DEFAULT,
  0
};


