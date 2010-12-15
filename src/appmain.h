/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2009-2010 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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


class AppClass: public FXApp {
  FXDECLARE(AppClass);
private:
  TopWindow* mainwin;
  bool quitting;
  AppClass() {}
  FXString sock_name;
  FXString server_name;
  void ClientParse();
  void ExecuteClientRequest();
  void CreatePathOrDie(const FXString &dirname);
  void CreateConfigDir();
#ifdef WIN32
  FXString dde_string;
  virtual long dispatchEvent(FXID hwnd,unsigned int iMsg,unsigned int wParam,long lParam);
#else
  int sock_fd;
#endif
  bool is_server;
  bool InitClient();
  bool InitServer();
  FXString srv_commands;
  FXString cli_commands;
  FXString configdir;
  FXString sessionfile;
public:
  enum{
    ID_CLOSEALL=FXApp::ID_LAST,
    ID_SOCKET_READ,
    ID_LAST
  };
  long onCmdCloseAll(FXObject*o,FXSelector sel,void*p);
#ifndef WIN32
  long onSocketRead(FXObject*o,FXSelector sel,void*p);
#endif
  AppClass(const FXString& name, const FXString& title);
  void MainWin(TopWindow*w) { mainwin=w; }
  TopWindow*MainWin() { return mainwin; }
  void exit(FXint code);
  virtual void init(int& argc,char** argv,bool connect=TRUE);
  bool IsServer() { return is_server; }
  FXString &Commands() { return srv_commands; }
  const FXString &ConfigDir() { return configdir; }
  const FXString &SessionFile() { return sessionfile; }
  const FXString &SocketName() { return sock_name; }
  const FXString &ServerName() { return server_name; }
#ifndef FOX_1_6
  FXString migration_errors;
#endif
};

