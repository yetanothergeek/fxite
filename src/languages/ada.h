/* lexname=ada lexpfx=SCE_ADA_ lextag=SCLEX_ADA */

static StyleDef ada_style[] = {
  { "character", SCE_ADA_CHARACTER, STRING_FG, _DEFLT_BG, Normal },
  { "charactereol", SCE_ADA_CHARACTEREOL, STREOL_FG, _DEFLT_BG, Normal },
  { "commentline", SCE_ADA_COMMENTLINE, COMMNT_FG, _DEFLT_BG, Italic },
  { "default", SCE_ADA_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "delimiter", SCE_ADA_DELIMITER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "identifier", SCE_ADA_IDENTIFIER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "illegal", SCE_ADA_ILLEGAL, _DEFLT_FG, _DEFLT_BG, Normal },
  { "label", SCE_ADA_LABEL, _DEFLT_FG, _DEFLT_BG, Normal },
  { "number", SCE_ADA_NUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "string", SCE_ADA_STRING, STRING_FG, _DEFLT_BG, Normal },
  { "stringeol", SCE_ADA_STRINGEOL, STREOL_FG, _DEFLT_BG, Normal },
  { "word", SCE_ADA_WORD, _WORD1_FG, _DEFLT_BG, Bold },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* ada_words[]= {
  "abort abstract accept access aliased all array at begin body case constant declare delay delta digits do else elsif end entry exception exit for function generic goto if in is limited loop new null of others out package pragma private procedure protected raise range record renames requeue return reverse select separate subtype tagged task terminate then type until use when while with abs and mod not or rem xor",
  NULL
};


static const char* ada_mask = "*.ads|*.adb";


static const char* ada_apps = "";


static LangStyle LangAda = {
  "ada",
  SCLEX_ADA,
  ada_style,
  (char**)ada_words,
  (char*)ada_mask,
  (char*)ada_apps,
  0,
  TABS_DEFAULT,
  0
};


