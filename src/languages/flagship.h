/* lexname=flagship lexpfx=SCE_B_ lextag=SCLEX_FLAGSHIP */

static StyleDef flagship_style[] = {
  { "default", SCE_B_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "comment", SCE_B_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "number", SCE_B_NUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "keyword", SCE_B_KEYWORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "string", SCE_B_STRING, STRING_FG, _DEFLT_BG, Normal },
  { "preprocessor", SCE_B_PREPROCESSOR, PREPRC_FG, _DEFLT_BG, Normal },
  { "operator", SCE_B_OPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { "identifier", SCE_B_IDENTIFIER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "date", SCE_B_DATE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "stringeol", SCE_B_STRINGEOL, STREOL_FG, _DEFLT_BG, Normal },
  { "keyword2", SCE_B_KEYWORD2, _WORD2_FG, _DEFLT_BG, Bold },
  { "keyword3", SCE_B_KEYWORD3, _WORD1_FG, _DEFLT_BG, Bold },
  { "keyword4", SCE_B_KEYWORD4, _WORD1_FG, _DEFLT_BG, Bold },
  { "constant", SCE_B_CONSTANT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "asm", SCE_B_ASM, _DEFLT_FG, _DEFLT_BG, Normal },
  { "label", SCE_B_LABEL, _DEFLT_FG, _DEFLT_BG, Normal },
  { "error", SCE_B_ERROR, _DEFLT_FG, _DEFLT_BG, Normal },
  { "hexnumber", SCE_B_HEXNUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "binnumber", SCE_B_BINNUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* flagship_words[]= {
  "? @ accept access all alternate announce ansi any append as assign autolock average begin bell bitmap blank box call cancel case century charset checkbox clear close cls color combobox commit confirm console constant continue copy count create cursor date dbread dbwrite decimals declare default delete deleted delimiters device dir directory display do draw edit else elseif eject end endcase enddo endif endtext epoch erase error escape eval eventmask exact exclusive extended external extra field file filter find fixed font for form format from get gets global global_extern go goto gotop guialign guicolor guicursor guitransl html htmltext if image index input intensity join key keyboard keytransl label lines list listbox local locate margin memory memvar menu message method multibyte multilocks next nfs nfslock nfs_force note on openerror order outmode pack parameters path pixel pop printer private prompt public push pushbutton quit radiobutton radiogroup read recall refresh reindex relation release rename replace report request restore richtext rowadapt rowalign run save say scoreboard scrcompress screen seek select sequence set setenhanced setstandard setunselected skip softseek sort source static store struct structure sum tag tbrowse text to total type typeahead unique unlock update use wait while with wrap xml zap zerobyteout",
  "",
  "function procedure return exit",
  "class instance export hidden protect prototype",
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
  TABS_DEFAULT
};


