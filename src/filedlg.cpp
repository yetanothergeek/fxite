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
#include <fxkeys.h>

#include "intl.h"
#include "filedlg.h"


class MyDirBox: public FXDirBox {
  public:
  FXButton*fld() { return field; }
  FXMenuButton*btn() { return button; }
  FXPopup*pn() { return pane; }
};



class FileSel: public FXFileSelector {
  public:
    FXTextField*txtfld() { return filename; }
    FXFileList*fbox() { return filebox; }
    FXHorizontalFrame* nav() { return navbuttons; }
    FXButton* cncl() { return cancel; }
    MyDirBox* dbox() { return (MyDirBox*)dirbox; }
    FXButton *icobtn() {
      for (FXWindow*w=navbuttons->getFirst(); w; w=w->getNext()) {
        if ((dynamic_cast<FXButton*>(w)!=NULL) && (((FXButton*)w)->getIcon()==iconsicon)) {
          return (FXButton*)w;
        }
      }
      return NULL;
    }
    void ToggleMulti(bool multi) {
     // Give some indication that the user can't manually enter file names in multi mode.
     FXString label_text;
      if (multi) {
        FXColor bgcolor=getApp()->getBaseColor();
        label_text=_("Selected : ");
        filename->setEditable(false);
        filename->setBackColor(bgcolor);
        filename->setHiliteColor(bgcolor);
        filename->setShadowColor(bgcolor);
        filename->setBorderColor(bgcolor);
        filename->setTextColor(getApp()->getForeColor());
      } else {
        label_text=_("&File Name:");
        filename->setEditable(true);
        filename->setBackColor(getApp()->getBackColor());
        filename->setHiliteColor(getApp()->getHiliteColor());
        filename->setShadowColor(getApp()->getShadowColor());
        filename->setBorderColor(getApp()->getBorderColor());
        filename->setTextColor(getApp()->getForeColor());
      }
      FXLabel*label=dynamic_cast<FXLabel*>(filename->getPrev());
      if (label) { label->setText(label_text); }
    }
};




FXDEFMAP(FileDlg) MyFileDlgMap[]={
  FXMAPFUNC(SEL_COMMAND,FileDlg::ID_TOGGLE_MULTI,FileDlg::onToggleMulti),
  FXMAPFUNC(SEL_KEYPRESS,0,FileDlg::onKeyPress),
};

FXIMPLEMENT(FileDlg,FXFileDialog,MyFileDlgMap,ARRAYNUMBER(MyFileDlgMap));




FileDlg::FileDlg(FXWindow*win, const FXString&caption, bool optmulti):FXFileDialog(win,caption)
{
#ifdef WIN32
  filenames=NULL;
#endif
  ((FileSel*)filebox)->icobtn()->hide();
  multi_btn=new FXToggleButton( fsel()->nav(),_(" &M "),_(" &M "),NULL,NULL,this,
                                  ID_TOGGLE_MULTI,TOGGLEBUTTON_TOOLBAR|FRAME_RAISED );
  multi_btn->setTipText(_("Enable selection of multiple files"));
  multi_btn->setAltTipText(_("Enable manual entry of filename"));
  if (!optmulti) { multi_btn->hide(); }
}



void FileDlg::create()
{
  FXFileDialog::create();
  switch (getSelectMode()) {
    case SELECTFILE_ANY: {
      fsel()->txtfld()->setFocus();
      break;
    }
    case SELECTFILE_MULTIPLE: {
      fsel()->fbox()->setFocus();
      fsel()->fbox()->setCurrentItem(0);
      break;
    }
  }
}


// Make the F4 key behave like it does with some other "toolkits"
// where it drops down the directory selection panel.
// Also makes tab-key navigation feel a little more intuitive (at least for me.)
long FileDlg::onKeyPress(FXObject*o,FXSelector sel,void*p)
{
  FXEvent*ev=(FXEvent*)p;
  switch (ev->code) {
    case KEY_F4: {
#ifdef FOX_1_6
      fsel()->dbox()->btn()->handle(fsel()->dbox()->btn(),FXSEL(SEL_COMMAND,ID_POST),NULL);
#else
      fsel()->dbox()->btn()->showMenu(true);
#endif
      fsel()->dbox()->setFocus();
      fsel()->dbox()->pn()->getFirst()->setFocus();
      break;
    }
    case KEY_Escape: {
#ifdef FOX_1_6
      if (fsel()->dbox()->isPaneShown()) {
        fsel()->dbox()->btn()->handle(fsel()->dbox()->btn(),FXSEL(SEL_COMMAND,ID_UNPOST),NULL);
        return 1;
      }
#else
      if (fsel()->dbox()->btn()->isMenuShown()) {
        fsel()->dbox()->btn()->showMenu(false);
        return 1;
      }
#endif
      break;
    }
    case KEY_Return: {
#ifdef FOX_1_6
      if (fsel()->dbox()->isPaneShown()||fsel()->dbox()->fld()->hasFocus()) {
        fsel()->dbox()->btn()->handle(fsel()->dbox()->btn(),FXSEL(SEL_COMMAND,ID_UNPOST),NULL);
        setDirectory(fsel()->dbox()->getDirectory());
        return 1;
      }
#else
      if (fsel()->dbox()->btn()->isMenuShown()||fsel()->dbox()->fld()->hasFocus()) {
        fsel()->dbox()->btn()->showMenu(false);
        setDirectory(fsel()->dbox()->getDirectory());
        return 1;
      }
#endif
      break;
    }
    case KEY_Tab: {
      if (fsel()->dbox()->fld()->hasFocus()) {
        if (ev->state & SHIFTMASK) {
          fsel()->setFocus();
          fsel()->txtfld()->setFocus();
        } else {
          fsel()->dbox()->getNext()->getNext()->setFocus();
        }
        return 1;
      } else {
        if (multi_btn->hasFocus()||(multi_btn->getPrev()->hasFocus()&&!multi_btn->shown())) {
          fsel()->fbox()->setFocus();
          return 1;
        } else {
          if (fsel()->fbox()->hasFocus()) {
            fsel()->txtfld()->setFocus();
            return 1;
          } else {
            if (fsel()->cncl()->hasFocus()) {
              fsel()->dbox()->fld()->setFocus();
              return 1;
            }
          }
        }
      }
      break;
    }
    case KEY_ISO_Left_Tab: {
      if (ev->state & SHIFTMASK) {
        if (fsel()->dbox()->fld()->hasFocus()) {
          fsel()->cncl()->setFocus();
          return 1;
        } else {
          if (fsel()->fbox()->hasFocus()) {
            if (multi_btn->shown()) {
              multi_btn->setFocus();
            } else {
              multi_btn->getPrev()->setFocus();
            }
            return 1;
          }
        }
      }
      break;
    }
  }
  return FXFileDialog::handle(o,sel,p);
}



long FileDlg::onToggleMulti(FXObject*o,FXSelector sel,void*p)
{
  setSelectMode(p?SELECTFILE_MULTIPLE:SELECTFILE_EXISTING);
  fsel()->txtfld()->setText(FXString::null);
  fsel()->fbox()->killSelection(true);
  if (p) {
    fsel()->fbox()->setFocus();
    fsel()->fbox()->setCurrentItem(0);
  } else {
    fsel()->txtfld()->setFocus();
  }
  return 1;
}



void FileDlg::setSelectMode(FXuint mode) {
  FXFileDialog::setSelectMode(mode);
  if (multi_btn->shown()) {
    FXString label_text;
    if ( mode == SELECTFILE_MULTIPLE ) {
      fsel()->ToggleMulti(true);
      multi_btn->setState(true);
      multi_btn->setBackColor(getApp()->getSelbackColor());
      multi_btn->setTextColor(getApp()->getSelforeColor());
    } else {
      fsel()->ToggleMulti(false);
      multi_btn->setState(false);
      multi_btn->setBackColor(getApp()->getBackColor());
      multi_btn->setTextColor(getApp()->getForeColor());
    }
  }
}



FileSel*FileDlg::fsel()
{
  return (FileSel*)filebox;
}



#ifdef WIN32

extern "C" {
  int ReadShortcut(char **dst, const char *src);
}


// True if file extension is '*.lnk'
#define IsLinkExt(s) (FXPath::extension(s).lower()=="lnk")


/*
  Fox doesn't automatically handle MS-Windows shortcut files, so...
  Iterate through each filename in the file dialog's getFilenames() list,
  if any of them are shortcut (*.lnk) files, dereference the link and
  make the string point to the "real" disk file. If we have multiple files,
  remove any links that point to a directory. But if we only have one string
  in the list, and the string is a link pointing to a directory, we will
  dereference it so the dialog can change into that folder.
*/
static void FixupShortcuts(FXWindow*w, FXString* filenames)
{
  if (!filenames) return;
  FXString* fn;
  FXString* tail=filenames;
  FXuint count=0;
  for (fn=filenames; !fn->empty(); fn++) {
    if (IsLinkExt(*fn)) {
      char*tmp=NULL;
      if (ReadShortcut(&tmp, fn->text())) {
        *fn=tmp;
      } else {
        FXMessageBox::error(w,MBOX_OK,_("Error in shortcut"),"%s\n%s",fn->text(),tmp);
      }
      free(tmp);
    }
    tail=fn;
    count++;
  }
  if (count>1) {
    for (fn=filenames; !fn->empty(); fn++) {
      if (FXStat::isDirectory(*fn)) {
        *fn=tail->text();
        *tail="";
        tail--;
      }
    }
  }
}

/*
  Read the Windows shortcut (*.lnk) file passed in as "filename".
  If the filename does not end with the *.lnk extension, returns
  true, the filename parameter is unchanged.
  If the link cannot be read (e.g. corrupted file) it displays an
  error dialog describing the reason for the failure and returns
  false, the filename parameter is unchanged.
  If reading of the link is successful, it returns true and the
  "filename" parameter is modified and will contain the name of
  the file that the shortcut points to.
*/
bool FileDlg::ReadShortcut(FXWindow*w, FXString &filename)
{
  bool rv=true;
  if (IsLinkExt(filename)) {
    char*tmp=NULL;
     if (::ReadShortcut(&tmp, filename.text())) {
      filename=FXPath::simplify(FXPath::absolute(tmp));
    } else {
      FXMessageBox::error(w,MBOX_OK,_("Error in shortcut"),"%s\n%s",filename.text(),tmp);
      rv=false;
    }
    free(tmp);
  }
  return rv;
}



void FileDlg::DeleteFilenames()
{
  if (filenames&&own_filenames) {
    delete[] filenames;
    filenames=NULL;
  }
}



FileDlg::~FileDlg()
{
  DeleteFilenames();
}



FXString*FileDlg::getFilenames()
{
  own_filenames=false;
  return filenames;
}



FXString FileDlg::getFilename() {
  if (getSelectMode()==SELECTFILE_ANY) {
    FXString fn=FXFileDialog::getFilename();
    if (ReadShortcut(getShell(), fn)) {
      FXFileDialog::setFilename(fn);
    }
    return FXFileDialog::getFilename();
  } else {
    return filenames ? (*filenames) : FXFileDialog::getFilename();
  }
}



FXuint FileDlg::execute(FXuint placement) {
  DeleteFilenames();
  FXuint rv=FXFileDialog::execute(placement);
  if (rv) {
    filenames = FXFileDialog::getFilenames();
    own_filenames=true;
    if (filenames) {
      if (getSelectMode()!=SELECTFILE_MULTIPLE) {
        filenames[0]=FXFileDialog::getFilename();
      }
      FixupShortcuts(getShell(), filenames);
      if (FXStat::isDirectory(filenames->text())) {
        setFilename("*");
        filenames->append("\\.");
        setDirectory(filenames->text());
        DeleteFilenames();
        return execute(placement);
      }
    }
  }
  return rv;
}
#endif

