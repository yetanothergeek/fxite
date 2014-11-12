#!/usr/bin/lua

local scite_props_dir='/usr/share/scite/' -- Where are the SciTE properties files
local outfilename='./util/scite-props-out.lua' -- Output filename for writing the Lua table.

-- Search through the haystack list for an exact match of the needle string.
-- Return the one-based index of the found string, or zero if not found.
function find_literal_in_list(needle,haystack)
  for i,v in ipairs(haystack) do
    if needle==v then return i end
  end
  return 0
end

-- Search through the haystack list for an pattern that matches the needle string.
-- Return the one-based index of the matched pattern, or zero if not found.
function match_pattern_in_list(needle,haystack)
  for i,v in ipairs(haystack) do
    if needle:find(v) then return i end
  end
  return 0
end


-- Compare two arrays of strings, return true if they are identical.
function lists_match(a,b)
  if #a==#b then
    for i=1,#a do
      if a[i] ~= b[i] then return false end
    end
    return true
  end
  return false
end



-- Adds a key and its value to a table, but only if the key is not already defined.
-- (Will not overwrite existing values)
function ifndef_def(tbl, key, value)
  if not tbl[key] then tbl[key]=value  end
end



-- Remove leading and trailing whitespace, convert any tabs to spaces
-- and replace consecutive spaces with a single space.
function white_trim(s)
  s=s:gsub('^%s+','')
  s=s:gsub('%s+$','')
  s=s:gsub('%s+', ' ')
  return s
end



-- These are some lines we want to ignore
function ignore_line(line)
  return 
    #line==0 or
    line:find('^if PLAT_') or
    line:find('^%s*#') or
    not line:find('=')
end



-- Read the file, merging lines ending with a backslash -
-- Returns a table containing the lines of the "unwrapped" file.
function unwrap_file(filename)
  local fh=io.open(filename, "r")
  local prev_line=""
  local lines={}
  for line in fh:lines() do
    if line:find('^if PLAT_') then
    else
      if line:find("\\$") then
        prev_line=prev_line..line:gsub("\\$","")
      else
        prev_line=prev_line..line
        if not ignore_line(prev_line) then
          table.insert(lines, white_trim(prev_line))
        end
        prev_line=""
      end
    end
  end
  fh:close()
  return lines
end



-- Split the "raw" array of strings into a key=value hash table.
function build_lookup_table(t)
  local lookup={}
  for i,line in ipairs(t) do
    local k=line:gsub("^(.-)=.*$", "%1")
    if not lookup[k] then
      local v=line:gsub("^.-=(.*)$", "%1")
      lookup[k]=v
    end
  end
  return lookup
end

local override_lookup={
  '^file%.patterns%.',
  '^colour%..*%.comment%..*$',
  '^colour%.string$',
  '^colour%.number$',
  '^colour%.operator$',
  '^colour%.preproc$',
  '^colour%.keyword$',
}


-- Mostly we just look up the value of key in the lookup table.
-- But sometimes we might want to do some substitutions
function do_lookup(var,lookup)
  if match_pattern_in_list(var,override_lookup)>0 then
    return var:gsub('%.','_')
  end
  return lookup[var]
end




-- Try to resolve any $(variable) names in the 'entry' by finding them in
-- the key/value 'lookup' table. If found, replace the $(variable) with
-- its value and append the resulting string to the 'resolved' array.
-- There are some exceptions, one is for keyword lists which are qualified 
-- with file_patterns_* variables. We treat those variables in a special way,
-- rather than replacing the variable with the list of file patterns 
-- (which would be ugly) we replace the qualifier with the language name 
-- extracted from the variable name. 
-- In other words, "keywords2.$(file.patterns.lua)" becomes simply "keywords2.lua"
-- Other exceptions are handled in the do_lookup() function, above.
function var_to_val(entry, resolved, lookup)
  local cmp=entry
  while entry:find('%$%(.-%)') do
    if entry:find('^keywords.-%.$%(') or entry:find('^lexer.-%.$%(') then 
      local k=entry:gsub('^(.-)=.*$','%1')
      local v=entry:gsub('^.-=(.*)$','%1')
      local var=k:gsub('^.-%$%((.-)%).*$','%1',1)
      var=var:gsub('%.','_')
      var=var:gsub('^file_patterns_','')
      k=k:gsub('^(.-)%$%(.-%)(.*)$','%1'..var..'%2',1)
      entry=k..'='..v
    else
      local var=entry:gsub('^.-%$%((.-)%).*$','%1',1)
      local val=do_lookup(var,lookup)
      if var:find('keywordclass%.primitives') and not val then 
        print('WTF', var)
      end
      if val then
        val=val:gsub('%%','%%%%')
        entry=entry:gsub('^(.-)%$%(.-%)(.*)$','%1'..val..'%2',1)
      end
    end
    if entry==cmp then break end -- bail out if nothing changed
    cmp=entry
  end
  table.insert(resolved,white_trim(entry))
end


-- Parse a properties file, try to resolve any $(variable) names from 
-- within the file itself and append the results to the 'collector' 
-- array. Each file begins by appending a [section] header to the
-- collector, e.g. "lua.properties" adds a [lua] section.
function parse_file(filename,collector)
  local lang=filename:gsub('^.*/(.*)%..*$','%1')
  if lang=='modula3' then lang='modula' end -- WHY CAN'T PROPS AND LEXER HAVE SAME NAME ???!!!
  table.insert(collector,'['..lang..']')
  local lines=unwrap_file(filename)
  local lookup=build_lookup_table(lines)
  local resolved={}
  for i,line in ipairs(lines) do
    local entry=line:gsub('%s+', ' ')
    var_to_val(entry,resolved,lookup)
  end
  table.sort(resolved)
  for i, line in ipairs(resolved) do table.insert(collector,line) end
end



-- Sort the list of filenames, mostly a natural sort but
-- make sure "Embedded.properties" comes first.
function sort_files(a,b) 
  if a:find('Embedded%.properties$') then
    return true
  else 
    return a<b
  end
end


-- Build a list of files from the properties directory
local files={}
local find =io.popen('find '..scite_props_dir..' -type f -name "*.properties"')
for file in find:lines() do table.insert(files,file) end
find:close()
table.sort(files,sort_files)


-- Parse each file and append the results (unwrapped lines) to
-- the 'collector' table
local collector={}
for i,file in ipairs(files) do parse_file(file,collector) end


-- Now that we have a 'global' list, try again to replace any $(variable)
-- names with their values.
local resolved={}
while true do
  resolved={}
  local lookup=build_lookup_table(collector)
  -- These are used but for some reason never defined...
  ifndef_def(lookup, 'colour.code.comment.document', lookup['colour.code.comment'] or 'fore:#a04040')
  ifndef_def(lookup, 'colour.identifier', 'fore:#000000')
  ifndef_def(lookup, 'file.patterns.as', '*.as;*.asc')
  ifndef_def(lookup, 'file.patterns.cobol','*.cob') -- case mismatch: file.patterns.COBOL in props
  ifndef_def(lookup, 'font.code.base', 'font:courier')
  ifndef_def(lookup, 'font.code.comment.document','italic,bold')
  ifndef_def(lookup, 'font.code.comment', 'italic')
  ifndef_def(lookup, 'font.computer', 'font:courier')
  ifndef_def(lookup, 'font.identifier', 'font:courier')
  ifndef_def(lookup, 'font.monospace',  'font:courier')
  ifndef_def(lookup, 'font.notused','font:courier') 
  ifndef_def(lookup, 'font.opal', 'font:courier')
  ifndef_def(lookup, 'font.preproc', 'font:courier')
  ifndef_def(lookup, 'font.string.literal', 'font:courier')
  ifndef_def(lookup, 'keywordclass.pseudoelement.other','')
  ifndef_def(lookup, 'style.stringeol', 'fore:#ff0000')
--ifndef_def(lookup, ('keywordclass.primitives.pdftex.experimental','')
  for i,entry in ipairs(collector) do var_to_val(entry,resolved,lookup) end
  if lists_match(collector,resolved) then break end -- bail out if nothing changed
  collector=resolved
end


-- Now we can parse the "flat" list of properties into a table of
-- sub-tables, each sub-table represents a separate properties file.
local complex={}
local current=nil
for i,line in ipairs(resolved) do
 if line:find('^%[.*%]$') then
   current=line:gsub('[%[%]]','')
   complex[current]={}
 else
    local k=line:gsub('^(.-)=.*$','%1')
    local v=line:gsub('^.-=(.*)$','%1')
    complex[current][k]=v
 end
end

-- Remove any entries from the "Embedded" section if they are also
-- referenced in another section
for sect,entries in pairs(complex) do
  if sect ~= 'Embedded' then
    for k,v in pairs(entries) do
      if complex['Embedded'][k] then complex['Embedded'][k]=nil end
    end
  end
end


-- There seems to be an SQL language "keywords5" property that exists in 
-- the [Embedded] section but not in the [sql] section. This loop tries to
-- re-unite any such orphaned properties with their rightful parents.
for k,v in pairs(complex['Embedded'])
do
  local tail=k:gsub('.*%.','') -- last segment of the dot-delimited key
  if complex[tail] then
    if not complex[tail][k] then complex[tail][k]=v end
    complex['Embedded'][k]=nil
  end
end



local skip_langs={ -- Languages we don't care about
  'null',
  'sorcins', 'clarionnocase', 'magiksf',
  'blitzbasic', 'powerbasic', 'vb', 'purebasic', 'opal', 'mmixal', 'nncrontab',
  'abaqus', 'powerpro', 'powershell', 'mssql', 'gui4cli', 'kix', 'progress', 'au3',
  'escript', 'bullant', 'cppnocase', 'clarion', 'mysql', 'baan', 'matlab', 'octave',
  'php', 'phpscript',
  'csound', 'tads3', 'apdl', 'nimrod', 'ave', 'gap', 'lout', 'lot', 'TAL', 'TACL',
  'spice', 'specman', 'rebol', 'scriptol',
  'inno', 'nsis', 'vbscript', 'asp',
  'eiffel', 'eiffelkw', 'forth',
  'pov', 'asn1', 'SML', 'yaml',
  'PL/M', 'asy', 'oscript', 
  'ecl','asl','avs','tacl','tal'
}

function skip_lang(lang)
  return find_literal_in_list(lang,skip_langs)>0
end


local skip_props={ -- Properties we don't care about
  '^all%.',
  '^api%.',
  '^are%.',
  '^autocomplete%.',
  '^background%.',
  '^block%.',
  '^bookmark%.',
  '^braces%.',
  '^buffered%.',
  '^buffers$',
  '^calltip%.',
  '^caret%.',
  '^code%.',
  '^command%.',
  '^comment%.',
  '^default%.',
  '^edge%.',
  '^eol%.',
  '^export%.',
  '^ext%.',
  '^file%.',
  '^file$',
  '^filter%.',
  '^filter$',
  '^find%.',
  '^fold%.',
  '^font%.',
  '^highlight%.',
  '^imports%.',
  '^indent%.',
  '^keyHTML$',
  '^keyMake$',
  '^keyText$',
  '^keyXML$',
  '^line%.',
  '^margin%.',
  '^menu%.',
  '^open%.',
  '^os%.',
  '^output%.',
  '^position%.',
  '^print%.',
  '^replace%.',
  '^selection%.',
  '^shbang%.',
  '^source%.',
  '^split%.',
  '^statement%.',
  '^statusbar%.',
  '^tab%.',
  '^tabbar%.',
  '^tabsize$',
  '^technology$',
  '^top%.',
  '^use%.',
  '^user%.',
  '^view%.',
  '^word%.',
  '^keywordclass%d+%.lua4$',
  '^keywordclass%d+%.lua5$',
  '^keywordclass%d+%.lua50$',
  '^keywordclass%d+%.lua51$',
  '^keywordclass%d+%.lua52$',
  '^keywordclass%.lua4$',
  '^keywordclass%.lua5$',
  '^keywordclass%.lua50$',
  '^keywordclass%.lua51$',
  '^keywordclass%.lua52$',
  '^keywordclass3%.bit32$',
  '^keywordclass4%.package$',
  '^keywordclass%.context%.all%.%a%a$',
  '^keywords%d+%.context',
  '^keyword.*flash$',
}


-- Properties we don't care about
function skip_prop(prop)
  return match_pattern_in_list(prop,skip_props)>0
end



-- FXiTe doesn't care about font names and/or sizes in style defintions,
-- only foreground/background colors and styles like "italic" or "bold"
function strip_font_styles(k,v)
  if k:find('^style%..-%.%d+$') then
    v=','..v
    v=v:gsub(',font:[^,]+',',')
    v=v:gsub(',size:[^,]+',',')
    v=v:gsub(',+',',')
    v=v:gsub('^,+','')
    v=v:gsub(',+$','')
  end
  return v
end


-- Create a sorted list of section names
local section_names={}
for sect,entries in pairs(complex) do 
  if not skip_lang(sect) then table.insert(section_names,sect) end
end
table.sort(section_names)



-- Since the keys are each in their own section, having the section name 
-- as a part of the key name seems redundant.
function strip_sect_from_key(k,sect)
  if k:find('%.'..sect) then
    if k:find('%.'..sect..'%.') then k=k:gsub('%.'..sect..'%.', '.') end
    if k:find('%.'..sect..'$') then k=k:gsub('%.'..sect..'$', '') end
  end
  return k
end



-- Sort keys naturally, except style numbers are sorted numerically
function key_sorter(a,b)
  if a:find('^style%..*%.%d+') and b:find('^style%..*%.%d+') then
    local a_str=a:gsub('^(.*)%.%d+$','%1')
    local b_str=b:gsub('^(.*)%.%d+$','%1')
    if a_str==b_str then
      a_num=tonumber((a:gsub('^.*%.(%d+)$','%1')))
      b_num=tonumber((b:gsub('^.*%.(%d+)$','%1')))
      return a_num<b_num
    end
  end
  return a<b
end


-- Some lexers support multiple languages and may have different sets of keyword lists
-- for each language (or dialect) but if they only have one set of keyword lists, we
-- can remove the type qualifiers from the keyword lists' key names.
function has_multi_keyword_keys(t)
  local has={}
  for k,v in pairs(t) do
    if k:find('^keywords') then
      local kw=k:gsub('^(.-)%..*$', '%1')
      if has[kw] then
        return true
      else
        has[kw]=true
      end
    end
  end
  return false
end

local masks={}

local outfile=io.open(outfilename,'w')
outfile:write('properties={\n')
for i,sect in ipairs(section_names) do
  local key_names={}
  for k,v in pairs(complex[sect]) do -- Create a sorted list of keys
    if not skip_prop(k) then table.insert(key_names,k) end
    if k:find('^file%.patterns%.'..sect..'$') then
      masks[sect]=v
    end
  end
  if #key_names>0 then
    outfile:write(sect,'={\n')
    table.sort(key_names, key_sorter)
    local strip_kw=not has_multi_keyword_keys(complex[sect])
    for i,k in ipairs(key_names) do
      local v=strip_font_styles(k,complex[sect][k])
      k=strip_sect_from_key(k:lower(),sect)
      if k:find('%.%*%.') then k=k:gsub('%.%*%.','.') end
      if k:find('%.%*$') then k=k:gsub('%.%*$','') end
      if v:find(' \\ ') then v=v:gsub(' \\ ',' \\\\ ') end
      if v:find('"') then v=v:gsub('"','\\"') end
      if sect=='lua' then k=strip_sect_from_key(k,'lua5x') end
      if sect=='html' then k=strip_sect_from_key(k,'hypertext') end
      if strip_kw and k:find('^keywords') then k=k:gsub('^(.-)%..*$', '%1') end
      if k=='lexer' and v==sect then v='' end
      k=k:gsub('%.','_')
      v=white_trim(v)
      if #v>0 then outfile:write(k,'="',v,'",\n') end
    end 
    if masks[sect] then outfile:write('masks="', masks[sect]:gsub(';','|'), '",\n'); end
    outfile:write('},\n')
  end
end
outfile:write('}')
outfile:close()

