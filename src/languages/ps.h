/* lexname=ps lexpfx=SCE_PS_ lextag=SCLEX_PS */

static StyleDef ps_style[] = {
  { "badstringchar", SCE_PS_BADSTRINGCHAR, STRING_FG, _DEFLT_BG, Normal },
  { "base85string", SCE_PS_BASE85STRING, STRING_FG, _DEFLT_BG, Normal },
  { "comment", SCE_PS_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "default", SCE_PS_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "dsccomment", SCE_PS_DSC_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "dscvalue", SCE_PS_DSC_VALUE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "hexstring", SCE_PS_HEXSTRING, STRING_FG, _DEFLT_BG, Normal },
  { "immeval", SCE_PS_IMMEVAL, _DEFLT_FG, _DEFLT_BG, Normal },
  { "keyword", SCE_PS_KEYWORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "literal", SCE_PS_LITERAL, _DEFLT_FG, _DEFLT_BG, Normal },
  { "name", SCE_PS_NAME, _DEFLT_FG, _DEFLT_BG, Normal },
  { "number", SCE_PS_NUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "parenarray", SCE_PS_PAREN_ARRAY, _DEFLT_FG, _DEFLT_BG, Normal },
  { "parendict", SCE_PS_PAREN_DICT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "parenproc", SCE_PS_PAREN_PROC, _DEFLT_FG, _DEFLT_BG, Normal },
  { "text", SCE_PS_TEXT, _DEFLT_FG, _DEFLT_BG, Normal },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* ps_words[]= {
  "$error = == FontDirectory StandardEncoding UserObjects abs add aload anchorsearch and arc arcn arcto array ashow astore atan awidthshow begin bind bitshift bytesavailable cachestatus ceiling charpath clear cleardictstack cleartomark clip clippath closefile closepath concat concatmatrix copy copypage cos count countdictstack countexecstack counttomark currentcmykcolor currentcolorspace currentdash currentdict currentfile currentflat currentfont currentgray currenthsbcolor currentlinecap currentlinejoin currentlinewidth currentmatrix currentmiterlimit currentpagedevice currentpoint currentrgbcolor currentscreen currenttransfer cvi cvlit cvn cvr cvrs cvs cvx def defaultmatrix definefont dict dictstack div dtransform dup echo end eoclip eofill eq erasepage errordict exch exec execstack executeonly executive exit exp false file fill findfont flattenpath floor flush flushfile for forall ge get getinterval grestore grestoreall gsave gt idetmatrix idiv idtransform if ifelse image imagemask index initclip initgraphics initmatrix inustroke invertmatrix itransform known kshow le length lineto ln load log loop lt makefont mark matrix maxlength mod moveto mul ne neg newpath noaccess nor not null nulldevice or pathbbox pathforall pop print prompt pstack put putinterval quit rand rcheck rcurveto read readhexstring readline readonly readstring rectstroke repeat resetfile restore reversepath rlineto rmoveto roll rotate round rrand run save scale scalefont search setblackgeneration setcachedevice setcachelimit setcharwidth setcolorscreen setcolortransfer setdash setflat setfont setgray sethsbcolor setlinecap setlinejoin setlinewidth setmatrix setmiterlimit setpagedevice setrgbcolor setscreen settransfer setvmthreshold show showpage sin sqrt srand stack start status statusdict stop stopped store string stringwidth stroke strokepath sub systemdict token token transform translate true truncate type ueofill undefineresource userdict usertime version vmstatus wcheck where widthshow write writehexstring writestring xcheck xor",
  "GlobalFontDirectory ISOLatin1Encoding SharedFontDirectory UserObject arct colorimage cshow currentblackgeneration currentcacheparams currentcmykcolor currentcolor currentcolorrendering currentcolorscreen currentcolorspace currentcolortransfer currentdevparams currentglobal currentgstate currenthalftone currentobjectformat currentoverprint currentpacking currentpagedevice currentshared currentstrokeadjust currentsystemparams currentundercolorremoval currentuserparams defineresource defineuserobject deletefile execform execuserobject filenameforall fileposition filter findencoding findresource gcheck globaldict glyphshow gstate ineofill infill instroke inueofill inufill inustroke languagelevel makepattern packedarray printobject product realtime rectclip rectfill rectstroke renamefile resourceforall resourcestatus revision rootfont scheck selectfont serialnumber setbbox setblackgeneration setcachedevice2 setcacheparams setcmykcolor setcolor setcolorrendering setcolorscreen setcolorspace setcolortranfer setdevparams setfileposition setglobal setgstate sethalftone setobjectformat setoverprint setpacking setpagedevice setpattern setshared setstrokeadjust setsystemparams setucacheparams setundercolorremoval setuserparams setvmthreshold shareddict startjob uappend ucache ucachestatus ueofill ufill undef undefinefont undefineresource undefineuserobject upath ustroke ustrokepath vmreclaim writeobject xshow xyshow yshow",
  "cliprestore clipsave composefont currentsmoothness findcolorrendering setsmoothness shfill",
  ".begintransparencygroup .begintransparencymask .bytestring .charboxpath .currentaccuratecurves .currentblendmode .currentcurvejoin .currentdashadapt .currentdotlength .currentfilladjust2 .currentlimitclamp .currentopacityalpha .currentoverprintmode .currentrasterop .currentshapealpha .currentsourcetransparent .currenttextknockout .currenttexturetransparent .dashpath .dicttomark .discardtransparencygroup .discardtransparencymask .endtransparencygroup .endtransparencymask .execn .filename .filename .fileposition .forceput .forceundef .forgetsave .getbitsrect .getdevice .inittransparencymask .knownget .locksafe .makeoperator .namestring .oserrno .oserrorstring .peekstring .rectappend .runandhide .setaccuratecurves .setblendmode .setcurvejoin .setdashadapt .setdebug .setdefaultmatrix .setdotlength .setfilladjust2 .setlimitclamp .setmaxlength .setopacityalpha .setoverprintmode .setrasterop .setsafe .setshapealpha .setsourcetransparent .settextknockout .settexturetransparent .stringbreak .stringmatch .tempfile .type1decrypt .type1encrypt .type1execchar .unread arccos arcsin copydevice copyscanlines currentdevice finddevice findlibfile findprotodevice flushpage getdeviceprops getenv makeimagedevice makewordimagedevice max min putdeviceprops setdevice",
  "",
  NULL
};


static const char* ps_mask = "*.ps";


static const char* ps_apps = "";


static LangStyle LangPS = {
  "ps",
  SCLEX_PS,
  ps_style,
  (char**)ps_words,
  (char*)ps_mask,
  (char*)ps_apps,
  0,
  TABS_DEFAULT,
  0
};


