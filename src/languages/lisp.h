/* lexname=lisp lexpfx=SCE_LISP_ lextag=SCLEX_LISP */

static StyleDef lisp_style[] = {
  { "comment", SCE_LISP_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "default", SCE_LISP_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "identifier", SCE_LISP_IDENTIFIER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "keyword", SCE_LISP_KEYWORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "keywordkw", SCE_LISP_KEYWORD_KW, _WORD1_FG, _DEFLT_BG, Bold },
  { "multicomment", SCE_LISP_MULTI_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "number", SCE_LISP_NUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "operator", SCE_LISP_OPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { "special", SCE_LISP_SPECIAL, _DEFLT_FG, _DEFLT_BG, Normal },
  { "string", SCE_LISP_STRING, STRING_FG, _DEFLT_BG, Normal },
  { "stringeol", SCE_LISP_STRINGEOL, STREOL_FG, _DEFLT_BG, Normal },
  { "symbol", SCE_LISP_SYMBOL, _DEFLT_FG, _DEFLT_BG, Normal },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* lisp_words[]= {
  "not defun + - * / = < > <= >= princ eval apply funcall quote identity function complement backquote lambda set setq setf defun defmacro gensym make symbol intern symbol name symbol value symbol plist get getf putprop remprop hash make array aref car cdr caar cadr cdar cddr caaar caadr cadar caddr cdaar cdadr cddar cdddr caaaar caaadr caadar caaddr cadaar cadadr caddar cadddr cdaaar cdaadr cdadar cdaddr cddaar cddadr cdddar cddddr cons list append reverse last nth nthcdr member assoc subst sublis nsubst nsublis remove length list length mapc mapcar mapl maplist mapcan mapcon rplaca rplacd nconc delete atom symbolp numberp boundp null listp consp minusp zerop plusp evenp oddp eq eql equal cond case and or let l if prog prog1 prog2 progn go return do dolist dotimes catch throw error cerror break continue errset baktrace evalhook truncate float rem min max abs sin cos tan expt exp sqrt random logand logior logxor lognot bignums logeqv lognand lognor logorc2 logtest logbitp logcount integer length nil",
  "",
  NULL
};


static const char* lisp_mask = "*.lsp|*.lisp";


static const char* lisp_apps = "";


static LangStyle LangLisp = {
  "lisp",
  SCLEX_LISP,
  lisp_style,
  (char**)lisp_words,
  (char*)lisp_mask,
  (char*)lisp_apps,
  0,
  TABS_DEFAULT,
  0
};


