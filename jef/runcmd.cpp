/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2009-2013 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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


#include <fx.h>

#include "compat.h"
#include "intl.h"
#include "runcmd.h"


CmdIO::CmdIO(FXMainWindow *window, const char*shellcmd):FXObject()
{
  win=window;
  if (shellcmd) {
    _shellcmd=shellcmd?strdup(shellcmd):NULL;
    _shellarg=strchr(_shellcmd, ' ');
    if (_shellarg) {
      *_shellarg='\0';
      _shellarg++;
    } else {
      _shellarg=NULL;
    }
  } else {
    _shellcmd=NULL;
    _shellarg=NULL;
  }
  stdinFD=0;
  stdoutFD=0;
  stderrFD=0;
  remaining=0;
  SendString="";
  RecvString="";
  ErrString="";
  stdoutEOF=false;
  stderrEOF=false;
  app=window->getApp();
  target=NULL;
  message=0;
  ensure_final_newline=false;
}



CmdIO::~CmdIO()
{
  if (_shellcmd) { free(_shellcmd); }
}




void CmdIO::appendLine(FXString&s, FXSelector selid)
{
  if (target&&message) {
    FXint nlines=s.contains('\n');
    FXString trailer=s.section('\n', nlines);
    s.trunc(s.length()-trailer.length());
    if (multiline_mode) {
      target->handle(this, FXSEL(selid,message), (void*)(&s));
    } else {
      for (FXint i=0; i<nlines; i++) {
        FXString sect=s.section('\n',i);
        target->handle(this, FXSEL(selid,message), (void*)(&sect));
      }
    }
    s=trailer;
  }
}




FXint CmdIO::warning(const char*msg)
{
  return FXMessageBox::warning(win, MBOX_YES_NO, _("Shell Command"),
    "%s:\n\n%s\n\n%s", _("Command warning"), msg, _("Cancel output?"));
}



FXint CmdIO::error(const char*msg)
{
  return FXMessageBox::error(win, MBOX_YES_NO, _("Shell Command"),
    "%s:\n\n%s\n\n%s", _("Command error"), msg, _("Cancel output?"));
}



bool CmdIO::checkCurrDir()
{
  if (FXSystem::setCurrentDirectory(FXSystem::getCurrentDirectory())) {
    return true;
  } else {
    return FXMessageBox::warning(win, MBOX_YES_NO, _("Shell Command"),
      "%s:\n\n%s\n%s\n%s\n\n%s",
      _("Command warning"),
      _("Unable to determine current working directory."),
      _("This can cause problems with some external commands."),
      _("A new working directory can be selected via the \"File\" menu."),
      _("Cancel command?")
    ) == MBOX_CLICKED_NO;
  }
}



bool CmdIO::filter(const char *command, const FXString &input, FXString &output)
{
  SendString=input.text();
  remaining=SendString.length();
  message=0;
  target=NULL;
  ensure_final_newline=false;
  if (!checkCurrDir()) { return false; }
  bool success=run(command);
  if (IsCancelled()) {
    return false;
  }
  output=RecvString.text();
  if (success) {
    return ErrString.empty() ? true : ( warning(ErrString.text() ) == MBOX_CLICKED_NO );
  } else {
    return ( error(ErrString.empty() ? _("Unknown error.") : ErrString.text() ) == MBOX_CLICKED_NO );
  }
}



bool CmdIO::lines(const char *command, FXObject *trg, FXSelector sel, bool multiline)
{
  message=sel;
  target=trg;
  multiline_mode=multiline;
  ensure_final_newline=true;
  if (!checkCurrDir()) { return false; }
  return run(command);
}

