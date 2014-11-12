/* lexname=flagship lexpfx=SCE_FS_ lextag=SCLEX_FLAGSHIP */

static StyleDef flagship_style[] = {
  { "comment", SCE_FS_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentdoc", SCE_FS_COMMENTDOC, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentdocc", SCE_FS_COMMENTDOC_C, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentdockeyword", SCE_FS_COMMENTDOCKEYWORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "commentdockeyworderror", SCE_FS_COMMENTDOCKEYWORDERROR, _WORD1_FG, _DEFLT_BG, Bold },
  { "commentline", SCE_FS_COMMENTLINE, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentlinedoc", SCE_FS_COMMENTLINEDOC, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentlinedocc", SCE_FS_COMMENTLINEDOC_C, COMMNT_FG, _DEFLT_BG, Italic },
  { "constant", SCE_FS_CONSTANT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "date", SCE_FS_DATE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "default", SCE_FS_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "defaultc", SCE_FS_DEFAULT_C, _DEFLT_FG, _DEFLT_BG, Normal },
  { "disabledcode", SCE_FS_DISABLEDCODE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "identifier", SCE_FS_IDENTIFIER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "identifierc", SCE_FS_IDENTIFIER_C, _DEFLT_FG, _DEFLT_BG, Normal },
  { "keyword", SCE_FS_KEYWORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "keyword2", SCE_FS_KEYWORD2, _WORD2_FG, _DEFLT_BG, Bold },
  { "keyword2c", SCE_FS_KEYWORD2_C, _WORD2_FG, _DEFLT_BG, Bold },
  { "keyword3", SCE_FS_KEYWORD3, _WORD1_FG, _DEFLT_BG, Bold },
  { "keyword4", SCE_FS_KEYWORD4, _WORD1_FG, _DEFLT_BG, Bold },
  { "keywordc", SCE_FS_KEYWORD_C, _WORD1_FG, _DEFLT_BG, Bold },
  { "number", SCE_FS_NUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "numberc", SCE_FS_NUMBER_C, NUMBER_FG, _DEFLT_BG, Normal },
  { "operator", SCE_FS_OPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { "operatorc", SCE_FS_OPERATOR_C, OPERTR_FG, _DEFLT_BG, Bold },
  { "preprocessor", SCE_FS_PREPROCESSOR, PREPRC_FG, _DEFLT_BG, Normal },
  { "preprocessorc", SCE_FS_PREPROCESSOR_C, PREPRC_FG, _DEFLT_BG, Normal },
  { "string", SCE_FS_STRING, STRING_FG, _DEFLT_BG, Normal },
  { "stringc", SCE_FS_STRING_C, STRING_FG, _DEFLT_BG, Normal },
  { "stringeol", SCE_FS_STRINGEOL, STREOL_FG, _DEFLT_BG, Normal },
  { "stringeolc", SCE_FS_STRINGEOL_C, STREOL_FG, _DEFLT_BG, Normal },
  { "wordoperator", SCE_FS_WORDOPERATOR, _WORD1_FG, _DEFLT_BG, Bold },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* flagship_words[]= {
  "? @ accept access all alternate announce ansi any append as assign autolock average begin bell bitmap blank box call cancel case century charset checkbox clear close cls color combobox commit confirm console constant continue copy count create cursor date dbread dbwrite decimals declare default delete deleted delimiters device dir directory display do draw edit else elseif eject end endcase enddo endif endtext epoch erase error escape eval eventmask exact exclusive extended external extra field file filter find fixed font for form format from get gets global global_extern go goto gotop guialign guicolor guicursor guitransl html htmltext if image index input intensity join key keyboard keytransl label lines list listbox local locate margin memory memvar menu message method multibyte multilocks next nfs nfslock nfs_force note on openerror order outmode pack parameters path pixel pop printer private prompt public push pushbutton quit radiobutton radiogroup read recall refresh reindex relation release rename replace report request restore richtext rowadapt rowalign run save say scoreboard scrcompress screen seek select sequence set setenhanced setstandard setunselected skip softseek sort source static store struct structure sum tag tbrowse text to total type typeahead unique unlock update use wait while with wrap xml zap zerobyteout",
  "_displarr _displarrerr _displarrstd _displobj _displobjerr _displobjstd aadd abs achoice aclone acopy adel adir aelemtype aeval afields afill ains alert alias alltrim altd ansi2oem appiomode appmdimode appobject array asc ascan asize asort at atail atanychar autoxlock between bin2i bin2l bin2w binand binlshift binor binrshift binxor bof break browse cdow chr chr2screen cmonth col col2pixel color2rgb colorselect colvisible consoleopen consolesize crc32 ctod curdir date datevalid day dbappend dbclearfilter dbclearindex dbclearrelation dbcloseall dbclosearea dbcommit dbcommitall dbcreate dbcreateindex dbdelete dbedit dbeval dbf dbfilter dbfinfo dbflock dbfused dbgetlocate dbgobottom dbgoto dbgotop dbobject dbrecall dbreindex dbrelation dbrlock dbrlocklist dbrselect dbrunlock dbseek dbselectarea dbsetdriver dbsetfilter dbsetindex dbsetlocate dbsetorder dbsetrelation dbskip dbstruct dbunlock dbunlockall dbusearea default deleted descend devout devoutpict devpos directory diskspace dispbegin dispbox dispcount dispend dispout doserror doserror2str dow drawline dtoc dtos empty eof errorblock errorlevel eval execname execpidnum exp fattrib fclose fcount fcreate ferase ferror ferror2str fieldblock fielddeci fieldget fieldgetarr fieldlen fieldname fieldpos fieldput fieldputarr fieldtype fieldwblock file findexefile fklabel fkmax flagship_dir flock flockf fopen found fread freadstdin freadstr freadtxt frename fs_set fseek fwrite getactive getalign getapplykey getdosetkey getenv getenvarr getfunction getpostvalid getprevalid getreader guidrawline hardcr header hex2num i2bin iif indexcheck indexcount indexdbf indexext indexkey indexnames indexord infobox inkey inkey2read inkey2str inkeytrap instdchar instdstring int int2num isalpha isbegseq iscolor isdbexcl isdbflock isdbmultip isdbmultiple isdbmultipleopen isdbrlock isdigit isfunction isguimode islower isobjclass isobjequiv isobjproperty isprinter isupper l2bin lastkey lastrec left len listbox lock log lower ltrim lupdate macroeval macrosubst max max_col max_row maxcol maxrow mcol mdblck mdiclose mdiopen mdiselect memocode memodecode memoedit memoencode memoline memoread memory memotran memowrit memvarblock mhide min minmax mlcount mlctopos mleftdown mlpos mod month mpostolc mpresent mreststate mrightdown mrow msavestate msetcursor msetpos mshow mstate neterr netname nextkey num2hex num2int objclone oem2ansi onkey ordbagext ordbagname ordcond ordcondset ordcreate orddescend orddestroy ordfor ordisinique ordkey ordkeyadd ordkeycount ordkeydel ordkeygoto ordkeyno ordkeyval ordlistadd ordlistclear ordlistrebui ordname ordnumber ordscope ordsetfocu ordsetrelat ordskipunique os outerr outstd padc padl padr param parameters pcalls pcol pcount pixel2col pixel2row printstatus procfile procline procname procstack proper prow qout qout2 qqout qqout2 rat rddlist rddname rddsetdefault readexit readinsert readkey readkill readmodal readsave readupdated readvar reccount recno recsize replicate restscreen right rlock rlockverify round row row2pixel rowadapt rowvisible rtrim savescreen scrdos2unix screen2chr scroll scrunix2dos seconds secondscpu select serial set setansi setblink setcancel setcol2get setcolor setcolorba setcursor setevent setguicursor setkey setmode setpos setprc setvarempty sleep sleepms soundex space sqrt statbarmsg statusmessage stod str strlen strlen2col strlen2pix strlen2space strpeek strpoke strtran strzero stuff substr tbcolumnnew tbmouse tbrowsearr tbrowsedb tbrowsenew tempfilename time tone transform trim truepath type updated upper used usersactive usersdbf usersmax val valtype version webdate weberrorhandler webgetenvir webgetformdata webhtmlbegin webhtmlend weblogerr webmaildomain weboutdata websendmail word year",
  "function procedure return exit",
  "class instance export hidden protect prototype",
  "a addindex addtogroup anchor arg attention author b brief bug c class code date def defgroup deprecated dontinclude e em endcode endhtmlonly endif endlatexonly endlink endverbatim enum example exception f$ f[ f] file fn hideinitializer htmlinclude htmlonly if image include ingroup internal invariant interface latexonly li line link mainpage name namespace nosubgrouping note overload p page par param param[in] param[out] post pre ref relates remarks return retval sa section see showinitializer since skip skipline struct subsection test throw throws todo typedef union until var verbatim verbinclude version warning weakgroup $ @ \\ & < > # { }",
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
  TABS_DEFAULT,
  0
};


