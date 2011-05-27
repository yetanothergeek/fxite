/* lexname=sql lexpfx=SCE_SQL_ lextag=SCLEX_SQL */

static StyleDef sql_style[] = {
  { "default", SCE_SQL_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "comment", SCE_SQL_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentline", SCE_SQL_COMMENTLINE, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentdoc", SCE_SQL_COMMENTDOC, COMMNT_FG, _DEFLT_BG, Italic },
  { "number", SCE_SQL_NUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "word", SCE_SQL_WORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "string", SCE_SQL_STRING, STRING_FG, _DEFLT_BG, Normal },
  { "character", SCE_SQL_CHARACTER, STRING_FG, _DEFLT_BG, Normal },
  { "sqlplus", SCE_SQL_SQLPLUS, _DEFLT_FG, _DEFLT_BG, Normal },
  { "sqlplusprompt", SCE_SQL_SQLPLUS_PROMPT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "operator", SCE_SQL_OPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { "identifier", SCE_SQL_IDENTIFIER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "sqlpluscomment", SCE_SQL_SQLPLUS_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentlinedoc", SCE_SQL_COMMENTLINEDOC, COMMNT_FG, _DEFLT_BG, Italic },
  { "word2", SCE_SQL_WORD2, _WORD2_FG, _DEFLT_BG, Bold },
  { "commentdockeyword", SCE_SQL_COMMENTDOCKEYWORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "commentdockeyworderror", SCE_SQL_COMMENTDOCKEYWORDERROR, _WORD1_FG, _DEFLT_BG, Bold },
  { "user1", SCE_SQL_USER1, _DEFLT_FG, _DEFLT_BG, Normal },
  { "user2", SCE_SQL_USER2, _DEFLT_FG, _DEFLT_BG, Normal },
  { "user3", SCE_SQL_USER3, _DEFLT_FG, _DEFLT_BG, Normal },
  { "user4", SCE_SQL_USER4, _DEFLT_FG, _DEFLT_BG, Normal },
  { "quotedidentifier", SCE_SQL_QUOTEDIDENTIFIER, _DEFLT_FG, _DEFLT_BG, Normal },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* sql_words[]= {
  "absolute action add admin after aggregate alias all allocate alter and any are array as asc assertion at authorization before begin binary bit blob body boolean both breadth by call cascade cascaded case cast catalog char character check class clob close collate collation column commit completion connect connection constraint constraints constructor continue corresponding create cross cube current current_date current_path current_role current_time current_timestamp current_user cursor cycle data date day deallocate dec decimal declare default deferrable deferred delete depth deref desc describe descriptor destroy destructor deterministic dictionary diagnostics disconnect distinct domain double drop dynamic each else end end-exec equals escape every except exception exec execute exists exit external false fetch first float for foreign found from free full function general get global go goto grant group grouping having host hour identity if ignore immediate in indicator initialize initially inner inout input insert int integer intersect interval into is isolation iterate join key language large last lateral leading left less level like limit local localtime localtimestamp locator map match minute modifies modify module month names national natural nchar nclob new next no none not null numeric object of off old on only open operation option or order ordinality out outer output package pad parameter parameters partial path postfix precision prefix preorder prepare preserve primary prior privileges procedure public read reads real recursive ref references referencing relative restrict result return returns revoke right role rollback rollup routine row rows savepoint schema scroll scope search second section select sequence session session_user set sets size smallint some| space specific specifictype sql sqlexception sqlstate sqlwarning start state statement static structure system_user table temporary terminate than then time timestamp timezone_hour timezone_minute to trailing transaction translation treat trigger true under union unique unknown unnest update usage user using value values varchar variable varying view when whenever where with without work write year zone",
  "",
  "param author since return see deprecated todo",
  "acc~ept a~ppend archive log attribute bre~ak bti~tle c~hange cl~ear col~umn comp~ute conn~ect copy def~ine del desc~ribe disc~onnect e~dit exec~ute exit get help ho~st i~nput l~ist passw~ord pau~se pri~nt pro~mpt quit recover rem~ark repf~ooter reph~eader r~un sav~e set sho~w shutdown spo~ol sta~rt startup store timi~ng tti~tle undef~ine var~iable whenever oserror whenever sqlerror ",
  "dbms_output.disable dbms_output.enable dbms_output.get_line dbms_output.get_lines dbms_output.new_line dbms_output.put dbms_output.put_line ",
  "",
  "",
  "",
  NULL
};


static const char* sql_mask = "*.sql";


static const char* sql_apps = "";


static LangStyle LangSQL = {
  "sql",
  SCLEX_SQL,
  sql_style,
  (char**)sql_words,
  (char*)sql_mask,
  (char*)sql_apps,
  0,
  TABS_DEFAULT
};


