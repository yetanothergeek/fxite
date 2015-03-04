/*
  ctokens.h - a simple and naive parser for "C" style languages.
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

#ifndef CTOKENS_H
#define CTOKENS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  synNULL=0,      /* Reserved, must be first */
  synNAME,        /* Identifiers */
  synKWORD,       /* Keywords */
  synMLCOMM,      /* Multi-line comments */
  synSLCOMM,      /* Single-line comments */
  synCHAR,        /* Character constants */
  synSTRING,      /* Quoted strings */
  synSYMBOL,      /* Operators and other symbols */
  synNUMBER,      /* Numbers */
  synSPACE,       /* White space */
  synPREPROC,     /* Preprocessor directives */
  synEOL,         /* New-line characters */
  synLAST         /* Reserved, must be last */
} TokenType;


typedef struct _TokenInfo TokenInfo;

typedef int (*token_cb)(const TokenInfo*info);

struct _TokenInfo {
  size_t i;                /* Internal counter */
  const char*src;          /* Pointer to beginning of source code */
  const char*token;        /* Pointer to beginning of token */
  size_t len;              /* Length of token */
  TokenType type;          /* Type of token */
  size_t line;             /* Line number this token begins on, first line is #1 */
  size_t col;              /* Column number this token begins on, first column is zero */
  size_t vspan;            /* Number of extra lines consumed by a multi-line token */
  size_t hspan;            /* Number of columns consumed by last line of a multi-line token */
  token_cb cb;             /* User defined callback, invoked by tokenizer */
  int is_utf8;             /* True if source is encoded as UTF-8 */
  void*user_data;          /* User defined data, passed untouched to callback */
  const char**keywords;    /* List of keywords for this language; Last element must be NULL! */
  unsigned int*len_kwords; /* Lengths of words in keywords list (internal use) */
};



/*
 NOTES:
 * Operators are always handled individually: for example += is treated as 
   two tokens, a "plus" sign followed by an "equals" sign. 
 * Preprocessor macros are treated monolithically, their individual elements
   are not parsed separately. For example:
     #include <stdio.h>
   is considered a single token.
 * There is no pre-defined list of keywords, If the "keywords" argument is NULL,
   keywords will be reported as identifiers.
 * For numbers with a leading (plus+ or minus-) sign, the sign is treated as a
   separate unary operator, it is not considered part of the number. However, 
   an embedded (plus+ or minus-) sign IS considered part of a number for exponents,
   for example:  
     6.02E-23
   is considered a single token.
*/
void tokenize(const char*src, token_cb cb, void*user_data, const char*keywords[], int is_utf8);

#ifdef __cplusplus
}
#endif

#endif

