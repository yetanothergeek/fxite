/* lexname=html lexpfx=SCE_H lextag=SCLEX_HTML */

static StyleDef html_style[] = {
  { "default", SCE_H_DEFAULT, "#000000", _DEFLT_BG, Normal },
  { "tag", SCE_H_TAG, PREPRC_FG, _DEFLT_BG, Normal },
  { "tagunknown", SCE_H_TAGUNKNOWN, "#FF0000", _DEFLT_BG, Normal },
  { "attribute", SCE_H_ATTRIBUTE, "#008080", _DEFLT_BG, Normal },
  { "attributeunknown", SCE_H_ATTRIBUTEUNKNOWN, "#FF0000", _DEFLT_BG, Normal },
  { "number", SCE_H_NUMBER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "doublestring", SCE_H_DOUBLESTRING, STRING_FG, _DEFLT_BG, Normal },
  { "singlestring", SCE_H_SINGLESTRING, STRING_FG, _DEFLT_BG, Normal },
  { "other", SCE_H_OTHER, "#800080", _DEFLT_BG, Normal },
  { "comment", SCE_H_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "entity", SCE_H_ENTITY, "#800080", _DEFLT_BG, Normal },
  { "tagend", SCE_H_TAGEND, PREPRC_FG, _DEFLT_BG, Normal },
  { "xmlstart", SCE_H_XMLSTART, "#0000FF", _DEFLT_BG, Normal },
  { "xmlend", SCE_H_XMLEND, "#0000FF", _DEFLT_BG, Normal },
  { "script", SCE_H_SCRIPT, "#000080", _DEFLT_BG, Normal },
  { "asp", SCE_H_ASP, _DEFLT_FG, "#FFFF00", Normal },
  { "aspat", SCE_H_ASPAT, _DEFLT_FG, "#FFDF00", Normal },
  { "cdata", SCE_H_CDATA, _DEFLT_FG, "#FFDF00", Normal },
  { "question", SCE_H_QUESTION, "#0000FF", "#FFEFBF", Normal },
  { "value", SCE_H_VALUE, "#FF00FF", "#FFEFFF", Normal },
  { "xccomment", SCE_H_XCCOMMENT, "#000000", "#FFFFD0", Normal },
  { "sgmldefault", SCE_H_SGML_DEFAULT, "#000080", "#EFEFFF", Normal },
  { "sgmlcommand", SCE_H_SGML_COMMAND, "#000080", "#EFEFFF", Normal },
  { "sgml1stparam", SCE_H_SGML_1ST_PARAM, "#006600", "#EFEFFF", Normal },
  { "sgmldoublestring", SCE_H_SGML_DOUBLESTRING, STRING_FG, _DEFLT_BG, Normal },
  { "sgmlsimplestring", SCE_H_SGML_SIMPLESTRING, "#993300", "#EFEFFF", Normal },
  { "sgmlerror", SCE_H_SGML_ERROR, "#800000", "#FF6666", Normal },
  { "sgmlspecial", SCE_H_SGML_SPECIAL, "#3366FF", "#EFEFFF", Normal },
  { "sgmlentity", SCE_H_SGML_ENTITY, "#333333", "#EFEFFF", Normal },
  { "sgmlcomment", SCE_H_SGML_COMMENT, "#808000", "#EFEFFF", Normal },
  { "sgml1stparamcomment", SCE_H_SGML_1ST_PARAM_COMMENT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "sgmlblockdefault", SCE_H_SGML_BLOCK_DEFAULT, "#000066", "#CCCCE0", Normal },
  { "jstart", SCE_HJ_START, "#7F7F00", _DEFLT_BG, Normal },
  { "jdefault", SCE_HJ_DEFAULT, "#000000", _DEFLT_BG, Normal },
  { "jcomment", SCE_HJ_COMMENT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "jcommentline", SCE_HJ_COMMENTLINE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "jcommentdoc", SCE_HJ_COMMENTDOC, _DEFLT_FG, _DEFLT_BG, Normal },
  { "jnumber", SCE_HJ_NUMBER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "jword", SCE_HJ_WORD, "#000000", _DEFLT_BG, Normal },
  { "jkeyword", SCE_HJ_KEYWORD, _DEFLT_FG, _DEFLT_BG, Normal },
  { "jdoublestring", SCE_HJ_DOUBLESTRING, STRING_FG, _DEFLT_BG, Normal },
  { "jsinglestring", SCE_HJ_SINGLESTRING, STRING_FG, _DEFLT_BG, Normal },
  { "jsymbols", SCE_HJ_SYMBOLS, _DEFLT_FG, _DEFLT_BG, Normal },
  { "jstringeol", SCE_HJ_STRINGEOL, _DEFLT_FG, "#BFBBB0", Normal },
  { "jregex", SCE_HJ_REGEX, _DEFLT_FG, "#FFBBB0", Normal },
  { "jastart", SCE_HJA_START, "#7F7F00", _DEFLT_BG, Normal },
  { "jadefault", SCE_HJA_DEFAULT, "#000000", "#DFDF7F", Normal },
  { "jacomment", SCE_HJA_COMMENT, "#007F00", "#DFDF7F", Normal },
  { "jacommentline", SCE_HJA_COMMENTLINE, "#007F00", "#DFDF7F", Normal },
  { "jacommentdoc", SCE_HJA_COMMENTDOC, "#7F7F7F", "#DFDF7F", Normal },
  { "janumber", SCE_HJA_NUMBER, "#007F7F", "#DFDF7F", Normal },
  { "jaword", SCE_HJA_WORD, "#000000", "#DFDF7F", Normal },
  { "jakeyword", SCE_HJA_KEYWORD, "#00007F", "#DFDF7F", Normal },
  { "jadoublestring", SCE_HJA_DOUBLESTRING, STRING_FG, _DEFLT_BG, Normal },
  { "jasinglestring", SCE_HJA_SINGLESTRING, STRING_FG, _DEFLT_BG, Normal },
  { "jasymbols", SCE_HJA_SYMBOLS, "#000000", "#DFDF7F", Normal },
  { "jastringeol", SCE_HJA_STRINGEOL, _DEFLT_FG, "#BFBBB0", Normal },
  { "jaregex", SCE_HJA_REGEX, _DEFLT_FG, "#FFBBB0", Normal },
  { "bstart", SCE_HB_START, _DEFLT_FG, _DEFLT_BG, Normal },
  { "bdefault", SCE_HB_DEFAULT, _DEFLT_FG, "#EFEFFF", Normal },
  { "bcommentline", SCE_HB_COMMENTLINE, _DEFLT_FG, "#EFEFFF", Normal },
  { "bnumber", SCE_HB_NUMBER, _DEFLT_FG, "#EFEFFF", Normal },
  { "bword", SCE_HB_WORD, _DEFLT_FG, "#EFEFFF", Normal },
  { "bstring", SCE_HB_STRING, _DEFLT_FG, "#EFEFFF", Normal },
  { "bidentifier", SCE_HB_IDENTIFIER, _DEFLT_FG, "#EFEFFF", Normal },
  { "bstringeol", SCE_HB_STRINGEOL, _DEFLT_FG, "#7F7FFF", Normal },
  { "bastart", SCE_HBA_START, _DEFLT_FG, _DEFLT_BG, Normal },
  { "badefault", SCE_HBA_DEFAULT, _DEFLT_FG, "#CFCFEF", Normal },
  { "bacommentline", SCE_HBA_COMMENTLINE, _DEFLT_FG, "#CFCFEF", Normal },
  { "banumber", SCE_HBA_NUMBER, _DEFLT_FG, "#CFCFEF", Normal },
  { "baword", SCE_HBA_WORD, _DEFLT_FG, "#CFCFEF", Normal },
  { "bastring", SCE_HBA_STRING, _DEFLT_FG, "#CFCFEF", Normal },
  { "baidentifier", SCE_HBA_IDENTIFIER, _DEFLT_FG, "#CFCFEF", Normal },
  { "bastringeol", SCE_HBA_STRINGEOL, _DEFLT_FG, "#7F7FBF", Normal },
  { "pstart", SCE_HP_START, "#808080", _DEFLT_BG, Normal },
  { "pdefault", SCE_HP_DEFAULT, "#808080", "#EFFFEF", Normal },
  { "pcommentline", SCE_HP_COMMENTLINE, "#007F00", "#EFFFEF", Normal },
  { "pnumber", SCE_HP_NUMBER, "#007F7F", "#EFFFEF", Normal },
  { "pstring", SCE_HP_STRING, "#7F007F", "#EFFFEF", Normal },
  { "pcharacter", SCE_HP_CHARACTER, "#7F007F", "#EFFFEF", Normal },
  { "pword", SCE_HP_WORD, "#00007F", "#EFFFEF", Normal },
  { "ptriple", SCE_HP_TRIPLE, "#7F0000", "#EFFFEF", Normal },
  { "ptripledouble", SCE_HP_TRIPLEDOUBLE, "#7F0000", "#EFFFEF", Normal },
  { "pclassname", SCE_HP_CLASSNAME, "#0000FF", "#EFFFEF", Normal },
  { "pdefname", SCE_HP_DEFNAME, "#007F7F", "#EFFFEF", Normal },
  { "poperator", SCE_HP_OPERATOR, _DEFLT_FG, "#EFFFEF", Normal },
  { "pidentifier", SCE_HP_IDENTIFIER, _DEFLT_FG, "#EFFFEF", Normal },
  { "phpcomplexvariable", SCE_HPHP_COMPLEX_VARIABLE, "#007F00", "#FFF8F8", Normal },
  { "pastart", SCE_HPA_START, "#808080", _DEFLT_BG, Normal },
  { "padefault", SCE_HPA_DEFAULT, "#808080", "#CFEFCF", Normal },
  { "pacommentline", SCE_HPA_COMMENTLINE, "#007F00", "#CFEFCF", Normal },
  { "panumber", SCE_HPA_NUMBER, "#007F7F", "#CFEFCF", Normal },
  { "pastring", SCE_HPA_STRING, "#7F007F", "#CFEFCF", Normal },
  { "pacharacter", SCE_HPA_CHARACTER, "#7F007F", "#CFEFCF", Normal },
  { "paword", SCE_HPA_WORD, "#00007F", "#CFEFCF", Normal },
  { "patriple", SCE_HPA_TRIPLE, "#7F0000", "#CFEFCF", Normal },
  { "patripledouble", SCE_HPA_TRIPLEDOUBLE, "#7F0000", "#CFEFCF", Normal },
  { "paclassname", SCE_HPA_CLASSNAME, "#0000FF", "#CFEFCF", Normal },
  { "padefname", SCE_HPA_DEFNAME, "#007F7F", "#CFEFCF", Normal },
  { "paoperator", SCE_HPA_OPERATOR, _DEFLT_FG, "#CFEFCF", Normal },
  { "paidentifier", SCE_HPA_IDENTIFIER, _DEFLT_FG, "#CFEFCF", Normal },
  { "phpdefault", SCE_HPHP_DEFAULT, "#000033", "#FFF8F8", Normal },
  { "phphstring", SCE_HPHP_HSTRING, "#007F00", "#FFF8F8", Normal },
  { "phpsimplestring", SCE_HPHP_SIMPLESTRING, "#009F00", "#FFF8F8", Normal },
  { "phpword", SCE_HPHP_WORD, "#7F007F", "#FFF8F8", Normal },
  { "phpnumber", SCE_HPHP_NUMBER, "#CC9900", "#FFF8F8", Normal },
  { "phpvariable", SCE_HPHP_VARIABLE, "#00007F", "#FFF8F8", Normal },
  { "phpcomment", SCE_HPHP_COMMENT, "#999999", "#FFF8F8", Normal },
  { "phpcommentline", SCE_HPHP_COMMENTLINE, "#666666", "#FFF8F8", Normal },
  { "phphstringvariable", SCE_HPHP_HSTRING_VARIABLE, "#007F00", "#FFF8F8", Normal },
  { "phpoperator", SCE_HPHP_OPERATOR, "#000000", "#FFF8F8", Normal },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* html_words[]= {
  "a abbr acronym address applet area b base basefont bdo big blockquote body br button caption center cite code col colgroup dd del dfn dir div dl dt em fieldset font form frame frameset h1 h2 h3 h4 h5 h6 head hr html i iframe img input ins isindex kbd label legend li link map menu meta noframes noscript object ol optgroup option p param pre q s samp script select small span strike strong style sub sup table tbody td textarea tfoot th thead title tr tt u ul var xml xmlnsabbr accept-charset accept accesskey action align alink alt archive axis background bgcolor border cellpadding cellspacing char charoff charset checked cite class classid clear codebase codetype color cols colspan compact content coords data datafld dataformatas datapagesize datasrc datetime declare defer dir disabled enctype event face for frame frameborder headers height href hreflang hspace http-equiv id ismap label lang language leftmargin link longdesc marginwidth marginheight maxlength media method multiple name nohref noresize noshade nowrap object onblur onchange onclick ondblclick onfocus onkeydown onkeypress onkeyup onload onmousedown onmousemove onmouseover onmouseout onmouseup onreset onselect onsubmit onunload profile prompt readonly rel rev rows rowspan rules scheme scope selected shape size span src standby start style summary tabindex target text title topmargin type usemap valign value valuetype version vlink vspace width text password checkbox radio submit reset file hidden imagearticle aside calendar canvas card command commandset datagrid datatree footer gauge header m menubar menulabel nav progress section switch tabboxactive command contenteditable ping public !doctype",
  "abstract boolean break byte case catch char class const continue debugger default delete do double else enum export extends final finally float for function goto if implements import in instanceof int interface long native new package private protected public return short static super switch synchronized this throw throws transient try typeof var void volatile while with",
  "",
  "",
  "and array as bool boolean break case cfunction class const continue declare default die directory do double echo else elseif empty enddeclare endfor endforeach endif endswitch endwhile eval exit extends false float for foreach function global if include include_once int integer isset list new null object old_function or parent print real require require_once resource return static stdclass string switch true unset use var while xor abstract catch clone exception final implements interface php_user_filter private protected public this throw try __class__ __file__ __function__ __line__ __method__ __sleep __wakeup",
  "ELEMENT DOCTYPE ATTLIST ENTITY NOTATION",
  NULL
};


static const char* html_mask = "*.html|*.htm|*.asp|*.shtml|*.htd|*.jsp";


static const char* html_apps = "";


static LangStyle LangHTML = {
  "html",
  SCLEX_HTML,
  html_style,
  (char**)html_words,
  (char*)html_mask,
  (char*)html_apps,
  0,
  TABS_DEFAULT
};


