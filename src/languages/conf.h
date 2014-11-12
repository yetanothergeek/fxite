/* lexname=conf lexpfx=SCE_CONF_ lextag=SCLEX_CONF */

static StyleDef conf_style[] = {
  { "comment", SCE_CONF_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "default", SCE_CONF_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "directive", SCE_CONF_DIRECTIVE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "extension", SCE_CONF_EXTENSION, _DEFLT_FG, _DEFLT_BG, Normal },
  { "identifier", SCE_CONF_IDENTIFIER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "ip", SCE_CONF_IP, _DEFLT_FG, _DEFLT_BG, Normal },
  { "number", SCE_CONF_NUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "operator", SCE_CONF_OPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { "parameter", SCE_CONF_PARAMETER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "string", SCE_CONF_STRING, STRING_FG, _DEFLT_BG, Normal },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* conf_words[]= {
  "",
  "",
  NULL
};


static const char* conf_mask = "*.conf|.htaccess";


static const char* conf_apps = "";


static LangStyle LangConf = {
  "conf",
  SCLEX_CONF,
  conf_style,
  (char**)conf_words,
  (char*)conf_mask,
  (char*)conf_apps,
  0,
  TABS_DEFAULT,
  0
};


