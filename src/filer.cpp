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


#include <unistd.h>
#include <cerrno>
#include <fx.h>
#include <fxkeys.h>

#include "scidoc_util.h"
#include "export.h"
#include "prefs_base.h"
#include "compat.h"
#include "filedlg.h"
#include "doctabs.h"

#include "intl.h"
#include "filer.h"


FXIMPLEMENT(FileDialogs, FXObject, NULL, 0);


FileDialogs::FileDialogs(FXObject*tgt, FXSelector sel):FXObject()
{
  user_data=NULL;
  target=tgt;
  message=sel;
}

#ifdef WIN32
bool FileDialogs::ReadShortcut(FXWindow*w, FXString &filename)
{
  return FileDlg::ReadShortcut(w,filename);
}
#endif

static void GetPathForDlg(SciDoc*sci, FXString &path)
{
  if (SciDocUtils::Filename(sci).empty()) {
    path=FXSystem::getCurrentDirectory();
  } else {
    path=FXPath::directory(SciDocUtils::Filename(sci));
  }
#ifndef WIN32
  path.append(PATHSEP);
#endif
}



#define prefs SettingsBase::instance()

bool FileDialogs::SaveFileAs(SciDoc*sci, bool as_itself, const FXString &suggestion)
{
  FXString result="";
  FXString path;
  GetPathForDlg(sci,path);
  FileDlg dlg((FXMainWindow*)target,_("Save file as"));
  dlg.setPatternList(prefs->FileFilters);
  dlg.setCurrentPattern(prefs->FileFilterIndex);
  if (!suggestion.empty()) { dlg.setFilename(suggestion); }
  dlg.setDirectory(path);
  if (dlg.execute(PLACEMENT_OWNER)) {
    result=dlg.getFilename();
    prefs->FileFilterIndex=prefs->KeepFileFilter?dlg.getCurrentPattern():0;
  }
  if (!result.empty()) {
    if (FXStat::exists(result)) {
      switch (FXMessageBox::question((FXMainWindow*)target, MBOX_YES_NO_CANCEL, _("Overwrite?"),
                "%s:\n%s\n\n%s", _("File exists"), result.text(), _("Do you want to replace it?"))
             ) {
        case MBOX_CLICKED_YES: { break; }
        case MBOX_CLICKED_NO: { return SaveFileAs(sci); }
        case MBOX_CLICKED_CANCEL: {return false;}
      }
    }
    return SaveFile(sci,result,as_itself);
  }
  return false;
}



bool FileDialogs::Export(SciDoc*sci,
  const char* title, const char*patts, const char*ext, ExportFunc func, const char*filename)
{
  FXString path;
  FXString saveName="";
  FXString tmp;
  if (!sci) { return false; }
  if (filename) {
    saveName=filename;
  } else {
    FileDlg dlg((FXMainWindow*)target,title);
    if (SciDocUtils::Filename(sci).empty()) {
      path=FXSystem::getCurrentDirectory();
      tmp=_("Untitled");
      tmp.append(ext);
      dlg.setFilename(tmp);
    } else {
      path=FXPath::directory(SciDocUtils::Filename(sci));
      tmp=FXPath::name(SciDocUtils::Filename(sci));
      tmp.substitute('.','_');
      tmp.append(ext);
      dlg.setFilename(tmp);
    }
#ifndef WIN32
    path.append(PATHSEP);
#endif
    dlg.setPatternList(patts);
    dlg.setDirectory(path);
    if (dlg.execute(PLACEMENT_OWNER)) {
      saveName=dlg.getFilename();
    }
  }
  if (!saveName.empty()) {
    if (FXStat::exists(saveName)) {
      switch (FXMessageBox::question((FXMainWindow*)target, MBOX_YES_NO_CANCEL, _("Overwrite?"),
                "%s:\n%s\n\n%s", _("File exists"), saveName.text(), _("Do you want to replace it?")
                )) {
        case MBOX_CLICKED_YES: { break; }
        case MBOX_CLICKED_NO: { return Export(sci,title,patts,ext,func); }
        case MBOX_CLICKED_CANCEL: {return false;}
      }
    }
    FILE *fp = fopen(saveName.text(), "wt");
    if (fp) {
      func(sci,fp);
      if (fclose(fp)==0) { return true; }
    }
    FXMessageBox::error((FXMainWindow*)target,MBOX_OK,
      _("Export error"), "%s:\n%s\n\n%s", _("Failed to save file"), saveName.text(), SystemErrorStr());
  }
  return false;
}



bool FileDialogs::ExportHtml(SciDoc*sci, const char* filename) {
  return Export(sci,
           _("Export to HTML"), _("HTML files (*.html;*.htm)\nAll files (*)"), ".html", SaveToHTML, filename);
}



bool FileDialogs::ExportPdf(SciDoc*sci, const char* filename) {
  return Export(sci, _("Export to PDF"),
            _("Portable Document Format (*.pdf)\nAll files (*)"), ".pdf", SaveToPDF, filename);
}



bool FileDialogs::SaveFile(SciDoc*sci, const FXString &filename, bool as_itself)
{
  if (filename.empty()) { return SaveFileAs(sci,as_itself); }
  if (SciDocUtils::SaveToFile(sci,filename.text(),as_itself)) {
    if (target && message) { target->handle(this, FXSEL(SEL_COMMAND,message), (void*)sci); }
    return true;
  } else {
    FXMessageBox::error( (FXMainWindow*)target,MBOX_OK,_("Error saving file"), "%s:\n%s\n%s", _("Failed to save file"),
                           filename.text(), SciDocUtils::GetLastError(sci).text() );
    return false;
  }
}



bool FileDialogs::TryClose(SciDoc*sci, const char*alt)
{
  if (!sci) return false;
  if (SciDocUtils::Dirty(sci)) {
    switch ( FXMessageBox::question((FXMainWindow*)target, MBOX_YES_NO_CANCEL, _("Save file?"),
                _("File has been modified:\n%s\n\nSave before closing?"),
                SciDocUtils::Filename(sci).empty()?(alt?alt:_("Untitled")):SciDocUtils::Filename(sci).text())
           )
    {
      case MBOX_CLICKED_YES: {
        if (!SaveFile(sci,SciDocUtils::Filename(sci))) { return false; }
        break;
      }
      case MBOX_CLICKED_NO: {
        break;
      }
      case MBOX_CLICKED_CANCEL: {
        return false;
        break;
      }
      default:{return false;}
    }
  }
  return true;
}



bool FileDialogs::GetOpenFilenames(SciDoc*sci, FXString* &filenames, bool multi)
{
  const char* caption=multi?_("Select files to open"):_("Select file to open");
  FXString path="";
  GetPathForDlg(sci,path);
  FileDlg dlg((FXMainWindow*)target, caption, multi);
  dlg.setPatternList(prefs->FileFilters);
  dlg.setCurrentPattern(prefs->FileFilterIndex);
  dlg.setDirectory(path);
  dlg.setSelectMode(multi&&prefs->FileOpenMulti?SELECTFILE_MULTIPLE:SELECTFILE_EXISTING);
  if (dlg.execute(PLACEMENT_OWNER)) {
    filenames=dlg.getFilenames();
    if ( (dlg.getSelectMode()==SELECTFILE_EXISTING) && (!filenames) ) {
      if (!dlg.getFilename().empty()) {
        filenames=new FXString[2];
        filenames[0]=dlg.getFilename();
        filenames[1]="";
      } else { filenames=NULL; }
    }
    prefs->FileFilterIndex=prefs->KeepFileFilter?dlg.getCurrentPattern():0;
    if (multi) { prefs->FileOpenMulti=dlg.MultiMode(); }
  } else { filenames=NULL; }
  return filenames!=NULL;
}



bool FileDialogs::GetOpenTagFilename(SciDoc*sci, FXString &filename)
{
  const char* caption=_("Open tags file");
  FXString path="";
  GetPathForDlg(sci,path);
  FileDlg dlg((FXMainWindow*)target, caption);
  dlg.setPatternList(_("Tag Files (TAGS,tags)\nAll Files (*)"));
  dlg.setDirectory(path);
  dlg.setSelectMode(SELECTFILE_EXISTING);
  if (dlg.execute(PLACEMENT_OWNER)) {
    filename=dlg.getFilename().text();
  } else filename="";
  return (!filename.empty());
}



bool FileDialogs::AskReload(SciDoc*sci) {
  if (SciDocUtils::Dirty(sci)) {
    if ( FXMessageBox::warning((FXMainWindow*)target,
           MBOX_YES_NO, _("Unsaved changes"), "%s - \n%s\n\n%s",
           _("Existing buffer has unsaved changes"),
           _("These changes will be lost if you proceed!"),
           _("Proceed with reloading?")
       )!=MBOX_CLICKED_YES ) { return false; }
  }
  return SciDocUtils::Reload(sci);
}



bool FileDialogs::AskReloadForExternalChanges(SciDoc*sci)
{
  if ( FXMessageBox::question((FXMainWindow*)target, MBOX_YES_NO, _("File changed"),
         "%s\n%s\n\n%s",
         SciDocUtils::Filename(sci).text(),
         _("was modified externally."),
         _("Reload from disk?")
       )==MBOX_CLICKED_YES ) { return AskReload(sci); } else { return false; }
}



bool FileDialogs::AskSaveMissingFile(SciDoc*sci)
{
  return FXMessageBox::question((FXMainWindow*)target, MBOX_YES_NO, _("File status error"),
               "%s:\n%s\n(%s)\n\n%s",
               _("Error checking the status of"),
               SciDocUtils::Filename(sci).text(), SciDocUtils::GetLastError(sci).text(),
               _("Save to disk now?")
               )==MBOX_CLICKED_YES;
}



bool FileDialogs::AskSaveModifiedCommand(SciDoc*sci, const FXString &script)
{
  if (SciDocUtils::Dirty(sci) && (SciDocUtils::Filename(sci)==script)) {
    switch (FXMessageBox::warning((FXMainWindow*)target,
          MBOX_YES_NO_CANCEL,_("Unsaved changes"),
          _("The disk file for the \"%s\" command is currently\n"
            " open in the editor, and has unsaved changes.\n\n"
            "  Save the file before continuing?"), SciDocUtils::Filename(sci).text()))
    {
      case MBOX_CLICKED_YES: { return SaveFile(sci,SciDocUtils::Filename(sci)); }
      case MBOX_CLICKED_NO: { return true; }
      default: { return false; }
    }
  } else { return true; }
}



class WkDirDlg: public FXDirDialog {
private:
  class DirSel: public FXDirSelector {
    public:
    FXDirList* list()  { return dirbox; }
  };
public:
  WkDirDlg(FXWindow* win):FXDirDialog(win, _("Set Working Directory")) {
    setHeight(420);
    setDirectory(FXSystem::getCurrentDirectory()+PATHSEP);
  }
  void setDirectory(const FXString& path) {
    FXDirDialog::setDirectory((FXPath::simplify(path)));
    if (FXPath::isTopDirectory(getDirectory())) {
      FXDirList*list=((DirSel*)dirbox)->list();
      list->expandTree(list->getFirstItem());
    }
  }
  virtual FXuint execute(FXuint placement=PLACEMENT_CURSOR) {
    FXuint rv=FXDirDialog::execute(placement);
    if (rv) {
      FXSystem::setCurrentDirectory(getDirectory());
    }
    return rv;
  }
};



void FileDialogs::SetWorkingDirectory(FXWindow*w)
{
  WkDirDlg(w).execute(PLACEMENT_OWNER);
}



bool FileDialogs::FileExistsOrConfirmCreate(FXMainWindow*w, const FXString &fn)
{
  if ((!fn.empty())&&(!FXStat::exists(fn))) {
    for (FXint i=0; (i<1024) && (!w->getApp()->getFocusWindow()); i++) {
      w->setFocus();
      w->getApp()->runWhileEvents();
    }
    if (FXMessageBox::question( w, MBOX_YES_NO, _("File not found"),
          "%s:\n%s\n %s",
          _("Can't find the file"),
          fn.text(),
          _("Would you like to create it?")
     )==MBOX_CLICKED_YES) {
       if (!FXStat::exists(fn)) { /* <-maybe someone created it while we were waiting for a response? */
         FXFile fh(fn, FXFile::Writing);
         if (!(fh.isOpen() && fh.close())) {
           FXMessageBox::error(w, MBOX_OK, _("File error"),
            "%s:\n%s\n%s",
             _("Failed to create the file"),
             fn.text(),
             SystemErrorStr());
           return false;
         }
       }
    } else { return false; }
  }
  return true;
}

