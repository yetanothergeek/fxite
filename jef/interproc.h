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
  HANDLE listen_fd;
  FXString sock_name;
  void OpenThePipe();
public:
  long onCheckPipe(FXObject*o,FXSelector sel,void*p);
  enum {ID_CHECK_PIPE,ID_LAST};
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
