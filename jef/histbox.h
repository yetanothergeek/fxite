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

#ifndef HISTBOX_H
#define HISTBOX_H

class MainWinWithClipBrd: public FXMainWindow {
  FXDECLARE(MainWinWithClipBrd)
protected:
  MainWinWithClipBrd() {}
  FXString cliptext;
public:
  MainWinWithClipBrd( FXApp* a,const FXString& name,FXIcon *ic=NULL,FXIcon *mi=NULL,
                      FXuint opts=DECOR_ALL,FXint x=0,FXint y=0,FXint w=0,FXint h=0,
                      FXint pl=0,FXint pr=0,FXint pt=0,FXint pb=0,FXint hs=0,FXint vs=0 ):
                      FXMainWindow(a,name,ic,mi,opts,x,y,w,h,pl,pr,pt,pb,hs,vs) {}
  void SaveClipboard();
  long onClipboardRequest(FXObject*o, FXSelector sel, void*p);
};



/*
  When a ClipTextField object is destroyed and it "owns" the clipoard, it searches
  up through the chain of "owner" windows trying to find a window that inherits
  from "MainWinWithClipBrd". If found, it tries to hand off the clipboard data  
  by calling MainWinWithClipBrd::SaveClipboard().
*/
class ClipTextField: public FXTextField {
private:
  FXDECLARE(ClipTextField);
  void SurrenderClipboard();
protected:
  ClipTextField(){}
public:
   long onRightBtnPress(FXObject*o, FXSelector sel, void*p);
   ClipTextField(FXComposite *p,
     FXint ncols, FXObject *tgt=NULL, FXSelector sel=0, FXuint opts=TEXTFIELD_NORMAL, FXint x=0, FXint y=0,
     FXint w=0, FXint h=0, FXint pl=DEFAULT_PAD, FXint pr=DEFAULT_PAD, FXint pt=DEFAULT_PAD, FXint pb=DEFAULT_PAD
   ):FXTextField(p,ncols,tgt,sel,opts,x,y,w,h,pl,pr,pt,pb) {}
  ~ClipTextField();
  void destroy();
};




/* A basic FXInputDialog with ClipTextField functionality. */
class ClipTextDialog: public FXInputDialog {
public:
  ClipTextDialog(FXWindow* p, const FXString& caption,const FXString& label);
};




/* The "memory engine" for the objects below */
class RegHistory;



/* FXTextField that "remembers" its previous 26 entries */
class HistoryTextField: public ClipTextField {
  FXDECLARE(HistoryTextField);
protected:
  HistoryTextField(){}
private:
  FXHorizontalFrame* hframe;
  FXArrowButton* arUP;
  FXArrowButton* arDN;
  FXVerticalFrame* searcharrows;
  RegHistory*hist;
  HistoryTextField*slave;
public:
  HistoryTextField(FXComposite *p, FXint ncols, const FXString &regname, const FXchar prefixes[2],
    FXObject *tgt=NULL, FXSelector sel=0, FXuint opts=TEXTFIELD_NORMAL,
      FXint x=0, FXint y=0, FXint w=0, FXint h=0, FXint
        pl=DEFAULT_PAD, FXint pr=DEFAULT_PAD, FXint pt=DEFAULT_PAD, FXint pb=DEFAULT_PAD);

  ~HistoryTextField();
  long onHistory(FXObject*o,FXSelector sel,void*p);
  long onKeyPress(FXObject*o,FXSelector sel,void*p);
  long onChanged(FXObject*o,FXSelector sel,void*p);
  void setWatch(FXint *watch);
  void setWatch(FXuint *watch);
  void enslave(HistoryTextField*enslaved);
  void show();
  void hide();
  void append();
  void start(); // reset the index back to zero
  enum {
    ID_HIST_UP=FXTextField::ID_LAST,
    ID_HIST_DN,
    ID_HIST_KEY,
    ID_LAST
  };
};



/* FXInputDialog that "remembers" its previous 26 entries */
class HistBox: public FXInputDialog {
FXDECLARE(HistBox);
protected:
  HistBox(){}
private:
public:
  HistBox(FXWindow* p, const FXString& caption, const FXString& label, const FXString &regname);
  virtual FXuint execute(FXuint placement=PLACEMENT_CURSOR);
};

#endif

