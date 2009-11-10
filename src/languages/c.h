/* lexname=c lexpfx=UNKNOWN lextag=UNKNOWN */
#define SCLEX_C SCLEX_CPP

#define c_style cpp_style

static const char* c_words[]= {
  "asm auto break case char const continue default do double else enum extern float for goto if inline int long register return short signed sizeof static struct switch typedef union unsigned void volatile while",
  "",
  "a addindex addtogroup anchor arg attention author b brief bug c class code date def defgroup deprecated dontinclude e em endcode endhtmlonly endif endlatexonly endlink endverbatim enum example exception f$ f[ f] file fn hideinitializer htmlinclude htmlonly if image include ingroup internal invariant interface latexonly li line link mainpage name namespace nosubgrouping note overload p page par param param[in] param[out] post pre ref relates remarks return retval sa section see showinitializer since skip skipline struct subsection test throw todo typedef union until var verbatim verbinclude version warning weakgroup $ @  & < > # { }",
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


