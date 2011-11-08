/* lexname=asm lexpfx=SCE_ASM_ lextag=SCLEX_ASM */

static StyleDef asm_style[] = {
  { "default", SCE_ASM_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "comment", SCE_ASM_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "number", SCE_ASM_NUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "string", SCE_ASM_STRING, STRING_FG, _DEFLT_BG, Normal },
  { "operator", SCE_ASM_OPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { "identifier", SCE_ASM_IDENTIFIER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "cpuinstruction", SCE_ASM_CPUINSTRUCTION, _DEFLT_FG, _DEFLT_BG, Normal },
  { "mathinstruction", SCE_ASM_MATHINSTRUCTION, _DEFLT_FG, _DEFLT_BG, Normal },
  { "register", SCE_ASM_REGISTER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "directive", SCE_ASM_DIRECTIVE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "directiveoperand", SCE_ASM_DIRECTIVEOPERAND, _DEFLT_FG, _DEFLT_BG, Normal },
  { "commentblock", SCE_ASM_COMMENTBLOCK, COMMNT_FG, _DEFLT_BG, Italic },
  { "character", SCE_ASM_CHARACTER, STRING_FG, _DEFLT_BG, Normal },
  { "stringeol", SCE_ASM_STRINGEOL, STREOL_FG, _DEFLT_BG, Normal },
  { "extinstruction", SCE_ASM_EXTINSTRUCTION, _DEFLT_FG, _DEFLT_BG, Normal },
  { "commentdirective", SCE_ASM_COMMENTDIRECTIVE, COMMNT_FG, _DEFLT_BG, Italic },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* asm_words[]= {
  "crc32 popcnt lzcnt tzcnt movbe pclmulqdq rdrand",
  "f2xm1 fabs fadd faddp fbld fbstp fchs fclex fcom fcomp fcompp fdecstp fdisi fdiv fdivp fdivr fdivrp feni ffree fiadd ficom ficomp fidiv fidivr fild fimul fincstp finit fist fistp fisub fisubr fld fld1 fldcw fldenv fldenvw fldl2e fldl2t fldlg2 fldln2 fldpi fldz fmul fmulp fnclex fndisi fneni fninit fnop fnsave fnsavew fnstcw fnstenv fnstenvw fnstsw fpatan fprem fptan frndint frstor frstorw fsave fsavew fscale fsqrt fst fstcw fstenv fstenvw fstp fstsw fsub fsubp fsubr fsubrp ftst fwait fxam fxch fxtract fyl2x fyl2xp1 fsetpm fcos fldenvd fnsaved fnstenvd fprem1 frstord fsaved fsin fsincos fstenvd fucom fucomp fucompp fcomi fcomip fucomi fucomip ffreep fcmovb fcmove fcmovbe fcmovu fcmovnb fcmovne fcmovnbe fcmovnu",
  "al ah bl bh cl ch dl dh ax bx cx dx si di bp eax ebx ecx edx esi edi ebx esp st0 st1 st2 st3 st4 st5 st6 st7 mm0 mm1 mm2 mm3 mm4 mm5 mm6 mm7 xmm0 xmm1 xmm2 xmm3 xmm4 xmm5 xmm6 xmm7 ymm0 ymm1 ymm2 ymm3 ymm4 ymm5 ymm6 ymm7 fssil dil bpl r8b r9b r10b r11b r12b r13b r14b r15b r8w r9w r10w r11w r12w r13w r14w r15w rax rcx rdx rbx rsp rbp rsi rdi r8 r9 r10 r11 r12 r13 r14 r15 xmm8 xmm9 xmm10 xmm11 xmm12 xmm13 xmm14 xmm15 ymm8 ymm9 ymm10 ymm11 ymm12 ymm13 ymm14 ymm15 gs",
  "db dw dd dq dt do dy resb resw resd resq rest reso resy incbin equ times safeseh __utf16__ __utf32__ %+ default cpu float start imagebase osabi ..start ..imagebase ..gotpc ..gotoff ..gottpoff ..got ..plt ..sym ..tlsie section segment __sect__ group absolute .bss .comment .data .lbss .ldata .lrodata .rdata .rodata .tbss .tdata .text alloc bss code exec data noalloc nobits noexec nowrite progbits rdata tls write private public common stack overlay class extern global common import export %define %idefine %xdefine %ixdefine %assign %undef %? %?? %defstr %idefstr %deftok %ideftok %strcat %strlen %substr %macro %imacro %rmacro %exitmacro %endmacro %unmacro %if %ifn %elif %elifn %else %endif %ifdef %ifndef %elifdef %elifndef %ifmacro %ifnmacro %elifmacro %elifnmacro %ifctx %ifnctx %elifctx %elifnctx %ifidn %ifnidn %elifidn %elifnidn %ifidni %ifnidni %elifidni %elifnidni %ifid %ifnid %elifid %elifnid %ifnum %ifnnum %elifnum %elifnnum %ifstr %ifnstr %elifstr %elifnstr %iftoken %ifntoken %eliftoken %elifntoken %ifempty %elifempty %ifnempty %elifnempty %ifenv %ifnenv %elifenv %elifnenv %rep %exitrep %endrep %while %exitwhile %endwhile %include %pathsearch %depend %use %push %pop %repl %arg %local %stacksize flat flat64 large small %error %warning %fatal %00 .nolist %rotate %line %! %final %clear struc endstruc istruc at iend align alignb sectalign bits use16 use32 use64 __nasm_major__ __nasm_minor__ __nasm_subminor__ ___nasm_patchlevel__ __nasm_version_id__ __nasm_ver__ __file__ __line__ __pass__ __bits__ __output_format__ __date__ __time__ __date_num__ __time_num__ __posix_time__ __utc_date__ __utc_time__ __utc_date_num__ __utc_time_num__ __float_daz__ __float_round__ __float__ __use_altreg__ altreg __use_smartalign__ smartalign __alignmode__ __use_fp__ __infinity__ __nan__ __qnan__ __snan__ __float8__ __float16__ __float32__ __float64__ __float80m__ __float80e__ __float128l__ __float128h__",
  "a16 a32 a64 o16 o32 o64 strict byte word dword qword tword oword yword nosplit %0 %1 %2 %3 %4 %5 %6 %7 %8 %9 abs rel $ $$ seg wrt",
  "",
  "",
  "",
  NULL
};


static const char* asm_mask = "*.asm";


static const char* asm_apps = "";


static LangStyle LangAsm = {
  "asm",
  SCLEX_ASM,
  asm_style,
  (char**)asm_words,
  (char*)asm_mask,
  (char*)asm_apps,
  0,
  TABS_DEFAULT,
  0
};


