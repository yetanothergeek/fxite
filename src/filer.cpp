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


#include <unistd.h>
#include <cerrno>
#include <fx.h>
#include <fxkeys.h>
#include <Scintilla.h>
#include <FXScintilla.h>

#include "scidoc.h"
#include "export.h"
#include "prefs.h"
#include "compat.h"
#include "filedlg.h"

#include "intl.h"
#include "filer.h"


FXIMPLEMENT(FileDialogs, FXObject, NULL, 0);


FileDialogs::FileDialogs(FXObject*tgt, FXSelector sel):FXObject()
{
  _patterns=_("All Files (*)");
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
  if (sci->Filename().empty()) {
    path=FXSystem::getCurrentDirectory();
  } else {
    path=FXPath::directory(sci->Filename());
  }
#ifndef WIN32
  path.append(PATHSEP);
#endif
}



#define prefs Settings::instance()

bool FileDialogs::SaveFileAs(SciDoc*sci, bool as_itself)
{
  FXString result="";
  FXString path;
  GetPathForDlg(sci,path);
  FileDlg dlg(sci->getShell(),_("Save file as"));
  dlg.setPatternList(_patterns);
  dlg.setCurrentPattern(prefs->FileFilterIndex);
  dlg.setDirectory(path);
  if (dlg.execute(PLACEMENT_OWNER)) {
    result=dlg.getFilename();
    prefs->FileFilterIndex=prefs->KeepFileFilter?dlg.getCurrentPattern():0;
  }
  if (!result.empty()) {
    if (FXStat::exists(result)) {
      switch (FXMessageBox::question(sci->getShell(), MBOX_YES_NO_CANCEL, _("Overwrite?"),
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
    FileDlg dlg(sci->getShell(),title);
    if (sci->Filename().empty()) {
      path=FXSystem::getCurrentDirectory();
      tmp=_("Untitled");
      tmp.append(ext);
      dlg.setFilename(tmp);
    } else {
      path=FXPath::directory(sci->Filename());
      tmp=FXPath::name(sci->Filename());
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
      switch (FXMessageBox::question(sci->getShell(), MBOX_YES_NO_CANCEL, _("Overwrite?"),
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
    FXMessageBox::error(sci->getShell(),MBOX_OK,
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



bool FileDialogs::SaveFile(SciDoc*sci, FXString filename, bool as_itself)
{
  if (filename.empty()) { return SaveFileAs(sci,as_itself); }
  if (sci->SaveToFile(filename.text(),as_itself)) {
    if (as_itself) {
      ((FXTabItem*)(sci->getParent()->getPrev()))->setText(FXPath::name(filename));
    }
    if (target && message) { target->handle(this, FXSEL(SEL_COMMAND,message), (void*)sci); }
    return true;
  } else {
    FXMessageBox::error( sci->getShell(),MBOX_OK,_("Error saving file"), "%s:\n%s\n%s", _("Failed to save file"),
                           filename.text(), sci->GetLastError().text() );
    return false;
  }
}



bool FileDialogs::TryClose(SciDoc*sci, const char*alt)
{
  if (!sci) return false;
  if (sci->Dirty()) {
    switch ( FXMessageBox::question(sci->getShell(), MBOX_YES_NO_CANCEL, _("Save file?"),
                _("File has been modified:\n%s\n\nSave before closing?"),
                sci->Filename().empty()?(alt?alt:_("Untitled")):sci->Filename().text())
           )
    {
      case MBOX_CLICKED_YES: {
        if (!SaveFile(sci,sci->Filename())) { return false; }
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
  FileDlg dlg(sci->getShell(), caption, multi);
  dlg.setPatternList(_patterns);
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
  FileDlg dlg(sci->getShell(), caption);
  dlg.setPatternList(_("Tag Files (TAGS,tags)\nAll Files (*)"));
  dlg.setDirectory(path);
  dlg.setSelectMode(SELECTFILE_EXISTING);
  if (dlg.execute(PLACEMENT_OWNER)) {
    filename=dlg.getFilename().text();
  } else filename="";
  return (!filename.empty());
}



bool FileDialogs::AskReload(SciDoc*sci) {
  if (sci->Dirty()) {
    if ( FXMessageBox::warning(sci->getShell(),
           MBOX_YES_NO, _("Unsaved changes"), "%s - \n%s\n\n%s",
           _("Existing buffer has unsaved changes"),
           _("These changes will be lost if you proceed!"),
           _("Proceed with reloading?")
       )!=MBOX_CLICKED_YES ) { return false; }
  }
  long pos=sci->sendMessage(SCI_GETCURRENTPOS,0,0);
  SciDoc*sci2=sci->Slave();
  long pos2=sci2?sci2->sendMessage(SCI_GETCURRENTPOS,0,0):0;
  if ( !sci->LoadFromFile(sci->Filename().text()) ) {
    FXMessageBox::error(sci->getShell(),
      MBOX_OK,_("Reload failed"), "%s\n%s", sci->Filename().text(), sci->GetLastError().text());
    return false;
  }
  sci->sendMessage(SCI_GOTOPOS,pos,0);
  if (sci2) { sci2->sendMessage(SCI_GOTOPOS,pos2,0); }
  FXTabItem*tab=(FXTabItem*)sci->getParent()->getPrev();
  tab->setText(FXPath::name(sci->Filename()));
  sci->DoStaleTest(true);
  return true;
}

