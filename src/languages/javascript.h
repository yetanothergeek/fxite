/* lexname=javascript lexpfx=UNKNOWN lextag=UNKNOWN */
#define SCLEX_JAVASCRIPT SCLEX_CPP

#define javascript_style cpp_style

static const char* javascript_words[]= {
  "abstract boolean break byte case catch char class const continue debugger default delete do double else enum export extends final finally float for function goto if implements import in instanceof int interface long native new package private protected public return short static super switch synchronized this throw throws transient try typeof var void volatile while with",
  "",
  "",
  "",
  "",
  NULL
};


static const char* javascript_mask = "*.js";


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


