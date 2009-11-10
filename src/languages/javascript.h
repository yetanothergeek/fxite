/* lexname=javascript lexpfx=UNKNOWN lextag=UNKNOWN */
#define SCLEX_JAVASCRIPT SCLEX_CPP

#define javascript_style cpp_style

static const char* javascript_words[]= {
  "",
  "",
  "",
  "",
  "",
  NULL
};


static const char* javascript_mask = "";


static const char* javascript_apps = "";


static LangStyle LangJavascript = {
  "javascript",
  SCLEX_JAVASCRIPT,
  javascript_style,
  (char**)javascript_words,
  (char*)javascript_mask,
  (char*)javascript_apps,
  0,
  TABS_DEFAULT
};


