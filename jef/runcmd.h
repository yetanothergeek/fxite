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



class CmdIO: public FXObject {
private:
  CmdIO() {}
  FXInputHandle stdinFD, stdoutFD, stderrFD;
  FXuint remaining;
  FXString SendString;
  FXString RecvString;
  FXString ErrString;
  bool stdoutEOF,stderrEOF;
  FXApp* app;
  char* _shellcmd;
  char* _shellarg;
  bool  multiline_mode;
  bool ensure_final_newline;
  FXObject* target;
  FXSelector message;
  void* userdata;
  bool cleanup(bool rv);
  FXint warning(const char*msg);
  FXint error(const char*msg);
  void appendLine(FXString&s, FXSelector selid);
  bool run(const char *command);
  bool checkCurrDir();
  virtual bool IsCancelled() { return false; }
#ifdef WIN32
  FXival childPid;
  FXInputHandle StdIN_Rd, StdOUT_Wr, StdERR_Wr;
#else
  FXDECLARE(CmdIO);
  FXint childPid;
  bool excess();
  FXint stack_level;
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


protected:
  FXMainWindow* win;
public:
  CmdIO(FXMainWindow *window, const char*shellcmd="/bin/sh -c");
  virtual ~CmdIO();
  bool filter(const char *command, const FXString &input, FXString &output);
  bool lines(const char *command, FXObject *trg, FXSelector sel, bool multiline=false);
  void setUserData(void* p) { userdata=p; }
  void *getUserData() { return userdata; }
};


