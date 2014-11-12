#!/usr/bin/lua
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


-- There are three stages to the process of generating the header files:
--
--   1. Compile a minimal program from the Scintilla sources to extract the
--      lexer ID and "word list" count for each of the compiled-in lexers.
--      (See "get-lexer-info.lua" for more info.)
--
--   2. Scan all the properties files in SciTE's configuration directory
--      and build a Lua table containing the styles, keyword lists, etc. for
--      each language. (See "get-scite-props.lua" for more info.)
--
--   3. Generate a set of C++ headers containing FXiTe's interpretation of 
--      the information above, along with a "master" header to #include the
--      other headers and a "Makefile.am" file for automake.


local stage=tonumber(arg[1]) or 1
local lexer_info='./util/lexer-info-out.lua'
local scite_props='./util/scite-props-out.lua'

local exists=nil

exists=io.open(lexer_info,'r')
if exists then exists:close() else dofile('./util/get-lexer-info.lua') end
dofile(lexer_info)

exists=io.open(scite_props,'r')
if exists then exists:close() else dofile('./util/get-scite-props.lua') end
dofile(scite_props)

if arg[1] ~= '-k' then
  os.remove(lexer_info)
  os.remove(scite_props)
end

dofile('./util/extra-info.lua')
  
local scintilla_iface='./fxscintilla/Scintilla.iface'
local output_dir='./src/languages/'
local includes={}
local langstyles={}
local lexids={}
local iface_file=io.open(scintilla_iface,'r')







-- Create a table from the string STR, by splitting it at each DELIM
-- Note that DELIM must be a single char!
function split(str,delim)
  local results={}
  local rx="^([^"..delim.."]+)"..delim.."(.*)"
  while str and str:find(delim)
  do
   local field,rem=str:match(rx)
   table.insert(results,field)
   str=rem
  end
  table.insert(results,str)
  return results
end


-- Remove leading and trailing whitespace, convert any tabs to spaces
-- and replace consecutive spaces with a single space.
function white_trim(s)
  s=s:gsub('^%s+','')
  s=s:gsub('%s+$','')
  s=s:gsub('%s+', ' ')
  return s
end





-- Some lexer properties are defined within another lexer, for instance 
-- "bash" is a part of "perl".  This function creates a new property table
-- for the "to" language and populates it with the  information extracted 
-- out of the "from" language. Usually the keyword lists have a prefix 
-- identifying the language, but for e.g. extracting the "batch" lexer from
-- the "others" lexers, the unqualified keyword list names are used. 
-- The "skip_kw" parameter controls this behavior, normally it is "true" 
-- but for cases like others->batch it is "false".
function extract_sub_props(from, to, skip_kw)
  properties[to]={}
  for k,v in pairs(properties[from]) do
    if k:find('keywords.*_'..to..'$') or k:find('^'..to..'_keywords.*') then
      local k2=k:gsub(to, ''):gsub('_+','_'):gsub('^_',''):gsub('_$','')
      properties[to][k2]=v
      properties[from][k]=nil
    elseif k:find('style_'..to..'_%d+$') then
      local k2=k:gsub(to, ''):gsub('_+','_')
      properties[to][k2]=v
      properties[from][k]=nil
    elseif skip_kw and k:find('^keywords') then
    else
      properties[to][k]=v
    end
  end
end

extract_sub_props('perl','bash',true)
extract_sub_props('others','diff',true)
extract_sub_props('others','batch')
extract_sub_props('others','makefile',true)
extract_sub_props('cpp','java',true)
extract_sub_props('fortran','f77',true)
extract_sub_props('cpp','vala',true)
extract_sub_props('html','xml',true)
extract_sub_props('html','mako',true)
extract_sub_props('html','js',true)
extract_sub_props('html','vxml',true)
extract_sub_props('asm','as',true)
extract_sub_props('caml','sml',true)


-- Use the txt2tags styles also for markdown
if properties.txt2tags and not properties.markdown then
  properties.markdown=properties.txt2tags
end

-- These got tangled up somehow
if properties.pascal and properties.pascal.keywords and properties.pascal.keywords_pascal_package then
  properties.pascal.keywords=properties.pascal.keywords_pascal_package
  properties.pascal.keywords_pascal_package=nil
end

-- The "conf" lexer has keywords for Apache, but it actually works
-- well on a lot of other config files. So let's get rid of the
-- specialzed keywords and make it more generic.
if properties.conf then
  if properties.conf.keywords then properties.conf.keywords='' end
  if properties.conf.keywords2 then properties.conf.keywords2='' end
end


-- Delete the doxygen properties from the C++ lexer.
-- (Excessive styling of comments seems like overkill to me.)
if properties.cpp then
  for k,v in pairs(properties.cpp) do
    if k:find('doxygen') then properties.cpp[k]='' end
  end
end


-- Takes an array table of space-delimited word list strings
-- and returns a single, sorted, space-delimited string 
-- containing all unique words from all the lists.
function merge_word_lists(lists)
  local all={}
  local results={}
  for i,list in ipairs(lists) do
    local elems=split(list,' ')
    for j,elem in ipairs(elems) do all[elem]=1 end
  end
  for word,one in pairs(all) do table.insert(results,word) end
  table.sort(results)
  return white_trim(table.concat(results,' '))
end
	

-- Maybe something went wrong with my SciTE-properties parser here. The metapost 
-- lexer from the Scintilla source code only has two keyword lists named "MetaPost"
-- and "MetaFun", but there are four keyword lists defined in my properties table
-- named "keywords", "keywords_metafun", "keywords4", and "keywords5". The "keywords"
-- and "keywords_metafun" lists appear to be identical. The "keywords4" list contains
-- far fewer words, but all the words listed in "keywords4" are already listed in 
-- "keywords". Most of the words in "keywords5" are also already in "keywords", but
-- a couple of them are not. ARGHHHH.... I am just going to dump all of the words
-- into the "keywords" list, remove the other lists, and create a new empty list 
-- named "keywords2" to keep the number of lists the same as the lexer sources.
if properties.metapost then
  properties.metapost.keywords=merge_word_lists({
    properties.metapost.keywords or '',
    properties.metapost.keywords4 or '',
    properties.metapost.keywords5 or '',
  })
  properties.metapost.keywords2=''
  properties.metapost.keywords4=nil
  properties.metapost.keywords5=nil
end


-- Fix up a few of the keyword lists
for key,val in pairs(extra_props) do
  if properties[key] then
     for subkey,subval in pairs(val) do
       properties[key][subkey]=subval
     end
  else
    properties[key]=val
  end
end


-- One more extraction, this one needs to go in after the preceding tweaks
extract_sub_props('html','docbook',true)


-- Some things that have property tables we don't want
properties.SciTEGlobal=nil
properties.as=nil
properties.js=nil
properties.mako=nil
properties.others=nil
properties.registry=nil
properties.rust=nil
properties.sml=nil
properties.vala=nil
properties.vxml=nil



-- Some languages borrow their lexer states from another language
function get_iface_for_lang(lang)
  local tbl={
    cobol='cpp',
    freebasic='powerbasic'
  }
  return tbl[lang] or lang
end


-- Print some warnings, errors and notes to stderr
function say(msg)
  io.stderr:write(arg[0]:gsub('.*/',''), ': ',msg,'\n')
end



-- Scan the "Scintilla.iface" file and build a table of lexical state names and
-- their const values. Some lexers like "html" use more than one prefix for the
-- const names, so we also return a list of prefixes. The third return value 
-- is the const name of the lexer itself.
function read_iface(lang)
  local sce_list={}
  local prefixes,lexer=nil,nil,nil
  ilang=get_iface_for_lang(lang)
  iface_file:seek('set',0)
  for line in iface_file:lines() do
    local s=line:lower()
    local start,stop=nil,nil
    start,stop,lexer,prefixes=s:lower():find('^lex%s+'..ilang..'=(sclex_[^%s]+)%s+(sce_[^_]+_.*)$')
    if prefixes and lexer then break end
  end
  if prefixes and lexer then
    prefixes=white_trim(prefixes:upper())
    prefixes=split(prefixes,' ')
    lexer=lexer:upper()
    for i,prefix in ipairs(prefixes) do
      local needle='^val%s+('..prefix..'[^=]+)%s*=%s*(%d+)$'
      iface_file:seek('set',0)
      for line in iface_file:lines() do
        local start,stop,key,val=line:find(needle)
        if key and val then
          sce_list[val]=key
        end
      end
    end
  else
    say('NOTE: read_iface() failed to parse Scintilla.iface for language "'..lang..'"')
  end
  if lang ~= ilang then
    iface_file:seek('set',0)
    lang=lang:upper()
    for line in iface_file:lines() do
      local start,stop,lx=line:find('^%s*val%s+(SCLEX_'..lang..')%s*=%s*%d+%s*$')
      if lx then
        lexer=lx
        break
      end
    end
  end
  return sce_list,prefixes,lexer
end


-- Usually we can just use a default background color and deduce a 
-- suitable foreground color from the name of the style. But for 
-- some of the more complex lexers, we extract SciTE's colors instead.
function extract_color(fg_or_bg,styles)
  local needle='^'..fg_or_bg..':(#%x+)$'
  for i,v in ipairs(styles) do
    local start,stop,color=v:find(needle)
    if color then return '"'..color..'"' end
  end
  return nil
end


-- These languages will have their colors extracted
-- from SciTE's properties files.
local extracted={
  latex=1,
  html=1,
}


-- Try to come up with a suitable foreground color.
function get_foreground(lang, style_name, styles)
  if lang=='html' then
    if style_name:find('singlestring') or style_name:find('doublestring') then return 'STRING_FG' end
    if style_name:find('tag$') or style_name:find('tagend$') then return 'PREPRC_FG' end
    if style_name:find('^comment$') then return 'COMMNT_FG' end
    if style_name:find('^number$') then return '_DEFLT_FG' end
  end
  if extracted[lang] then
    local fg=extract_color('fore',styles)
    if fg then return fg end
    return '_DEFLT_FG'
  end
  if custom_styles and custom_styles[lang] and custom_styles[lang][style_name] then
    return custom_styles[lang][style_name][1]
  end
  if style_name:find('backticks')    then return '__ORANGE_' end
  if style_name:find('unknown')      then return 'STREOL_FG' end
  if style_name:find('class')        then return '_WORD3_FG' end
  if style_name:find('eol')          then return 'STREOL_FG' end
  if style_name:find('target')       then return '_WORD1_FG' end
  if false then -- Scite has some pre-defined styles, but currently we don't use them
    for i,style in ipairs(styles) do
      if named_fg_styles[style] then
        return named_fg_styles[style]
      end
    end
  end
  if style_name:find('character') or style_name:find('string') then
    return style_name:find('eol') and 'STREOL_FG' or 'STRING_FG'
  end
  if style_name:find('word') then
    if style_name:find('word2') then
      return '_WORD2_FG'
    elseif false and style_name:find('word3') then
      return '_WORD3_FG'
    else
      return '_WORD1_FG'
    end
  end
  if style_name:find('comment')    then return 'COMMNT_FG' end
  if style_name:find('operator')   then return 'OPERTR_FG' end
  if style_name:find('number')     then return 'NUMBER_FG' end
  if style_name:find('scalar')     then return 'SCALAR_FG' end
  if style_name:find('^param$')    then return 'SCALAR_FG' end
  if style_name:find('preprocess') then return 'PREPRC_FG' end
  return '_DEFLT_FG';
end


-- Try to come up with a suitable background color.
function get_background(lang, style_name, styles)
  if lang=='html' and html_default_backgrounds[style_name] then return '_DEFLT_BG' end
  if extracted[lang] then
    local bg=extract_color('back',styles)
    if bg then return bg end
  end
  for i,style in ipairs(styles) do
    if style:find('^colour.*_heredoc$') then return '_HERE_BG_' end
  end
  if style_name:find('^here') then return '_HERE_BG_' end
  return '_DEFLT_BG';
end


function extract_style_attr(styles)
  for i,v in ipairs(styles) do
    local lv = v:lower()
    if lv=='bold' then return 'Bold' end
    if lv=='italic' then return 'Italic' end
  end
  return nil
end


-- Try to come up with a suitable style attribute, 
-- either "Normal", "Bold", or "Italic".
function get_style_attr(lang, style_name, styles)
  if lang=='html' then
    if style_name:find('comment') and not style_name:find('^comment$') then return 'Normal' end
    if html_normal_styles[style_name] then
      return 'Normal'
    end
  end
  if extracted[lang] then
    local attr=extract_style_attr(styles)
    if attr then return attr end
  end
  if custom_styles and custom_styles[lang] and custom_styles[lang][style_name] then
    return custom_styles[lang][style_name][2]
  end
  if style_name:find('word') or style_name:find('operator') or style_name:find('target') then
    return 'Bold'
  elseif style_name:find('comment') then
    return 'Italic'
  elseif style_name:find('unknown') then
    return 'Normal'
  elseif style_name:find('class') then
    return 'Bold'
  end
  return 'Normal'
end


-- Format the style name, const name, foreground/background colors and
-- style attributes into an initializer for a "StyleDef" C struct.
function insert_style_def(outlist,lang,const_name,prefixes,styles)
  local style_name=const_name:gsub('SCE_[^_]+_',''):lower()
  local prefix=#prefixes>1 and const_name:gsub('^SCE_([^_]+_).*$', '%1') or ''
  prefix=prefix:lower()
  if lang=='html' then 
    prefix=prefix:gsub('^h','')
    prefix=prefix:gsub('^_','')
  end
  style_name=prefix..style_name
  table.insert(outlist, string.format(
    '  { "%s", %s, %s, %s, %s },\n',
      style_name:gsub('_',''),
      const_name,
      get_foreground(lang, style_name, styles),
      get_background(lang, style_name, styles),
      get_style_attr(lang, style_name, styles)
  ))
end


-- Parse one language and write its C++ header file.
function read_lang(lang)
  local sce_list,prefixes,lexer=read_iface(lang)
  local lxinf=lexerinfo[lang] or {id=-1, count=0}
  local lxid=lexids[lxinf.id]
  local langstyle ='Lang'..fixup_lexer_name(lang)
  table.insert(langstyles,langstyle)
  table.insert(includes, lang)
  if lexer ~= lxid then
    if lxid then
      say('WARNING: read_lang() ID mismatch  iface="'..(lexer or '?')..'" library="'..lxid..'"')
    else
      say('NOTE: lexer ID for "'..(lexer or '?')..'" not found in library.')
    end
  end
  local outfile=io.open(output_dir..lang..'.h','w')
  outfile:write(string.format(
    '/* lexname=%s lexpfx=%s lextag=%s */\n',
    lang, (prefixes and prefixes[1]) or 'UNKNOWN', lexer or 'UNKNOWN'
  ))

  local outlist={}
  if not properties[lang] then
    say('ERROR: read_lang() failed to find property table for language "'..lang..'"')
    return
  end

  if style_aliases[lang] then
    lexer='SCLEX_'..lang:upper()
    if lexerinfo[style_aliases[lang]] then lxinf.count=lexerinfo[style_aliases[lang]].count end
    if lxid then
      outfile:write('\n')
    else
      outfile:write('#define ',lexer,' SCLEX_',style_aliases[lang]:upper(),'\n\n')
    end
    outfile:write('#define ',lang,'_style ',style_aliases[lang],'_style\n\n')
  else
    outfile:write('\n')
    for k,v in pairs(properties[lang]) do
      local start,stop,style_num=k:find('^style_(%d+)$')
      local const_name=sce_list[style_num]
      if const_name then
        sce_list[style_num]=nil
        insert_style_def(outlist,lang,const_name,prefixes,split(v,','))
      end
    end
    for style_num,const_name in pairs(sce_list) do
      insert_style_def(outlist,lang,const_name,prefixes,{})
    end
    table.sort(outlist)
    outfile:write('static StyleDef ',lang,'_style[] = {\n')
    for i,v in ipairs(outlist) do outfile:write(v) end
    outfile:write('  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }\n')
    outfile:write('};\n\n\n')
  end
  outlist={}
  local keyword_keys={}
  for k,v in pairs(properties[lang]) do
    if k=='keywords' or k:find('keywords%d+') then
      table.insert(keyword_keys,k)
    end
  end
  for i=1,lxinf.count do
    local kw = 'keywords' .. ((i==1 and '') or tostring(i))
    if not properties[lang][kw] then
      table.insert(keyword_keys,kw)
    end
  end
  table.sort(keyword_keys)
  for k,v in pairs(keyword_keys) do
    local s=white_trim(properties[lang][v]or''):gsub('\\','\\\\'):gsub('"','\\"')
    table.insert(outlist,string.format('  "%s",\n',s))
  end
  while #outlist > lxinf.count do
    before=#outlist
    for i,v in ipairs(outlist) do
      if v=='  "",\n' then
        table.remove(outlist,i)
        break
      end
    end
    if before==#outlist then break end
  end
  if #outlist > lxinf.count then
    say('WARNING: keyword list count mismatch for language "'..lang..'"  '..#outlist..' <> '..lxinf.count)
  end
  outfile:write('static const char* '..lang..'_words[]= {')
  if #outlist>0 then
    outfile:write('\n')
    for i,v in ipairs(outlist) do outfile:write(v) end
    outfile:write('  NULL\n')
  else
    outfile:write('NULL')
  end
  outfile:write('};\n\n\n')

  outfile:write('static const char* ',lang,'_mask = "',masks[lang] or properties[lang].masks or '','";\n\n\n')
  outfile:write('static const char* ',lang,'_apps = "',shbang_apps[lang] or '','";\n\n\n')

  outfile:write('static LangStyle ',langstyle,' = {\n')
  outfile:write('  "', lang, '",\n')
  outfile:write('  ', lexer or '0', ',\n')
  outfile:write('  ', lang, '_style,\n')
  outfile:write('  (char**)',lang,'_words,\n')
  outfile:write('  (char*)',lang,'_mask,\n')
  outfile:write('  (char*)',lang,'_apps,\n')
  outfile:write('  0,\n')
  outfile:write(lang=='makefile' and
                '  TABS_ALWAYS,\n' or
                '  TABS_DEFAULT,\n')
  outfile:write('  0\n')
  outfile:write('};\n\n\n')
  if outfile ~= io.stdout then outfile:close() end
end


-- Scan the "Scintilla.iface" file and create a table of lexer const names and their values.
iface_file:seek('set',0)
for line in iface_file:lines() do
  local start,stop,const,val=line:find('^%s*val%s+(SCLEX_[^=]+)%s*=%s*(%d+)%s*$')
  if const and val then
    lexids[tonumber(val)]=const
  end
end


os.execute('mkdir -p '..output_dir)

-- Create the headers in alphabetical order
local sorted={}
for lang,_ in pairs(properties) do table.insert(sorted,lang) end
table.sort(sorted)

-- Create a header file for each language
for i,lang in ipairs(sorted) do read_lang(lang) end

iface_file:close()


-- Create the "Makefile.am" template for automake.
local makefile_am=io.open(output_dir..'Makefile.am', 'w')
makefile_am:write('noinst_HEADERS=\\\n')
makefile_am:write()
for i,lang in ipairs(includes) do
  makefile_am:write(lang,'.h', (i==#includes and '\n') or ' \\\n')
end
makefile_am:close()




-- Custom sort: The "html" and "cpp" headers need to come first,
-- because some of the other headers reference them.

table.sort(includes,
  function(a,b) 
    if b=='html' or b=='cpp' then return false end
    if a=='html' or a=='cpp' then return true end
    return a<b
  end
)


-- Create the main language list header, including all other headers
-- and an array of all the LangStyle structs. 

local langlist_h=io.open('./src/langlist.h','w')
for i,lang in ipairs(includes) do
  langlist_h:write('#include "./languages/',lang,'.h"\n')
end
langlist_h:write('\n')
langlist_h:write('LangStyle languages[]={\n')
for i,lang in ipairs(langstyles) do
  langlist_h:write('  ',lang,',\n')
end
langlist_h:write('  LangNULL\n')
langlist_h:write('};\n\n')
langlist_h:close()

