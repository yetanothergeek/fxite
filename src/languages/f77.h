/* lexname=f77 lexpfx=SCE_F_ lextag=SCLEX_F77 */

static StyleDef f77_style[] = {
  { "default", SCE_F_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "comment", SCE_F_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "number", SCE_F_NUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "string1", SCE_F_STRING1, STRING_FG, _DEFLT_BG, Normal },
  { "string2", SCE_F_STRING2, STRING_FG, _DEFLT_BG, Normal },
  { "stringeol", SCE_F_STRINGEOL, STREOL_FG, _DEFLT_BG, Normal },
  { "operator", SCE_F_OPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { "identifier", SCE_F_IDENTIFIER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "word", SCE_F_WORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "word2", SCE_F_WORD2, _WORD2_FG, _DEFLT_BG, Bold },
  { "word3", SCE_F_WORD3, _WORD1_FG, _DEFLT_BG, Bold },
  { "preprocessor", SCE_F_PREPROCESSOR, PREPRC_FG, _DEFLT_BG, Normal },
  { "operator2", SCE_F_OPERATOR2, OPERTR_FG, _DEFLT_BG, Bold },
  { "label", SCE_F_LABEL, _DEFLT_FG, _DEFLT_BG, Normal },
  { "continuation", SCE_F_CONTINUATION, _DEFLT_FG, _DEFLT_BG, Normal },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* f77_words[]= {
  "access action advance allocatable allocate apostrophe assign assignment associate asynchronous backspace bind blank blockdata call case character class close common complex contains continue cycle data deallocate decimal delim default dimension direct do dowhile double doubleprecision else elseif elsewhere encoding end endassociate endblockdata enddo endfile endforall endfunction endif endinterface endmodule endprogram endselect endsubroutine endtype endwhere entry eor equivalence err errmsg exist exit external file flush fmt forall form format formatted function go goto id if implicit in include inout integer inquire intent interface intrinsic iomsg iolength iostat kind len logical module name named namelist nextrec nml none nullify number only open opened operator optional out pad parameter pass pause pending pointer pos position precision print private procedure program protected public quote read readwrite real rec recl recursive result return rewind save select selectcase selecttype sequential sign size stat status stop stream subroutine target then to type unformatted unit use value volatile wait where while write",
  "",
  "",
  NULL
};


static const char* f77_mask = "*.f|*.for";


static const char* f77_apps = "";


static LangStyle LangF77 = {
  "f77",
  SCLEX_F77,
  f77_style,
  (char**)f77_words,
  (char*)f77_mask,
  (char*)f77_apps,
  0,
  TABS_DEFAULT
};


