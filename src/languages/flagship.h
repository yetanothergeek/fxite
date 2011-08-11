/* lexname=flagship lexpfx=SCE_FS_ lextag=SCLEX_FLAGSHIP */

static StyleDef flagship_style[] = {
  { "default", SCE_FS_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "comment", SCE_FS_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentline", SCE_FS_COMMENTLINE, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentdoc", SCE_FS_COMMENTDOC, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentlinedoc", SCE_FS_COMMENTLINEDOC, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentdockeyword", SCE_FS_COMMENTDOCKEYWORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "commentdockeyworderror", SCE_FS_COMMENTDOCKEYWORDERROR, _WORD1_FG, _DEFLT_BG, Bold },
  { "keyword", SCE_FS_KEYWORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "keyword2", SCE_FS_KEYWORD2, _WORD2_FG, _DEFLT_BG, Bold },
  { "keyword3", SCE_FS_KEYWORD3, _WORD1_FG, _DEFLT_BG, Bold },
  { "keyword4", SCE_FS_KEYWORD4, _WORD1_FG, _DEFLT_BG, Bold },
  { "number", SCE_FS_NUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "string", SCE_FS_STRING, STRING_FG, _DEFLT_BG, Normal },
  { "preprocessor", SCE_FS_PREPROCESSOR, PREPRC_FG, _DEFLT_BG, Normal },
  { "operator", SCE_FS_OPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { "identifier", SCE_FS_IDENTIFIER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "date", SCE_FS_DATE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "stringeol", SCE_FS_STRINGEOL, STREOL_FG, _DEFLT_BG, Normal },
  { "constant", SCE_FS_CONSTANT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "wordoperator", SCE_FS_WORDOPERATOR, _WORD1_FG, _DEFLT_BG, Bold },
  { "disabledcode", SCE_FS_DISABLEDCODE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "defaultc", SCE_FS_DEFAULT_C, _DEFLT_FG, _DEFLT_BG, Normal },
  { "commentdocc", SCE_FS_COMMENTDOC_C, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentlinedocc", SCE_FS_COMMENTLINEDOC_C, COMMNT_FG, _DEFLT_BG, Italic },
  { "keywordc", SCE_FS_KEYWORD_C, _WORD1_FG, _DEFLT_BG, Bold },
  { "keyword2c", SCE_FS_KEYWORD2_C, _WORD2_FG, _DEFLT_BG, Bold },
  { "numberc", SCE_FS_NUMBER_C, NUMBER_FG, _DEFLT_BG, Normal },
  { "stringc", SCE_FS_STRING_C, STRING_FG, _DEFLT_BG, Normal },
  { "preprocessorc", SCE_FS_PREPROCESSOR_C, PREPRC_FG, _DEFLT_BG, Normal },
  { "operatorc", SCE_FS_OPERATOR_C, OPERTR_FG, _DEFLT_BG, Bold },
  { "identifierc", SCE_FS_IDENTIFIER_C, _DEFLT_FG, _DEFLT_BG, Normal },
  { "stringeolc", SCE_FS_STRINGEOL_C, STREOL_FG, _DEFLT_BG, Normal },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* flagship_words[]= {
  "? @ accept access all alternate announce ansi any append as assign autolock average begin bell bitmap blank box call cancel case century charset checkbox clear close cls color combobox commit confirm console constant continue copy count create cursor date dbread dbwrite decimals declare default delete deleted delimiters device dir directory display do draw edit else elseif eject end endcase enddo endif endtext epoch erase error escape eval eventmask exact exclusive extended external extra field file filter find fixed font for form format from get gets global global_extern go goto gotop guialign guicolor guicursor guitransl html htmltext if image index input intensity join key keyboard keytransl label lines list listbox local locate margin memory memvar menu message method multibyte multilocks next nfs nfslock nfs_force note on openerror order outmode pack parameters path pixel pop printer private prompt public push pushbutton quit radiobutton radiogroup read recall refresh reindex relation release rename replace report request restore richtext rowadapt rowalign run save say scoreboard scrcompress screen seek select sequence set setenhanced setstandard setunselected skip softseek sort source static store struct structure sum tag tbrowse text to total type typeahead unique unlock update use wait while with wrap xml zap zerobyteout",
  "",
  "function procedure return exit",
  "class instance export hidden protect prototype",
  "a addindex addtogroup anchor arg attention author b brief bug c class code date def defgroup deprecated dontinclude e em endcode endhtmlonly endif endlatexonly endlink endverbatim enum example exception f$ f[ f] file fn hideinitializer htmlinclude htmlonly if image include ingroup internal invariant interface latexonly li line link mainpage name namespace nosubgrouping note overload p page par param param[in] param[out] post pre ref relates remarks return retval sa section see showinitializer since skip skipline struct subsection test throw throws todo typedef union until var verbatim verbinclude version warning weakgroup $ @  & < > # { }",
  NULL
};


static const char* flagship_mask = "*.prg";


static const char* flagship_apps = "";


static LangStyle LangFlagShip = {
  "flagship",
  SCLEX_FLAGSHIP,
  flagship_style,
  (char**)flagship_words,
  (char*)flagship_mask,
  (char*)flagship_apps,
  0,
  TABS_DEFAULT,
  0
};


