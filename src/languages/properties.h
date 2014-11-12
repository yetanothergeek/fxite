/* lexname=properties lexpfx=SCE_PROPS_ lextag=SCLEX_PROPERTIES */

static StyleDef properties_style[] = {
  { "assignment", SCE_PROPS_ASSIGNMENT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "comment", SCE_PROPS_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "default", SCE_PROPS_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "defval", SCE_PROPS_DEFVAL, _DEFLT_FG, _DEFLT_BG, Normal },
  { "key", SCE_PROPS_KEY, _DEFLT_FG, _DEFLT_BG, Normal },
  { "section", SCE_PROPS_SECTION, _DEFLT_FG, _DEFLT_BG, Normal },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* properties_words[]= {NULL};


static const char* properties_mask = "";


static const char* properties_apps = "";


static LangStyle LangProperties = {
  "properties",
  SCLEX_PROPERTIES,
  properties_style,
  (char**)properties_words,
  (char*)properties_mask,
  (char*)properties_apps,
  0,
  TABS_DEFAULT,
  0
};


