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

#include <fx.h>

#ifdef WIN32
# include <windows.h>
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


#ifdef WIN32 // For MS-Windows, we use the old-fashioned DDE for client/server communications...

FXIMPLEMENT(InterProc,FXObject,NULL,0);


static FXHash Conversations;
static FXHash Servers;
static FXHash Clients;

void InterProc::MakeAtoms()
{
  FXString strTopic;
  FXString strApplication;
  if (DdeTopic.empty()) {
    strTopic=DdePrefix+"_Topic";
    strApplication=DdePrefix+"_Application";
  } else {
    strTopic=DdeTopic;
    strApplication=DdePrefix;
  }
  atomApplication=GlobalAddAtom((LPSTR) strApplication.text());
  atomTopic=GlobalAddAtom((LPSTR) strTopic.text());
  atoms=MAKELONG(atomApplication, atomTopic);
  Conversations.insert((void*)atoms, this);
}



void InterProc::KillAtoms()
{
  if (atomApplication) { GlobalDeleteAtom(atomApplication); }
  if (atomTopic) { GlobalDeleteAtom(atomTopic); }
  Conversations.remove((void*)atoms);
}


/*
The InterProc object should never be a client and server at the same time, else we would be
communicating with ourself, which is pointless. But problems arise when the same application
has one object acting as a client and another as a server, how can dispatchEvent() know which
object it is dealing with? The solution is to make dispatchEvent() into a static method, and
use global lookup tables to dispatch the event to the correct object. Since each object should
have a unique conversation atom within the same application, we can hash it to the object and
use that as a starting point for keeping things straight...

When iMsg is WM_DDE_INITIATE:
  We can find the InterProc object in the Conversations table.
  The wParam contains the remote client id, we must add it to the Clients table.
When iMsg is WM_DDE_EXECUTE:
  The wParam contains the remote client id, we can find the InterProc object via the
  Clients table. After we find it, we must remove client id from the table.
When iMsg is WM_DDE_ACK:
  If we find the InterProc object in the Conversations table:
    The wParam contains the remote server id, we must add it to the Servers table.
  Else If we don't find the InterProc object in the Conversations table:
    The wParam contains the remote server id, we can find the InterProc object via the
    Servers table. After we find it, we must remove server id from the table.

*/


void InterProc::dispatchEvent(FXID hwnd,unsigned int iMsg,unsigned int wParam,long lParam)
{
  InterProc*ipc=NULL;

  switch (iMsg) {
    case WM_DDE_INITIATE: {
      ipc=(InterProc*)(Conversations.find((void*)((FXival)lParam)));
      break;
    }
    case WM_DDE_EXECUTE: {
      ipc=(InterProc*)(Clients.remove((void*)((FXival)wParam)));
      break;
    }
    case WM_DDE_ACK: {
      ipc=(InterProc*)(Conversations.find((void*)((FXival)lParam)));
      if (!ipc) {
        ipc=(InterProc*)(Servers.remove((void*)((FXival)wParam)));
      }
      break;
    }
  }
  if (ipc) { ipc->DoDispatch(hwnd,iMsg,wParam,lParam); }
}



#define AtomsMatch(lParam) ((LOWORD(lParam)==atomApplication)&&(HIWORD(lParam)==atomTopic))

void InterProc::DoDispatch(FXID hwnd,unsigned int iMsg,unsigned int wParam,long lParam)
{
  switch (iMsg) {
    case  WM_DDE_INITIATE: {
      if (LocalServerID && (wParam!=RemoteClientID) && AtomsMatch(lParam)) {
        // We are the server, one of our clients wants to connect...
        RemoteClientID=wParam;
        Clients.insert((void*)RemoteClientID, (void*)this);
        SendMessage((HWND)RemoteClientID, WM_DDE_ACK, LocalServerID, atoms);
      }
      break;
    }
    case WM_DDE_EXECUTE:{
      if (LocalServerID&&RemoteClientID&&(wParam==RemoteClientID)) {
        // We are the server, and the client sent us something to do...
        UINT uiLo;
        char* puiHi;
        if (UnpackDDElParam(WM_DDE_EXECUTE,lParam,(PUINT)&uiLo,(PUINT)&puiHi)) {
          FXString ReceivedFromClient=(char*)GlobalLock((void*)puiHi);
          GlobalUnlock((void*)puiHi);
          PostMessage((HWND)RemoteClientID, WM_DDE_ACK, LocalServerID, lParam);
          RemoteClientID=0;
          ExecuteClientRequest(&ReceivedFromClient);
        }
      }
      break;
    }
    case WM_DDE_ACK: {
      if (LocalClientID && (((WPARAM)hwnd)==LocalClientID) && !LocalServerID) {
        if (AtomsMatch(lParam)) {
          // We are the client, and the server acknowledged out initial request...
          RemoteServerID=wParam;
          hCommand = GlobalAlloc(GMEM_MOVEABLE, commands->length()+1);
          if (!hCommand) { break; }
          char* lpCommand = (char*)GlobalLock(hCommand);
          if (!lpCommand) {
            GlobalFree(hCommand);
            hCommand=NULL;
            break;
          }
          strncpy(lpCommand,commands->text(),commands->length());
          lpCommand[commands->length()]='\0';
          GlobalUnlock(hCommand);
          LPARAM ddeParam=PackDDElParam(WM_DDE_EXECUTE,0,(UINT)hCommand);
          Servers.insert((void*)RemoteServerID, (void*)this);
          if (PostMessage((HWND)RemoteServerID,WM_DDE_EXECUTE,(WPARAM)hwnd,ddeParam)) {
            found_server=true;
          } else {
            GlobalFree(hCommand);
            FreeDDElParam(WM_DDE_EXECUTE, (LPARAM)hCommand);
            hCommand=NULL;
            Servers.remove((void*)RemoteServerID);
          }
        } else if (hCommand && RemoteServerID && (wParam==RemoteServerID)) {
          // We are the client, and the server has completed processing our commands...
          GlobalFree(hCommand);
          FreeDDElParam(WM_DDE_EXECUTE, (LPARAM)hCommand);
          hCommand=NULL;
        }
      }
      break;
    }
  }
}



class ClientApp: public FXApp {
  virtual long dispatchEvent(FXID hwnd,unsigned int iMsg,unsigned int wParam,long lParam){
    ipc->dispatchEvent(hwnd,iMsg,wParam,lParam);
    return FXApp::dispatchEvent(hwnd,iMsg,wParam,lParam);
  }
  InterProc*ipc;
public:
  ClientApp(InterProc*p):FXApp() { ipc=p; }
};



bool InterProc::ClientSend(FXTopWindow *client, const FXString &data)
{
  if (LocalServerID) {
    fxerror(_("%s: object cannot act as both client and server"), getClassName());
  }
  found_server=false;
  hCommand=NULL;
  RemoteServerID=0;
  FXApp*a=app?app:new ClientApp(this);
  if (!a->isInitialized()) {
    static char argv0[MAX_PATH+1]="\0";
    int argc=1;
    if (!argv0[0]) { GetModuleFileName(NULL,argv0,MAX_PATH); }
    char *argv[]={argv0,NULL};
    a->init(argc,argv,true);
  }
  FXTopWindow *w=client?client:new FXMainWindow(a, DdePrefix+"_Client");
  if (!w->id()) {
    a->create();
    w->create();
  }
  LocalClientID=(WPARAM)w->id();
  commands=&data;
  MakeAtoms();
  SendMessage((HWND)HWND_BROADCAST,WM_DDE_INITIATE,(WPARAM)w->id(),atoms);
  for (FXint i=0; i<5; i++) { // wait for first DDE_ACK
    a->runWhileEvents();
    if (found_server) { // got first DDE_ACK
       for (FXint j=0; j<5; j++) { // wait for second DDE_ACK
         if (hCommand==NULL) { // got second DDE_ACK
           break;
         }
         fxsleep(100000);
         a->runWhileEvents();
       }
      break;
    }
    fxsleep(100000);
  }
  LocalClientID=0;
  if (hCommand) {
    GlobalFree(hCommand);
    FreeDDElParam(WM_DDE_EXECUTE, (LPARAM)hCommand);
    hCommand=NULL;
  }
  KillAtoms();
  if (w!=client) {
    w->destroy();
    delete w;
  }
  if (a!=app) {
    a->destroy();
    delete a;
  }
  if (RemoteServerID) {
    Servers.remove((void*)RemoteServerID);
    RemoteServerID=0;
  }
  return found_server;
}



void InterProc::StartServer(FXTopWindow *win, FXObject*trg, FXSelector sel)
{
  if (win) {
    if (!win->id()) { win->create(); }
    LocalServerID=(WPARAM)(win->id());
  } else LocalServerID=0;
  if (!LocalServerID) {
    fxerror(_("%s: invalid window parameter.\n"), getClassName());
  }
  target=trg;
  message=sel;
  MakeAtoms();
}



void InterProc::StopServer()
{
  LocalServerID=0;
  target=NULL;
  message=0;
  KillAtoms();
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



void InterProc::StopServer()
{
  app->removeInput(listen_fd,INPUT_READ);
  close(listen_fd);
  FXFile::remove(sock_name);
  if (connlist) {
    while (connlist->no()) {
      FXString*s=(FXString*)connlist->value(0);
      FXival fd=(FXival)connlist->key(0);
      if (fd) {
        app->removeInput(fd,INPUT_READ);
        close(fd);
      }
      if (s) { delete s; }
      connlist->remove(connlist->key(0));
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
  LocalClientID=0;
  RemoteClientID=0;
  DdePrefix=connection;
  DdeTopic=topic;
  LocalServerID=0;
#else
  sock_name=connection;
  connlist=NULL;
#endif
}

