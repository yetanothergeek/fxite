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

#ifndef STATUSBAR_H
#define STATUSBAR_H

class StatusBar: public FXHorizontalFrame {
  FXDECLARE(StatusBar)
  StatusBar() {}
protected:
  FXTextField* coords;
  FXTextField* docname;
  FXTextField* encname;
  FXLabel* mode;
  FXSelector id_kill;
  void Mode(const char*msg);
  void Coords(long line, long col);
public:
  void Colorize();
  void Running(const char*what);
  void Recording(bool recording);
  void FileInfo(const FXString &filename, const char* enc, long line, long column);
  void Normal();
  void Clear();
  void Show(bool showit);
  void SetKillID(FXSelector kill_id);
  StatusBar( FXComposite *p, void* dont_freeze );
};

#endif

