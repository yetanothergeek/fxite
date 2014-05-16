/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2009-2014 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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


#ifdef __FreeBSD__
# define _GNU_SOURCE
# define HAVE_GETLINE
#endif

#ifdef linux
# define HAVE_GETLINE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "sl.h"


/*
  If INI_NUMERIC_SORT is defined, keys with embedded integers are sorted in
  natural decimal order, for instance Key_9 will come before Key_10.
*/
#define INI_NUMERIC_SORT



/*
  This getline() doesn't behave exactly like the GNU version,
  but it should be close enough for this purpose...
*/
#ifndef HAVE_GETLINE
static ssize_t getline(char **lineptr, size_t *n, FILE *f)
{
  if (feof(f)) {
    return -1;
  } else {
    long int startpos=ftell(f);
    do {
      char c=(char)fgetc(f);
      if (c=='\n') { break; }
    } while (!feof(f));
    long int endpos=ftell(f);
    if (endpos>startpos) {
      long int size=(endpos-startpos);
      char*buf=malloc(size);
      char*p=buf;
      memset(buf,0,size);
      *n=size;
      size--;
      fseek(f,startpos,SEEK_SET);
      while (size>0) {
        ssize_t rd=fread(p,1,size,f);
        if (rd>0) {
          p+=rd;
          size-=rd;
        } else { break; }
      }
      *lineptr=buf;
      fseek(f,endpos,SEEK_SET);
      return size;
    } else {
      return 0;
    }
  }
}
#endif



typedef int (*SlFunc)(void *a, void *b);


typedef struct {
  void* next;
  char* data;
} item;



typedef struct {
  void* next;
  char* name;
  void* items;
} section;


#ifdef INI_NUMERIC_SORT
static int string_cmp(char *a, char *b)
{
  char*p1,*p2;
  for (p1=a, p2=b; *p1 && *p2; p1++, p2++) {
    if (isdigit(*p1) && isdigit(*p2) && (strncmp(a,b,(p1-a))==0) ) {
      long int n1=strtol(p1,NULL,10);
      long int n2=strtol(p2,NULL,10);
      return (n1<n2)?(-1):(n1>n2)?(1):(0);
    }
  }
  return strcasecmp(a, b);
}
#else
# define string_cmp strcasecmp
#endif


static int section_cmp(section *a, section *b)
{
  return string_cmp(a->name, b->name);
}



static int item_cmp(item *a, item *b)
{
  return string_cmp(a->data,b->data);
}



static int sort_items(section *a, void *b)
{
  a->items=sl_mergesort(a->items,(SlFunc)item_cmp);
  return 0;
}



static int write_item(item *a, FILE *f)
{
  fprintf(f, "%s\n", a->data);
  return 0;
}



static int write_section(section *a, FILE *f)
{
  fprintf(f, "%s\n", a->name);
  sl_map(a->items,(SlFunc)write_item,f);
  fprintf(f, "\n");
  return 0;
}



static void free_item(void *p)
{
  item*i=(item*)p;
  if (i->data) { free(i->data); }
  free(i);
}



static void free_section(void *p)
{
  section*s=(section*)p;
  if (s->name) { free(s->name); }
  sl_free(s->items,free_item);
  free(s);
}


/*
  There is nothing fancy about the parser. It expects the file to be
  "well formed" -- any line that begins with '[' is considered as a
  section header, any other non-empty line is regarded as a key=value
  line under the current section. This means if you feed it a file
  that contains comment lines, you can expect to get a mess in return!
*/
int ini_sort(const char *filename)
{
  FILE*f=fopen(filename, "r");
  if (f) {
    int rv=0;
    void*sects=NULL;
    section*sect=NULL;
    do {
      char*line=NULL;
      size_t len=0;
      ssize_t g=getline(&line, &len, f);
      if (g<0) {
        if (line) {
          free(line);
          line=NULL;
        }
        break;
      }
      if (line) {
        char*p;
        p=strchr(line,'\n');
        if (p) { *p='\0'; }
        p=strchr(line,'\r');
        if (p) { *p='\0'; }
        if (*line=='[') {
          sect=(section*)calloc(1,sizeof(section));
          sect->name=line;
          sects=sl_push(sects, (void*)sect);
        } else {
          if (sect && *line) {
            item*it=(item*)calloc(1,sizeof(item));
            it->data=line;
            sect->items=sl_push(sect->items, it);
          } else {
            free(line);
          }
        }
      }
    } while(1);
    fclose(f);
    sects=sl_mergesort(sects,(SlFunc)section_cmp);
    sl_map(sects,(SlFunc)sort_items,NULL);
    f=fopen(filename,"w");
    if (f) {
      sl_map(sects,(SlFunc)write_section,f);
      if (fclose(f)==0) { rv=1; }
    }
    sl_free(sects,free_section);
    return rv;
  } else { return 0; }
}

