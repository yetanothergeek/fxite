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
# include <ctype.h>
#else
# include <sys/socket.h>
# include <sys/un.h>
# include <X11/Xlib.h>
#endif

#include <unistd.h>
#include <fcntl.h>
#include <cerrno>

#include <fx.h>

#include "appname.h"
#include "compat.h"
#include "appwin.h"

#include "interproc.h"

#include "intl.h"
#include "appmain.h"


FXDEFMAP(AppClass) AppMap[]={
  FXMAPFUNC(SEL_SIGNAL,  AppClass::ID_CLOSEALL,    AppClass::onCmdCloseAll),
  FXMAPFUNC(SEL_COMMAND, AppClass::ID_CLOSEALL,    AppClass::onCmdCloseAll),
  FXMAPFUNC(SEL_COMMAND, AppClass::ID_IPC_EXEC,    AppClass::onIpcExec),
};


FXIMPLEMENT(FXiTe,FXApp,AppMap,ARRAYNUMBER(AppMap));



AppClass::AppClass(const FXString& name, const FXString& title):FXApp(name,title)
{
  mainwin=NULL;
  quitting=false;
  commands=FXString::null;
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
#ifndef WIN32
  if (((FXival)p)==SIGPIPE) { return 1; }
#endif
  if (quitting) { return 1; }
  quitting=true;
  mainwin->close();
  quitting=false;
  return 1;
}



void AppClass::ParseCommandLine()
{
  bool skip_next=false;
  bool is_macro=false;
  commands=FXString::null;
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
        commands.append(filename.text());
      } else {
        if ( (arg[0]=='-') && (strchr("cs", arg[1])) ) {
          if ( arglen == 2 ) {
            skip_next=true;
            continue;
          }
        } else {
          if ( (strchr("-+",arg[0])) ) {
            commands.append(arg);
          } else {
            if (is_macro) {
              commands.append(arg);
              is_macro=false;
            } else {
              FXString filename=FXPath::absolute(arg);
              commands.append(filename.text());
            }
          }
        }
      }
      commands.append("\n");
    }
  }
  commands.append("\n\n");
}



long AppClass::onIpcExec(FXObject*o, FXSelector sel, void*p)
{
  mainwin->ParseCommands(*((FXString*)p));
  return 1;
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
long AppClass::dispatchEvent(FXID hwnd,unsigned int iMsg,unsigned int wParam,long lParam)
{
  ipc->dispatchEvent(hwnd,iMsg,wParam,lParam);
  return FXApp::dispatchEvent(hwnd,iMsg,wParam,lParam);
}
#else
static char display_opt[]="-display";
#endif


void AppClass::CreatePathOrDie(const FXString &dirname)
{
  FXString dn="";
  FXint n=dirname.contains(PATHSEP);
#ifdef WIN32
  if ((dirname[1]==':') && isalpha(dirname[0])) {
    dn.append(dirname[0]);
    dn.append(':');
  }
#endif
  dn.append(PATHSEP);
  for (FXint i=1; i<=n; i++) {
    dn.append(dirname.section(PATHSEP,i));
    if (!(IsDir(dn)||FXDir::create(dn,FXIO::OwnerFull))) {
      FXString msg=SystemErrorStr();
      fxwarning("\n%s: %s:\n    %s\n(%s)\n\n",
         getArgv()[0],
         _("FATAL: Failed to create directory path"),
         dn.text(), msg.text()
      );
      create();
      FXMessageBox::error(this, MBOX_OK, _(APP_NAME" error"), "%s:\n\n    %s\n\n(%s)\n\n",
      _("FATAL: Failed to create directory path"),
      dn.text(), msg.text()
      );
      destroy();
      fflush(stderr);
      ::exit(EXIT_FAILURE);
    }
    dn.append(PATHSEP);
  }
}

#ifdef FOX_1_6

// Old FOX-1.6 config directory location...

void AppClass::CreateConfigDir()
{
  configdir=FXSystem::getHomeDirectory()+ PATHSEP+ ".foxrc"+ PATHSEP+ getVendorName()+ PATHSEP;
  CreatePathOrDie(configdir);
}


#else

extern void MigrateConfigDir(FXApp*a, const FXString &src, const FXString &dst, FXString &errors);

# ifdef WIN32

// New Win32 config directory location...
void AppClass::CreateConfigDir()
{
  configdir=reg().getUserDirectory()+PATHSEP+getVendorName();
  configdir.substitute('/',PATHSEP,true);
  FXString oldconfig=FXString::null;
  FXString newconfig=FXPath::directory(configdir);
  const char*config_tail=PATHSEPSTRING "foxrc" PATHSEPSTRING "fxite";
  if (IsWin9x()) {
    if (!IsDir(newconfig)) {
      FXString homedir_cfg=FXSystem::getEnvironment("HOME");
      if (!homedir_cfg.empty()) { 
        homedir_cfg+=config_tail;
        homedir_cfg.substitute('/',PATHSEP,true);
      }
      if (IsDir(homedir_cfg)) {
        oldconfig=homedir_cfg;
      } else {
        FXString mydocs_cfg=GetShellFolder("Personal")+config_tail;
        if (IsDir(mydocs_cfg)) {
          oldconfig=mydocs_cfg;
        }
      }
    }
  } else {
    oldconfig=FXSystem::getEnvironment("USERPROFILE");
    if (!oldconfig.empty()) {
      oldconfig+=config_tail;
    }
  }
  if (!oldconfig.empty()) {
    MigrateConfigDir(this, oldconfig, newconfig, migration_errors);
  }
  configdir.append(PATHSEP);
  CreatePathOrDie(configdir);
}

# else

// New FOX-1.7 XDG config directory location...
void AppClass::CreateConfigDir()
{
  migration_errors="";
  FXString old_config=FXSystem::getHomeDirectory()+ PATHSEP+ ".foxrc"+ PATHSEP+ getVendorName();
  FXString xdg_config="";
  if (use_xdg_config()) {
    xdg_config=getenv("XDG_CONFIG_HOME");
    if (xdg_config.empty()) {
      xdg_config=FXSystem::getHomeDirectory()+ PATHSEP+ ".config";
    } else {
      xdg_config=FXPath::simplify(FXPath::absolute(xdg_config));
    }
    CreatePathOrDie(xdg_config);
    xdg_config += PATHSEP + getVendorName();
    MigrateConfigDir(this,
      FXPath::directory(old_config), FXPath::directory(xdg_config), migration_errors);
    configdir=xdg_config.text();
  } else {
    configdir=old_config.text();
  }
  configdir=FXPath::simplify(FXPath::absolute(configdir));
  configdir.append( PATHSEP );
  CreatePathOrDie(configdir);
}

# endif // OS

#endif // FOX Version



void AppClass::init(int& argc, char** argv, bool connect)
{
#ifdef WIN32
  FXString AppDataDir;
  GetAppDataDir(AppDataDir);
  reg().setUserDirectory(AppDataDir);
  reg().setAsciiMode(true);
  FXApp::init(argc,argv,connect);
  reg().setUserDirectory(AppDataDir);
  reg().read();
#else
  for (int i=1; i<argc; i++) {
    if (argv[i] && (strcmp(argv[i],"-d")==0)) { argv[i]=display_opt; }
  }
  FXApp::init(argc,argv,connect);
#endif
  CreateConfigDir();
  for (FXint i=1; i<getArgc(); i++) {
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
  sessionfile=configdir+"sessions"+PATHSEP;
  CreatePathOrDie(sessionfile);
  sessionfile.append(sock_name);
  settingsfile=configdir+"settings";
#ifdef WIN32
  sock_name.prepend(APP_NAME"_");
  settingsfile.append(".ini");
#else
  FXString serverdir=configdir+"servers"+PATHSEP;
  CreatePathOrDie(serverdir);
  sock_name.prepend(serverdir);
  if (use_xdg_config()) { settingsfile.append(".rc"); }
#endif
  ParseCommandLine();
  ipc=new InterProc(this, sock_name);
  if (ipc->ClientSend(NULL, commands)) {
    delete ipc;
    destroy();
    ::exit(0);
  } else {
#if defined(WIN32) && !defined(FOX_1_6)
    setToolTipTime(2000000000);
    setToolTipPause(250000000);
    FXToolTip* tips=new FXToolTip(this,0);
    tips->create();
#else
    new FXToolTip(this,0);
#endif
    mainwin=new TopWindow(this);
    create();
    mainwin->create();
    ipc->StartServer(mainwin, this,ID_IPC_EXEC);
#ifndef WIN32
    fclose(stdin);
    stdin=fopen(NULL_FILE, "r");
#endif
  }
}



extern "C" { int ini_sort(const char *filename); }


void AppClass::exit(FXint code)
{
  ipc->StopServer();
  delete ipc;
  FXApp::exit(code);
  ini_sort(settingsfile.text());
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



static void check_info_args(int argc, char *argv[])
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
  app.init(argc,argv);
  return app.run();
}

