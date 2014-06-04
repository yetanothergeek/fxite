/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2009-2014 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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

#ifndef FXITE_FILER_H
#define FXITE_FILER_H


class BackupMgr;

class FileDialogs: public FXObject {
  FXDECLARE(FileDialogs);
  FileDialogs() {}
private:
  void *user_data;
  FXSelector message;
  FXObject *target;

  typedef void (*ExportFunc) (SciDoc*sci,  FILE *fp);
  bool Export(SciDoc*sci,
    const char*title, const char*patts, const char*ext, ExportFunc func, const char*filename=NULL);
public:
  bool TryClose(SciDoc*sci, const char *alt);
  bool SaveFile(SciDoc*sci, const FXString &filename, bool as_itself=true);
  bool SaveFileAs(SciDoc*sci, bool as_itself=true, const FXString &suggestion=FXString::null);
  bool ExportPdf(SciDoc*sci, const char* filename=NULL);
  bool ExportHtml(SciDoc*sci, const char* filename=NULL);
  bool GetOpenFilenames(SciDoc*sci, FXString* &filenames, bool multi=true);
  bool GetOpenTagFilename(SciDoc*sci, FXString &filename);
  bool AskReload(SciDoc*sci);
  bool AskReloadForExternalChanges(SciDoc*sci);
  bool AskSaveMissingFile(SciDoc*sci);
  bool AskSaveModifiedCommand(SciDoc*sci, const FXString &script);
  void *getUserData() { return user_data; }
  void setUserData(void*p) { user_data=p; }
  static void SetWorkingDirectory(FXWindow*w);
  static bool FileExistsOrConfirmCreate(FXMainWindow*w, const FXString &fn);
  void setSelector(FXSelector sel) { message=sel; }
  FileDialogs(FXObject*tgt, FXSelector sel);
#ifdef WIN32
  static bool ReadShortcut(FXWindow*w, FXString &filename);
#endif
};

#endif

