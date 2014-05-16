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


class TopWindow;
class Server;
class InterProc;

class AppClass: public FXApp {
  FXDECLARE(AppClass);
private:
  bool quitting;
  AppClass() {}
  FXString sock_name;
  FXString server_name;
  void ParseCommandLine();
  void CreatePathOrDie(const FXString &dirname);
  void CreateConfigDir();
  InterProc *ipc;
#ifdef WIN32
  virtual FXival dispatchEvent(FXID hwnd, FXuint iMsg, FXuval wParam, FXival lParam);
#endif
  FXString commands;
  FXString configdir;
  FXString sessionfile;
  FXString settingsfile;
public:
  enum{
    ID_CLOSEALL=FXApp::ID_LAST,
    ID_IPC_EXEC,
    ID_LAST
  };
  long onCmdCloseAll(FXObject*o,FXSelector sel,void*p);
  long onIpcExec(FXObject*o,FXSelector sel,void*p);
  AppClass(const FXString& name, const FXString& title);
  void exit(FXint code);
  virtual void init(int& argc,char** argv,bool connect=true);
  FXString &Commands() { return commands; }
  const FXString &ConfigDir() { return configdir; }
  const FXString &SessionFile() { return sessionfile; }
  const FXString &SettingsFile() { return settingsfile; }
  const FXString &ServerName() { return server_name; }
  const FXString &Connector() { return sock_name; }
#ifndef FOX_1_6
  FXString migration_errors;
#endif
};

