/* lexname=c lexpfx=UNKNOWN lextag=UNKNOWN */
#define SCLEX_C SCLEX_CPP

#define c_style cpp_style

static const char* c_words[]= {
  "asm auto break case char const continue default do double else enum extern float for goto if inline int long register return short signed sizeof static struct switch typedef union unsigned void volatile while",
  "",
  "",
  "",
  "",
  NULL
};


static const char* c_mask = "*.c";


static const char* c_apps = "tcc";


static LangStyle LangC = {
  "c",
  SCLEX_C,
  c_style,
  (char**)c_words,
  (char*)c_mask,
  (char*)c_apps,
  0,
  TABS_DEFAULT
};


