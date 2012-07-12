#ifdef WIN32
#include <fx.h>
#include <windows.h>
#define _WIN32_IE 0x0400
#include <shlobj.h>
#include <sys/stat.h>
#include "compat.h"

bool IsDir(const FXString &dn)
{
  if (dn.empty()) { return false; }
  struct stat st;
  if (strchr("/\\", dn[dn.length()-1])) {
    FXString tmp=dn.text();
    tmp.trunc(dn.length()-1);
    return ( (stat(tmp.text(),&st)==0) && S_ISDIR(st.st_mode) );
  } else {
    return ( (stat(dn.text(),&st)==0) && S_ISDIR(st.st_mode) );
  }
}



bool IsWin9x()
{
  OSVERSIONINFO OSversion;
  OSversion.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
  ::GetVersionEx(&OSversion);
  return (OSversion.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS);
}



const char* SystemErrorStr()
{
  DWORD code=GetLastError();
  static TCHAR lpMsgBuf[512];
  lpMsgBuf[0]=0;
  FormatMessage(
      FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL, code, MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT), lpMsgBuf, sizeof(lpMsgBuf), NULL);
  lpMsgBuf[sizeof(lpMsgBuf)-1]=0;
  char *p;
  for (p=(char*)lpMsgBuf; *p; p++) { if (*p=='\r') { *p=' '; } }
  p=strrchr((char*)lpMsgBuf, '\n');
  if ( p && (*(p+1)=='\0')) { *p='\0'; }
  return (const char*)lpMsgBuf;
}



FXString GetRegKey(HKEY hive, LPCSTR path, LPCSTR value) {
  HKEY hKey;
  LONG err;
  LPBYTE data=NULL;
  DWORD size;
  if (RegOpenKeyExA(hive,path,0,KEY_READ,&hKey)==ERROR_SUCCESS) {
    err=RegQueryValueExA(hKey,value,NULL,NULL,NULL,&size);
    if(err==ERROR_SUCCESS) {
      data=(LPBYTE)calloc(size,1);
      err=RegQueryValueExA(hKey,value,NULL,NULL,data,&size);
    }
    RegCloseKey(hKey);
    if (err==ERROR_SUCCESS) {
      FXString rv=(FXchar*)data;
      free(data);
      return rv; // FXString((FXchar*)data);
    }
  }
  return FXString::null;
}



void GetAppDataDir(FXString &AppDataDir)
{
  AppDataDir=FXString::null;
  if (IsWin9x()) {
    FXString CurrentUser=FXString::null;
    DWORD size=0;
    TCHAR* lpBuffer;
    GetUserNameA(NULL,&size);
    lpBuffer=(TCHAR*)calloc(size+1,sizeof(TCHAR));
    if ( GetUserNameA(lpBuffer,&size) ) {
      CurrentUser=(FXchar*)lpBuffer;
    }
    free(lpBuffer);
    if (!CurrentUser.empty()) {
      FXString ProfileList="Software\\Microsoft\\Windows\\CurrentVersion\\ProfileList\\"+CurrentUser;
      FXString ProfileImagePath=GetRegKey(HKEY_LOCAL_MACHINE,ProfileList.text(), "ProfileImagePath");
      if (!ProfileImagePath.empty()) {
        if (FXStat::isDirectory(ProfileImagePath)) {
          AppDataDir=ProfileImagePath+PATHSEP+"Application Data";
        }
      }
    }
  } else {
    FXString AppData=GetShellFolder("AppData");
    if (!AppData.empty()) {
      AppDataDir=AppData.text();
    }
  }
  if (AppDataDir.empty()) {
    TCHAR strPath[MAX_PATH];
    if (SHGetSpecialFolderPath(0,strPath,CSIDL_APPDATA,FALSE)) {
      AppDataDir=(FXchar*)strPath;
    } else {
      AppDataDir="C:\\";
    }
  }
  AppDataDir.substitute('/', PATHSEP, true);
  AppDataDir=FXPath::simplify(FXPath::absolute(AppDataDir));
}


FXString GetShellFolder(const char*shellfolder)
{
  return GetRegKey( HKEY_CURRENT_USER,
             "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",
              shellfolder);
}

#endif
