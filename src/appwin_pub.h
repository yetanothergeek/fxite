/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2009-2012 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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

#ifndef APPWIN_PUB_H
#define APPWIN_PUB_H


class SciDoc;
class UserMenu;
class DocTabs;
class FileDialogs;
class TopWindow;


/*
  The main window class has grown to be a huge and unwieldy beast, and trying to
  refactor it is painful because so many other source files depend on its header.
  This class wraps some of the methods provided by the TopWindow class in order
  to reduce dependencies and speed up compilation whenever the "appwin" header
  is modified.
*/

class TopWinPub: public FXObject {
public:
  static bool OpenFile(const char*caption, const char*rowcol, bool readonly, bool hooked);
  static bool CloseFile(bool close_last, bool hooked);
  static int IsFileOpen(const FXString &filename, bool activate);
  static FXString* NamedFiles();
  static SciDoc* ControlDoc();
  static SciDoc* FocusedDoc();
  static void ActiveWidget(FXID aw);
  static void ShowOutputPane(bool showit);
  static FXSelector KillCmdID();
  static FXSelector LastID();
  static void SetKillCommandAccelKey(FXHotKey acckey);
  static UserMenu**UserMenus();
  static void SaveClipboard();
  static const FXString &Connector();
  static const FXString &ConfigDir();
  static void ParseCommands(FXString &commands);
  static FXMainWindow* instantiate(FXApp*a);
  static void create();
  static FXbool close(FXbool notify=false);
  static FXMainWindow* instance();
  static void DumpLexers();
  static void FindText(const char*searchfor, FXuint searchmode, bool forward);
  static void FindAndReplace(const char*searchfor, const char*replacewith, FXuint mode, bool forward);
  static void ReplaceAllInSelection(const char*searchfor, const char*replacewith,  FXuint mode);
  static void ReplaceAllInDocument(const char*searchfor, const char*replacewith,  FXuint mode);
  static void Paste();
  static void AdjustIndent(SciDoc*sci,char ch);
  static DocTabs* Tabs();
  static FileDialogs* FileDlgs();
  static void update();
  static bool IsMacroCancelled();
  static bool Destroying();
  static bool Closing();
  static void CloseWait();
  static bool SetReadOnly(SciDoc*sci, bool rdonly);
  static void SetWordWrap(SciDoc*sci, bool wrapped);
  static FXMenuCaption* TagFiles();
  static void AddOutput(const FXString&line);
};

#endif

