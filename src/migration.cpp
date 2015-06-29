/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2009-2010 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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

#include <fx.h>
#include "compat.h"


#ifndef FOX_1_6

// Some strategy to transfer configuration settings from ~/.foxrc to ~/.config

#include "intl.h"
#include <errno.h>
#include "appname.h"


static void AppendError(FXString &errors, const char*err, const FXString &filename)
{
  FXString tmp;
  tmp.format("\n%s (%s): %s", err, SystemErrorStr(), filename.text());
  errors+=tmp;
}

#define Error(e,f) AppendError(errors,e,f)


# ifdef WIN32
#  define DoStat FXStat::statFile
# else
#  define DoStat FXStat::statLink
# endif

static void copyConfigFiles(const FXString& srcfile,const FXString& dstfile, FXString &errors,FXint level)
{
  if (srcfile!=dstfile) {
    FXString name,linkname;
    FXStat srcstat;
    FXStat dststat;
    if (DoStat(srcfile,srcstat)) {
      if (DoStat(dstfile,dststat)) {
        errno=0;
        Error(_("File exists"), dstfile);
        return;
      }
      if (srcstat.isDirectory()) {
# ifdef WIN32
        if ((level==2) && (FXPath::name(srcfile)=="servers")) {
          FXDir::remove(srcfile);
          return;
        }
# endif
        if (!dststat.isDirectory()) {
          if (!FXDir::create(dstfile,srcstat.mode()|FXIO::OwnerWrite)) {
             Error(_("Create failed"), dstfile);
             return;
          }
        }
        FXDir dir(srcfile);
        if (dir.isOpen()) {
          while(dir.next(name)) {
            if (name[0]=='.' && (name[1]=='\0' || (name[1]=='.' && name[2]=='\0'))) continue;
            copyConfigFiles(srcfile+PATHSEP+name,dstfile+PATHSEP+name,errors,level+1);
          }
        } else {
          Error(_("Access failed"), srcfile);
        }
        return;
      }
      FXString newname=dstfile.text();
# ifndef WIN32
      if ((level==2) && ((FXPath::name(srcfile)=="settings")||(FXPath::name(srcfile)=="styles"))) {
        newname+=".rc";
      }
# endif
      if (srcstat.isFile()) {
        if (!FXFile::copy(srcfile,newname,false)) {
          Error(_("Copy failed"), srcfile);
        }
        return;
      }
      if (srcstat.isLink()) {
        linkname=FXFile::symlink(srcfile);
        if (!FXFile::symlink(linkname,newname)) {
          Error(_("Link failed"), newname);
        }
        return;
      }
      if (srcstat.isFifo()) {
        if (!FXPipe::create(dstfile,srcstat.mode())) {
          Error(_("Create fifo failed"), dstfile);
        }
        return;
      }
      if (srcstat.isSocket()) {
        errno=0;
        Error(_("Ignored socket"), srcfile);
        return;
      }
      if (srcstat.isCharacter()||srcstat.isBlock()) {
        errno=0;
        Error(_("Ignored device node"), srcfile);
        return;
      }
    }
  }
}



void MigrateConfigDir(FXApp*a, const FXString &src, const FXString &dst, FXString &errors)
{
  if (!FXStat::isDirectory(src)) { return; }
  if (FXStat::isDirectory(dst)) { return; }

  FXMessageBox dlg(a,
    _("IMPORTANT NOTICE"),
    _(
    "\n"
    "The location of the "APP_NAME" configuration directory has changed.\n"
    "\n"
#ifndef WIN32
    "This is due to changes in the FOX toolkit, in accordance with\n"
    "the freedesktop.org  \"XDG base directory specification\".\n"
    "\n"
#endif
    "Migration options:\n"
    " Click  [ Yes ]  to automatically copy your old settings (recommended).\n"
    " Click  [ No ]  to create a new configuration.\n"
    " Click  [Cancel]  to exit "APP_NAME" now without any changes.\n"
    "\n"
    "Do you want me to copy your existing configuration?"
    ),
    NULL,MBOX_YES_NO_CANCEL
  );
  a->create();
  FXint rv=dlg.execute(PLACEMENT_SCREEN);
  switch (rv) {
    case MBOX_CLICKED_YES: {
      copyConfigFiles(src,dst,errors,0);
      if (!errors.empty()) {
        errors.prepend("================================\n");
        errors.prepend(_("Settings migration messages:\n"));
        errors.prepend("================================\n");
      }
      a->reg().read();
      return;
    }
    case MBOX_CLICKED_NO: { return; }
    default: { ::exit(0); }
  }
}

#endif


static void MovePref(FXRegistry*old_reg, const char*old_sect, const char*old_name, FXRegistry*new_reg, const char*new_sect, const char*new_name)
{
  if (old_reg->existingEntry(old_sect,old_name)) {
    const char*value=old_reg->readStringEntry(old_sect,old_name);
    new_reg->writeStringEntry(new_sect,new_name,value);
    old_reg->deleteEntry(old_sect,old_name);
  }
}



void MigrateTheme(FXRegistry*r)
{
  static const char*old_sect="CustomColors";
  static const char*new_sect="SETTINGS";
  MovePref(r, old_sect, "Font",    r, new_sect, "Font");
  MovePref(r, old_sect, "Back",    r, new_sect, "backcolor");
  MovePref(r, old_sect, "Base",    r, new_sect, "basecolor");
  MovePref(r, old_sect, "Border",  r, new_sect, "bordercolor");
  MovePref(r, old_sect, "Fore",    r, new_sect, "forecolor");
  MovePref(r, old_sect, "Hilite",  r, new_sect, "hilitecolor");
  MovePref(r, old_sect, "Selback", r, new_sect, "selbackcolor");
  MovePref(r, old_sect, "Selfore", r, new_sect, "selforecolor");
  MovePref(r, old_sect, "Shadow",  r, new_sect, "shadowcolor");
  MovePref(r, old_sect, "Tipback", r, new_sect, "tipbackcolor");
  MovePref(r, old_sect, "Tipfore", r, new_sect, "tipforecolor");
  MovePref(r, old_sect, "SelMenuBack",  r, new_sect, "selmenubackcolor");
  MovePref(r, old_sect, "SelMenuText",  r, new_sect, "selmenutextcolor");
  MovePref(r, old_sect, "UseSystemColors", r, new_sect, "UseSystemColors");
  r->deleteEntry(old_sect, "CurrentTheme");
  r->deleteSection(old_sect);
}



static void MoveSection(FXRegistry*from, FXRegistry*to, const FXString&sect)
{
  if (!from->existingSection(sect.text())) { return; }
#ifdef FOX_1_7_45_OR_NEWER
  const FXStringDictionary *dict=&(from->at(sect));
#else
  const FXStringDict *dict=from->find(sect.text());
#endif
  for (FXint i=0; i<dict->no(); i++) {
    if (!dict->empty(i)) {
      const FXString k=dict->key(i);
      const FXString v=dict->data(i);
      to->writeStringEntry(sect.text(),k.text(),v.text());
    }
  }
  from->deleteSection(sect.text());
}


void MigrateHistory(FXRegistry*from, FXRegistry*to)
{
  MoveSection(from,to,"Commands");
  MoveSection(from,to,"InsertOutput");
  MoveSection(from,to,"Filters");
  MoveSection(from,to,"RecentFiles");
  MoveSection(from,to,"Search");
  MoveSection(from,to,"Replace");
}

