#ifndef RUNCMD_H
#define RUNCMD_H

class TopWindowBase;

class CommandUtils: public FXObject {
private:
  FXAccelTable* saved_accels;
  FXAccelTable* temp_accels;
  bool command_busy;
  FXHotKey killkey;
  TopWindow*tw;
  FXApp*app;
#ifdef WIN32
  FXuint winkey;
#endif
  void SetKillKey(FXHotKey k);
  void InitKillKey();
public:
  static void Freeze(FXWindow*win, bool frozen);
  static const char* DontFreezeMe();
  FXHotKey KillKey() { return killkey; }
  void SetKillCommandAccelKey(FXHotKey acckey);
  void DisableUI(bool disabled);
  void CommandBusy(bool bsy) { command_busy=bsy; }
  bool CommandBusy() { return command_busy; }
  bool IsCommandReady();
  bool IsMacroCancelled(bool &command_timeout);
  static void SetShellEnv(const char*file, long line);
  static const FXString FixUpCmdLineEnv(const FXString&command);
  CommandUtils(TopWindowBase*w);
  ~CommandUtils();
};

#endif

