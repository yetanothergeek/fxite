/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2009 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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

#ifndef FXITE_MACRO_H
#define FXITE_MACRO_H


class TopWindow;

struct lua_State;

#define LUA_MODULE_NAME "fxite"

class MacroRunner: public FXObject {
private:
  Dictionary keepers; // <- Data stored between Lua states
  void ClearKeepers();
  void PushKeepers(lua_State *L);
  void PopKeepers(lua_State *L);
  bool RunMacro(const FXString &source, bool isfilename);
public:
  MacroRunner();
  ~MacroRunner();
  bool DoFile(const FXString &filename);
  bool DoString(const FXString &sourcecode);
};

#endif

