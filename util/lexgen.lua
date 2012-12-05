#!/usr/bin/env lua

require "util/wildcards"
require "util/wordlist"
require "util/htmlstyle"
require "util/markdownstyle"
require "util/latexstyle"


filetypes.cpp="*.cc|*.cpp|*.cxx|*.hh|*.hpp|*.hxx|*.ipp|*.sma"
filetypes.c="*.c"
filetypes.makefile="[Mm]akefile*"
filetypes.javascript="*.js"
filetypes.ruby="*.rb"
filetypes.python="*.py"
filetypes.css="*.css"
filetypes.po="*.po|*.pot"

local word_lists={
  c=[[
    asm auto break case char const continue default do double else enum
    extern float for goto if inline int long register return short signed
    sizeof static struct switch typedef union unsigned void volatile while
  ]],

  bash=[[
    break case continue do done elif else esac exit fi for function
    if in select then until while time
  ]],

  javascript=[[
    abstract boolean break byte case catch char class const continue
    debugger default delete do double else enum export extends final
    finally float for function goto if implements import in instanceof
    int interface long native new package private protected public
    return short static super switch synchronized this throw throws
    transient try typeof var void volatile while with
  ]],

  ruby=[[
    __FILE__ and def end in or self unless __LINE__ begin defined? ensure module
    redo super until BEGIN break do false next rescue then when END case else
    for nil retry true while alias class elsif if not return undef yield
  ]],
  
  python=[[
    and as assert break class continue def del elif else except exec
    finally for from global if import in is lambda None not or pass
    print raise return try while with yield
  ]],
}


if filetypes.html and filetypes.php then
  filetypes.html=filetypes.html.."|"..filetypes.php
  filetypes.php=nil
end

local shbangs={
  bash =   "ash|bash|dash|sh",
  c =      "tcc",
  lua =    "lua",
  perl =   "perl",
  python = "python",
  ruby =   "ruby",
}


lexname=(arg and arg[1]) or (io.stderr:write(string.format("Usage: %s <lexer>\n", arg[0])) and os.exit(1) )
numdesc=(arg and arg[2]) and tonumber(arg[2]) or 1


local lexname_lower=lexname:lower()
local lexname_upper=lexname:upper()

local iface_file="./fxscintilla/Scintilla.iface"
local iface=io.open(iface_file, "r")
local values={}
local lextag=nil
local lexpfx=nil
local casename=nil

for line in iface:lines()
do
  if line:find("^%s*lex%s+[%w]+%s*=%s*SCLEX_[%u%d]+%s+SCE_[%u%d]+_?.*$") then
    local tempname=line:match("^%s*lex%s+([%w]+)%s*=%s*SCLEX_[%u%d]+%s+SCE_[%u%d]+_?.*$")
    if tempname and (tempname:upper()==lexname_upper) then 
      if line:find("^%s*lex%s+" .. tempname .. "%s*=%s*SCLEX_[%u%d]+%s+SCE_[%u%d]+_?.*$") then
        lextag,lexpfx=line:match("^%s*lex%s+" .. tempname .. "%s*=%s*(SCLEX_[%u%d]+)%s+(SCE_[%u%d]+_?).*$")
        casename=tempname
      end
    end
  end
end

if lexpfx=="SCE_H_" then lexpfx="SCE_H" end

if not casename then
  local a,b=string.match(lexname, "(.)(.*)")
  casename=((a and b) and a:upper()..b) or lexname
end


-- The "state" argument is like the "MODULE_NAME" portion
-- of "SCE_RB_MODULE_NAME" (where "lexpfx" is "SCE_RB_")
function defstyle(state)
  if (lextag =="SCLEX_HTML") then
    local ht_index=(ht_vals["SCE_H"..state] or -1)+1
    local tbl=hypertext[ht_index]
    if (tbl) then
      if (state:find("DOUBLESTRING")) or (state:find("SINGLESTRING"))
      then
        return "STRING_FG","Normal"
      elseif (state:find("_TAG$")) or (state:find("_TAGEND$"))
      then
        return "PREPRC_FG","Normal"
      elseif (state=="_COMMENT")
      then
        return "COMMNT_FG","Italic"
      else
        return tbl[1],"Normal",tbl[2]
      end
    else 
      return "_DEFLT_FG","Normal"
    end
  end
  if (lextag=="SCLEX_LATEX") then
    local lx_index=(lx_vals["SCE_L_"..state] or -1)+1
    local tbl=latex[lx_index]
    if (tbl) then
      return tbl[1], (state:find("COMMENT") and "Italic" or tbl[3]), tbl[2]
    else
      return "_DEFLT_FG","Normal"
    end

  end
  if (lextag=="SCLEX_MAKEFILE") and state:find("IDENTIFIER")
  then
    return "SCALAR_FG", "Normal"
  end
  if (lextag=="SCLEX_MARKDOWN")
  then
    if markdown[state]
    then
      return markdown[state][1],markdown[state][2] 
    else
      return "_DEFLT_FG","Normal"
    end
  end

  if (lextag =="SCLEX_PO") then
    if state:find("COMMENT")     then return "__GREEN__", "Italic" end
    if state:find("MSGID_TEXT")  then return "__PURPLE_", "Normal" end
    if state:find("MSGSTR_TEXT") then return "___BLUE__", "Normal" end
    if state:find("MSG")         then return "_DEFLT_FG", "Bold"   end
    if state:find("FLAGS")       then return "__ORANGE_", "Normal" end
    if state:find("FUZZY")       then return "STREOL_FG", "Bold"   end
    if state:find("REFERENCE")   then return "_DEFLT_FG", "Normal" end
  end

  if state:find("WORD2")        then return "_WORD2_FG", "Bold"   end
  if state:find("WORD")         then return "_WORD1_FG", "Bold"   end
  if state:find("NUMBER")       then return "NUMBER_FG", "Normal" end
  if state:find("EOL")          then return "STREOL_FG", "Normal" end
  if state:find("UNKNOWN")      then return "STREOL_FG", "Normal" end
  if state:find("STRING")       then return "STRING_FG", "Normal" end
  if state:find("CHARACTER")    then return "STRING_FG", "Normal" end
  if state:find("COMMENT")      then return "COMMNT_FG", "Italic" end
  if state:find("OPERATOR")     then return "OPERTR_FG", "Bold"   end
  if state:find("PREPROCESSOR") then return "PREPRC_FG", "Normal" end
  if state:find("CLASS")        then return "_WORD3_FG", "Bold"   end
  if state:find("SCALAR")       then return "SCALAR_FG", "Normal" end
  if state:find("PARAM$")      then return "SCALAR_FG", "Normal" end
  if state:find("BACKTICKS")    then return "__ORANGE_", "Normal" end
  if state:find("TARGET")       then return "_WORD1_FG", "Bold"   end

  return "_DEFLT_FG","Normal"
end




print(string.format("/* lexname=%s lexpfx=%s lextag=%s */", 
  lexname or "UNKNOWN", lexpfx or "UNKNOWN", lextag or "UNKNOWN"))

if (lexname=="c") or (lexname=="javascript") or (lexname=="java") then
  print(string.format("#define SCLEX_%s SCLEX_CPP", lexname_upper))
else
  if (lexname=="docbook") then
    print(string.format("#define SCLEX_%s SCLEX_HTML", lexname_upper))
    lexpfx="SCE_H"
    lextag="SCLEX_DOCBOOK"
  end
end


print()
--print(string.format("/* %s %s %s */", lexname or "???", lexpfx or "???", lextag or "???"))
if (lexpfx=="SCE_H") and (lextag ~="SCLEX_HTML") then
  print(string.format("#define %s_style html_style\n", lexname_lower))
elseif (lexname=="c") or (lexname=="javascript") or (lexname=="java") then
  print(string.format("#define %s_style cpp_style\n", lexname_lower))
else
  print(string.format("static StyleDef %s_style[] = {",lexname_lower))
  if (lextag and lexpfx) then
    iface=io.open(iface_file, "r")
    for line in iface:lines() do
      if line:find("%s*val%s+"..lexpfx.."[%u%d_]+=%d+") then
        local state=line:match("%s*val%s+"..lexpfx.."([%u%d_]+)=%d+")
        local fg,style,bg=defstyle(state)
        bg=bg or ( (state:find("HERE") and "_HERE_BG_")  or "_DEFLT_BG")
        local entry=string.format("  { \"%s\", %s%s, %s, %s, %s },",state:lower():gsub("_",""),lexpfx,state,fg,bg,style)
        if not ( (lextag =="SCLEX_HTML") and (state:find("^A_")) ) then  print(entry) end
      end
    end
  end
  print("  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }\n};\n\n")
end


function simplify(s)
  if not s then return "" end
  s=s:gsub('[\n\t ]+',' ')
  s=s:gsub('^ ', '')
  s=s:gsub(' $', '')
  return s
end


if (numdesc>0) then
  print(string.format("static const char* %s_words[]= {",lexname_lower))
  local tmp_lang=((lexname=="c") and "cpp") or lexname_lower

  for num = 1,numdesc
  do
    local words=wordlist["keywords"..tostring(num)][tmp_lang]
    if words and (type(words)=="table") then
      local tmpwords=""
      for _,v in pairs(words)
      do
        tmpwords=tmpwords..v
      end
      words=tmpwords
    end
    if (num==1) and word_lists[lexname] then
      words=simplify(word_lists[lexname])
    end
    if (lexname=="conf") then
      words=""
    elseif (lexname=="flagship") and (num==2) then
      words=""
    elseif (lexname=="r") and (num>1) then
      words=""
    end 
    print(string.format("  \"%s\",", words or ""))
  end
  print("  NULL\n};\n\n")
else 
  print(string.format("static const char* %s_words[]= {NULL};\n\n", lexname_lower))
end

print(string.format("static const char* %s_mask = \"%s\";\n\n", lexname_lower, filetypes[lexname_lower] or ""))

print(string.format("static const char* %s_apps = \"%s\";\n\n", lexname_lower, shbangs[lexname_lower] or ""))


print(string.format("static LangStyle Lang%s = {",casename))
print(string.format("  \"%s\",",lexname_lower))
print(string.format("  %s,",lextag or ("SCLEX_"..lexname_upper)))
print(string.format("  %s_style,",lexname_lower))
print(string.format("  (char**)%s_words,",lexname_lower))
print(string.format("  (char*)%s_mask,",lexname_lower))
print(string.format("  (char*)%s_apps,",lexname_lower))
print(              "  0,")
print( (lextag=="SCLEX_MAKEFILE")
       and          "  TABS_ALWAYS,"
       or           "  TABS_DEFAULT,"
     )
print(              "  0")
print(              "};\n\n")


