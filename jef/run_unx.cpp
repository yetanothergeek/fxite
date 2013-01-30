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

#ifndef WIN32

#include <cerrno>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>

#include <fx.h>

#include "compat.h"
#include "intl.h"

#include "runcmd.h"


FXDEFMAP(CmdIO) CmdIOMap[]={
  FXMAPFUNC(SEL_IO_WRITE,CmdIO::ID_IO_STDIN,CmdIO::onData),
  FXMAPFUNC(SEL_IO_READ,CmdIO::ID_IO_STDOUT,CmdIO::onData),
  FXMAPFUNC(SEL_IO_READ,CmdIO::ID_IO_STDERR,CmdIO::onData),
  FXMAPFUNC(SEL_TIMEOUT,CmdIO::ID_TIMER,CmdIO::onTimer),
};

FXIMPLEMENT(CmdIO,FXObject,CmdIOMap,ARRAYNUMBER(CmdIOMap));




/*
  Manually test for cancellation once every second, in case the
  application's event queue gets overloaded.
*/
long CmdIO::onTimer(FXObject*o,FXSelector sel,void*p)
{
  app->repaint();
  if (!IsCancelled()) {
    app->addTimeout(this,ID_TIMER,ONE_SECOND);
  }
  return 1;
}



long CmdIO::onData(FXObject*o,FXSelector sel,void*p)
{
  static const ssize_t bufsize=1024;
  char buf[bufsize];
  stack_level++;
  switch (FXSELID(sel)) {
    case ID_IO_STDIN:{
      if (remaining>0) {
        ssize_t sent=write(stdinFD,SendString.text()+(SendString.length()-remaining),remaining);
        remaining -= sent;
      }
      if (remaining<=0) {
        app->removeInput(stdinFD, INPUT_WRITE);
        fsync(stdinFD);
        close(stdinFD);
      }
      break;
    }
    case ID_IO_STDOUT:{
      while (1) {
        ssize_t rcvd=read(stdoutFD,buf,bufsize);
        if (rcvd>0) {
          RecvString.append(buf,rcvd);
          appendLine(RecvString,SEL_IO_WRITE);
        }
        if (rcvd==0) {
          stdoutEOF=true;
          app->removeInput(stdoutFD, INPUT_READ);
          app->removeInput(stdinFD, INPUT_WRITE);
          if (!RecvString.empty()) {
            if (ensure_final_newline) { RecvString.append('\n'); }
            appendLine(RecvString,SEL_IO_WRITE);
          }
          break;
        }
        if (rcvd<bufsize) { break; }
        if (stack_level<1024) { app->runWhileEvents(); }
        if (excess()) { break; }
      }
      break;
    }
    case ID_IO_STDERR:{
      while (1) {
        ssize_t rcvd=read(stderrFD,buf,bufsize);
        if (rcvd>0) {
          ErrString.append(buf,rcvd);
          appendLine(ErrString,SEL_IO_EXCEPT);
        }
        if (rcvd==0) {
          stderrEOF=true;
          app->removeInput(stderrFD, INPUT_READ);
          app->removeInput(stdinFD, INPUT_WRITE);
          if (!ErrString.empty()) {
            ErrString.append('\n');
            appendLine(ErrString,SEL_IO_EXCEPT);
          }
          break;
        }
        if (rcvd<bufsize) { break; }
        if (stack_level<1024) { app->runWhileEvents(); }
        if (excess()) { break; }
      }
      break;
    }
  }
  stack_level--;
  return 1;
}



#define ERRMSG(w,s) FXMessageBox::error(w, MBOX_OK, _("Shell command"), "%s", s);

static int ERROR(FXMainWindow *parent,const char*s)
{
  FXMessageBox::error(parent, MBOX_OK, _("Shell command"), "%s", s);
  return -1;
}



static pid_t forkCommand(FXMainWindow *win,
  const char *cmd, const char *shell, const char *shellarg, int &stdinFD, int &stdoutFD, int &stderrFD)
{
  int childStdoutFD, childStdinFD, childStderrFD, pipeFDs[2];
  int dupFD;
  pid_t childPid;
  signal(SIGPIPE, SIG_IGN);

  if (pipe(pipeFDs) != 0) { return ERROR(win, _("Internal error opening stdout pipe!")); }
  stdoutFD = pipeFDs[0];
  childStdoutFD = pipeFDs[1];

  if (pipe(pipeFDs) != 0) { return ERROR(win, _("Internal error opening stdin pipe!")); }
  stdinFD = pipeFDs[1];
  childStdinFD = pipeFDs[0];

  if (pipe(pipeFDs) != 0) { return ERROR(win, _("Internal error opening stderr pipe!")); }
  stderrFD = pipeFDs[0];
  childStderrFD = pipeFDs[1];

  childPid = fork();

  if (0 == childPid) {

    close(stdinFD);
    close(stdoutFD);
    close(stderrFD);

    close(fileno(stdin));
    close(fileno(stdout));
    close(fileno(stderr));

    dupFD = dup2(childStdinFD, fileno(stdin));
    if (dupFD == -1) { ERRMSG(win, _("dup of stdin failed")); }

    dupFD = dup2(childStdoutFD, fileno(stdout));
    if (dupFD == -1) { ERRMSG(win, _("dup of stdout failed")); }

    dupFD = dup2(childStderrFD, fileno(stderr));
    if (dupFD == -1) { ERRMSG(win, _("dup of stderr failed")); }

    close(childStdinFD);
    close(childStdoutFD);
    close(childStderrFD);

    setsid();
    if (shellarg) {
      execlp(shell, shell, shellarg, cmd, NULL);
    } else {
      execlp(shell, shell, cmd, NULL);
    }
    fprintf(stderr, _("Error starting shell: %s\n"), shell);
    exit(EXIT_FAILURE);
  }
  if (-1 == childPid ) {
    ERRMSG(win, _("Error starting shell command (fork failed)"));
  }
  close(childStdinFD);
  close(childStdoutFD);
  close(childStderrFD);
  return childPid;
}



bool CmdIO::cleanup(bool rv)
{
  app->removeTimeout(this, ID_TIMER);
  if (stdinFD>0) { close(stdinFD); }
  if (stdoutFD>0) { close(stdoutFD); }
  if (stderrFD>0) { close(stderrFD); }
  fflush(stdout);
  fflush(stderr);
  childPid=-1;
  return rv;
}



bool CmdIO::run(const char *command, bool*canceler)
{
  stdinFD = stdoutFD = stderrFD = 0;
  _canceler=canceler;
  stack_level=0;
  app=(win->getApp());
  stdoutEOF=false;
  stderrEOF=false;
  RecvString="";
  ErrString="";
  app->addTimeout(this,ID_TIMER,ONE_SECOND);
  childPid = forkCommand(win, command, _shellcmd, _shellarg, stdinFD, stdoutFD, stderrFD);
  if (childPid==(-1)) {
    return cleanup(false);
  }
  if (fcntl(stdinFD,  F_SETFL, O_NONBLOCK) < 0) { ERRMSG(win, _("fcntl of stdin failed.")); }
  if (fcntl(stdoutFD, F_SETFL, O_NONBLOCK) < 0) { ERRMSG(win, _("fcntl of stdout failed.")); }
  if (fcntl(stderrFD, F_SETFL, O_NONBLOCK) < 0) { ERRMSG(win, _("fcntl of stderr failed.")); }
  app->AddInput(stdinFD,  INPUT_WRITE, this, ID_IO_STDIN);
  app->AddInput(stdoutFD, INPUT_READ,  this, ID_IO_STDOUT);
  app->AddInput(stderrFD, INPUT_READ,  this, ID_IO_STDERR);
  int status=0;
  int wpid=0;
  int wstatus=0;
  while (!(stdoutEOF&&stderrEOF)) {
    wstatus=0;
    wpid = waitpid(childPid,&wstatus,WNOHANG);
    if (wpid>0) { status=wstatus; }
    app->runWhileEvents();
    if (canceler&&*canceler) {
      app->removeInput(stdinFD,  INPUT_WRITE);
      app->removeInput(stdoutFD, INPUT_READ);
      app->removeInput(stderrFD, INPUT_READ);
      kill(childPid,SIGKILL);
      return cleanup(false);
    }
  }
  waitpid(childPid,&wstatus,0);
  app->removeInput(stdinFD,  INPUT_WRITE);
  app->removeInput(stdoutFD, INPUT_READ);
  app->removeInput(stderrFD, INPUT_READ);
  return cleanup(status==0);
}



bool CmdIO::excess()
{
  if (_canceler&&*_canceler) {
    stdoutEOF=true;
    stderrEOF=true;
    app->removeInput(stdoutFD, INPUT_READ);
    app->removeInput(stderrFD, INPUT_READ);
    app->removeInput(stdinFD, INPUT_WRITE);
    kill(childPid,SIGKILL);
    return true;
  } else {
    return false;
  }
}



#endif

