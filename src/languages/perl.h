/* lexname=perl lexpfx=SCE_PL_ lextag=SCLEX_PERL */

static StyleDef perl_style[] = {
  { "default", SCE_PL_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "error", SCE_PL_ERROR, _DEFLT_FG, _DEFLT_BG, Normal },
  { "commentline", SCE_PL_COMMENTLINE, COMMNT_FG, _DEFLT_BG, Italic },
  { "pod", SCE_PL_POD, _DEFLT_FG, _DEFLT_BG, Normal },
  { "number", SCE_PL_NUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "word", SCE_PL_WORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "string", SCE_PL_STRING, STRING_FG, _DEFLT_BG, Normal },
  { "character", SCE_PL_CHARACTER, STRING_FG, _DEFLT_BG, Normal },
  { "punctuation", SCE_PL_PUNCTUATION, _DEFLT_FG, _DEFLT_BG, Normal },
  { "preprocessor", SCE_PL_PREPROCESSOR, PREPRC_FG, _DEFLT_BG, Normal },
  { "operator", SCE_PL_OPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { "identifier", SCE_PL_IDENTIFIER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "scalar", SCE_PL_SCALAR, SCALAR_FG, _DEFLT_BG, Normal },
  { "array", SCE_PL_ARRAY, _DEFLT_FG, _DEFLT_BG, Normal },
  { "hash", SCE_PL_HASH, _DEFLT_FG, _DEFLT_BG, Normal },
  { "symboltable", SCE_PL_SYMBOLTABLE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "variableindexer", SCE_PL_VARIABLE_INDEXER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "regex", SCE_PL_REGEX, _DEFLT_FG, _DEFLT_BG, Normal },
  { "regsubst", SCE_PL_REGSUBST, _DEFLT_FG, _DEFLT_BG, Normal },
  { "longquote", SCE_PL_LONGQUOTE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "backticks", SCE_PL_BACKTICKS, __ORANGE_, _DEFLT_BG, Normal },
  { "datasection", SCE_PL_DATASECTION, _DEFLT_FG, _DEFLT_BG, Normal },
  { "heredelim", SCE_PL_HERE_DELIM, _DEFLT_FG, _HERE_BG_, Normal },
  { "hereq", SCE_PL_HERE_Q, _DEFLT_FG, _HERE_BG_, Normal },
  { "hereqq", SCE_PL_HERE_QQ, _DEFLT_FG, _HERE_BG_, Normal },
  { "hereqx", SCE_PL_HERE_QX, _DEFLT_FG, _HERE_BG_, Normal },
  { "stringq", SCE_PL_STRING_Q, STRING_FG, _DEFLT_BG, Normal },
  { "stringqq", SCE_PL_STRING_QQ, STRING_FG, _DEFLT_BG, Normal },
  { "stringqx", SCE_PL_STRING_QX, STRING_FG, _DEFLT_BG, Normal },
  { "stringqr", SCE_PL_STRING_QR, STRING_FG, _DEFLT_BG, Normal },
  { "stringqw", SCE_PL_STRING_QW, STRING_FG, _DEFLT_BG, Normal },
  { "podverb", SCE_PL_POD_VERB, _DEFLT_FG, _DEFLT_BG, Normal },
  { "subprototype", SCE_PL_SUB_PROTOTYPE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "formatident", SCE_PL_FORMAT_IDENT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "format", SCE_PL_FORMAT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "stringvar", SCE_PL_STRING_VAR, STRING_FG, _DEFLT_BG, Normal },
  { "xlat", SCE_PL_XLAT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "regexvar", SCE_PL_REGEX_VAR, _DEFLT_FG, _DEFLT_BG, Normal },
  { "regsubstvar", SCE_PL_REGSUBST_VAR, _DEFLT_FG, _DEFLT_BG, Normal },
  { "backticksvar", SCE_PL_BACKTICKS_VAR, __ORANGE_, _DEFLT_BG, Normal },
  { "hereqqvar", SCE_PL_HERE_QQ_VAR, _DEFLT_FG, _HERE_BG_, Normal },
  { "hereqxvar", SCE_PL_HERE_QX_VAR, _DEFLT_FG, _HERE_BG_, Normal },
  { "stringqqvar", SCE_PL_STRING_QQ_VAR, STRING_FG, _DEFLT_BG, Normal },
  { "stringqxvar", SCE_PL_STRING_QX_VAR, STRING_FG, _DEFLT_BG, Normal },
  { "stringqrvar", SCE_PL_STRING_QR_VAR, STRING_FG, _DEFLT_BG, Normal },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* perl_words[]= {
  "NULL __FILE__ __LINE__ __PACKAGE__ __DATA__ __END__ AUTOLOAD BEGIN CORE DESTROY END EQ GE GT INIT LE LT NE CHECK abs accept alarm and atan2 bind binmode bless caller chdir chmod chomp chop chown chr chroot close closedir cmp connect continue cos crypt dbmclose dbmopen defined delete die do dump each else elsif endgrent endhostent endnetent endprotoent endpwent endservent eof eq eval exec exists exit exp fcntl fileno flock for foreach fork format formline ge getc getgrent getgrgid getgrnam gethostbyaddr gethostbyname gethostent getlogin getnetbyaddr getnetbyname getnetent getpeername getpgrp getppid getpriority getprotobyname getprotobynumber getprotoent getpwent getpwnam getpwuid getservbyname getservbyport getservent getsockname getsockopt glob gmtime goto grep gt hex if index int ioctl join keys kill last lc lcfirst le length link listen local localtime lock log lstat lt map mkdir msgctl msgget msgrcv msgsnd my ne next no not oct open opendir or ord our pack package pipe pop pos print printf prototype push quotemeta qu rand read readdir readline readlink readpipe recv redo ref rename require reset return reverse rewinddir rindex rmdir scalar seek seekdir select semctl semget semop send setgrent sethostent setnetent setpgrp setpriority setprotoent setpwent setservent setsockopt shift shmctl shmget shmread shmwrite shutdown sin sleep socket socketpair sort splice split sprintf sqrt srand stat study sub substr symlink syscall sysopen sysread sysseek system syswrite tell telldir tie tied time times truncate uc ucfirst umask undef unless unlink unpack unshift untie until use utime values vec wait waitpid wantarray warn while write xor given when default break say state UNITCHECK",
  NULL
};


static const char* perl_mask = "*.pl|*.pm|*.pod";


static const char* perl_apps = "perl";


static LangStyle LangPerl = {
  "perl",
  SCLEX_PERL,
  perl_style,
  (char**)perl_words,
  (char*)perl_mask,
  (char*)perl_apps,
  0,
  TABS_DEFAULT,
  0
};


