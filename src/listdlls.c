/* Adapted from PEDUMP - by Matt Pietrek 1994-1998 */

#ifdef WIN32

#include <windows.h>
#include "listdlls.h"

// True if the bounds of *ptr are between base and end
#define within(base,ptr,end) ( \
  (ptr) && ( (char*)ptr > (char*)base ) && ( ((char*)ptr+sizeof(*ptr)) <= (char*)end ) )

// Returns a pointer to the header of a relative virtual address
static PIMAGE_SECTION_HEADER RvaToHdr(LPVOID base, DWORD rva, PIMAGE_NT_HEADERS nt, LPVOID end)
{
  PIMAGE_SECTION_HEADER sect = IMAGE_FIRST_SECTION(nt);
  unsigned i;
  for ( i=0; (i < nt->FileHeader.NumberOfSections) && within(base,sect,end); i++, sect++ ) {
    if ( (rva>=sect->VirtualAddress) && (rva<(sect->VirtualAddress+sect->Misc.VirtualSize))) {
      return within(base,sect,end)?sect:NULL;
    }
  }
  return NULL;
}


static LPVOID RvaToPtr(LPVOID base, DWORD rva, PIMAGE_NT_HEADERS nt, LPVOID end)
{
  PIMAGE_SECTION_HEADER shdr = RvaToHdr(base, rva, nt, end);
  if (shdr) {
    LPVOID rv=(PVOID)(((DWORD)base)+rva-((INT)(shdr->VirtualAddress-shdr->PointerToRawData)));
    return ((rv>base)&&(rv<end))?rv:NULL;
  } else {
    return NULL;
  }
}



static void do_cleanup(HANDLE fh, HANDLE map, void*base)
{
  if (base) { UnmapViewOfFile(base); }
  if (map) { CloseHandle(map); }
  if (fh != INVALID_HANDLE_VALUE) { CloseHandle(fh); }
}

#define RETURN(code) do_cleanup(fh,map,base); return(code);


int ListDlls(LPSTR exename, ListDllsCallback cb, void*user_data)
{
  HANDLE fh=INVALID_HANDLE_VALUE;
  HANDLE map=0;
  LPVOID base=NULL;
  LPVOID end=NULL;
  DWORD size=0;
  DWORD sizehi=0;
  PIMAGE_NT_HEADERS nt=NULL;
  PIMAGE_IMPORT_DESCRIPTOR desc=NULL;
  PIMAGE_SECTION_HEADER sect=NULL;
  DWORD imports=0;

  fh = CreateFile(exename,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
  if ( fh == INVALID_HANDLE_VALUE ) { RETURN(LISTDLL_ERR_CANT_OPEN); }

  size=GetFileSize(fh,&sizehi);
  if (sizehi) { RETURN(LISTDLL_ERR_TOO_BIG); }

  map = CreateFileMapping(fh, NULL, PAGE_READONLY, 0, 0, NULL);
  if (!map) { RETURN(LISTDLL_ERR_MAP); }

  base = MapViewOfFile(map, FILE_MAP_READ, 0, 0, 0);
  if (!base) { RETURN(LISTDLL_ERR_MAPVIEW); }
  end=(char*)base+size;

  if ( ((PIMAGE_DOS_HEADER)base)->e_magic != IMAGE_DOS_SIGNATURE ) { RETURN(LISTDLL_ERR_NOT_EXE); }

  nt = (PIMAGE_NT_HEADERS) ((DWORD)base + ((PIMAGE_DOS_HEADER)base)->e_lfanew);

  if ( (void*)nt > end ) { RETURN(LISTDLL_ERR_BAD_PE); }

  if ( nt->Signature != IMAGE_NT_SIGNATURE ) { RETURN(LISTDLL_ERR_NOT_PE); }

  imports = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
  if ( (imports==0) || (imports>(DWORD)end) ) { RETURN(LISTDLL_ERR_NO_IMP_SECT); }

  sect = RvaToHdr(base,imports,nt,end);
  if (!sect) { RETURN(LISTDLL_ERR_NO_IMP_HDR); }

  desc = (PIMAGE_IMPORT_DESCRIPTOR) RvaToPtr(base,imports,nt,end);
  if (!desc) { RETURN(LISTDLL_ERR_NO_IMP_DESC); }

  while ( within(base,desc,end) && (desc->TimeDateStamp || desc->Name) )  {
    const char*name=(const char*)RvaToPtr(base,desc->Name,nt,end);
    if (!name) { continue; }
    if (!cb(name,user_data)) { break; }
    desc++;
  }
  RETURN(LISTDLL_SUCCESS);
}

/*
  This thing can also be compiled as a standalone program for listing
  dependencies, at least for testing and debugging purposes,  but
  maybe even as a semi-useful utility. Just compile it with -DAS_EXE .
*/
#ifdef AS_EXE
#include <stdio.h>

char*ErrorMessages[] =
{
  "",
  "CreateFile() failed.",
  "File size too large (maximum 4GB).",
  "CreateFileMapping() failed.",
  "MapViewOfFile() failed.",
  "Not an executable (EXE) file.",
  "Not a portable executable (PE) file.",
  "Corrupted file header (maybe UPX?).",
  "Can't find imports section start.",
  "Can't find imports header.",
  "Can't find imports descriptions.",
};


int OnListDll(const char*name, void*user_data)
{
  printf("%s\n", name);
  return 1;
}


int main(int argc, char *argv[])
{
  if ( argc != 2 ) {
    printf( "Usage: %s filename\n", argv[0] );
    return 1;
  }
  int rv=ListDlls(argv[1],OnListDll,NULL);
  if (rv) { printf("%s\n", ErrorMessages[rv]); }
  return rv;
}

#endif


#endif
