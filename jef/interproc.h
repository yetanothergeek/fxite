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

#ifndef INTERPROC_H
#define INTERPROC_H

#ifdef WIN32
# include <windows.h>
#endif

class InterProc: public FXObject {
  FXDECLARE(InterProc);
  InterProc() {}
private:
  FXApp*app;
  FXSelector message;
  FXObject *target;
  long ExecuteClientRequest(FXString*s);
#ifdef WIN32
private:
  const FXString *commands;
  WPARAM LocalServerID;
  WPARAM RemoteClientID;
  WPARAM LocalClientID;
  WPARAM RemoteServerID;
  FXString DdePrefix;
  FXString DdeTopic;
  ATOM atomApplication;
  ATOM atomTopic;
  LPARAM atoms;
  HGLOBAL hCommand;
  bool found_server;
  void MakeAtoms();
  void KillAtoms();
  void DoDispatch(FXID hwnd,unsigned int iMsg,unsigned int wParam,long lParam);
public:
  static void dispatchEvent(FXID hwnd,unsigned int iMsg,unsigned int wParam,long lParam);
  enum {ID_LAST};
#else
private:
  FXString sock_name;
  FXHash *connlist;
  int listen_fd;
public:
  long onSocketRead(FXObject*o,FXSelector sel,void*p);
  enum {
    ID_SOCKET_ACCEPT,
    ID_SOCKET_READ,
    ID_LAST
  };
#endif
public:
  bool ClientSend(FXTopWindow *win, const FXString &data);
  void StartServer(FXTopWindow *win, FXObject*trg, FXSelector sel);
  void StopServer();
  InterProc(FXApp*a, const FXString &connection, const FXString &topic=FXString::null);
};

#endif
