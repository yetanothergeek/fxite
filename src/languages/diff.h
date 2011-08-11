/* lexname=diff lexpfx=SCE_DIFF_ lextag=SCLEX_DIFF */

static StyleDef diff_style[] = {
  { "default", SCE_DIFF_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "comment", SCE_DIFF_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "command", SCE_DIFF_COMMAND, _DEFLT_FG, _DEFLT_BG, Normal },
  { "header", SCE_DIFF_HEADER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "position", SCE_DIFF_POSITION, _DEFLT_FG, _DEFLT_BG, Normal },
  { "deleted", SCE_DIFF_DELETED, _DEFLT_FG, _DEFLT_BG, Normal },
  { "added", SCE_DIFF_ADDED, _DEFLT_FG, _DEFLT_BG, Normal },
  { "changed", SCE_DIFF_CHANGED, _DEFLT_FG, _DEFLT_BG, Normal },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* diff_words[]= {NULL};


static const char* diff_mask = "*.diff|*.patch";


static const char* diff_apps = "";


static LangStyle LangDiff = {
  "diff",
  SCLEX_DIFF,
  diff_style,
  (char**)diff_words,
  (char*)diff_mask,
  (char*)diff_apps,
  0,
  TABS_DEFAULT,
  0
};


