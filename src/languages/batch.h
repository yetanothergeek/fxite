/* lexname=batch lexpfx=SCE_BAT_ lextag=SCLEX_BATCH */

static StyleDef batch_style[] = {
  { "default", SCE_BAT_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "comment", SCE_BAT_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "word", SCE_BAT_WORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "label", SCE_BAT_LABEL, _DEFLT_FG, _DEFLT_BG, Normal },
  { "hide", SCE_BAT_HIDE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "command", SCE_BAT_COMMAND, _DEFLT_FG, _DEFLT_BG, Normal },
  { "identifier", SCE_BAT_IDENTIFIER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "operator", SCE_BAT_OPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* batch_words[]= {
  "rem set if exist errorlevel for in do break call chcp cd chdir choice cls country ctty date del erase dir echo exit goto loadfix loadhigh mkdir md move path pause prompt rename ren rmdir rd shift time type ver verify vol com con lpt nul color copy defined else not start",
  "",
  NULL
};


static const char* batch_mask = "*.bat|*.cmd|*.nt";


static const char* batch_apps = "";


static LangStyle LangBatch = {
  "batch",
  SCLEX_BATCH,
  batch_style,
  (char**)batch_words,
  (char*)batch_mask,
  (char*)batch_apps,
  0,
  TABS_DEFAULT,
  0
};


