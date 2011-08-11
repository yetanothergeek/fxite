/* lexname=fortran lexpfx=SCE_F_ lextag=SCLEX_FORTRAN */

static StyleDef fortran_style[] = {
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


static const char* fortran_words[]= {
  "",
  "abs achar acos acosd adjustl adjustr aimag aimax0 aimin0 aint ajmax0 ajmin0 akmax0 akmin0 all allocated alog alog10 amax0 amax1 amin0 amin1 amod anint any asin asind associated atan atan2 atan2d atand bitest bitl bitlr bitrl bjtest bit_size bktest break btest cabs ccos cdabs cdcos cdexp cdlog cdsin cdsqrt ceiling cexp char clog cmplx conjg cos cosd cosh count cpu_time cshift csin csqrt dabs dacos dacosd dasin dasind datan datan2 datan2d datand date date_and_time dble dcmplx dconjg dcos dcosd dcosh dcotan ddim dexp dfloat dflotk dfloti dflotj digits dim dimag dint dlog dlog10 dmax1 dmin1 dmod dnint dot_product dprod dreal dsign dsin dsind dsinh dsqrt dtan dtand dtanh eoshift epsilon errsns exp exponent float floati floatj floatk floor fraction free huge iabs iachar iand ibclr ibits ibset ichar idate idim idint idnint ieor ifix iiabs iiand iibclr iibits iibset iidim iidint iidnnt iieor iifix iint iior iiqint iiqnnt iishft iishftc iisign ilen imax0 imax1 imin0 imin1 imod index inint inot int int1 int2 int4 int8 iqint iqnint ior ishft ishftc isign isnan izext jiand jibclr jibits jibset jidim jidint jidnnt jieor jifix jint jior jiqint jiqnnt jishft jishftc jisign jmax0 jmax1 jmin0 jmin1 jmod jnint jnot jzext kiabs kiand kibclr kibits kibset kidim kidint kidnnt kieor kifix kind kint kior kishft kishftc kisign kmax0 kmax1 kmin0 kmin1 kmod knint knot kzext lbound leadz len len_trim lenlge lge lgt lle llt log log10 logical lshift malloc matmul max max0 max1 maxexponent maxloc maxval merge min min0 min1 minexponent minloc minval mod modulo mvbits nearest nint not nworkers number_of_processors pack popcnt poppar precision present product radix random random_number random_seed range real repeat reshape rrspacing rshift scale scan secnds selected_int_kind selected_real_kind set_exponent shape sign sin sind sinh size sizeof sngl snglq spacing spread sqrt sum system_clock tan tand tanh tiny transfer transpose trim ubound unpack verify",
  "cdabs cdcos cdexp cdlog cdsin cdsqrt cotan cotand dcmplx dconjg dcotan dcotand decode dimag dll_export dll_import doublecomplex dreal dvchk encode find flen flush getarg getcharqq getcl getdat getenv gettim hfix ibchng identifier imag int1 int2 int4 intc intrup invalop iostat_msg isha ishc ishl jfix lacfar locking locnear map nargs nbreak ndperr ndpexc offset ovefl peekcharqq precfill prompt qabs qacos qacosd qasin qasind qatan qatand qatan2 qcmplx qconjg qcos qcosd qcosh qdim qexp qext qextd qfloat qimag qlog qlog10 qmax1 qmin1 qmod qreal qsign qsin qsind qsinh qsqrt qtan qtand qtanh ran rand randu rewrite segment setdat settim system timer undfl unlock union val virtual volatile zabs zcos zexp zlog zsin zsqrt",
  NULL
};


static const char* fortran_mask = "";


static const char* fortran_apps = "";


static LangStyle LangFortran = {
  "fortran",
  SCLEX_FORTRAN,
  fortran_style,
  (char**)fortran_words,
  (char*)fortran_mask,
  (char*)fortran_apps,
  0,
  TABS_DEFAULT,
  0
};


