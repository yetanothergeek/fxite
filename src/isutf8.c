/*
 * isutf8.c - do the input files look like valid utf-8 byte streams?
 *
 * Copyright (C) 2005  Lars Wirzenius
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */


/*
  Editor's Note:
   This file was shamelessly stolen from the original author and subsequently butchered
   by the perpetrator, so please don't blame Lars Wirzenius if you find any bugs!
  - Jeff Pohlmeyer 2009-2013
*/

#include <assert.h>
#include <stdio.h>
#include <string.h>



#define ISUTF8_VERSION "1.1"


/*
 * Code to indicate an invalid UTF8 character.
 */
enum { INVALID_CHAR = 0xffffffff };


/*
 * Produce shortest UTF8 encoding of a 31-bit value in 'u', returning it
 * in the array 'buf'. Return the number of bytes in the encoded value.
 * If the value is too large (more than 32 bits or would take more than
 * 'maxbytes' bytes), return -1.
 */
static int encodeutf8(unsigned long u, unsigned char *buf, size_t maxbytes)
{
  static const struct {
    unsigned int nbytes;
    unsigned long max;
  } tab[] = {
    { 1, 0x0000007F },
    { 2, 0x000007FF },
    { 3, 0x0000FFFF },
    { 4, 0x001FFFFF },
    { 5, 0x03FFFFFF },
    { 6, 0x7FFFFFFF },
  };
  static const int ntab = sizeof(tab) / sizeof(tab[0]);
  int i, j;
  if (u > tab[ntab-1].max) { return -1; }
  for (i = 0; i < ntab; ++i) {  if (u <= tab[i].max) { break;} }
  assert(i < ntab);
  if (tab[i].nbytes > maxbytes) { return -1; }
  if (tab[i].nbytes == 1) {
    buf[0] = u;
  } else {
    for (j = tab[i].nbytes-1; j > 0; --j) {
      buf[j] = 0x80 | (u & 0x3f);
      u >>= 6;
    }
    unsigned char mask = ~(0xFF >> tab[i].nbytes);
    buf[0] = mask | u;
  }
  return tab[i].nbytes;
}


/*
 * Return number of ones at the top of a byte.
 *
 * I'm pretty sure there is a fancy trick to do this without a loop,
 * but I'm too tired to figure it out now. --liw
 */
static int high_ones(int c) {
  int n;
  for (n = 0; (c & 0x80) == 0x80; c <<= 1) { ++n; }
  return n;
}


/*
 * Decode a UTF8 character from an array of bytes. Return character code.
 * Upon error, return INVALID_CHAR.
 */
static unsigned long decodeutf8(unsigned char *buf, int nbytes)
{
  unsigned long u;
  int i, j;
  if (nbytes <= 0) { return INVALID_CHAR; }
  if (nbytes == 1) {
    if (buf[0] >= 0x80) { return INVALID_CHAR; }
    return buf[0];
  }
  i = high_ones(buf[0]);
  if (i != nbytes) { return INVALID_CHAR; }
  u = buf[0] & (0xff >> i);
  for (j = 1; j < nbytes; ++j) {
    if ((buf[j] & 0xC0) != 0x80) { return INVALID_CHAR; }
    u = (u << 6) | (buf[j] & 0x3f);
  }
  /* UTF-8 can't contain codes 0xd800-0xdfff (UTF-16 surrogates) OR 0xfffe OR 0xffff */
  if (u >= 0xD800 && u <= 0xDFFF) { return INVALID_CHAR; }
  if (u == 0xFFFE || u == 0xFFFF) { return INVALID_CHAR; }
  return u;
}


/*
  Determine if the contents of an open file form a valid UTF8 byte stream.
  Do this by collecting bytes for a character into a buffer and then decode
  the bytes and re-encode them and compare that they are identical to the
  original bytes. If any step fails, return 'H' for "high" (extended ASCII).
  If EOF is reached, return 'U' for UTF-8, or 'T' for text if the file might also
  be interpreted as seven-bit US-ASCII. At the same time, also check for control
  characters: we will accept carriage-returns, line-feeds, form-feeds, audibles[1],
  and horizontal or vertical tabs - Any other characters with a value less than 32
  would probably indicate this is not a text file at all, so return 'B' for binary.
  [1] Some autoconf configure scripts contain BEL chars as an awk field separator.
 */
static char get_stream_encoding(FILE *file) {
  enum { MAX_UTF8_BYTES = 6 };
  unsigned char buf[MAX_UTF8_BYTES];
  unsigned char buf2[MAX_UTF8_BYTES];
  int nbytes=0;
  int nbytes2;
  int c;
  unsigned long code;
  char result='T';
  fseek(file,0,SEEK_SET);
  if ((getc(file)==0xFF)&&(getc(file)==0xFE)) { return 'e'; } else { fseek(file,0,SEEK_SET); }
  if ((getc(file)==0xFE)&&(getc(file)==0xFF)) { return 'E'; } else { fseek(file,0,SEEK_SET); }
  if ((getc(file)==0xEF)&&(getc(file)==0xBB)&&(getc(file)==0xBF)) { return 'M'; } else { fseek(file,0,SEEK_SET); }  
  for (;;) {
    c = getc(file);
    if (c != EOF) {
      if ( (c<32) && ( (c==0) || (!strchr("\n\t\r\f\v\a",c)) ) ) {
        /* If the very last byte is [SUB] it's probably an ancient CP/M text file. */
        if ((c=='\032') && (getc(file)==EOF)) return 'T';
        /* Probably not a text file, so bail out now. */
        return 'B';
      }
      if ((result=='T') && (c >= 0x80)) {
        /* Can't be 7-bit, so it's either valid UTF-8, "extended" ASCII, or binary. */
        result='U';
      }
    }
    if (result!='H') {
      if (c == EOF || c < 0x80 || (c & 0xC0) != 0x80) {
        /* New char starts, deal with previous one. */
        if (nbytes > 0) {
          code = decodeutf8(buf, nbytes);
          if (code == INVALID_CHAR) { result='H'; }
          nbytes2 = encodeutf8(code, buf2, MAX_UTF8_BYTES);
          if (nbytes != nbytes2 || memcmp(buf, buf2, nbytes) != 0) { result='H'; }
        }
        nbytes = 0;
        /* If it's UTF8, start collecting again. */
        if (c != EOF && c >= 0x80) { buf[nbytes++] = c; }
      } else {
         /* This is a continuation byte, append to buffer. */
         if (nbytes == MAX_UTF8_BYTES) { result='H'; }
         buf[nbytes++] = c;
      }
    }
    if (c == EOF) { break; }
  }
  if (nbytes != 0) { return 'H'; }
  return result;
}



/*
Test the contents of FILENAME and return one of the following values:

  B: Binary file ( contains null bytes or control codes not normally found in text. )
  T: Plain US-ASCII text file ( with no extended characters. )
  H: High (extended ASCII) text file.
  U: UTF-8 encoded text file w/o BOM (content validated).
  M: UTF-8 BOM (content not validated)
  Z: Zero-length (empty) file.
  F: Failure, could not read the file.
  e: UTF-16LE BOM
  E: UTF-16BE BOM
Notes:
  A return value of 'T' could also be treated as valid UTF-8
  Unknown encodings might be incorrectly returned as 'B' or 'H' !!!

*/

char get_file_encoding(const char*filename)
{
  FILE *file=fopen(filename,"rb");
  if (file) {
    char rv='F';
    if ( getc(file)==EOF ) {
      rv='Z';
    } else {
      rv=get_stream_encoding(file);
    }
    fclose(file);
    return rv;
  } else {
    return 'F';
  }
}


#ifdef TEST_FOR_GET_FILE_ENCODING
  int main (int argc, char*argv[]) { printf("%c\n", get_file_encoding(argv[1])); }
#endif

