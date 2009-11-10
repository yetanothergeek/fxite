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



class CmdIO: public FXObject {
private:
  CmdIO() {}
  FXInputHandle stdinFD, stdoutFD, stderrFD;
  FXuint remaining;
  FXString SendString;
  FXString RecvString;
  FXString ErrString;
  bool stdoutEOF,stderrEOF;
  FXMainWindow* win;
  FXApp* app;
  char* _shellcmd;
  char* _shellarg;
  FXList* _list;
  bool* _canceler;
  bool cleanup(bool rv);
  FXint warning(const char*msg);
  FXint error(const char*msg);
  void appendLine(FXString&s);
  bool run(const char *command, bool*command_timeout);
#ifdef WIN32
  FXival childPid;
  FXInputHandle StdIN_Rd, StdOUT_Wr, StdERR_Wr;
#else
  FXDECLARE(CmdIO);
  pid_t childPid;
  bool excess();
public:
  long onData(FXObject*o,FXSelector sel,void*p);
  long onTimer(FXObject*o,FXSelector sel,void*p);
  enum {
    ID_IO_STDIN,
    ID_IO_STDOUT,
    ID_IO_STDERR,
    ID_TIMER,
    ID_LAST
  };
#endif

public:
  CmdIO(FXMainWindow *window, const char*shellcmd="/bin/sh -c");
  virtual ~CmdIO();
  bool filter(const char *command, const FXString &input, FXString &output, bool*canceler=NULL);
  bool list(const char *command, FXList *lines, bool*canceler=NULL);

};


