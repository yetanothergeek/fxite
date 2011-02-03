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

#include "compat.h"
#include "intl.h"
#include "prefdlg_ext.h"


static const char*file_filter_intro=_("\
Edit the list of wildcard masks (File Filters) that appear in \n\
the drop-down box for File Open and File Save dialogs.\n\
");


FileFiltersDlg::FileFiltersDlg(FXWindow* w):
  DescListDlg( w, _("File dialog filters"), 
               _("File mask"),_("File masks:   (separated by comma)"), file_filter_intro)
{
  before=Settings::instance()->FileFilters;
  before.substitute('\n', '|', true);
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
    items->appendItem(desc.simplify() + '\t' + mask.simplify());
  }
}



const FXString& FileFiltersDlg::getText()
{
  after.clear();
  for (FXint i=0; i<items->getNumItems(); i++) {
    FXString item=items->getItemText(i);
    FXString desc=item.section('\t',0);
    FXString mask=item.section('\t',1);
    item.format("%s (%s)|", desc.text(), mask.text());
    after+=item;
  }
  return after;
}



FXuint FileFiltersDlg::execute(FXuint placement)
{
  if (DescListDlg::execute(placement)) { 
    FXString filters=getText();
    filters.substitute('|', '\n', true);
    Settings::instance()->FileFilters=filters.text();
    return true;    
  } else {
    return false;
  }
}



static const char*errpat_intro=_("\
A list of regular expressions used to parse file name and line number \n\
information from compiler output, etc. Each expression uses two pairs \n\
of capturing parenthesis: the first pair for the filename, and the second \n\
pair for the line number. The expressions are tried in the order they \n\
appear here, first one to match an existing file wins. \n\
");


ErrPatDlg::ErrPatDlg(FXWindow* w):
  DescListDlg(w, _("Output pane line matching patterns"), _("Regular Expression"),
  _("Regular Expression:\n First capture is filename, second is line number"), errpat_intro)
{
  before.clear();
  ErrorPattern*ep= Settings::ErrorPatterns();
  for (FXint i=0; i<Settings::ErrorPatternCount(); i++) {
    before+=ep[i].id;
    before+='\t';
    before+=ep[i].pat;
    before+='\n';
  } 
  desc_max_len=sizeof(ep->id)-1;
  item_max_len=sizeof(ep->pat)-1;
  items_max=Settings::MaxErrorPatterns();
}



void ErrPatDlg::setText(const FXString str)
{
  items->clearItems();
  for (FXint i=0; i<str.contains('\n'); i++) {
    items->appendItem(str.section('\n',i));
  }
}



const FXString& ErrPatDlg::getText()
{
  after.clear();
  for (FXint i=0; i<items->getNumItems(); i++) {
    FXString item=items->getItemText(i)+'\n';
    after+=item;
  }
  return after;
}



bool ErrPatDlg::Verify(FXString&item)
{
  FXRex *rx=new FXRex();
  FXRexError err=rx->parse(item,REX_CAPTURE|REX_SYNTAX);
  delete rx;
  if (err!=REGERR_OK) {
    FXMessageBox::error(this, MBOX_OK, _("Syntax error"), "%s:\n%s",
        _("Error parsing regular expression"), FXRex::getError(err));
    return false;
  } else {
    return true;
  }
}



void ErrPatDlg::RestoreAppDefaults()
{
  FXString txt=FXString::null;
  ErrorPattern*ep= Settings::DefaultErrorPatterns();
  for (FXint i=0; i<Settings::MaxErrorPatterns(); i++) {
    txt+=ep[i].id;
    txt+='\t';
    txt+=ep[i].pat;
    txt+='\n';
  }
  setText(txt); 
}



FXuint ErrPatDlg::execute(FXuint placement)
{
  ErrorPattern *ep=Settings::ErrorPatterns();
  if (DescListDlg::execute(placement)) { 
    FXString txt=getText();
    for (FXint i=0; (i<Settings::MaxErrorPatterns()); i++) {
      if (i<txt.contains('\n')) {
        FXString line=txt.section('\n',i);
        strncpy(ep[i].id, line.section('\t',0).text(), sizeof(ep[i].id)-1);
        strncpy(ep[i].pat, line.section('\t',1).text(), sizeof(ep[i].pat)-1);
      } else {
        ep[i].id[0]=0;
        ep[i].pat[0]=0;
      }
    }
    return true;    
  } else {
    return false;
  }  
}
