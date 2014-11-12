/* lexname=markdown lexpfx=SCE_MARKDOWN_ lextag=SCLEX_MARKDOWN */

static StyleDef markdown_style[] = {
  { "blockquote", SCE_MARKDOWN_BLOCKQUOTE, "#00c000", _DEFLT_BG, Normal },
  { "code", SCE_MARKDOWN_CODE, "#00aaaa", _DEFLT_BG, Normal },
  { "code2", SCE_MARKDOWN_CODE2, "#00bbbb", _DEFLT_BG, Normal },
  { "codebk", SCE_MARKDOWN_CODEBK, "#00cccc", _DEFLT_BG, Normal },
  { "default", SCE_MARKDOWN_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "em1", SCE_MARKDOWN_EM1, "#808080", _DEFLT_BG, Bold },
  { "em2", SCE_MARKDOWN_EM2, _DEFLT_FG, _DEFLT_BG, Bold },
  { "header1", SCE_MARKDOWN_HEADER1, "#ddb0b0", _DEFLT_BG, Bold },
  { "header2", SCE_MARKDOWN_HEADER2, "#dda0a0", _DEFLT_BG, Bold },
  { "header3", SCE_MARKDOWN_HEADER3, "#dd8080", _DEFLT_BG, Bold },
  { "header4", SCE_MARKDOWN_HEADER4, "#dd6060", _DEFLT_BG, Bold },
  { "header5", SCE_MARKDOWN_HEADER5, "#cc6060", _DEFLT_BG, Bold },
  { "header6", SCE_MARKDOWN_HEADER6, "#bb4040", _DEFLT_BG, Bold },
  { "hrule", SCE_MARKDOWN_HRULE, "#ffaa00", _DEFLT_BG, Normal },
  { "linebegin", SCE_MARKDOWN_LINE_BEGIN, _DEFLT_FG, _DEFLT_BG, Normal },
  { "link", SCE_MARKDOWN_LINK, "#0000ee", _DEFLT_BG, Bold },
  { "olistitem", SCE_MARKDOWN_OLIST_ITEM, "#4040e0", _DEFLT_BG, Normal },
  { "prechar", SCE_MARKDOWN_PRECHAR, "#cc00cc", _DEFLT_BG, Normal },
  { "strikeout", SCE_MARKDOWN_STRIKEOUT, "#ee0000", _DEFLT_BG, Normal },
  { "strong1", SCE_MARKDOWN_STRONG1, "#808080", _DEFLT_BG, Bold },
  { "strong2", SCE_MARKDOWN_STRONG2, _DEFLT_FG, _DEFLT_BG, Bold },
  { "ulistitem", SCE_MARKDOWN_ULIST_ITEM, "#4040e0", _DEFLT_BG, Normal },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* markdown_words[]= {NULL};


static const char* markdown_mask = "";


static const char* markdown_apps = "";


static LangStyle LangMarkdown = {
  "markdown",
  SCLEX_MARKDOWN,
  markdown_style,
  (char**)markdown_words,
  (char*)markdown_mask,
  (char*)markdown_apps,
  0,
  TABS_DEFAULT,
  0
};


