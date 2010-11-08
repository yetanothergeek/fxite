/* lexname=txt2tags lexpfx=SCE_TXT2TAGS_ lextag=SCLEX_TXT2TAGS */

static StyleDef txt2tags_style[] = {
  { "default", SCE_TXT2TAGS_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "linebegin", SCE_TXT2TAGS_LINE_BEGIN, _DEFLT_FG, _DEFLT_BG, Normal },
  { "strong1", SCE_TXT2TAGS_STRONG1, _DEFLT_FG, _DEFLT_BG, Normal },
  { "strong2", SCE_TXT2TAGS_STRONG2, _DEFLT_FG, _DEFLT_BG, Normal },
  { "em1", SCE_TXT2TAGS_EM1, _DEFLT_FG, _DEFLT_BG, Normal },
  { "em2", SCE_TXT2TAGS_EM2, _DEFLT_FG, _DEFLT_BG, Normal },
  { "header1", SCE_TXT2TAGS_HEADER1, _DEFLT_FG, _DEFLT_BG, Normal },
  { "header2", SCE_TXT2TAGS_HEADER2, _DEFLT_FG, _DEFLT_BG, Normal },
  { "header3", SCE_TXT2TAGS_HEADER3, _DEFLT_FG, _DEFLT_BG, Normal },
  { "header4", SCE_TXT2TAGS_HEADER4, _DEFLT_FG, _DEFLT_BG, Normal },
  { "header5", SCE_TXT2TAGS_HEADER5, _DEFLT_FG, _DEFLT_BG, Normal },
  { "header6", SCE_TXT2TAGS_HEADER6, _DEFLT_FG, _DEFLT_BG, Normal },
  { "prechar", SCE_TXT2TAGS_PRECHAR, _DEFLT_FG, _DEFLT_BG, Normal },
  { "ulistitem", SCE_TXT2TAGS_ULIST_ITEM, _DEFLT_FG, _DEFLT_BG, Normal },
  { "olistitem", SCE_TXT2TAGS_OLIST_ITEM, _DEFLT_FG, _DEFLT_BG, Normal },
  { "blockquote", SCE_TXT2TAGS_BLOCKQUOTE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "strikeout", SCE_TXT2TAGS_STRIKEOUT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "hrule", SCE_TXT2TAGS_HRULE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "link", SCE_TXT2TAGS_LINK, _DEFLT_FG, _DEFLT_BG, Normal },
  { "code", SCE_TXT2TAGS_CODE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "code2", SCE_TXT2TAGS_CODE2, _DEFLT_FG, _DEFLT_BG, Normal },
  { "codebk", SCE_TXT2TAGS_CODEBK, _DEFLT_FG, _DEFLT_BG, Normal },
  { "comment", SCE_TXT2TAGS_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "option", SCE_TXT2TAGS_OPTION, _DEFLT_FG, _DEFLT_BG, Normal },
  { "preproc", SCE_TXT2TAGS_PREPROC, _DEFLT_FG, _DEFLT_BG, Normal },
  { "postproc", SCE_TXT2TAGS_POSTPROC, _DEFLT_FG, _DEFLT_BG, Normal },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* txt2tags_words[]= {NULL};


static const char* txt2tags_mask = "*.t2t";


static const char* txt2tags_apps = "";


static LangStyle LangTxt2tags = {
  "txt2tags",
  SCLEX_TXT2TAGS,
  txt2tags_style,
  (char**)txt2tags_words,
  (char*)txt2tags_mask,
  (char*)txt2tags_apps,
  0,
  TABS_DEFAULT
};


