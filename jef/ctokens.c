/*
  ctokens.c - a simple and naive parser for "C" style languages.
  Copyright (C) 2015 Jeff Pohlmeyer <yetanothergeek@gmail.com>
  
  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the
  "Software"), to deal in the Software without restriction, including
  without limitation the rights to use, copy, modify, merge, publish,
  distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to
  the following conditions:
  
  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "ctokens.h"

static void run_callback(TokenInfo*info)
{
  info->cb(info);
  info->i+=info->len;
  info->line+=info->vspan;
  if (info->vspan) { 
    info->col=info->hspan;
  } else {
    info->col+=info->len;
  }
  info->vspan=0;
  info->hspan=0;
}


#define is_eol(c) (((c)=='\n')||((c)=='\r'))


static void consume_eol(TokenInfo*info, int notify)
{
  int nchars=1;
  switch (info->token[info->len]) {
    case '\n': {
      if (info->token[info->len+1]=='\r') { nchars++; }
      break;
    }
    case '\r': {
      if (info->token[info->len+1]=='\n') { nchars++; }
      break;
    }
    default: { 
      return;
    }
  }
  if (notify) {
    info->type=synEOL;
    info->len=nchars;
    run_callback(info);
    info->line++;
    info->col=0;
  } else {
    info->vspan++;
    info->hspan=0;
    info->len+=nchars;
  }
}



static void consume_sl_comment(TokenInfo*info)
{
  info->type=synSLCOMM;
  while (info->token[info->len]&&!is_eol(info->token[info->len])) {
    info->len++;
  }
  run_callback(info);
}


int consume_mbchar(TokenInfo*info)
{
  if (info->is_utf8) {
    static const unsigned char ExtraUTF8[256] = {
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
      1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
      2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
      3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
    };
    unsigned char bytes=ExtraUTF8[((unsigned char)info->token[info->len])]+1;
    if (bytes>1) {
      info->len+=bytes;
      info->hspan+=bytes;
      return 1;
    }
  }
  return 0;
}

static void consume_ml_comment(TokenInfo*info)
{
  info->type=synMLCOMM;
  info->vspan=0;
  info->hspan=info->col;
  info->len=2;
  while (info->token[info->len]) {
    if (consume_mbchar(info)) { continue; }
    if ((info->token[info->len]=='*') && (info->token[info->len+1]=='/')) {
      info->len+=2;
      info->hspan+=2;
      run_callback(info);
      break;
    } else if (is_eol(info->token[info->len])) {
      consume_eol(info,0);
    } else {
      info->len++;
      info->hspan++;
    }
  } 
}



static void consume_string(TokenInfo*info, int notify)
{
  char c=info->token[info->len];
  info->hspan=info->col+1;
  info->len=(notify?1:info->len+1);
  while (info->token[info->len]) {
    if (consume_mbchar(info)) { continue; }
    if (info->token[info->len]==c) {
      info->len++;
      info->hspan++;
      if (notify) {
        info->type=c=='"'?synSTRING:synCHAR;
        run_callback(info);
      }
      return;
    } else if ((info->token[info->len]=='\\')) {
      info->len++;
      info->hspan++;
      if (is_eol(info->token[info->len])) {
        consume_eol(info,0);
      }
    }
    info->len++;
    info->hspan++;
  }
}



static void consume_preproc(TokenInfo*info)
{
  int terminate=0;
  info->type=synPREPROC;
  info->vspan=0;
  info->hspan=info->col;
  for (info->len=1; ; info->len++,info->hspan++) {
    if (terminate) {
      run_callback(info);
      break;
    }
    switch (info->token[info->len]) {
      case '\\': {
        info->len++;
        if (is_eol(info->token[info->len])) {
          consume_eol(info,0);
        }
        break;
      }
      case '\'':
      case '"' : {
        consume_string(info,0);
        break;
      }
      case '/': { /* If trailing comment at end of macro, process separately */
        if ((info->token[info->len+1]=='/')||(info->token[info->len+1]=='*')) {
          info->len--;
          if (info->vspan) { info->hspan--; }
          terminate=1;
          break;
        }
      }
      default: {
        if (is_eol(info->token[info->len])||(info->token[info->len]=='\0')) {
          info->len--;
          if (info->vspan) { info->hspan--; }
          terminate=1;
          break;          
        }
      }
    }
  }
}



static void consume_number(TokenInfo*info)
{
  const char*p=NULL;
  info->type=synNUMBER;
  if ((info->token[1]=='x')||(info->token[1]=='X')) {
    p=info->token+2;
    while (isxdigit(*p)) { p++; }
  } else {
    p=info->token+1;
    while (isdigit(*p)||(*p=='.')||(*p=='e')||(*p=='E')) {
      if ( ((*p=='e')||(*p=='E')) && ((p[1]=='+')||(p[1]=='-')) ) { p++; }
      p++;
    }
  }
  if (*p=='f') {
    p++;
  } else {
    while (strchr("LlUu",*p)) {p++;} 
  }
  info->len=p-info->token;
  run_callback(info);
}



static void consume_whitespace(TokenInfo*info)
{
  const char*p=info->token+1;
  info->type=synSPACE;
  while (isblank(*p)) { p++; }
  info->len=p-info->token;
  run_callback(info);
}



static void consume_identifier(TokenInfo*info)
{
  info->type=synNAME;
  info->len=1;
  while (isalnum(info->token[info->len])||info->token[info->len]=='_') {
    info->len++;
  }
  if (info->keywords) {
    int i=0;
    const char**kw;
    for (kw=info->keywords; *kw; kw++) {
      if ( (info->len_kwords[i]==info->len) && (strncmp(*kw,info->token,info->len)==0) ) {
        info->type=synKWORD;
        break;
      }
      i++;
    }
  }
  run_callback(info);
}



static void consume_operator(TokenInfo*info)
{
  info->type=synSYMBOL;
  info->len=1;
  run_callback(info);
}



void tokenize(const char*src, token_cb cb, void*user_data, const char*keywords[], int is_utf8)
{
  TokenInfo info;
  memset(&info,0,sizeof(TokenInfo));
  info.src=src;
  info.line=1;
  info.cb=cb;
  info.user_data=user_data;
  info.is_utf8=is_utf8;
  if (keywords) {
    const char **kw;
    int n=0;
    int i=0;
    for (kw=keywords; *kw; kw++) { n++; }
    info.len_kwords=calloc(n,sizeof(unsigned int));
    for (i=0; i<n; i++) {
      info.len_kwords[i]=strlen(keywords[i]);
    }
    info.keywords=keywords;
  }
  while (src[info.i]) {
    info.token=src+info.i;
    info.len=0;
    switch (*info.token) {
      case '\r': 
      case '\n': {
        consume_eol(&info,1);
        break;
      }
      case '\'':
      case '"': {
        consume_string(&info,1);
        break;
      }
      case '#': {
        consume_preproc(&info);
        break;
      }
      case '/': {
        if (info.token[1]=='/') {
          consume_sl_comment(&info);
        } else if (info.token[1]=='*') {
          consume_ml_comment(&info);
        } else {
          consume_operator(&info);
        }
        break;
      }
      case '.': {
        if (isdigit(info.token[1])) {
          consume_number(&info);
        } else {
          consume_operator(&info);
        }
        break;
      }
      default: {
        if (isdigit(*info.token)) {
          consume_number(&info);
        } else if (isblank(*info.token)) {
          consume_whitespace(&info);
        } else if (isalpha(*info.token)||(*info.token=='_')) {
          consume_identifier(&info);
        } else {
          consume_operator(&info);
        }
        break;
      }
    }
  }
  if (info.len_kwords) { free(info.len_kwords); }
}

