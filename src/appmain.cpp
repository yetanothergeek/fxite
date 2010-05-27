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


#include <csignal>
#ifdef WIN32
# include <windows.h>
#else
# include <sys/socket.h>
# include <sys/un.h>
# include <X11/Xlib.h>
#endif

#include <unistd.h>
#include <fcntl.h>
#include <cerrno>

#include <fx.h>

#include "compat.h"
#include "appwin.h"

#include "intl.h"
#include "appmain.h"



FXDEFMAP(AppClass) AppMap[]={
  FXMAPFUNC(SEL_SIGNAL,  AppClass::ID_CLOSEALL,    AppClass::onCmdCloseAll),
  FXMAPFUNC(SEL_COMMAND, AppClass::ID_CLOSEALL,    AppClass::onCmdCloseAll),
#ifndef WIN32
  FXMAPFUNC(SEL_IO_READ, AppClass::ID_SOCKET_READ, AppClass::onSocketRead)
#endif
};


FXIMPLEMENT(FXiTe,FXApp,AppMap,ARRAYNUMBER(AppMap));


void AppClass::ExecuteClientRequest() {
  bool quiet=false;
  if (compare(cli_commands,"-q\n",2)==0) {
    quiet=true;
    cli_commands=cli_commands.erase(0,3);
  }
  if (cli_commands.contains("\n-q\n")) {
    quiet=true;
    cli_commands=cli_commands.substitute("\n-q\n","\n",true);
  }
  if (!quiet) {
    mainwin->hide();
    mainwin->show();
    runWhileEvents();
  }
  mainwin->ParseCommands(cli_commands);
  cli_commands="";
}



#ifdef WIN32 // For MS-Windows, we use the old-fashioned DDE for client/server communications...


# define MakeAtoms() \
    atomApplication=GlobalAddAtom((LPSTR) sock_name.text()); \
    atomTopic=GlobalAddAtom((LPSTR) szTopic); \
    atoms=MAKELONG(atomApplication,atomTopic);

#define KillAtoms() \
    if (atomApplication) { GlobalDeleteAtom(atomApplication); } \
    if (atomTopic) { GlobalDeleteAtom(atomTopic); }


static const char *szTopic=EXE_NAME"_TopicName";
static ATOM atomApplication=0;
static ATOM atomTopic=0;
static LPARAM atoms=0;
static WPARAM client_id=0;
static WPARAM server_id=0;
static WPARAM tmpwin_id=0;
static HGLOBAL hCommand=NULL;
static bool found_server=false;


long AppClass::dispatchEvent(FXID hwnd,unsigned int iMsg,unsigned int wParam,long lParam)
{
  switch (iMsg) {
    case  WM_DDE_INITIATE: {
      if (mainwin && (wParam!=client_id)&&(LOWORD(lParam)==atomApplication)&&(HIWORD(lParam)==atomTopic)) {
         // We are the server, one of our clients wants to connect...
        client_id=wParam;
        SendMessage((HWND)client_id, WM_DDE_ACK, (WPARAM)mainwin->id(), atoms);
      }
      break;
    }
    case WM_DDE_ACK: {
      if (!mainwin) {
        if ((LOWORD(lParam)==atomApplication)&&(HIWORD(lParam)==atomTopic)) {
          // We are the client, and the server acknowledged out initial request...
          server_id=wParam;
          char* lpCommand;
          hCommand = GlobalAlloc(GMEM_MOVEABLE, dde_string.length()+1);
          if (!hCommand) {break;}
          lpCommand = (char*)GlobalLock(hCommand);
          if (!lpCommand) {
            GlobalFree(hCommand);
            break;
          }
          strncpy(lpCommand,dde_string.text(),dde_string.length());
          lpCommand[dde_string.length()]='\0';
          GlobalUnlock(hCommand);
          if (PostMessage((HWND)server_id,WM_DDE_EXECUTE,tmpwin_id,PackDDElParam(WM_DDE_EXECUTE,0,(UINT)hCommand))) {
            found_server=true;
          } else {
            GlobalFree(hCommand);
            FreeDDElParam(WM_DDE_EXECUTE, (LPARAM)hCommand);
          }
        } else if (hCommand && (lParam==(LPARAM)hCommand)) {
          // We are the client, and the server has completed processing our commands...
          GlobalFree(hCommand);
          FreeDDElParam(WM_DDE_EXECUTE, (LPARAM)hCommand);
        }
      }
      break;
    }
    case WM_DDE_EXECUTE:{
      if (mainwin&&client_id&&(wParam==client_id)) {
        // We are the server, and the client sent us something to do...
        UINT uiLo;
        char* puiHi;
        if (UnpackDDElParam(WM_DDE_EXECUTE,lParam,(PUINT)&uiLo,(PUINT)&puiHi)) {
          cli_commands=(char*)GlobalLock((void*)puiHi);
          ExecuteClientRequest();
          SendMessage((HWND)wParam, WM_DDE_ACK, (WPARAM)mainwin->id(), lParam);
        }
      }
      break;
    }
  }
  // Process all events through the default handler...
  return FXApp::dispatchEvent(hwnd,iMsg,wParam,lParam);
}



bool AppClass::InitClient()
{
  dde_string="";
  ClientParse();
  sock_name=FXPath::name(sock_name);
  sock_name.prepend(APP_NAME);
  FXMainWindow*tmpwin=new FXMainWindow(this,EXE_NAME"_tmpwin");
  tmpwin->getParent()->create();
  tmpwin->create();
  tmpwin_id=(WPARAM)tmpwin->id();
  MakeAtoms();
  SendMessage((HWND)HWND_BROADCAST,WM_DDE_INITIATE,(WPARAM)tmpwin->id(),atoms);
  for (FXint i=0; i<5; i++) {
    runWhileEvents();
    if (found_server) break;
    fxsleep(100000);
  }
  KillAtoms();
  tmpwin->destroy();
  delete tmpwin;
  return found_server;
}

#else // For X11, we use a UNIX socket for client/server communications...

long AppClass::onSocketRead(FXObject*o,FXSelector sel,void*p)
{
  static const ssize_t bufsize=1024;
  ssize_t len;
  char buf[bufsize];
  bool eod=false;
  while (1) {
    memset(buf,0,sizeof(buf));
    len=read(sock_fd,buf,sizeof(buf)-1);
    if (len>0) {
      cli_commands.append(buf);
    }
    eod=strstr(buf,"\n\n")?true:false;
    if ( eod || (len<(bufsize-1)) ) { break; }
  }
  if (eod) { ExecuteClientRequest(); }
  return 1;
}



static int create_socket(const char *filename, bool listening)
{
  struct sockaddr_un sa;
  int sock;
  size_t size;

  sock = socket(PF_LOCAL, SOCK_DGRAM, 0);
  if (sock < 0) {
    fxwarning(_("Error calling %s function: %s\n"), "socket", strerror(errno));
    exit(EXIT_FAILURE);
  }

  sa.sun_family = AF_LOCAL;
  strncpy(sa.sun_path, filename, sizeof(sa.sun_path));
  sa.sun_path[sizeof(sa.sun_path) - 1] = '\0';

  size = SUN_LEN(&sa);
  if (listening) {
    if ( bind(sock, (struct sockaddr*)&sa, size) < 0 ) {
      fxwarning(_("Error calling %s function: %s\n"), "bind", strerror(errno));
      exit(EXIT_FAILURE);
    }
  } else {
    if ( connect(sock, (struct sockaddr*)&sa, size) < 0 ) {
      if ( errno==ECONNREFUSED ) {
        close(sock);
        return -1; // Stale socket, returning -1 tells us to delete it (see below).
      } else {
        fxwarning(_("Error calling %s function: %s\n"), "connect", strerror(errno));
        exit(EXIT_FAILURE);
      }
    }
  }
  return sock;
}



bool AppClass::InitClient()
{
  FXStat st;
  if ( FXStat::statFile(sock_name, st) && st.isSocket() ) {
    sock_fd=create_socket(sock_name.text(), false);
    if (sock_fd<0) {
      // Probably a stale socket from a previous crash -
      // So remove it, and try server mode instead.
      FXFile::remove(sock_name);
      return false;
    }
    ClientParse();
    close(sock_fd);
    return true;
  } else {
    return false;
  }
}


#endif



AppClass::AppClass(const FXString& name, const FXString& title):FXApp(name,title)
{
  mainwin=NULL;
  quitting=false;
//  addSignal(SIGINT,  this, App::ID_CLOSEALL);
#ifndef WIN32
  addSignal(SIGQUIT, this, AppClass::ID_CLOSEALL);
  addSignal(SIGHUP,  this, AppClass::ID_CLOSEALL);
  addSignal(SIGPIPE, this, AppClass::ID_CLOSEALL);
  addSignal(SIGTERM, this, AppClass::ID_CLOSEALL);
#endif
}



long AppClass::onCmdCloseAll(FXObject*o,FXSelector sel,void*p)
{
  if (quitting) { return 1; }
  quitting=true;
  mainwin->close();
  quitting=false;
  return 1;
}



#ifdef WIN32
# define AppendToServer(s,n) dde_string.append(s)
#else
# define AppendToServer(s,n) write(sock_fd,s,n);
#endif
void AppClass::ClientParse()
{
  bool skip_next=false;
  bool is_macro=false;
  for (FXint i=1; i<getArgc(); i++) {
    if (skip_next) {
      skip_next=false;
      continue;
    }
    const char *arg=getArgv()[i];
    FXuint arglen=arg?strlen(arg):0;
    if (arglen)  {
      if ( (arg[0]=='-') && (arg[1]=='e') ) { is_macro=true; }
      if ( (!strchr("-+",arg[0])) && FXStat::exists(arg) && FXStat::isFile(arg) ) {
        FXString filename=FXPath::absolute(arg);
        AppendToServer(filename.text(),filename.length());
      } else {
        if ( (arg[0]=='-') && (strchr("cs", arg[1])) ) {
          if ( arglen == 2 ) {
            skip_next=true;
            continue;
          }
        } else {
          if ( (strchr("-+",arg[0])) ) {
            AppendToServer(arg,arglen);
          } else {
            if (is_macro) {
              AppendToServer(arg,arglen);
              is_macro=false;
            } else {
              FXString filename=FXPath::absolute(arg);
              AppendToServer(filename.text(),filename.length());
            }
          }
        }
      }
      AppendToServer("\n", 1);
    }
  }
  AppendToServer("\n\n", 2);
}




bool AppClass::InitServer()
{
  is_server=true;
  bool skip_next=false;
  int i;
  for (i=1; i<getArgc(); i++) {
    if (skip_next) {
      skip_next=false;
      continue;
    }
    const char *arg=getArgv()[i];
    FXuint arglen=arg?strlen(arg):0;
    if (arglen)  {
      if ((arg[0]!='-') && (arg[0]!='+') && FXStat::exists(arg) && FXStat::isFile(arg)) {
        FXString filename=FXPath::absolute(arg);
        srv_commands.append(filename.text());
      } else {
        if ( (arg[0]=='-') && ((arg[1]=='s')||(arg[1]=='c')) ) {
          if ( arglen == 2 ) { skip_next=true; }
        } else { srv_commands.append(arg); }
      }
      srv_commands.append('\n');
    }
  }
  while (srv_commands.contains("\n\n")) { srv_commands.substitute("\n\n", "\n", true); }
  srv_commands.append("\n\n");

#ifdef WIN32
  MakeAtoms();
#else
  sock_fd=create_socket(sock_name.text(), true);
  AddInput(sock_fd,INPUT_READ,this,ID_SOCKET_READ);
#endif
  return true;
}

static const char* helptext[]= {
  "",
  _("Options:"),
  "",
  _("  -s <name>        Create or control a named instance <name>."),
  _("  -c <name>        Use the alternate configuration <name>."),
  "",
  _("  -r               Open the next files read-only."),
  _("  -w               Open the next files read-write. (default)"),
  "",
  _("  +27              Open the next file to line 27."),
  _("  +35,8            Open the next file to line 35, column 8."),
  _("  readme.txt:163   Open \"readme.txt\" to line 163."),
  "",
  _("  -p               Restore previous session."),
  _("  -e <command>     Execute Lua macro string <command>."),
  _("  -t <file>        Load tags from <file>."),
  "",
  _("  -q               Quiet, do not raise existing instance."),
#ifndef WIN32
  _("  -d <name>        Connect to X display <name>."),
#endif
  _("  -v               Show version information and exit."),
  "",
  NULL
};


static void usage(const char*prog)
{
  fxmessage("\n");
  fxmessage(_("Usage: %s [options] [files] ...\n"), FXPath::name(prog).text());
  for (const char**s=helptext; *s; s++) {
    fxmessage("%s\n", *s);
  }
}



#ifdef WIN32
#define _WIN32_IE 0x0400
#include <shlobj.h>
#include <sys/stat.h>
bool IsDir(const FXString &dn)
{
  struct stat st;
  return ( (stat(dn.text(),&st)==0) && S_ISDIR(st.st_mode) );
}

bool IsWin9x()
{
  OSVERSIONINFO OSversion;
  OSversion.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
  ::GetVersionEx(&OSversion);
  return (OSversion.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS);
}
#else
static char display_opt[]="-display";
#endif



void AppClass::init(int& argc, char** argv, bool connect)
{
  FXApp::init(argc,argv,connect);
  is_server=false;
  configdir=FXSystem::getHomeDirectory();
  FXDir::create(configdir,FXIO::OwnerFull);
  configdir.append(PATHSEP);
#ifdef WIN32
  const char *path_parts[]={ "foxrc", "", "servers",  NULL };
  configdir.substitute('/',PATHSEP,true);
#else
  const char *path_parts[]={ ".foxrc", "", "servers",  NULL };
#endif
  FXint i;

  for (i=0; path_parts[i]; i++) {
    FXString parts=path_parts[i][0]?path_parts[i]:getVendorName().text();
    parts.append(PATHSEP);
    FXint nseps=parts.contains(PATHSEP);
    FXint j;
    for (j=0;j<nseps;j++) {
      configdir.append(parts.section(PATHSEP,j));
      if (!(IsDir(configdir)||FXDir::create(configdir,FXIO::OwnerFull))) {
        fxwarning("\n%s: %s:\n    %s\n(%s)\n\n",
           FXPath::name(getArgv()[0]).text(),
           _("FATAL: Failed to create directory path"),
           configdir.text(), strerror(errno)
         );
        flush(stderr);
        ::exit(EXIT_FAILURE);
      }
      configdir.append(PATHSEP);
    }
  }
  for ( i=1; i<getArgc(); i++) {
    const char *arg=getArgv()[i];
    if (argv[i][0]=='-') {
      switch (arg[1]) {
        case 's': {
          if (arg[2]) {
            sock_name=arg+2;
          } else {
             if ((i+1)<getArgc()) {
               sock_name=getArgv()[i+1];
             } else {
               sock_name="";
             }
          }
          if (sock_name.empty()) {
            fxwarning(_("Option -s requires an argument.\n"));
            return;
          }
          break;
        }
        case 'c':
#ifndef WIN32
        case 'd':
#endif
        case 'e':
        case 'p':
        case 'q':
        case 'r':
        case 't':
        case 'w':
        {
          break;
        }
        default: {
          fxwarning(_("Unrecognized option: -%c\n"), argv[i][1]);
          usage(argv[0]);
          return;
        }
      }
    }
  }
  if ( sock_name.empty() ) { sock_name="DEFAULT"; }
  server_name=sock_name.text();
#ifndef WIN32
  char*d=DisplayString(getDisplay());
  if (d) {
    if (*d!=':') { sock_name.append('_'); }
    sock_name.append(d);
    FXSystem::setEnvironment("DISPLAY", d);
  }
#endif
  sock_name.upper();
  server_name.upper();
  for (unsigned char c=1; ; c++) {
    if ( ((c>='A')&&(c<='Z')) || ((c>='0')&&(c<='9')) || (c=='_') ) { continue; }
    sock_name.substitute(c, '_', true);
    server_name.substitute(c, '_', true);
    if (c==255) { break; }
  }
  server_name.lower();
  sock_name.prepend(configdir.text());
  configdir=FXPath::directory(configdir);
  configdir=FXPath::directory(configdir);
  configdir.append(PATHSEP);
  sessionfile=configdir.text();
  sessionfile.append("sessions");
  if (!(IsDir(sessionfile)||FXDir::create(sessionfile,FXIO::OwnerFull))) {
    fxwarning("\n%s: %s:\n    %s\n\n",
       FXPath::name(getArgv()[0]).text(), _("FATAL: Failed to create directory path"), sessionfile.text());
    flush(stderr);
    ::exit(EXIT_FAILURE);
  }
  sessionfile.append(PATHSEP);
  sessionfile.append(FXPath::name(sock_name));
  if ( !InitClient() ) { InitServer(); }
  while (srv_commands[0]=='\n') { srv_commands.erase(0,1); }
}



void AppClass::exit(FXint code)
{
  if (is_server) {
#ifdef WIN32
    KillAtoms();
#else
    FXFile::remove(sock_name);
#endif
  }
  FXApp::exit(code);
}



static bool get_config_name(int argc, char *argv[], FXString &cfg_name)
{
  cfg_name="";
  int i;
  for (i=1; i<argc; i++) {
    const char*arg=argv[i];
    if ( (arg[0]=='-') && (arg[1]=='c') ) {
      if (arg[2]) { arg+=2; } else {
        if ((i+1)>=argc) {
          fxwarning("%s: %s\n", argv[0], _("Error: option -c requires an argument."));
          return false;
        }
        arg=argv[i+1];
      }
      int n=strlen(arg);
      int j;
      if (n<4) {
        fxwarning(
          "\n%s:\n  %s\n\n", argv[0], _("Error: Config name length must be at least 4 characters."));
        return false;
      }
      if (n>32) {
        fxwarning(
          "\n%s:\n %s\n\n", argv[0], _("Error: Config name length must not exceed 32 characters."));
        return false;
      }
      for (j=0; j<n; j++) {
        if (((arg[j]<'a')||(arg[j]>'z'))&&((arg[j]<'0')||(arg[j]>'9'))) {
          fxwarning(
          "\n%s:\n %s\n\n",
          argv[0], _("Error: Config name can have only lowercase [a-z] and numbers [0-9]."));
          return false;
        }
      }
      cfg_name=arg;
      break;
    }
  }
  if (cfg_name.empty()) { cfg_name="default"; }
  cfg_name.prepend(PATHSEP);
  cfg_name.prepend(APP_VENDOR);
  return true;
}



static bool check_info_args(int argc, char *argv[])
{
  for (int i=1; i<argc; i++) {
    if (argv[i][0]=='-') {
      switch (argv[i][1]) {
        case 'v': {
          TopWindow::VersionInfo();
          exit(0);
        }
        case 'h': {
          usage(argv[0]);
          exit(0);
        }
        case '-': {
          if ((strcmp(argv[i],"--help")==0)||(strcmp(argv[i],_("--help"))==0)) {
            usage(argv[0]);
            exit(0);
          }
        }
      }
    }
  }
}



int main(int argc, char *argv[])
{

#ifdef ENABLE_NLS
  bindtextdomain(PACKAGE, LOCALEDIR);
  textdomain(PACKAGE);
#endif
  if ((argc==2)&&(strcmp(argv[1],"--dump-lexers")==0)) {
    TopWindow::DumpLexers();
    exit(0);
  }
  check_info_args(argc,argv); // Checks for switches that exit after they print some info.
  FXString cfg_name="";
  if (!get_config_name(argc,argv,cfg_name)) { exit(1); }
  AppClass app("settings", cfg_name);
#ifdef WIN32
  // FXRegistry ascii mode on windows depends on USERPROFILE env, but Win98 doesn't set it
  FXString userprofile=FXSystem::getEnvironment("USERPROFILE");
  if (userprofile.empty()) {
    TCHAR strPath[MAX_PATH];
    if (SHGetSpecialFolderPath(0,strPath,CSIDL_APPDATA,FALSE)) {
      userprofile=FXPath::directory(strPath);
    }
  }
  userprofile.substitute('/', PATHSEP, true);
  FXSystem::setEnvironment("USERPROFILE", userprofile);
  app.reg().setAsciiMode(true);
#else
  for (int i=1; i<argc; i++) {
    if (argv[i] && (strcmp(argv[i],"-d")==0)) { argv[i]=display_opt; }
  }
#endif
  app.init(argc,argv);
  if ( app.IsServer() ) {

#if defined(WIN32) && !defined(FOX_1_6)
    app.setToolTipTime(2000000000);
    app.setToolTipPause(250000000);
    FXToolTip* tips=new FXToolTip(&app,0);
    tips->create();
#else
    new FXToolTip(&app,0);
#endif

    app.create();
    app.MainWin(new TopWindow(&app));
    app.MainWin()->create();
#ifndef WIN32
    fclose(stdin);
    stdin=fopen(NULL_FILE, "r");
#endif
    return app.run();
  }
}

