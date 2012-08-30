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


#include <unistd.h>
#include <cerrno>
#include <fx.h>

#include "scidoc_util.h"
#include "compat.h"

#include "intl.h"
#include "backup.h"


#define ErrorMessage(fmt, fn) \
{ \
  if (mainwin->shown()) { \
    FXMessageBox::error(mainwin, \
      MBOX_OK, _("Autosave Error"), "%s:\n%s\n(%s)", fmt,fn.text(), lasterror.text()); \
  } else { \
    FXMessageBox::error(FXApp::instance(), \
      MBOX_OK, _("Autosave Error"), "%s:\n%s\n(%s)", fmt, fn.text(), lasterror.text()); \
  } \
}



bool BackupMgr::MakePath(const FXString& path)
{
  lasterror="";
  FXString parts=FXPath::absolute(path);
  FXString dirs=FXPath::root(parts);
  parts.append(PATHSEP);
  FXint nseps=parts.contains(PATHSEP);
  FXint i;
  for (i=1;i<nseps;i++) {
    dirs.append(parts.section(PATHSEP,i));
    if (!(IsDir(dirs)||FXDir::create(dirs,FXIO::OwnerFull))) {
      lasterror=SystemErrorStr();
      ErrorMessage(_("Failed to create backup directory"), dirs);
      return false;
    }
    dirs.append(PATHSEP);
  }
  return true;
}



BackupMgr::BackupMgr(FXMainWindow*w, const FXString &configdir)
{
  mainwin=w;
  backupdir=configdir;
  backupdir=FXPath::directory(backupdir);
  backupdir=FXPath::directory(backupdir);
  backupdir.append(PATHSEP);
  backupdir.append("backups.d");
  MakePath(backupdir);
  backupdir.append(PATHSEP);
}

#ifdef WIN32
# define FN_FMT "%s%d-%p"
#else
# define FN_FMT "%s%d-%lx"
#endif


bool BackupMgr::SaveBackup(SciDoc*sci)
{
  FXString savename;
  FXString untitled;
  untitled.format(FN_FMT, backupdir.text(), abs(getpid()), SciDocUtils::ID(sci));
  if (SciDocUtils::Filename(sci).empty()) {
    savename=untitled;
  } else {
    if (FXStat::isFile(untitled)) {
      RemoveBackup(untitled);
    }
#ifdef WIN32
    savename=SciDocUtils::Filename(sci).text();
    savename.substitute(':', '%', true);
    savename.prepend(backupdir.text());
#else
    savename.format("%s%s", backupdir.text(), SciDocUtils::Filename(sci).text());
#endif
  }
  if (MakePath(FXPath::directory(savename))) {
    if (SciDocUtils::SaveToFile(sci,savename.text(),false)) {
      SciDocUtils::NeedBackup(sci, false);
      return true;
    } else {
      lasterror=SciDocUtils::GetLastError(sci);
      ErrorMessage(_("Failed to save backup"), savename);
      return false;
    }
  } else {
    return false;
  }
}



void BackupMgr::RemoveBackup(const FXString&filename) {
  if (FXFile::remove(filename)) {
    FXString dir=filename;
    while (1) {
      dir=FXPath::directory(dir);
      if ( (dir+PATHSEP) == backupdir ) { break; }
      if (!FXDir::remove(dir)) { break; }
    }
  }
}



void BackupMgr::RemoveBackup(SciDoc*sci)
{
  FXString untitled;
  SciDocUtils::NeedBackup(sci,false);
  untitled.format(FN_FMT, backupdir.text(), abs(getpid()), SciDocUtils::ID(sci));
  RemoveBackup(untitled);
  if (!SciDocUtils::Filename(sci).empty()) {
    FXString savename;
#ifdef WIN32
    savename=SciDocUtils::Filename(sci).text();
    savename.substitute(':', '%', true);
    savename.prepend(backupdir.text());
#else
    savename.format("%s%s", backupdir.text(), SciDocUtils::Filename(sci).text());
#endif
    RemoveBackup(savename);
  }
}

