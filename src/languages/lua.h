/* lexname=lua lexpfx=SCE_LUA_ lextag=SCLEX_LUA */

static StyleDef lua_style[] = {
  { "default", SCE_LUA_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "comment", SCE_LUA_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentline", SCE_LUA_COMMENTLINE, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentdoc", SCE_LUA_COMMENTDOC, COMMNT_FG, _DEFLT_BG, Italic },
  { "number", SCE_LUA_NUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "word", SCE_LUA_WORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "string", SCE_LUA_STRING, STRING_FG, _DEFLT_BG, Normal },
  { "character", SCE_LUA_CHARACTER, STRING_FG, _DEFLT_BG, Normal },
  { "literalstring", SCE_LUA_LITERALSTRING, STRING_FG, _DEFLT_BG, Normal },
  { "preprocessor", SCE_LUA_PREPROCESSOR, PREPRC_FG, _DEFLT_BG, Normal },
  { "operator", SCE_LUA_OPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { "identifier", SCE_LUA_IDENTIFIER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "stringeol", SCE_LUA_STRINGEOL, STREOL_FG, _DEFLT_BG, Normal },
  { "word2", SCE_LUA_WORD2, _WORD2_FG, _DEFLT_BG, Bold },
  { "word3", SCE_LUA_WORD3, _WORD1_FG, _DEFLT_BG, Bold },
  { "word4", SCE_LUA_WORD4, _WORD1_FG, _DEFLT_BG, Bold },
  { "word5", SCE_LUA_WORD5, _WORD1_FG, _DEFLT_BG, Bold },
  { "word6", SCE_LUA_WORD6, _WORD1_FG, _DEFLT_BG, Bold },
  { "word7", SCE_LUA_WORD7, _WORD1_FG, _DEFLT_BG, Bold },
  { "word8", SCE_LUA_WORD8, _WORD1_FG, _DEFLT_BG, Bold },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* lua_words[]= {
  "and break do else elseif end false for function if in local nil not or repeat return then true until while",
  "_VERSION assert collectgarbage dofile error gcinfo loadfile loadstring print rawget rawset require tonumber tostring type unpack_ALERT _ERRORMESSAGE _INPUT _PROMPT _OUTPUT _STDERR _STDIN _STDOUT call dostring foreach foreachi getn globals newtype sort tinsert tremove_G getfenv getmetatable ipairs loadlib next pairs pcall rawequal setfenv setmetatable xpcall string table math coroutine io os debug load module select",
  "abs acos asin atan atan2 ceil cos deg exp floor format frexp gsub ldexp log log10 max min mod rad random randomseed sin sqrt strbyte strchar strfind strlen strlower strrep strsub strupper tanstring.byte string.char string.dump string.find string.len string.lower string.rep string.sub string.upper string.format string.gfind string.gsub table.concat table.foreach table.foreachi table.getn table.sort table.insert table.remove table.setn math.abs math.acos math.asin math.atan math.atan2 math.ceil math.cos math.deg math.exp math.floor math.frexp math.ldexp math.log math.log10 math.max math.min math.mod math.pi math.pow math.rad math.random math.randomseed math.sin math.sqrt math.tan string.gmatch string.match string.reverse table.maxn math.cosh math.fmod math.modf math.sinh math.tanh math.huge",
  "openfile closefile readfrom writeto appendto remove rename flush seek tmpfile tmpname read write clock date difftime execute exit getenv setlocale timecoroutine.create coroutine.resume coroutine.status coroutine.wrap coroutine.yield io.close io.flush io.input io.lines io.open io.output io.read io.tmpfile io.type io.write io.stdin io.stdout io.stderr os.clock os.date os.difftime os.execute os.exit os.getenv os.remove os.rename os.setlocale os.time os.tmpname coroutine.running package.cpath package.loaded package.loadlib package.path package.preload package.seeall io.popen",
  "",
  "",
  "",
  "",
  NULL
};


static const char* lua_mask = "*.lua";


static const char* lua_apps = "lua";


static LangStyle LangLua = {
  "lua",
  SCLEX_LUA,
  lua_style,
  (char**)lua_words,
  (char*)lua_mask,
  (char*)lua_apps,
  0,
  TABS_DEFAULT
};


