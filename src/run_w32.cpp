/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2009-2011 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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

#ifdef WIN32

#include <io.h>
#include <process.h>
#include <windows.h>

#include <fx.h>

#include "compat.h"

#include "listdlls.h"

#include "intl.h"
#include "runcmd.h"

#define SafeClose(f) { \
  if ( (f) && (f!=INVALID_HANDLE_VALUE) ) { \
    CloseHandle(f); \
    f=NULL; \
  } \
}



// Puts a Windows system error message into the FXString err.
static void WinErrMsg(FXString &err, const char*msg, int code) {
  LPVOID lpMsgBuf;
  FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
                 NULL, code, MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
  for (char*p=(char*)lpMsgBuf; *p; p++) { if (*p=='\r') { *p=' '; } }
  err.format("Error %s:\n[%d] %s", msg, code, (char*)lpMsgBuf);
  LocalFree(lpMsgBuf);
}


#define Cleanup() \
  SafeClose(StdIN_Rd); \
  SafeClose(StdOUT_Wr); \
  SafeClose(StdERR_Wr); \
  SafeClose(stdinFD); \
  SafeClose(stdoutFD); \
  SafeClose(stderrFD);


#define CleanupAndReturn(s,b) \
  if (!(b)) { WinErrMsg(ErrString, s,GetLastError()); } \
  Cleanup(); \
  return (b);


/*
  For most console-based commands, we want to use the CREATE_NO_WINDOW flag with
  CreateProcess() to keep that annoying black window from flashing on the screen.
  But the problem with CREATE_NO_WINDOW is: if the launched application normally
  creates a GUI window, the window doesn't get created, so the application can't
  be used (or closed). So we check the exe file's dependencies to see if it looks
  like a GUI application. There are probably numerous reasons why this approach
  is just plain wrong, but I really don't see any easy solution.
*/
int ListDllsCB(const char*name, void*user_data)
{
  if (   (strncasecmp(name, "gdi",    3)==0)
      || (strncasecmp(name, "comctl", 6)==0)
      || (strncasecmp(name, "comdlg", 6)==0)
      || (strncasecmp(name, "msvbvm", 6)==0)
  )
  {
    *((bool*)user_data)=true;
    return 0;
  }
  return 1;
}


// Launch external application
bool CreateChildProcess(FXMainWindow*win, FXString &cmdline, HANDLE StdIN_Rd, HANDLE StdOUT_Wr, HANDLE StdERR_Wr, PROCESS_INFORMATION *pi)
{
  STARTUPINFO si;
  ZeroMemory( pi, sizeof(PROCESS_INFORMATION) );
  ZeroMemory( &si, sizeof(STARTUPINFO) );
  si.cb = sizeof(STARTUPINFO);
  si.hStdError = StdERR_Wr;
  si.hStdOutput = StdOUT_Wr;
  si.hStdInput = StdIN_Rd;
  si.dwFlags |= STARTF_USESTDHANDLES;
  si.dwFlags |=STARTF_USESHOWWINDOW;
  DWORD flags=CREATE_NO_WINDOW;
  char exename[MAX_PATH+1]="\0";
  if ( (FXuval)(FindExecutable(cmdline.text(),NULL,exename)) > 32 ) {
    static char myname[MAX_PATH+1]="\0";
    if  (!myname[0]) { GetModuleFileName(NULL,myname,MAX_PATH); }
    if (strcasecmp(myname,exename)==0) { // Trying to execute myself results in a deadlock!
      FXMessageBox::error(win, MBOX_OK, _("Command error"),
        _("%s is not able to launch itself."), FXPath::title(exename).text());
      return 0;
    }
    FXString ext = cmdline[0]=='"'?cmdline.section('"',1):cmdline.section(' ',0);
    ext=FXPath::extension(ext).lower();
    if ((!ext.empty())&&(ext!="exe")&&(ext!="bat")&&(ext!="com")) {
      // If the file is not executable, run the associated application instead.
      cmdline.prepend("\" ");
      cmdline.prepend(exename);
      cmdline.prepend('"');
    }
    bool isgui=false;
    switch (ListDlls(exename, ListDllsCB, &isgui)) {
      case LISTDLL_ERR_BAD_PE: { /* Take a wild guess, this is a UPX file with a GUI */
        isgui=true;
        break;
      }
    }
    if (isgui) {
      flags=0;
      si.dwFlags &= ~STARTF_USESHOWWINDOW;
    }
  }
  return CreateProcess(NULL,cmdline.text(),NULL,NULL,TRUE,flags,NULL,NULL,&si,pi);
}



// Set up a file handle for a child process:
//   DuplicateHandle() for Win9x; SetHandleInformation() for WinNT
static bool SetupHandle(HANDLE &FD)
{
  if (IsWin9x()) {
    HANDLE CurrProc=GetCurrentProcess();
    HANDLE tmpFD=NULL;
    if (DuplicateHandle(CurrProc,FD,CurrProc,&tmpFD,DUPLICATE_SAME_ACCESS,FALSE,DUPLICATE_SAME_ACCESS)) {
      CloseHandle(FD);
      FD=tmpFD;
      return true;
    } else {
      return false;
    }
  } else {
    return SetHandleInformation(FD, HANDLE_FLAG_INHERIT, 0);
  }
}



// Launch an external command - note that this windows implementation
// uses blocking I/O - the "canceler" is ignored!
bool CmdIO::run(const char *command, bool*canceler)
{
  stdinFD = stdoutFD = stderrFD = StdIN_Rd = StdOUT_Wr = StdERR_Wr = NULL;
  RecvString = ErrString = "";
  PROCESS_INFORMATION pi;
  SECURITY_ATTRIBUTES sa;
  FXString cmd=command;
  static const ssize_t bufsize=1024;
  char buf[bufsize];
  char TempDir[MAX_PATH];
  char TempOut[MAX_PATH];
  char TempErr[MAX_PATH];
  ZeroMemory(&sa,sizeof(SECURITY_ATTRIBUTES));
  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.bInheritHandle = TRUE;
  sa.lpSecurityDescriptor = NULL;
  DWORD exitcode=0;
  ZeroMemory(TempDir,MAX_PATH);
  ZeroMemory(TempOut,MAX_PATH);
  ZeroMemory(TempErr,MAX_PATH);
  cmd.substitute("%F%", FXSystem::getEnvironment("f"), true);
  cmd.substitute("%f%", FXSystem::getEnvironment("f"), true);
  cmd.substitute("%L%", FXSystem::getEnvironment("l"), true);
  cmd.substitute("%l%", FXSystem::getEnvironment("l"), true);
  if (SendString.empty()&&!IsWin9x()) {   // Output pipe only, no need for input...
    if (!CreatePipe(&stdoutFD, &StdOUT_Wr, &sa, 0)) { CleanupAndReturn("creating stdout pipe", false); }
    if (!SetHandleInformation(stdoutFD, HANDLE_FLAG_INHERIT, 0)) { CleanupAndReturn("setting up stdout",false); }
    if (!CreatePipe(&stderrFD, &StdERR_Wr, &sa, 0)) { CleanupAndReturn("creating stderr pipe", false); }
    if (!SetHandleInformation(stderrFD, HANDLE_FLAG_INHERIT, 0)) { CleanupAndReturn("setting up stderr",false); }
    StdIN_Rd=NULL;

    if (CreateChildProcess(win, cmd, StdIN_Rd, StdOUT_Wr,StdERR_Wr, &pi)) {
      SafeClose(StdOUT_Wr);
      SafeClose(StdERR_Wr);
      SafeClose(StdIN_Rd);
      while (1) {
        app->runWhileEvents();
        FXuval rcvd=0;
        ZeroMemory(buf,bufsize);
        BOOL ok = ReadFile(stderrFD,buf,bufsize,&rcvd,NULL);
        if (rcvd>0) {
          ErrString.append(buf,rcvd);
          appendLine(ErrString,SEL_IO_EXCEPT);
        } else {
          if (!ErrString.empty()) {
            ErrString.append('\n');
            appendLine(ErrString,SEL_IO_EXCEPT);
          }
          break;
        }
        if (!ok) { break; }
      }
      while (1) {
        app->runWhileEvents();
        FXuval rcvd=0;
        ZeroMemory(buf,bufsize);
        BOOL ok = ReadFile(stdoutFD,buf,bufsize,&rcvd,NULL);
        if (rcvd>0) {
          RecvString.append(buf,rcvd);
          appendLine(RecvString,SEL_IO_WRITE);
        } else {
          if (!RecvString.empty()) {
            if (ensure_final_newline) { RecvString.append('\n'); }
            appendLine(RecvString,SEL_IO_WRITE);
          }
          break;
        }
        if (!ok) { break; }
      }
    } else {
      CleanupAndReturn("creating process", false);
    }
  } else {   // Create only input pipe, use temp files for stdout and stderr...
    if (!CreatePipe(&StdIN_Rd, &stdinFD, &sa, 0)) { CleanupAndReturn("creating stdin pipe", false); }
    if (!SetupHandle(stdinFD)) { CleanupAndReturn("setting up stdin", false); }
    DWORD dwRetVal = GetTempPath(MAX_PATH, TempDir);
    if ((dwRetVal==0)||(dwRetVal>MAX_PATH)) {
      CleanupAndReturn("retrieving name of temporary directory", false);
    }
    if (!GetTempFileName(TempDir,"OUT",0,TempOut)) { CleanupAndReturn("getting stdout temp file name", false); }
    if (!GetTempFileName(TempDir,"ERR",0,TempErr)) { CleanupAndReturn("getting stderr temp file name", false); }
    StdOUT_Wr=CreateFile(TempOut,GENERIC_WRITE,0,&sa,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    if (StdOUT_Wr==INVALID_HANDLE_VALUE) { CleanupAndReturn("creating stdout temp file", false); }
    StdERR_Wr=CreateFile(TempErr,GENERIC_WRITE,0,&sa,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    if (StdERR_Wr==INVALID_HANDLE_VALUE) { CleanupAndReturn("creating stderr temp file", false); }
    if (CreateChildProcess(win, cmd, StdIN_Rd, StdOUT_Wr,StdERR_Wr, &pi)) {
      SafeClose(StdIN_Rd);
      SafeClose(StdOUT_Wr);
      SafeClose(StdERR_Wr);
      while (remaining>0) {
        FXuval sent=0;
        BOOL ok=WriteFile(stdinFD,SendString.text()+(SendString.length()-remaining),remaining,&sent,NULL);
        remaining -= sent;
        if ((sent==0)&&(!ok)) { break; }
      }
      SafeClose(stdinFD);
      SafeClose(stdoutFD);
      SafeClose(stderrFD);
    } else {
      CleanupAndReturn("creating process", false);
    }
  }
  do {
    app->runWhileEvents();
  } while (WaitForSingleObject(pi.hProcess,1000)==WAIT_TIMEOUT);
  // Wait up to 10 seconds for a valid exit code from child process...
  FXint timeout=10;
  do {
    app->runWhileEvents();
    GetExitCodeProcess(pi.hProcess,&exitcode);
    if (exitcode==STILL_ACTIVE) {
      Sleep(1000);
      timeout--;
      if (timeout==0) { break; } // Give up, maybe a rogue process exited with STILL_ACTIVE
    } else { break; }
  } while (1);
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
  SafeClose(stdinFD);
  SafeClose(stdoutFD);
  SafeClose(stderrFD);
  SafeClose(StdIN_Rd);
  SafeClose(StdOUT_Wr);
  SafeClose(StdERR_Wr);
  if (TempOut[0]) {   // Read 'stdout' output text from temp file...
    stdoutFD=CreateFile(TempOut,GENERIC_READ,0,&sa,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if (stdoutFD!=INVALID_HANDLE_VALUE) {
      FXuval rcvd;
      do {
        ZeroMemory(buf,bufsize);
        BOOL ok=ReadFile(stdoutFD,buf,bufsize,&rcvd,NULL);
        if (rcvd>0) {
          RecvString.append(buf,rcvd);
          appendLine(RecvString,SEL_IO_WRITE);
        } else {
          if (!RecvString.empty()) {
            if (ensure_final_newline) { RecvString.append('\n'); }
            appendLine(RecvString,SEL_IO_WRITE);
          }
          break;
        }
        if (!ok) { break; }
      } while (1);
      SafeClose(stdoutFD);
      RecvString.substitute("\r", "", true);
    } else {
      WinErrMsg(ErrString, "opening output log", GetLastError());
    }
    DeleteFile(TempOut);
  }
  if (TempErr[0]) {   // Read 'stderr' error text from temp file...
    stderrFD=CreateFile(TempErr,GENERIC_READ,0,&sa,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if (stderrFD!=INVALID_HANDLE_VALUE) {
      FXuval rcvd;
      do {
        ZeroMemory(buf,bufsize);
        BOOL ok=ReadFile(stderrFD,buf,bufsize,&rcvd,NULL);
        if (rcvd>0) {
          ErrString.append(buf,rcvd);
          appendLine(ErrString,SEL_IO_EXCEPT);
        } else {
          if (!ErrString.empty()) {
            ErrString.append('\n');
            appendLine(ErrString,SEL_IO_EXCEPT);
          }
          break;
        }
        if (!ok) { break; }
      } while (1);
      SafeClose(stderrFD);
      ErrString.substitute('\r', ' ', true);
    } else {
      WinErrMsg(ErrString, "opening error log", GetLastError());
    }
    DeleteFile(TempErr);
  }
  DWORD gle=GetLastError();
  if (gle && (gle!=ERROR_BROKEN_PIPE)) {
    WinErrMsg(ErrString,"running command",GetLastError());
  }
  Cleanup();
  return (exitcode==0);
}

#endif

