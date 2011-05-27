#!/usr/bin/env lua

--[[

  This script parses SciTE's configuration files tries to gather some
  sane defaults for things like keyword lists, highlighting styles,
  and filename/filetype associations.

  It uses that information to generate the header files in the
  ./src/languages/ directory, and the "Makefile.am" for that directory,
  and also creates the ./src/langlist.h file that #include's all the
  separate language headers.

  It uses standard Lua for everything except it needs the "find" command
  from GNU findutils and the "mkdir" command from GNU coreutils.

--]]

local props_dir="/usr/share/scite/"
local temp_dir="./util/"
local lang_dir="./src/languages/"

local wordlist_lua  = temp_dir .. "wordlist.lua"
local wildcards_lua = temp_dir .. "wildcards.lua"
local htmlstyle_lua = temp_dir .. "htmlstyle.lua"
local markdownstyle_lua = temp_dir .. "markdownstyle.lua"


-- Create a table from the string STR, by splitting it at each DELIM
-- Note that DELIM must be a single char!
function split(str,delim)
  local t={}
  local rx="^([^"..delim.."]+)"..delim.."(.*)"
  while str and str:find(delim)
  do
   local f,rem=str:match(rx)
   table.insert(t,f)
   str=rem
  end
  table.insert(t,str)
  return t
end


-- Read the file, merging lines ending with a backslash -
-- Returns a table containing the lines of the "unwrapped" file.
function unwrap_file(filename)
  local fh=io.open(filename, "r")
  local prev_line=""
  local lines={}
  for line in fh:lines() do
    if line:find("\\$") then
      prev_line=prev_line..line:gsub("\\$","")
    else
      prev_line=prev_line..line
      if (#prev_line>0) then
        table.insert(lines, prev_line)
      end
      prev_line=""
    end
  end
  fh:close()
  return lines
end


-- Grab the keyword lists from "Embedded.properties"
function print_words()
  io.output(wordlist_lua)
  io.write(
  [[
  module("wordlist")
  docbook={}
  hypertext={}
  html5={}
  vxml={}
  keywords={}
  keywords2={}
  keywords3={}
  keywords4={}
  keywords5={}
  keywords6={}
  keywords7={}
  keywords8={}
  keywordclass={}
  keywordclass2={}
  keywordclass3={}
  keywordclass4={}
  keywordoptional1={}
  keywordoptional2={}
  keywordstandard={}
  keywords.pascal={}
  keywords.tcl={}
  keywords2.tcl={}
  keywords3.tcl={}
  keywords4.itcl={}
  keywords5.tcl={}
  keywords3.doxygen={}
  keywords.docbook=docbook.attributes
  keywords2.docbook=docbook.elements42
  keywords.bash=bash_keywords1
  keywords2.bash=bash_keywords2
  bash_keywords1=nil
  bash_keywords2=nil
  keywords1=keywords
  ext_instruction=nil
  cpu_instruction=nil
  directive_nasm=nil
  directive=nil
  fpu_instruction=nil
  directive_operand_nasm=nil
  cpu_instruction2=nil
  directive_operand=nil
  register=nil
  ]]
  )

  local kwclasses = {
            "ada", "metapost", "metafun", "pascal", "spice", "primitives",
            "macros", "tex", "etex", "latex", "pdflatex", "context",
            "vendor", "pseudoclass", "pseudoelement" }

  for k,v in ipairs(kwclasses)
  do
    io.write("keywordclass.",v,"={}\n")
  end

  io.write([[
  keywordclass.context.all={}
  keywordclass.macros.context={}
  keywordclass.macros.context.undocumented={}
  keywordclass.macros.eplain={}
  keywordclass.macros.latex={}
  keywordclass.macros.plain={}
  keywordclass.primitives.pdftex={}
  keywordclass.pseudoelement.other="selection"
  ]]
  )

  -- skip any lines that match these:
  local skippers = {
    "^style%."     , "^file%.",
    "^word%."      , "^filter%.",
    "^comment%."   , "^preprocessor%.",
    "^indent%."    , "^braces%.",
    "^tabsize%."   , "^use%.",
    "^command%."   , "^calltip%.",
    "^fold%."      , "^shbang%.",
    "^tab%."       , "^colour%.",
    "^source%."    , "^menu%.",
    "^caret%."     , "^statement%.",
    "^export%."    , "^position%.",
    "^tabbar%."    , "^selection%.",
    "^tabsize%."   , "^statusbar%.",
    "^font%."      , "^print%.",
    "^import[^%w]" , "^if[^%w]",
    "^%#"
  }

  function skipit(line)
    for k,v in ipairs(skippers) do
      if line:find(v) then
        return true
      end
    end
    return false
  end

  -- Print only lines that match these:
  local printers={
    "^keyw"          ,"^[cf]pu_instruction",
    "^register"      ,"^ext_instruction",
    "^directive"     ,"^docbook%.",
    "^bash_keywords" ,"^vxml%.",
    "^hypertext%."   ,"^html5%."
  }

  local lines_out={}

  function printit(line)
    for k,v in ipairs(printers) do
      if line:find(v) then
        line=line:gsub( "%) %$%(", "..");
        line=line:gsub( "%)%|%$%("," or ");
        line=line:gsub( "=\"%$%(", "=");
        line=line:gsub( "%.attributes%) public !doctype", ".attributes..\" public !doctype");
        line=line:gsub( "^keywords=", "keywords.something=");
        line=line:gsub( "^keywordclass%.pascal=","keywordclass.pascal.something=");
        line=line:gsub( "^keywords%.pascal=keywordclass%.pascal","keywords.pascal.something=keywordclass.pascal.something");
        line=line:gsub( "^keywords%.pascal%.package=keywordclass%.pascal","keywords.pascal.package=keywordclass.pascal.something");
        line=line:gsub( "^keywordclass%.primitives%.pdftex=", "keywordclass.primitives.pdftex.something=");
        line=line:gsub( "%.%.keywordclass%.primitives.pdftex%.%.","..keywordclass.primitives.pdftex.something..");
        line=line:gsub( "^keywordclass%.context%.all=", "keywordclass.context.all.something=");
        line=line:gsub( "keywordclass%.python%) ","keywordclass.python .. \"")
        line=line:gsub( "%.prefixes%) filter\"$", ".prefixes .. \"filter\"")
        if line:find("%) \"$") and not line:find("\".*%) \"$")  then
          line=line:gsub("%) \"$", "")
        end
        if not line:find("^keywords%d*.mako") then
          table.insert(lines_out,line)
        end
        return
      end
    end
  end

  for k,v in ipairs(unwrap_file(props_dir.."Embedded.properties")) do
    if not skipit(v) then
      local s=v:gsub("[\t ]+", " ")
      s=s:gsub("= *", "=");
      s=s:gsub("%.%*%.", ".");
      s=s:gsub('"','\\"')
      s=s:gsub("\\\\\"","\\\"")
      if s:find("%.%$%(file%.patterns%.") then
        s=s:gsub("%.%$%(file%.patterns%.", ".")
        s=s:gsub("%)=", "=")
      end
      if s:find("=$%(.*%)$") then
        s=s:gsub("([^=]+)=%$%((.*)%)$","%1=%2")
       else
        s=s:gsub("([^=]+)=(.*)$","%1=\"%2\"")
      end
      printit(s)
    end
  end

  -- These must come before the "^keyword.*" lists
  local prereqs= { "^hypertext%.","^html5%.","^vxml%.","^docbook%." }

  function is_prereq(line)
    for i,v in ipairs(prereqs) do
      if line:find(v) then
        return true
      end
    end
    return false
  end

  for i,line in ipairs(lines_out)
  do
    if is_prereq(line) then
      io.write(line, '\n')
    end
  end

  for i,line in ipairs(lines_out)
  do
    if line:find("^keywordclass") then
      io.write(line, '\n')
    end
  end

  for i,line in ipairs(lines_out)
  do
    if not ( line:find("^keywordclass") or is_prereq(line) ) then
      io.write(line, '\n')
    end
  end

  io.write(
  [[
  docbook=nil
  keywords=nil
  keywordoptional1=nil
  keywordoptional2=nil
  keywordclass=nil
  keywordclass2=nil
  keywordclass3=nil
  keywordclass4=nil
  ]]
  )

end -- print_words()



-- Grab the filetype associations
function print_types()
  io.output(wildcards_lua)
  io.write("filetypes={\n")
  for propfile in io.popen("find ".. props_dir .. " -name '[a-z]*.properties'"):lines() do
    for _,line in ipairs(unwrap_file(propfile)) do
      if (#line>0) and not line:find("^[ \t]*%#") then
        local s=line:gsub("[ \t]+", " "):gsub("^ ", ""):gsub(" $","")
        if s:find("^file%.patterns%.")
        then
          s=s:gsub("^file%.patterns%.","");
          s=s:gsub(";","|");
          s=s:gsub("=", "=\"");
          s=s:gsub("$","\"");
          s=s:gsub("^web=", "html=");
          while s:find("%..*=") do
            s=s:gsub("([^.]+)\.(.*)","%1_%2")
          end
          if not s:find("=\"%$%(") then
            io.write(s..",\n");
          end
        end
      end
    end
  end
  io.write("}\n")
end -- print_types()



-- Grab the colors for html styling
function print_html_styles()
  io.output(htmlstyle_lua)
  io.write('ht_vals={\n')
  for line in io.open("./fxscintilla/Scintilla.iface"):lines() do
    if line:find("^val[ \t]+SCE_H[^A]")
    then
      io.write((line:gsub("val[ \t]+","")), ',\n')
    end
  end
  io.write("}\n")
  io.write('hypertext={}')

  for line in io.open(props_dir.."html.properties"):lines() do
    if line:find("^style\.hypertext\.%d+[ \t]*=") then
      local n=line:match("^style\.hypertext\.(%d+).*") or 0
      s=line:gsub("^style\.hypertext\.%d+[ \t]*=[ \t]*","")
      s=s:gsub("[ \t]", "")
      local t=split(s,',')
      local fg="_DEFLT_FG"
      local bg="_DEFLT_BG"
      for i,v in ipairs(t) do
        if v:find("^fore:#%x+$") then
          fg='"'..v:gsub("^fore:", "")..'"'
        elseif v:find("^back:#%x+$") then
          bg='"'..v:gsub("^back:", "")..'"'
        end
      end
      io.write(string.format("hypertext[%d]={'%s','%s'}\n", n+1, fg, bg))
    end
  end
end -- print_html_styles()



-- Make a style table for the "markdown" language.
function print_markdown_styles()
  io.output(markdownstyle_lua)

  io.write( [[
  markdown = {
    DEFAULT =    { '_DEFLT_FG', 'Normal' },
    LINE_BEGIN = { '_DEFLT_FG', 'Normal' },
    STRONG1 =    { '"#808080"', 'Bold'   },
    STRONG2 =    { '_DEFLT_FG', 'Bold'   },
    EM1 =        { '"#808080"', 'Bold'   },
    EM2 =        { '_DEFLT_FG', 'Bold'   },
    HEADER1 =    { '"#ddb0b0"', 'Bold'   },
    HEADER2 =    { '"#dda0a0"', 'Bold'   },
    HEADER3 =    { '"#dd8080"', 'Bold'   },
    HEADER4 =    { '"#dd6060"', 'Bold'   },
    HEADER5 =    { '"#cc6060"', 'Bold'   },
    HEADER6 =    { '"#bb4040"', 'Bold'   },
    PRECHAR =    { '"#cc00cc"', 'Normal' },
    ULIST_ITEM = { '"#4040e0"', 'Normal' },
    OLIST_ITEM = { '"#4040e0"', 'Normal' },
    BLOCKQUOTE = { '"#00c000"', 'Normal' },
    STRIKEOUT =  { '"#ee0000"', 'Normal' },
    HRULE =      { '"#ffaa00"', 'Normal' },
    LINK =       { '"#0000ee"', 'Bold'   },
    CODE =       { '"#00aaaa"', 'Normal' },
    CODE2 =      { '"#00bbbb"', 'Normal' },
    CODEBK =     { '"#00cccc"', 'Normal' }
  }
  ]]
  )
end --print_markdown_styles()


function create_headers()
  if os.execute("mkdir -p "..lang_dir) ~= 0 then os.exit(1) end

  -- Don't create definitions for these languages:
  local skippers={ "null",
    "sorcins", "clarionnocase", "magiksf",
    "blitzbasic", "powerbasic", "vb", "purebasic", "opal", "mmixal", "nncrontab",
    "abaqus", "powerpro", "powershell", "mssql", "gui4cli", "kix", "progress", "au3",
    "escript", "bullant", "cppnocase", "clarion", "mysql", "baan", "matlab", "octave",
    "php", "phpscript",
    "csound", "tads3", "apdl", "nimrod", "ave", "gap", "lout", "lot", "TAL", "TACL",
    "spice", "specman", "rebol", "scriptol",
    "inno", "nsis", "vbscript", "asp",
    "eiffel", "eiffelkw", "forth",
    "pov", "po", "asn1", "SML", "yaml",
    "PL/M", "asy"
  }

  function skipit(name)
    for i,v in ipairs(skippers) do
      if name==v then
        return true
      end
    end
    return false
  end

  local aliases={ props = "Properties", asy = "Asymptote", hypertext = "html", ["PL/M"] = "PLM" }

  for line in io.open("./util/numdesc.list"):lines() do
    local name,ndesc=line:match("([^%s]+)%s+(%-?%d+)")
    if name and ndesc and not skipit(name) then
      if aliases[name] then
        name=aliases[name]
      end
      local outfile=io.open( lang_dir..name:gsub("[^%w]","_"):lower()..".h", "w")
      for line in io.popen("./util/lexgen.lua "..name.." "..ndesc):lines() do
        outfile:write(line,"\n")
      end
      outfile:close()
    end
  end

  local header_list={}

  local find=io.popen("find ".. lang_dir .." -name '*.h'")
  for line in find:lines() do
    table.insert(header_list, line)
  end
  find:close()

  table.sort(header_list)

  local langlist_h=io.open("./src/langlist.h", "w")
  langlist_h:write(string.format("#include \"%shtml.h\"\n", lang_dir:gsub("^%./src/", "./")))
  langlist_h:write(string.format("#include \"%scpp.h\"\n",  lang_dir:gsub("^%./src/", "./")))

  for i,hdr in ipairs(header_list) do
    if not ( hdr:find('/html.h$') or hdr:find('/cpp.h$') ) then
      langlist_h:write(string.format("#include \"%s\"\n", hdr:gsub("^%./src/", "./")))
    end
  end

  langlist_h:write('\n')
  langlist_h:write('LangStyle languages[]={\n')
  for i,hdr in ipairs(header_list) do
    for line in io.open(hdr, "r"):lines() do
      if line:find("^static LangStyle ") then
        langlist_h:write("  ", (line:gsub("^static LangStyle +([%w_]+).*$", "%1")), ",\n")
      end
    end
  end
  langlist_h:write('  LangNULL\n};\n\n')
  langlist_h:close()


  local makefile_am=io.open("./src/languages/Makefile.am","w")
  makefile_am:write("noinst_HEADERS=\\\n")
  for i,hdr in ipairs(header_list) do
    if i==#header_list then
      makefile_am:write(hdr:gsub(".*/", ""), "\n")
    else
      makefile_am:write(hdr:gsub(".*/", ""), " \\\n")
    end
  end
  makefile_am:close()
end


function cleanup()
  os.remove (wordlist_lua)
  os.remove (wildcards_lua)
  os.remove (htmlstyle_lua)
  os.remove (markdownstyle_lua)
end


print_words()
print_types()
print_html_styles()
print_markdown_styles()
create_headers()
cleanup()

