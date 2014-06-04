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

#ifdef WIN32

#include <io.h>
#include <process.h>
#include <windows.h>

#include <fx.h>

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
    bool ispathext=false;
    if (ext.empty()) {
      ispathext=true;
    } else {
      ext.prepend('.');
      FXString pathext=FXSystem::getEnvironment("PATHEXT").lower();
      if ( pathext.empty() ) { pathext=".com;.exe;.bat;.cmd"; }
      for (FXint i=0; i<=pathext.contains(';'); i++) {
        if (pathext.section(';',i)==ext) {
          ispathext=true;
          break;
        }
      }
    }
    if (ispathext) {
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
  return CreateProcess(NULL,cmdline.text(),NULL,NULL,true,flags,NULL,NULL,&si,pi);
}



// Peek (without blocking) to see if we have anything to read
// Returns number of bytes available
// If the call fails, puts the error code in "status"
// (Usually status will be ERROR_BROKEN_PIPE when the external process finishes.)
static DWORD BytesAvail(HANDLE f, DWORD&status) {
  char peek[1];
  DWORD BytesRead=0;
  DWORD TotalBytesAvail=0;
  BOOL ok = PeekNamedPipe(f,&peek,1,&BytesRead,&TotalBytesAvail,NULL);
  status=ok?0:GetLastError();
  return ok?TotalBytesAvail:0;
}



// Launch an external command
bool CmdIO::run(const char *command)
{
  stdinFD = stdoutFD = stderrFD = StdIN_Rd = StdOUT_Wr = StdERR_Wr = NULL;
  RecvString = ErrString = "";
  PROCESS_INFORMATION pi;
  SECURITY_ATTRIBUTES sa;
  FXString cmd=command;
  static const ssize_t bufsize=1024;
  char buf[bufsize];
  ZeroMemory(&sa,sizeof(SECURITY_ATTRIBUTES));
  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.bInheritHandle = true;
  sa.lpSecurityDescriptor = NULL;
  DWORD exitcode=0;

  if (!CreatePipe(&stdoutFD, &StdOUT_Wr, &sa, 0)) { CleanupAndReturn("creating stdout pipe", false); }
  if (!SetHandleInformation(stdoutFD, HANDLE_FLAG_INHERIT, 0)) { CleanupAndReturn("setting up stdout",false); }

  if (!CreatePipe(&stderrFD, &StdERR_Wr, &sa, 0)) { CleanupAndReturn("creating stderr pipe", false); }
  if (!SetHandleInformation(stderrFD, HANDLE_FLAG_INHERIT, 0)) { CleanupAndReturn("setting up stderr",false); }

  if (!CreatePipe(&StdIN_Rd, &stdinFD, &sa, 0)) { CleanupAndReturn("creating stdin pipe", false); }
  if (!SetHandleInformation(stdinFD, HANDLE_FLAG_INHERIT, 0)) { CleanupAndReturn("setting up stdin", false); }

  if (CreateChildProcess(win, cmd, StdIN_Rd, StdOUT_Wr, StdERR_Wr, &pi)) {
    SafeClose(StdOUT_Wr);
    SafeClose(StdERR_Wr);
    SafeClose(StdIN_Rd);
    while (1) {
      app->runWhileEvents();
      if (IsCancelled()) {
        if (pi.hProcess) { TerminateProcess(pi.hProcess, 1); }
        break;
      }
      if (remaining>0) {
        FXuval sent=0;
        BOOL ok=WriteFile(stdinFD,SendString.text()+(SendString.length()-remaining),FXMIN(remaining,256),&sent,NULL);
        if (!ok) { /* Just ignore it? */ }
        remaining -= sent;
      } else {
        SafeClose(stdinFD);
      }
      DWORD stderrStatus;
      DWORD stderrBytesAvail=BytesAvail(stderrFD,stderrStatus);
      if (stderrBytesAvail>0) {
        ZeroMemory(buf,bufsize);
        FXuval rcvd=0;
        BOOL ok = ReadFile(stderrFD,buf,FXMIN((FXuval)bufsize,stderrBytesAvail),&rcvd,NULL);
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
      DWORD stdoutStatus;
      DWORD stdoutBytesAvail=BytesAvail(stdoutFD,stdoutStatus);
      if (stdoutBytesAvail>0) {
        ZeroMemory(buf,bufsize);
        FXuval rcvd=0;
        BOOL ok = ReadFile(stdoutFD,buf,FXMIN((FXuval)bufsize,stdoutBytesAvail),&rcvd,NULL);
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
      if ((stdoutStatus!=0)&&(stderrStatus!=0)&&(remaining==0)) { break; }
    }
  } else {
    CleanupAndReturn("creating process", false);
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
  DWORD gle=GetLastError();
  if (gle && (gle!=ERROR_BROKEN_PIPE)) {
    WinErrMsg(ErrString,"running command",GetLastError());
  }
  Cleanup();
  return (exitcode==0);
}

#endif

