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



/*
  When a ClipTextField object is destroyed, if it "owns" the clipoard,
  it tries to pass the clipboard data off to the object pointed to by
  its SetSaver() method. That window's class must implement its own
  SaveClipboard() method, and must also be referenced in the histbox.cpp
  file's ClipboardSaver macro.
*/
class ClipTextField: public FXTextField {
private:
  void SurrenderClipboard();
protected:
  ClipTextField(){}
public:
   ClipTextField(FXComposite *p,
     FXint ncols, FXObject *tgt=NULL, FXSelector sel=0, FXuint opts=TEXTFIELD_NORMAL, FXint x=0, FXint y=0,
     FXint w=0, FXint h=0, FXint pl=DEFAULT_PAD, FXint pr=DEFAULT_PAD, FXint pt=DEFAULT_PAD, FXint pb=DEFAULT_PAD
   ):FXTextField(p,ncols,tgt,sel,opts,x,y,w,h,pl,pr,pt,pb) {}
  ~ClipTextField();
  void destroy();
  static void SetSaver(FXWindow*w);
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
  FXTextField*input;
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

