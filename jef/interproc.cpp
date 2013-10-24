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

#ifdef WIN32
# include <windows.h>
# include "compat.h"
#else
# include <cerrno>
# include <unistd.h>
# include <sys/socket.h>
# include <sys/un.h>
#include <sys/stat.h>
#endif


#ifdef ENABLE_NLS
# include <libintl.h>
# define _(S) gettext(S)
#else
# define _
#endif


#include "interproc.h"


#ifdef WIN32  // For Win32, we use a named pipe for client/server communications...

FXDEFMAP(InterProc) InterProcMap[]={
  FXMAPFUNC(SEL_TIMEOUT, InterProc::ID_CHECK_PIPE, InterProc::onCheckPipe),
};

FXIMPLEMENT(InterProc,FXObject,InterProcMap,ARRAYNUMBER(InterProcMap));



void InterProc::OpenThePipe() {
  listen_fd=CreateNamedPipe(sock_name.text(),PIPE_ACCESS_INBOUND,PIPE_TYPE_BYTE|PIPE_READMODE_BYTE,1,0,1024,50,NULL);
}



long InterProc::onCheckPipe(FXObject*o,FXSelector sel,void*p)
{
  char peek[1];
  DWORD BytesRead=0;
  DWORD TotalBytesAvail=0;
  BOOL ok = PeekNamedPipe(listen_fd, &peek, 1, &BytesRead, &TotalBytesAvail, NULL);
  if (TotalBytesAvail) {
    FXString cmd=FXString::null;
    char buf[512];
    DWORD r=0;
    ok=ReadFile(listen_fd, buf, sizeof(buf), &r,NULL);
    if (ok) {
      cmd.append(buf,r);
    }
    ExecuteClientRequest(&cmd);
    CloseHandle(listen_fd);
    OpenThePipe();
  }
  app->addTimeout(this, ID_CHECK_PIPE, ONE_SECOND);
  return 1;
}



bool InterProc::ClientSend(FXTopWindow *client, const FXString &data)
{
  if (listen_fd!=INVALID_HANDLE_VALUE) {
    fxerror(_("%s: object cannot act as both client and server"), getClassName());
  }
  HANDLE h = CreateFile(sock_name.text(),GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
  if ( h == INVALID_HANDLE_VALUE) { return false; }
  DWORD written=0;
  DWORD len=(DWORD)data.length();
  while (written<len) {
    DWORD w=0;
    if (WriteFile(h,&data.text()[written],len-written,&w,NULL)) {written+=w; } else { break; }
  }
  CloseHandle(h);
  return true;
}


void InterProc::StartServer(FXTopWindow *win, FXObject*trg, FXSelector sel)
{
  target=trg;
  message=sel;
  OpenThePipe();
  app->addTimeout(this, ID_CHECK_PIPE, ONE_SECOND);
}



void InterProc::StopServer()
{
  CloseHandle(listen_fd);
}


#else  // For X11, we use a UNIX socket for client/server communications...


FXDEFMAP(InterProc) InterProcMap[]={
  FXMAPFUNC(SEL_IO_READ, InterProc::ID_SOCKET_ACCEPT, InterProc::onSocketRead),
  FXMAPFUNC(SEL_IO_READ, InterProc::ID_SOCKET_READ, InterProc::onSocketRead),
};

FXIMPLEMENT(InterProc,FXObject,InterProcMap,ARRAYNUMBER(InterProcMap));


#ifdef FOX_1_6
# define AddInput(file,mode,tgt,sel,ptr) addInput(file,mode,tgt,sel)
#else
# define AddInput(file,mode,tgt,sel,ptr) addInput(tgt,sel,file,mode,ptr)
#endif



static int SocketFailure(const char*func)
{
  fxwarning(_("Error calling %s() function: %s.\n"), func, strerror(errno));
  exit(EXIT_FAILURE);
  return -2;
}



long InterProc::onSocketRead(FXObject*o,FXSelector sel,void*p)
{
  switch (FXSELID(sel)) {
    case ID_SOCKET_ACCEPT: {
      struct sockaddr_un sa;
      socklen_t size=sizeof(sa);
      int read_fd=accept(listen_fd,(struct sockaddr*)&sa,&size);
      if (read_fd==-1) {
        SocketFailure("accept");
        return 1;
      }
      connlist->insert((void*)((FXival)read_fd),new FXString());
      app->AddInput(read_fd,INPUT_READ,this,ID_SOCKET_READ,(void*)((FXival)read_fd));
      break;
    }
    case ID_SOCKET_READ: {
      static const ssize_t bufsize=1024;
      ssize_t len=0;
      char buf[bufsize];
      int read_fd=(FXival)p;
      FXString*s=(FXString*)connlist->find(p);
      do {
        len=read(read_fd,buf,bufsize);
        if (len>0) { s->append(buf, len); }
#ifdef __minix
        if (s->contains("\n\n")) {
          len=0;
          break;
        }
#endif
      } while (len>0);
      if (len==0) {
        app->removeInput(read_fd,INPUT_READ);
        connlist->remove(p);
        close(read_fd);
        ExecuteClientRequest(s);
        delete s;
      }
      break;
    }
  }
  return 1;
}



static int CreateSocket(const char *filename, bool listening)
{
  struct sockaddr_un sa;
  int sock;
  size_t size;
  sock = socket(PF_LOCAL, SOCK_STREAM, 0);
  if (sock < 0) { return SocketFailure("socket"); }
  sa.sun_family = AF_UNIX;
  strncpy(sa.sun_path, filename, sizeof(sa.sun_path));
  sa.sun_path[sizeof(sa.sun_path) - 1] = '\0';
  size = SUN_LEN(&sa);
  if (listening) {
    struct stat st;
    if (stat(filename,&st)==0) {
      if (unlink(filename)!=0) {
        return SocketFailure("unlink");
      }
    } else {
      if (errno!=ENOENT) {
        return SocketFailure("stat");
      }
    }
    if ( bind(sock, (struct sockaddr*)&sa, size) < 0 ) { return SocketFailure("bind"); }
    if ( listen(sock,32) < 0 ) { return SocketFailure("listen"); }
  } else {
    if ( connect(sock, (struct sockaddr*)&sa, size) < 0 ) {
      if ( errno==ECONNREFUSED || errno==EACCES ) {
        close(sock);
        return -1;
      } else {
        return SocketFailure("connect");
      }
    }
  }
  return sock;
}



bool InterProc::ClientSend(FXTopWindow *client, const FXString &data)
{
  FXStat st;
  int write_fd;
  bool found_server=false;
  if ( FXStat::statFile(sock_name, st) && st.isSocket() ) {
    write_fd=CreateSocket(sock_name.text(), false);
    if (write_fd>=0) {
      const char*p=data.text();
      FXint rem=data.length();
      while (rem>0) {
        ssize_t wrote=write(write_fd,(const void*)p,rem);
        if (wrote>=0) {
          rem-=wrote;
          p+=wrote;
        } else {
          break;
        }
      }
      close(write_fd);
      found_server=true;
    }
  }
  return found_server;
}



void InterProc::StartServer(FXTopWindow *win, FXObject*trg, FXSelector sel)
{
  target=trg;
  message=sel;
  FXASSERT(connlist==NULL);
  connlist=new FXHash();
  listen_fd=CreateSocket(sock_name.text(), true);
  app->AddInput(listen_fd,INPUT_READ,this,ID_SOCKET_ACCEPT,(void*)((FXival)listen_fd));
}


#if (FOX_MINOR>6) 
# if (FOX_MAJOR>1) || (FOX_MINOR>7) || (FOX_LEVEL>42)
#  define FOX_1_7_43_OR_NEWER
# endif
#endif


#ifdef FOX_1_7_43_OR_NEWER
# define TotalSlotsInHash(h) (h)->no()
# define UsedSlotsInHash(h) (h)->used()
#else
# define TotalSlotsInHash(h) (h)->size()
# define UsedSlotsInHash(h) (h)->no()
#endif


void InterProc::StopServer()
{
  app->removeInput(listen_fd,INPUT_READ);
  close(listen_fd);
  FXFile::remove(sock_name);
  if ((connlist!=NULL) && (UsedSlotsInHash(connlist)>0)) {
    for (FXint i=0; i<TotalSlotsInHash(connlist); i++) {
      if  (!connlist->empty(i)) {
        FXString*s=(FXString*)connlist->value(i);
        FXival fd=(FXival)connlist->key(i);
        if (fd) {
          app->removeInput(fd,INPUT_READ);
          close(fd);
        }
        if (s) { delete s; }
      }
    }
    delete connlist;
    connlist=NULL;
  }
}


#endif


long InterProc::ExecuteClientRequest(FXString*s)
{
  long rv=0;
  if (target && message) {
    rv=target->handle(this, FXSEL(SEL_COMMAND,message), (void*)s);
  }
  s->clear();
  return rv;
}



InterProc::InterProc(FXApp*a, const FXString &connection, const FXString &topic)
{
  app=a;
#ifdef WIN32
  sock_name="\\\\.\\Pipe\\"+connection;
  listen_fd=INVALID_HANDLE_VALUE;
#else
  sock_name=connection;
  connlist=NULL;
#endif
}

