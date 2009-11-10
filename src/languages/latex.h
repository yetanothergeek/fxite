/* lexname=latex lexpfx=SCE_L_ lextag=SCLEX_LATEX */

static StyleDef latex_style[] = {
  { "default", SCE_L_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "command", SCE_L_COMMAND, _DEFLT_FG, _DEFLT_BG, Normal },
  { "tag", SCE_L_TAG, _DEFLT_FG, _DEFLT_BG, Normal },
  { "math", SCE_L_MATH, _DEFLT_FG, _DEFLT_BG, Normal },
  { "comment", SCE_L_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* latex_words[]= {NULL};


static const char* latex_mask = "*.tex|*.sty|*.aux|*.toc|*.idx";


static const char* latex_apps = "";


static LangStyle LangLaTeX = {
  "latex",
  SCLEX_LATEX,
  latex_style,
  (char**)latex_words,
  (char*)latex_mask,
  (char*)latex_apps,
  0,
  TABS_DEFAULT
};


