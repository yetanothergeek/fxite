/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2009-2011 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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

#ifndef FXITE_FILEDLG_H
#define FXITE_FILEDLG_H


class FileSel;

class FileDlg: public FXFileDialog {
private:
  FXDECLARE(FileDlg)
  FileDlg() {}
  FXToggleButton *multi_btn;
  FileSel*fsel();
#ifdef WIN32
  private:
    FXString*filenames;
    bool own_filenames;
    void DeleteFilenames();
  public:
    ~FileDlg();
    FXString*getFilenames();
    FXString getFilename();
    FXuint execute(FXuint placement=PLACEMENT_CURSOR);
    static bool ReadShortcut(FXWindow*w, FXString &filename);
#endif
  public:
    FileDlg(FXWindow*win, const FXString&caption, bool optmulti=false);
    virtual void create();
    long onKeyPress(FXObject*o,FXSelector sel,void*p);
    long onToggleMulti(FXObject*o,FXSelector sel,void*p);
    void setSelectMode(FXuint mode);
    bool MultiMode() { return multi_btn->shown() && multi_btn->getState(); }
    enum {
      ID_TOGGLE_MULTI=FXFileDialog::ID_LAST,
      ID_LAST
    };
};

#endif

