/* lexname=errorlist lexpfx=SCE_ERR_ lextag=SCLEX_ERRORLIST */

static StyleDef errorlist_style[] = {
  { "absf", SCE_ERR_ABSF, _DEFLT_FG, _DEFLT_BG, Normal },
  { "borland", SCE_ERR_BORLAND, _DEFLT_FG, _DEFLT_BG, Normal },
  { "cmd", SCE_ERR_CMD, _DEFLT_FG, _DEFLT_BG, Normal },
  { "ctag", SCE_ERR_CTAG, _DEFLT_FG, _DEFLT_BG, Normal },
  { "default", SCE_ERR_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "diffaddition", SCE_ERR_DIFF_ADDITION, _DEFLT_FG, _DEFLT_BG, Normal },
  { "diffchanged", SCE_ERR_DIFF_CHANGED, _DEFLT_FG, _DEFLT_BG, Normal },
  { "diffdeletion", SCE_ERR_DIFF_DELETION, _DEFLT_FG, _DEFLT_BG, Normal },
  { "diffmessage", SCE_ERR_DIFF_MESSAGE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "elf", SCE_ERR_ELF, _DEFLT_FG, _DEFLT_BG, Normal },
  { "gcc", SCE_ERR_GCC, _DEFLT_FG, _DEFLT_BG, Normal },
  { "gccincludedfrom", SCE_ERR_GCC_INCLUDED_FROM, _DEFLT_FG, _DEFLT_BG, Normal },
  { "ifc", SCE_ERR_IFC, _DEFLT_FG, _DEFLT_BG, Normal },
  { "ifort", SCE_ERR_IFORT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "javastack", SCE_ERR_JAVA_STACK, _DEFLT_FG, _DEFLT_BG, Normal },
  { "lua", SCE_ERR_LUA, _DEFLT_FG, _DEFLT_BG, Normal },
  { "ms", SCE_ERR_MS, _DEFLT_FG, _DEFLT_BG, Normal },
  { "net", SCE_ERR_NET, _DEFLT_FG, _DEFLT_BG, Normal },
  { "perl", SCE_ERR_PERL, _DEFLT_FG, _DEFLT_BG, Normal },
  { "php", SCE_ERR_PHP, _DEFLT_FG, _DEFLT_BG, Normal },
  { "python", SCE_ERR_PYTHON, _DEFLT_FG, _DEFLT_BG, Normal },
  { "tidy", SCE_ERR_TIDY, _DEFLT_FG, _DEFLT_BG, Normal },
  { "value", SCE_ERR_VALUE, _DEFLT_FG, _DEFLT_BG, Normal },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* errorlist_words[]= {NULL};


static const char* errorlist_mask = "";


static const char* errorlist_apps = "";


static LangStyle LangErrorList = {
  "errorlist",
  SCLEX_ERRORLIST,
  errorlist_style,
  (char**)errorlist_words,
  (char*)errorlist_mask,
  (char*)errorlist_apps,
  0,
  TABS_DEFAULT,
  0
};


