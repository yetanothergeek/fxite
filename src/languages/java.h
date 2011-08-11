/* lexname=java lexpfx=UNKNOWN lextag=UNKNOWN */
#define SCLEX_JAVA SCLEX_CPP

#define java_style cpp_style

static const char* java_words[]= {
  "abstract assert boolean break byte case catch char class const continue default do double else enum extends final finally float for goto if implements import instanceof int interface long native new package private protected public return short static strictfp super switch synchronized this throw throws transient try var void volatile while",
  "",
  "",
  "",
  "",
  NULL
};


static const char* java_mask = "*.java|*.jad|*.pde";


static const char* java_apps = "";


static LangStyle LangJava = {
  "java",
  SCLEX_JAVA,
  java_style,
  (char**)java_words,
  (char*)java_mask,
  (char*)java_apps,
  0,
  TABS_DEFAULT,
  0
};


