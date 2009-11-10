/* lexname=tex lexpfx=SCE_TEX_ lextag=SCLEX_TEX */

static StyleDef tex_style[] = {
  { "default", SCE_TEX_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "special", SCE_TEX_SPECIAL, _DEFLT_FG, _DEFLT_BG, Normal },
  { "group", SCE_TEX_GROUP, _DEFLT_FG, _DEFLT_BG, Normal },
  { "symbol", SCE_TEX_SYMBOL, _DEFLT_FG, _DEFLT_BG, Normal },
  { "command", SCE_TEX_COMMAND, _DEFLT_FG, _DEFLT_BG, Normal },
  { "text", SCE_TEX_TEXT, _DEFLT_FG, _DEFLT_BG, Normal },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* tex_words[]= {
  "above abovedisplayshortskip abovedisplayskip abovewithdelims accent adjdemerits advance afterassignment aftergroup atop atopwithdelims badness baselineskip batchmode begingroup belowdisplayshortskip belowdisplayskip binoppenalty botmark box boxmaxdepth brokenpenalty catcode char chardef cleaders closein closeout clubpenalty copy count countdef cr crcr csname day deadcycles def defaulthyphenchar defaultskewchar delcode delimiter delimiterfactor delimeters delimitershortfall delimeters dimen dimendef discretionary displayindent displaylimits displaystyle displaywidowpenalty displaywidth divide doublehyphendemerits dp dump edef else emergencystretch end endcsname endgroup endinput endlinechar eqno errhelp errmessage errorcontextlines errorstopmode escapechar everycr everydisplay everyhbox everyjob everymath everypar everyvbox exhyphenpenalty expandafter fam fi finalhyphendemerits firstmark floatingpenalty font fontdimen fontname futurelet gdef global group globaldefs halign hangafter hangindent hbadness hbox hfil horizontal hfill horizontal hfilneg hfuzz hoffset holdinginserts hrule hsize hskip hss horizontal ht hyphenation hyphenchar hyphenpenalty hyphen if ifcase ifcat ifdim ifeof iffalse ifhbox ifhmode ifinner ifmmode ifnum ifodd iftrue ifvbox ifvmode ifvoid ifx ignorespaces immediate indent input inputlineno input insert insertpenalties interlinepenalty jobname kern language lastbox lastkern lastpenalty lastskip lccode leaders left lefthyphenmin leftskip leqno let limits linepenalty line lineskip lineskiplimit long looseness lower lowercase mag mark mathaccent mathbin mathchar mathchardef mathchoice mathclose mathcode mathinner mathop mathopen mathord mathpunct mathrel mathsurround maxdeadcycles maxdepth meaning medmuskip message mkern month moveleft moveright mskip multiply muskip muskipdef newlinechar noalign noboundary noexpand noindent nolimits nonscript scriptscript nonstopmode nulldelimiterspace nullfont number omit openin openout or outer output outputpenalty over overfullrule overline overwithdelims pagedepth pagefilllstretch pagefillstretch pagefilstretch pagegoal pageshrink pagestretch pagetotal par parfillskip parindent parshape parskip patterns pausing penalty postdisplaypenalty predisplaypenalty predisplaysize pretolerance prevdepth prevgraf radical raise read relax relpenalty right righthyphenmin rightskip romannumeral scriptfont scriptscriptfont scriptscriptstyle scriptspace scriptstyle scrollmode setbox setlanguage sfcode shipout show showbox showboxbreadth showboxdepth showlists showthe skewchar skip skipdef spacefactor spaceskip span special splitbotmark splitfirstmark splitmaxdepth splittopskip string tabskip textfont textstyle the thickmuskip thinmuskip time toks toksdef tolerance topmark topskip tracingcommands tracinglostchars tracingmacros tracingonline tracingoutput tracingpages tracingparagraphs tracingrestores tracingstats uccode uchyph underline unhbox unhcopy unkern unpenalty unskip unvbox unvcopy uppercase vadjust valign vbadness vbox vcenter vfil vfill vfilneg vfuzz voffset vrule vsize vskip vsplit vss vtop wd widowpenalty write xdef xleaders xspaceskip yearTeX bgroup egroup endgraf space empty null newcount newdimen newskip newmuskip newbox newtoks newhelp newread newwrite newfam newlanguage newinsert newif maxdimen magstephalf magstep frenchspacing nonfrenchspacing normalbaselines obeylines obeyspaces raggedright ttraggedright thinspace negthinspace enspace enskip quad qquad smallskip medskip bigskip removelastskip topglue vglue hglue break nobreak allowbreak filbreak goodbreak smallbreak medbreak bigbreak line leftline rightline centerline rlap llap underbar strutbox strut cases matrix pmatrix bordermatrix eqalign displaylines eqalignno leqalignno pageno folio tracingall showhyphens fmtname fmtversion hphantom vphantom phantom smash",
  "",
  "",
  "",
  "",
  "",
  "",
  NULL
};


static const char* tex_mask = "*.tex|*.sty";


static const char* tex_apps = "";


static LangStyle LangTeX = {
  "tex",
  SCLEX_TEX,
  tex_style,
  (char**)tex_words,
  (char*)tex_mask,
  (char*)tex_apps,
  0,
  TABS_DEFAULT
};


