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

#include <fx.h>
#include "prefs.h"
#include "desclistdlg.h"

#include "intl.h"
#include "prefdlg_ext.h"


FileFiltersDlg::FileFiltersDlg(FXWindow* w, const FXString init):
  DescListDlg( w, _("File dialog filters"), init, 
               _("File mask"),_("File masks:   (separated by comma)"))
{
  setText(init);
}



bool FileFiltersDlg::Verify(FXString&item)
{
  for (const char*c=" \t()"; *c; c++ ) {
    const char s[2]={*c,0};
    item.substitute(s, "", true);
  }
  return true;
}



void FileFiltersDlg::RestoreAppDefaults()
{
  setText(Settings::defaultFileFilters()); 
}



void FileFiltersDlg::setText(const FXString str)
{
  items->clearItems();
  FXString FileFilters=str;
  FileFilters.substitute('|', '\n', true);
  for (FXint i=0; i<FileFilters.contains('\n'); i++) {
    FXString sect=FileFilters.section('\n',i);
    sect.simplify();
    FXString desc=sect.section("(", 0);
    FXString mask=sect.section("(", 1);
    mask.substitute(")", "");
    desc.simplify();
    mask.simplify();
    FXString txt;
    txt.format("%s\t%s", desc.text(), mask.text());
    items->appendItem(txt);
  }
}



const FXString& FileFiltersDlg::getText()
{
  after="";
  for (FXint i=0; i<items->getNumItems(); i++) {
    FXString item=items->getItemText(i);
    FXString desc=item.section('\t',0);
    FXString mask=item.section('\t',1);
    item.format("%s (%s)|", desc.text(), mask.text());
    after.append(item);
  }
  return after;
}

