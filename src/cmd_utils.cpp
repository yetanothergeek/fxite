#include <fx.h>
#include <fxkeys.h>

#ifdef WIN32
# include <windows.h>
#endif

#include "appwin.h"
#include "menuspec.h"
#include "intl.h"

#include "cmd_utils.h"


static const char* dont_freeze_me = "don't taze me, bro!";


void CommandUtils::Freeze(FXWindow*win, bool frozen)
{
  FXWindow*w;
  for (w=win->getFirst(); w; w=w->getNext()) {
    if (w->getUserData()==dont_freeze_me) { continue; }
    if (frozen) {
      w->disable();
      w->repaint();
    } else {w->enable();}
    Freeze(w,frozen);
  }
}



const char* CommandUtils::DontFreezeMe()
{
  return dont_freeze_me;
}



void CommandUtils::SetKillKey(FXHotKey k)
{
  killkey=k;
#ifdef WIN32
  winkey=VkKeyScan(FXSELID(killkey));
#endif
}



void CommandUtils::InitKillKey()
{
  MenuSpec*killcmd=MenuMgr::LookupMenu(TopWindow::ID_KILL_COMMAND);
  SetKillKey(parseAccel(killcmd->accel));
  if (killkey && FXSELID(killkey)) {
    temp_accels=new FXAccelTable();
    temp_accels->addAccel(killkey,this,FXSEL(SEL_COMMAND,TopWindow::ID_KILL_COMMAND),0);
   } else {
    FXMessageBox::warning(tw->getApp(), MBOX_OK, _("Configuration error"),
      "%s \"%s\"\n%s",
      _("Failed to parse accelerator for"),
      killcmd->pref,
      _("disabling support for macros and external commands.")
      );
    temp_accels=NULL;
  }
}



CommandUtils::CommandUtils(TopWindowBase*win)
{
  saved_accels=NULL;
  temp_accels=NULL;
  command_busy=false;
  tw=(TopWindow*)win;
  app=tw->getApp();
  InitKillKey();
}



CommandUtils::~CommandUtils()
{
  delete temp_accels;
  delete saved_accels;
}



void CommandUtils::SetKillCommandAccelKey(FXHotKey acckey)
{
  SetKillKey(acckey);
  delete temp_accels;
  temp_accels=new FXAccelTable();
  temp_accels->addAccel(acckey,this,FXSEL(SEL_COMMAND,TopWindow::ID_KILL_COMMAND),0);
}



void CommandUtils::DisableUI(bool disabled)
{
  if (tw->Destroying()) { return; }
  if (disabled) {
    saved_accels=tw->getAccelTable();
    tw->setAccelTable(temp_accels);
    temp_accels=NULL;
  } else {
    temp_accels=tw->getAccelTable();
    tw->setAccelTable(saved_accels);
    saved_accels=NULL;
  }
  CommandUtils::Freeze(tw,disabled);
}



bool CommandUtils::IsCommandReady()
{
  if (command_busy) {
    FXMessageBox::error(tw, MBOX_OK, _("Command error"),
      _("Multiple commands cannot be executed at the same time."));
    return false;
  }
  if (!temp_accels) {
    FXMessageBox::error(tw, MBOX_OK, _("Command support disabled"),
      _("Support for running macros and external commands has been\n"
        "disabled, because the interrupt key sequence is invalid.\n\n"
        "To fix this, go to:\n"
        "  Edit->Preferences->Keybindings\n"
        "and enter a valid setting for \"%s\""),
      MenuMgr::LookupMenu(TopWindow::ID_KILL_COMMAND)->pref
    );
    return false;
  }
  return true;
}



void CommandUtils::SetShellEnv(const char*file, long line)
{
  char linenum[8]="\0\0\0\0\0\0\0";
  snprintf(linenum,sizeof(linenum)-1, "%ld", line+1);
#ifdef WIN32
  FXSystem::setEnvironment("l",linenum);
  FXSystem::setEnvironment("f",file);
#else
  setenv("l",linenum,1);
  setenv("f",file,1);
#endif
}



const FXString CommandUtils::FixUpCmdLineEnv(const FXString&command)
{
#ifdef WIN32
  FXString cmd=command;
  cmd.substitute("%F%", FXSystem::getEnvironment("f"), true);
  cmd.substitute("%f%", FXSystem::getEnvironment("f"), true);
  cmd.substitute("%L%", FXSystem::getEnvironment("l"), true);
  cmd.substitute("%l%", FXSystem::getEnvironment("l"), true);
  return cmd;
#else
  return command;
#endif
}



#ifdef WIN32
# define kill_key_down()  ( GetKeyState(winkey) & 0x8000 )
# define ctrl_key_down()  ( GetKeyState(VK_CONTROL)   & 0x8000 )
# define alt_key_down()   ( GetKeyState(VK_MENU)      & 0x8000 )
# define shift_key_down() ( GetKeyState(VK_SHIFT)     & 0x8000 )
#else
# define kill_key_down()  ( app->getKeyState(FXSELID(killkey)) )
# define ctrl_key_down()  ( app->getKeyState(KEY_Control_L) || app->getKeyState(KEY_Control_R) )
# define alt_key_down()   ( app->getKeyState(KEY_Alt_L)     || app->getKeyState(KEY_Alt_R) )
# define shift_key_down() ( app->getKeyState(KEY_Shift_L)   || app->getKeyState(KEY_Shift_R) )
#endif

/*
  Usually, the application will catch the kill command key sequence by itself,
  but if the event queue gets really full e.g. when appending large amounts
  of data to the output window, the key event may get buried underneath
  everything else that's happening. This function "manually" checks for
  the key sequence and returns true if the user is trying to cancel.
*/
bool CommandUtils::IsMacroCancelled(bool &command_timeout)
{
  if (command_timeout) { return true; }
  if (kill_key_down()) {
    FXushort mods=FXSELTYPE(killkey);
    if ( (mods&CONTROLMASK) && (!ctrl_key_down())  ) { return false; }
    if ( (mods&SHIFTMASK)   && (!shift_key_down()) ) { return false; }
    if ( (mods&ALTMASK)     && (!alt_key_down())   ) { return false; }
    command_timeout=true;
  }
  return command_timeout;
}

