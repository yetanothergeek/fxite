/* realpath - return the canonicalized absolute pathname
 * Copyright (C) 2001 Steven Barker
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation in version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* Modified 2002-01-26 Charles Wilson:
 *   -- use popt for option handling
 *   -- remove gettext dependency
 */

/* Modified 2010-01-27 Jeffrey Pohlmeyer:
 *   -- Butchered for use with the FXiTe text editor
 */

#ifdef WIN32
#include <stdio.h>
#include <windows.h>
#include <shlobj.h>
#include <olectlid.h>
#include "intl.h"

/*
  Attempt to read a MS-Windows shortcut (*.lnk) file.
  Returns 1 on success and places the target path in *dst,
  on error it returns 0 and places an error message in *dst.
  *dst should be disposed of by the caller by using free()!
*/

#define debug(s) fprintf(stderr, "DEBUG: "s"\n"); fflush(stderr);

static int ole_is_init = 0;

int ReadShortcut(char**dst, const char*src)
{
  HRESULT hres;
  IShellLink *sh_lnk;
  IPersistFile *persist_file;
  int result = 1;
  *dst=(char*)calloc(MAX_PATH,1);
  if (!ole_is_init) {
    hres = OleInitialize(NULL);
    if (hres != S_FALSE && hres != S_OK) {
      strncpy(*dst, _("ReadShortcut: OleInitialize failed"), MAX_PATH-1);
      return 0;
    } else {
      ole_is_init=1;
    }
  }
  hres=CoCreateInstance(&CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,&IID_IShellLink,(void*)&sh_lnk);
  if (!SUCCEEDED(hres)) {
    strncpy(*dst, _("ReadShortcut: CoCreateInstance failed"), MAX_PATH-1);
    return 0;
  }
  hres = sh_lnk->lpVtbl->QueryInterface(sh_lnk, &IID_IPersistFile,(void*)&persist_file);
  if (SUCCEEDED(hres)) {
    WCHAR wsz[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, (LPCSTR)src, -1, wsz, MAX_PATH);
    hres = persist_file->lpVtbl->Load(persist_file, wsz, STGM_READ);
    if (SUCCEEDED(hres)) {
        sh_lnk->lpVtbl->GetPath(sh_lnk, *dst, MAX_PATH, NULL, SLGP_RAWPATH);
    } else {
      strncpy(*dst, _("ReadShortcut: Load failed"), MAX_PATH-1);
      result = 0;
    }
    persist_file->lpVtbl->Release(persist_file);
  } else {
    strncpy(*dst, _("ReadShortcut: QueryInterface failed"), MAX_PATH-1);
    result = 0;
  }
  sh_lnk->lpVtbl->Release(sh_lnk);
  return result;
}

#endif

