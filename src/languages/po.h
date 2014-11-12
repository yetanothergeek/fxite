/* lexname=po lexpfx=SCE_PO_ lextag=SCLEX_PO */

static StyleDef po_style[] = {
  { "comment", SCE_PO_COMMENT, __GREEN__, _DEFLT_BG, Italic },
  { "default", SCE_PO_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "error", SCE_PO_ERROR, _DEFLT_FG, _DEFLT_BG, Normal },
  { "flags", SCE_PO_FLAGS, __ORANGE_, _DEFLT_BG, Normal },
  { "fuzzy", SCE_PO_FUZZY, STREOL_FG, _DEFLT_BG, Bold },
  { "msgctxt", SCE_PO_MSGCTXT, _DEFLT_FG, _DEFLT_BG, Bold },
  { "msgctxttext", SCE_PO_MSGCTXT_TEXT, _DEFLT_FG, _DEFLT_BG, Bold },
  { "msgctxttexteol", SCE_PO_MSGCTXT_TEXT_EOL, _DEFLT_FG, _DEFLT_BG, Bold },
  { "msgid", SCE_PO_MSGID, _DEFLT_FG, _DEFLT_BG, Bold },
  { "msgidtext", SCE_PO_MSGID_TEXT, __PURPLE_, _DEFLT_BG, Normal },
  { "msgidtexteol", SCE_PO_MSGID_TEXT_EOL, __PURPLE_, _DEFLT_BG, Normal },
  { "msgstr", SCE_PO_MSGSTR, _DEFLT_FG, _DEFLT_BG, Bold },
  { "msgstrtext", SCE_PO_MSGSTR_TEXT, ___BLUE__, _DEFLT_BG, Normal },
  { "msgstrtexteol", SCE_PO_MSGSTR_TEXT_EOL, ___BLUE__, _DEFLT_BG, Normal },
  { "programmercomment", SCE_PO_PROGRAMMER_COMMENT, __GREEN__, _DEFLT_BG, Italic },
  { "reference", SCE_PO_REFERENCE, _DEFLT_FG, _DEFLT_BG, Normal },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* po_words[]= {NULL};


static const char* po_mask = "*.po|*.pot";


static const char* po_apps = "";


static LangStyle LangPo = {
  "po",
  SCLEX_PO,
  po_style,
  (char**)po_words,
  (char*)po_mask,
  (char*)po_apps,
  0,
  TABS_DEFAULT,
  0
};


