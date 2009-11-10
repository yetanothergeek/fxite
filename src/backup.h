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
class AppClass;

class BackupMgr: public FXObject {
private:
  AppClass*app;
  bool MakePath(const FXString& path);
  FXString lasterror;
  FXString backupdir;
  void RemoveBackup(const FXString&filename);

public:
  BackupMgr();
  bool SaveBackup(SciDoc*sci);
  void RemoveBackup(SciDoc*sci);
  const FXString &LastError();
};
