/* lexname=lua lexpfx=SCE_LUA_ lextag=SCLEX_LUA */

static StyleDef lua_style[] = {
  { "character", SCE_LUA_CHARACTER, STRING_FG, _DEFLT_BG, Normal },
  { "comment", SCE_LUA_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentdoc", SCE_LUA_COMMENTDOC, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentline", SCE_LUA_COMMENTLINE, COMMNT_FG, _DEFLT_BG, Italic },
  { "default", SCE_LUA_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "identifier", SCE_LUA_IDENTIFIER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "label", SCE_LUA_LABEL, _DEFLT_FG, _DEFLT_BG, Normal },
  { "literalstring", SCE_LUA_LITERALSTRING, STRING_FG, _DEFLT_BG, Normal },
  { "number", SCE_LUA_NUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "operator", SCE_LUA_OPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { "preprocessor", SCE_LUA_PREPROCESSOR, PREPRC_FG, _DEFLT_BG, Normal },
  { "string", SCE_LUA_STRING, STRING_FG, _DEFLT_BG, Normal },
  { "stringeol", SCE_LUA_STRINGEOL, STREOL_FG, _DEFLT_BG, Normal },
  { "word", SCE_LUA_WORD, _WORD1_FG, _DEFLT_BG, Bold },
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
  "and break do else elseif end for function if in local nil not or repeat return then until while false true goto",
  "assert collectgarbage dofile error _G getmetatable ipairs loadfile next pairs pcall print rawequal rawget rawset setmetatable tonumber tostring type _VERSION xpcall string table math coroutine io os debug getfenv gcinfo load loadlib loadstring require select setfenv unpack _LOADED LUA_PATH _REQUIREDNAME package rawlen package bit32 _ENV",
  "string.byte string.char string.dump string.find string.format string.gsub string.len string.lower string.rep string.sub string.upper table.concat table.insert table.remove table.sort math.abs math.acos math.asin math.atan math.atan2 math.ceil math.cos math.deg math.exp math.floor math.frexp math.ldexp math.log math.max math.min math.pi math.pow math.rad math.random math.randomseed math.sin math.sqrt math.tan string.gfind string.gmatch string.match string.reverse table.foreach table.foreachi table.getn table.setn table.maxn table.pack table.unpack math.cosh math.fmod math.huge math.log10 math.modf math.mod math.sinh math.tanh bit32.arshift bit32.band bit32.bnot bit32.bor bit32.btest bit32.bxor bit32.extract bit32.replace bit32.lrotate bit32.lshift bit32.rrotate bit32.rshift",
  "coroutine.create coroutine.resume coroutine.status coroutine.wrap coroutine.yield io.close io.flush io.input io.lines io.open io.output io.read io.tmpfile io.type io.write io.stdin io.stdout io.stderr os.clock os.date os.difftime os.execute os.exit os.getenv os.remove os.rename os.setlocale os.time os.tmpname coroutine.running io.popen module package.loaders package.seeall package.config package.searchers package.searchpath require package.cpath package.loaded package.loadlib package.path package.preload",
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
  TABS_DEFAULT,
  0
};


