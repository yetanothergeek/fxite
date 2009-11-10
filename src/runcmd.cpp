/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2009 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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
#include "appwin.h"
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
  _list=NULL;
}



CmdIO::~CmdIO()
{
  if (_shellcmd) { free(_shellcmd); }
}




void CmdIO::appendLine(FXString&s)
{
  if (_list) {
    FXint nl;
    static FXString line;
    FXListItem*item;
    line="";
    while (1) {
      nl=s.find('\n');
      if (nl<0) { break; }
      if (nl>0) {
        line.append(s.text(), nl);
        line.substitute('\t', ' ');
        line.substitute('\r', ' ');
        item=new FXListItem(line);
        _list->appendItem(item, true);
        FXint gni=_list->getNumItems();
        _list->makeItemVisible(gni-1);
        line="";
      }
      s.erase(0,nl+1);
    }
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



bool CmdIO::filter(const char *command, const FXString &input, FXString &output, bool*canceler)
{
  SendString=input.text();
  remaining=SendString.length();
  _list=NULL;
  bool success=run(command,canceler);
  if (canceler && *canceler) {
    return false;
  }
  output=RecvString.text();
  if (success) {
    return ErrString.empty() ? true : ( warning(ErrString.text() ) == MBOX_CLICKED_NO );
  } else {
    return ( error(ErrString.empty() ? _("Unknown error.") : ErrString.text() ) == MBOX_CLICKED_NO );
  }
}



bool CmdIO::list(const char *command, FXList *lines, bool*canceler) {
  _list=lines;
  return run(command,canceler);
}

