#!/usr/bin/lua

-- Compiles a minimal program from the Scintilla sources to output
-- the lexer ID and "word list" count for each of the compiled-in
-- lexers. This script compiles the binary, runs it, parses its
-- output and writes a Lua table containing the information to
-- a file for use by another script.



-- You can change the "compiler" to g++ if you want, but
-- clang is about 10 seconds faster on my machine
local compiler='clang++'

local src_dir='./fxscintilla/'
local lxinf_bin='./lxinf.bin'
local main=
[[
class WordList;
class Accessor;

#include <cstdio>
#include <cstring>
#include <cctype>
#include <SciLexer.h>
#include <ILexer.h>
#include <LexerModule.h>
#include <Catalogue.h>

int main() {
  printf("lexerinfo = {\n");
  for (int i=0; i<=SCLEX_AUTOMATIC; i++) {
    const LexerModule*lex=Catalogue::Find(i);
    if (lex&&(lex->GetNumWordLists()>=0)) {
      char buf[256];
      int k=0;
      memset(buf,0,sizeof(buf));
      for (int j=0; k<(sizeof(buf)-1); j++) {
        char c=lex->languageName[j];
        if (!c) { break; }
        if (isalnum(lex->languageName[j])) {
          buf[k]=tolower(c);
          k++;
        }
      }
      if (strcmp(buf,"hypertext")==0) {
        memset(buf,0,sizeof(buf));
        strncpy(buf,"html",sizeof(buf)-1);
      }
      printf("%s={id=%d, count=%d},\n", buf, i, lex->GetNumWordLists());
    }
  }
  printf("}\n");
  return 0;
}
]]
local f=io.open('main.cxx','w')
f:write(main)
f:close()


local sources={
'',
'Accessor.cxx',
'Catalogue.cxx',
'CharacterCategory.cxx',
'CharacterSet.cxx',
'Lex*.cxx',
'PropSetSimple.cxx',
'StyleContext.cxx',
'WordList.cxx',
}
local cmd='clang++ -I'..src_dir..' main.cxx '..table.concat(sources, ' '..src_dir)..' -o '..lxinf_bin
io.stderr:write('Compiling lexer information, please wait...\n')
os.execute(cmd)
os.remove('main.cxx')
local exe=io.popen(lxinf_bin)
local out=exe:read('*a')
exe:close()
os.remove(lxinf_bin)
f=io.open('./util/lexer-info-out.lua','w')
f:write(out)
f:close()


