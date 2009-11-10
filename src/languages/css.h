/* lexname=css lexpfx=SCE_CSS_ lextag=SCLEX_CSS */

static StyleDef css_style[] = {
  { "default", SCE_CSS_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "tag", SCE_CSS_TAG, _DEFLT_FG, _DEFLT_BG, Normal },
  { "class", SCE_CSS_CLASS, _WORD3_FG, _DEFLT_BG, Bold },
  { "pseudoclass", SCE_CSS_PSEUDOCLASS, _WORD3_FG, _DEFLT_BG, Bold },
  { "unknownpseudoclass", SCE_CSS_UNKNOWN_PSEUDOCLASS, STREOL_FG, _DEFLT_BG, Normal },
  { "operator", SCE_CSS_OPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { "identifier", SCE_CSS_IDENTIFIER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "unknownidentifier", SCE_CSS_UNKNOWN_IDENTIFIER, STREOL_FG, _DEFLT_BG, Normal },
  { "value", SCE_CSS_VALUE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "comment", SCE_CSS_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "id", SCE_CSS_ID, _DEFLT_FG, _DEFLT_BG, Normal },
  { "important", SCE_CSS_IMPORTANT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "directive", SCE_CSS_DIRECTIVE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "doublestring", SCE_CSS_DOUBLESTRING, STRING_FG, _DEFLT_BG, Normal },
  { "singlestring", SCE_CSS_SINGLESTRING, STRING_FG, _DEFLT_BG, Normal },
  { "identifier2", SCE_CSS_IDENTIFIER2, _DEFLT_FG, _DEFLT_BG, Normal },
  { "attribute", SCE_CSS_ATTRIBUTE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "identifier3", SCE_CSS_IDENTIFIER3, _DEFLT_FG, _DEFLT_BG, Normal },
  { "pseudoelement", SCE_CSS_PSEUDOELEMENT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "extendedidentifier", SCE_CSS_EXTENDED_IDENTIFIER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "extendedpseudoclass", SCE_CSS_EXTENDED_PSEUDOCLASS, _WORD3_FG, _DEFLT_BG, Bold },
  { "extendedpseudoelement", SCE_CSS_EXTENDED_PSEUDOELEMENT, _DEFLT_FG, _DEFLT_BG, Normal },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* css_words[]= {
  "color background-color background-image background-repeat background-attachment background-position background font-family font-style font-variant font-weight font-size font word-spacing letter-spacing text-decoration vertical-align text-transform text-align text-indent line-height margin-top margin-right margin-bottom margin-left margin padding-top padding-right padding-bottom padding-left padding border-top-width border-right-width border-bottom-width border-left-width border-width border-top border-right border-bottom border-left border border-color border-style width height float clear display white-space list-style-type list-style-image list-style-position list-style",
  "first-letter first-line link active visited first-child focus hover lang before after left right first",
  "border-top-color border-right-color border-bottom-color border-left-color border-color border-top-style border-right-style border-bottom-style border-left-style border-style top right bottom left position z-index direction unicode-bidi min-width max-width min-height max-height overflow clip visibility content quotes counter-reset counter-increment marker-offset size marks page-break-before page-break-after page-break-inside page orphans widows font-stretch font-size-adjust unicode-range units-per-em src panose-1 stemv stemh slope cap-height x-height ascent descent widths bbox definition-src baseline centerline mathline topline text-shadow caption-side table-layout border-collapse border-spacing empty-cells speak-header cursor outline outline-width outline-style outline-color volume speak pause-before pause-after pause cue-before cue-after cue play-during azimuth elevation speech-rate voice-family pitch pitch-range stress richness speak-punctuation speak-numeral",
  "",
  "",
  "",
  "",
  "",
  NULL
};


static const char* css_mask = "";


static const char* css_apps = "";


static LangStyle LangCSS = {
  "css",
  SCLEX_CSS,
  css_style,
  (char**)css_words,
  (char*)css_mask,
  (char*)css_apps,
  0,
  TABS_DEFAULT
};


