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


class BackupMgr;

class FileDialogs: public FXObject {
  FXDECLARE(FileDialogs);
  FileDialogs() {}
private:
  FXString _patterns;
  void *user_data;
  FXSelector message;
  FXObject *target;

  typedef void (*ExportFunc) (SciDoc*sci,  FILE *fp);
  bool Export(SciDoc*sci,
    const char*title, const char*patts, const char*ext, ExportFunc func, const char*filename=NULL);
public:
  void patterns(FXString &pats) { _patterns=pats.text(); }
  FXString patterns() { return _patterns; }
  bool TryClose(SciDoc*sci, const char *alt);
  bool SaveFile(SciDoc*sci, FXString filename, bool as_itself=true);
  bool SaveFileAs(SciDoc*sci, bool as_itself=true, const FXString &suggestion=FXString::null);
  bool ExportPdf(SciDoc*sci, const char* filename=NULL);
  bool ExportHtml(SciDoc*sci, const char* filename=NULL);
  bool GetOpenFilenames(SciDoc*sci, FXString* &filenames, bool multi=true);
  bool GetOpenTagFilename(SciDoc*sci, FXString &filename);
  bool AskReload(SciDoc*sci);
  void *getUserData() { return user_data; }
  void setUserData(void*p) { user_data=p; }
  void SetWorkingDirectory(FXWindow*w);
  FileDialogs(FXObject*tgt, FXSelector sel);
#ifdef WIN32
  static bool ReadShortcut(FXWindow*w, FXString &filename);
#endif
};

