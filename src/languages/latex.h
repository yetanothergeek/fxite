/* lexname=latex lexpfx=SCE_L_ lextag=SCLEX_LATEX */

static StyleDef latex_style[] = {
  { "default", SCE_L_DEFAULT, "#000000", _DEFLT_BG, Normal },
  { "command", SCE_L_COMMAND, "#AA0000", _DEFLT_BG, Bold },
  { "tag", SCE_L_TAG, "#880088", _DEFLT_BG, Bold },
  { "math", SCE_L_MATH, "#0000FF", _DEFLT_BG, Normal },
  { "comment", SCE_L_COMMENT, "#00AA00", _DEFLT_BG, Italic },
  { "tag2", SCE_L_TAG2, "#880088", _DEFLT_BG, Bold },
  { "math2", SCE_L_MATH2, "#0000AA", _DEFLT_BG, Normal },
  { "comment2", SCE_L_COMMENT2, "#00AA00", _DEFLT_BG, Italic },
  { "verbatim", SCE_L_VERBATIM, "#666666", _DEFLT_BG, Normal },
  { "shortcmd", SCE_L_SHORTCMD, "#AA6600", _DEFLT_BG, Bold },
  { "special", SCE_L_SPECIAL, "#AAAA00", _DEFLT_BG, Normal },
  { "cmdopt", SCE_L_CMDOPT, "#FF9900", _DEFLT_BG, Normal },
  { "error", SCE_L_ERROR, "#ffffff", "#ff0000", Normal },
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


