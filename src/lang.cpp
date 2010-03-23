/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2009-2010 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License version 3 as
  published by the Free Software Foundation.

  This software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/



#include <fx.h>
#include <Scintilla.h>
#include <SciLexer.h>
#include <FXScintilla.h>

#include "compat.h"
#include "scidoc.h"
#include "prefs.h"

#include "lang.h"

#define COMMNT_FG "#a04040"
#define PREPRC_FG "#4040e0"
#define NUMBER_FG "#0000ff"
#define OPERTR_FG "#000000"
#define STRING_FG "#00c000"
#define STREOL_FG "#ff0000"
#define _WORD1_FG "#b06000"
#define _WORD2_FG "#6000b0"
#define _WORD3_FG "#cc00cc"
#define SCALAR_FG "#00bbbb"
#define __ORANGE_ "#ff8000"
#define __PURPLE_ "#cc00cc"
#define ___BLUE__ "#0000ee"
#define __GREEN__ "#00cc00"
#define __DKRED__ "#cc6060"
#define _HERE_BG_ "#ffffee"

#define _DEFLT_BG "\0\0\0\0\0\0\0"
#define _DEFLT_FG "\0\0\0\0\0\0\0"

static LangStyle LangNULL = {
  NULL,
  SCLEX_NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  0,
  TABS_DEFAULT
};



#include "langlist.h"



static int KeywordListsCount(LangStyle*lang)
{
  int i=0;
  while (lang->words[i]) { i++; }
  return i;
}



void SetKeywordList(LangStyle*lang, int index, const FXString &keywords)
{
  FXuint flag=1<<index;
  if (!lang) { return; }
  if ((index<0)||(index>=KeywordListsCount(lang))) { return; }
  if (strcmp(keywords.text(),lang->words[index])==0) { return; }
  if ( lang->mallocs & flag ) {
    free(lang->words[index]);
    lang->words[index]=NULL;
  }
  lang->words[index]=strdup(keywords.text());
  lang->mallocs |= flag;
}



LangStyle*GetLangByName(const char*name)
{
  if (name) {
    LangStyle*lang;
    for (lang=languages; lang->name; lang++) {
      if (strcmp(name, lang->name)==0) { return lang; }
    }
  }
  return NULL;
}



void FreeAllKeywordLists()
{
  LangStyle*lang;
  for (lang=languages; lang->name; lang++) {
    int i;
    for (i=0; lang->words[i]; i++) {
      if (lang->mallocs & (1<<i)) {
        free(lang->words[i]);
      }
    }
    if (lang->mallocs & LANGSTYLE_MASK_ALLOCD) {
      free(lang->mask);
    }
    if (lang->mallocs & LANGSTYLE_APPS_ALLOCD) {
      free(lang->apps);
    }
  }
}


LangStyle*GetLangFromDocName(const char*filename)
{
  LangStyle*lang;
  for (lang=languages; lang->name; lang++) {
    if (PathMatch(lang->mask,filename,FILEMATCH_FILE_NAME|FILEMATCH_CASEFOLD)) { return lang; }
  }
  return NULL;
}


LangStyle*GetLangFromAppName(const char*app)
{
   LangStyle*lang;
  int len=app?strlen(app):0;
  if (!len) {return NULL;}
  for (lang=languages; lang->name; lang++) {
    char*p=lang->apps;
    while (p) {
      while (*p=='|') {p++;}
      if ((strncmp(p,app,len)==0) && ((p[len]=='|')||(p[len]=='\0')))  {
        return lang;
      }
      p=strchr(p, '|');
    };
  }
  return NULL;
}


StyleDef* GetStyleFromId(StyleDef*styles, int id)
{
  StyleDef*sd;
  for (sd=styles; sd->key; sd++) {
    if (sd->id==id) { return sd; }
  }
  return NULL;
}


static long InvertColor(long rgb)
{
  long r,g,b;
  r=FXREDVAL(rgb);
  g=FXGREENVAL(rgb);
  b=FXBLUEVAL(rgb);
  r=255-r;
  g=255-g;
  b=255-b;
  if ((r>0)&&(r<0x80)) { r+=0x40; }
  if ((g>0)&&(g<0x80)) { g+=0x40; }
  if ((b>0)&&(b<0x80)) { b+=0x40; }
  return FXRGB(r,g,b);
}



#define invert ( Settings::instance()? Settings::instance()->InvertColors : false )


void RgbToHex(FXColor rgb, ColorName &clr)
{
  if (invert) { rgb=InvertColor(rgb); }
  snprintf(clr, 8, "#%02x%02x%02x", FXREDVAL(rgb), FXGREENVAL(rgb), FXBLUEVAL(rgb));
}



long HexToRGB(const char* rgb)
{
  int r=0, g=0, b=0;
  sscanf(rgb+1,"%2x%2x%2x",&r,&g,&b);
  return invert? InvertColor(FXRGB(r, g, b)) : FXRGB(r, g, b);
}

