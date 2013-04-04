#include <fx.h>
#include "compat.h"


#ifdef WIN32
#include <windows.h>
#define _WIN32_IE 0x0400
#include <shlobj.h>
#include <sys/stat.h>


bool IsDir(const FXString &dn)
{
  if (dn.empty()) { return false; }
  struct stat st;
  if (strchr("/\\", dn[dn.length()-1])) {
    FXString tmp=dn.text();
    tmp.trunc(dn.length()-1);
    return ( (stat(tmp.text(),&st)==0) && S_ISDIR(st.st_mode) );
  } else {
    return ( (stat(dn.text(),&st)==0) && S_ISDIR(st.st_mode) );
  }
}



bool IsWin9x()
{
  OSVERSIONINFO OSversion;
  OSversion.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
  ::GetVersionEx(&OSversion);
  return (OSversion.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS);
}



const char* SystemErrorStr()
{
  DWORD code=GetLastError();
  static TCHAR lpMsgBuf[512];
  lpMsgBuf[0]=0;
  FormatMessage(
      FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL, code, MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT), lpMsgBuf, sizeof(lpMsgBuf), NULL);
  lpMsgBuf[sizeof(lpMsgBuf)-1]=0;
  char *p;
  for (p=(char*)lpMsgBuf; *p; p++) { if (*p=='\r') { *p=' '; } }
  p=strrchr((char*)lpMsgBuf, '\n');
  if ( p && (*(p+1)=='\0')) { *p='\0'; }
  return (const char*)lpMsgBuf;
}



FXString GetRegKey(HKEY hive, LPCSTR path, LPCSTR value) {
  HKEY hKey;
  LONG err;
  LPBYTE data=NULL;
  DWORD size;
  if (RegOpenKeyExA(hive,path,0,KEY_READ,&hKey)==ERROR_SUCCESS) {
    err=RegQueryValueExA(hKey,value,NULL,NULL,NULL,&size);
    if(err==ERROR_SUCCESS) {
      data=(LPBYTE)calloc(size,1);
      err=RegQueryValueExA(hKey,value,NULL,NULL,data,&size);
    }
    RegCloseKey(hKey);
    if (err==ERROR_SUCCESS) {
      FXString rv=(FXchar*)data;
      free(data);
      return rv; // FXString((FXchar*)data);
    }
  }
  return FXString::null;
}



void GetAppDataDir(FXString &AppDataDir)
{
  AppDataDir=FXString::null;
  if (IsWin9x()) {
    FXString CurrentUser=FXString::null;
    DWORD size=0;
    TCHAR* lpBuffer;
    GetUserNameA(NULL,&size);
    lpBuffer=(TCHAR*)calloc(size+1,sizeof(TCHAR));
    if ( GetUserNameA(lpBuffer,&size) ) {
      CurrentUser=(FXchar*)lpBuffer;
    }
    free(lpBuffer);
    if (!CurrentUser.empty()) {
      FXString ProfileList="Software\\Microsoft\\Windows\\CurrentVersion\\ProfileList\\"+CurrentUser;
      FXString ProfileImagePath=GetRegKey(HKEY_LOCAL_MACHINE,ProfileList.text(), "ProfileImagePath");
      if (!ProfileImagePath.empty()) {
        if (FXStat::isDirectory(ProfileImagePath)) {
          AppDataDir=ProfileImagePath+PATHSEP+"Application Data";
        }
      }
    }
  } else {
    FXString AppData=GetShellFolder("AppData");
    if (!AppData.empty()) {
      AppDataDir=AppData.text();
    }
  }
  if (AppDataDir.empty()) {
    TCHAR strPath[MAX_PATH];
    if (SHGetSpecialFolderPath(0,strPath,CSIDL_APPDATA,FALSE)) {
      AppDataDir=(FXchar*)strPath;
    } else {
      AppDataDir="C:\\";
    }
  }
  AppDataDir.substitute('/', PATHSEP, true);
  AppDataDir=FXPath::simplify(FXPath::absolute(AppDataDir));
}


FXString GetShellFolder(const char*shellfolder)
{
  return GetRegKey( HKEY_CURRENT_USER,
             "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",
              shellfolder);
}

#endif






#ifndef WIN32
#define NET_WM_ICON
#endif

#ifdef NET_WM_ICON
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include <X11/Xatom.h>


#ifdef FOX_1_6
#include <unistd.h>
static FXString GetHostName()
{
  FXchar name[1024];
  if(gethostname(name,sizeof(name))==0){
    return FXString(name);
  }
  return "localhost";
}
#else
#define GetHostName() FXSystem::getHostName()
#endif

void SetupXAtoms(FXTopWindow*win, const char*class_name, const char* APP_NAME)
{
  FXIcon*ico=win->getIcon();
  Display*d=(Display*)(win->getApp()->getDisplay());
  Atom net_wm_icon = XInternAtom(d, "_NET_WM_ICON", 0);
  Atom cardinal = XInternAtom(d, "CARDINAL", False);
  if (net_wm_icon&&cardinal) {
    FXint w=ico->getWidth();
    FXint h=ico->getHeight();
    unsigned long *icon_buf=NULL;
    FXint icon_buf_size=(w*h)+2;
    icon_buf=(unsigned long*)malloc(icon_buf_size*sizeof(unsigned long));
    FXint j=0;
    icon_buf[j++]=w;
    icon_buf[j++]=h;
    FXint x,y;
    for (y=0; y<h; y++) {
      for (x=0; x<w; x++) {
        FXColor px=ico->getPixel(x,y);
#if defined(FOX_1_7) && ((FOX_MAJOR>1)||(FOX_MINOR>7)||(FOX_LEVEL>25))
        icon_buf[j++]=px?px:FXRGBA(255,255,255,0);
#else
        icon_buf[j++]=px?FXRGB(FXBLUEVAL(px),FXGREENVAL(px),FXREDVAL(px)):FXRGBA(255,255,255,0);
#endif
      }
    }
    XChangeProperty(d, win->id(), net_wm_icon, cardinal, 32,
      PropModeReplace, (const FXuchar*) icon_buf, icon_buf_size);
    free(icon_buf);
  }

  Atom net_wm_pid = XInternAtom(d, "_NET_WM_PID", 0);
  pid_t pid=fxgetpid();
  XChangeProperty(d, win->id(), net_wm_pid, cardinal, 32,
    PropModeReplace, (const FXuchar*) &pid, sizeof(pid));
  FXString hn=GetHostName();
  if (!hn.empty()) {
    Atom wm_client_machine = XInternAtom(d, "WM_CLIENT_MACHINE", 0);
    XChangeProperty(d, win->id(), wm_client_machine, XA_STRING, 8,
      PropModeReplace, (const FXuchar*) hn.text(), hn.length());
  }
  FXString cn=class_name;
  cn.append(".");
  cn.append(APP_NAME);
  Atom wm_class = XInternAtom(d, "WM_CLASS", 0);
  XChangeProperty(d, win->id(), wm_class, XA_STRING, 8,
    PropModeReplace, (const FXuchar*) cn.text(), cn.length());
}



FXID GetNetActiveWindow()
{
  FXID rv=0;
  FXApp*app=FXApp::instance();
  Display*dpy=(Display*)app->getDisplay();
  Window root=app->getRootWindow()->id();
  static Atom xa=XInternAtom(dpy, "_NET_ACTIVE_WINDOW", False);
  Atom rtype;
  int fmt;
  unsigned long n;
  unsigned long rem;
  FXuchar *xw;
  if (XGetWindowProperty(dpy,root,xa,0,sizeof(Window),False,XA_WINDOW,&rtype,&fmt,&n,&rem,&xw)==Success) {
    rv=*((Window*)xw);
    XFree(xw);
  }
  return rv;
}

#else
#include <windows.h>
FXID GetNetActiveWindow()
{
  return GetForegroundWindow();
}
#endif




#ifndef WIN32
/*
  We don't want to pop up the notification dialog for external changes unless our application
  currently has the focus, and that's easy enough to do with the Fox API - But - we also don't
  want to pop up the dialog unless our application is on the current virtual desktop, and the
  only way I can see to do that is via the X11 API. I guess this stuff is fairly expensive,
  so we wait to call it until we are sure the dialog would otherwise need to be displayed.
*/
# include <X11/Xlib.h>
# include <X11/Xatom.h>

static int GetXIntProp(FXWindow *win, Atom prop)
{
  int num = 0;
  Atom type_ret;
  int format_ret;
  unsigned long items_ret;
  unsigned long after_ret;
  unsigned char *prop_ret=NULL;
  XGetWindowProperty( (Display*)win->getApp()->getDisplay(), win->id(), prop, 0, 0x7fffffff, False,
                        XA_CARDINAL, &type_ret, &format_ret, &items_ret, &after_ret, &prop_ret);
  if (prop_ret) {
    num = *((int*)prop_ret);
    XFree(prop_ret);
  }
  return num;
}



bool IsDesktopCurrent(FXMainWindow*tw)
{
  static bool failed=false;
  static Atom NET_WM_DESKTOP=None;
  static Atom NET_CURRENT_DESKTOP=None;
  if (!failed) {
    if (NET_WM_DESKTOP==None) {
      NET_WM_DESKTOP=XInternAtom((Display*)tw->getApp()->getDisplay(), "_NET_WM_DESKTOP",true);
    }
    if (NET_CURRENT_DESKTOP==None) {
      NET_CURRENT_DESKTOP=XInternAtom((Display*)tw->getApp()->getDisplay(), "_NET_CURRENT_DESKTOP",true);
    }
    if ((NET_CURRENT_DESKTOP==None)||(NET_WM_DESKTOP==None)) {
      failed=true;
    } else {
      int this_desk=GetXIntProp(tw,NET_WM_DESKTOP);
      int that_desk=GetXIntProp(tw->getApp()->getRootWindow(),NET_CURRENT_DESKTOP);
      bool rv=(this_desk==that_desk);
      return rv;
    }
  }
  return true;
}



static bool ClientMsg(FXTopWindow*w, const char *msg, FXulong d0=0, FXulong d1=0, FXulong d2=0, FXulong d3=0, FXulong d4=0) {
  Display*dpy=(Display*)w->getApp()->getDisplay();
  Window root=w->getApp()->getRootWindow()->id();
  XEvent event;
  long mask = SubstructureRedirectMask | SubstructureNotifyMask;
  event.type = ClientMessage;
  event.xclient.type = ClientMessage;
  event.xclient.serial = 0;
  event.xclient.send_event = True;
  event.xclient.message_type = XInternAtom(dpy, msg, False);
  event.xclient.window = w->id();
  event.xclient.format = 32;
  event.xclient.data.l[0] = d0;
  event.xclient.data.l[1] = d1;
  event.xclient.data.l[2] = d2;
  event.xclient.data.l[3] = d3;
  event.xclient.data.l[4] = d4;
  return XSendEvent(dpy, root, False, mask, &event)?true:false;
}

#endif



// Try for "msecs" milliseconds to give window "w" (or any of its children) the keyboard focus.
void WaitForWindowFocus(FXTopWindow*w, FXuint msecs)
{
  FXApp*a=w->getApp();
#ifdef FOX_1_6
  FXlong timeout=FXThread::time()+1000000*msecs;
#else
  FXTime timeout=FXThread::time()+1000000*msecs;
#endif

#ifndef WIN32
  Display*dpy=(Display*)a->getDisplay();
  ClientMsg(w,"_NET_ACTIVE_WINDOW", 2);
  XSetInputFocus(dpy, w->id(), RevertToPointerRoot, CurrentTime);
  XRaiseWindow(dpy,w->id());
#endif

  while (FXThread::time()<timeout) {
    FXWindow*fw=a->getFocusWindow();
    if (fw) {
      if ((fw==w)||fw->isChildOf(w)) { break; } else { fw->killFocus(); }
    }
    w->raise();
    w->setFocus();
    a->runWhileEvents();
  }
}

