/*
  fxasq.cpp - "FoX: Ask Some Questions" is a class for creating query dialogs.
  Copyright (c) 2007-2014 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#include <fx.h>

#include "intl.h"
#include "fxasq.h"

#define LISTBOX_OPTS FRAME_SUNKEN|FRAME_THICK|LISTBOX_NORMAL|LAYOUT_FILL_X

#define IsThis(o,t) ( dynamic_cast<t*>(o)!=NULL )

static bool IsList(FXObject*o)  { return IsThis(o,FXListBox); }
static bool IsRadio(FXObject*o) { return IsThis(o,FXRadioButton); }
static bool IsGroup(FXObject*o) { return IsThis(o,FXGroupBox); }
static bool IsCheck(FXObject*o) { return IsThis(o,FXCheckButton); }
static bool IsTextField(FXObject*o) { return IsThis(o,FXTextField); }
static bool IsTextArea(FXObject*o)  { return IsThis(o,FXText); }


class MyDialog: public FXDialogBox {
  FXDECLARE(MyDialog)
  MyDialog(){}
public:
  MyDialog(FXApp*a, const char*caption):FXDialogBox(a,caption,DECOR_TITLE|DECOR_BORDER){}
  MyDialog(FXWindow*w, const char*caption):FXDialogBox(w,caption,DECOR_TITLE|DECOR_BORDER){}
  long OnButton(FXObject*o, FXSelector sel, void*p) {
    getApp()->stopModal(this,FXSELID(sel)-ID_BTN);
    destroy();
    getApp()->runWhileEvents();
    return 1;
  }
  enum {
    ID_BTN=ID_LAST
  };
};


FXDEFMAP(MyDialog) MyDialogMap[]={
  FXMAPFUNCS(SEL_COMMAND,MyDialog::ID_BTN,MyDialog::ID_BTN+32, MyDialog::OnButton)
};

FXIMPLEMENT(MyDialog,FXDialogBox,MyDialogMap,ARRAYNUMBER(MyDialogMap))


FxAsqWin::FxAsqWin(const char*title, const char **buttons, FXint focus_btn)
{
  FXApp*a=FXApp::instance();
  FXWindow*w=NULL;
  focused_btn=focus_btn;
  if (!a) {
    a=new FXApp();
    int argc=1;
    const char*argv[]={""};
    a->init(argc,(char**)argv);
  } else {
    w=a->getActiveWindow();
  }
  if (w) {
    dlg=new MyDialog(w,title);
  } else {
    dlg=new MyDialog(a,title);
  }
  a->create();
  SetPad(dlg,0);
  btnbox=new FXHorizontalFrame(dlg,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|FRAME_RIDGE|FRAME_THICK);
  if (buttons) {
    FXint i=1;
    for (const char **txt=buttons; *txt; txt++,i++) {
      new FXButton(btnbox,*txt, NULL, dlg,MyDialog::ID_LAST+i,BUTTON_NORMAL|LAYOUT_CENTER_X);
    }
  }
  userbox=new FXVerticalFrame(dlg,LAYOUT_FILL|FRAME_RIDGE|FRAME_THICK);
}



class GroupBox: public FXGroupBox {
  FXDECLARE(GroupBox);
  GroupBox(){}
public:
  GroupBox(FXComposite*o, const char*caption):FXGroupBox(o,caption,FRAME_GROOVE|GROUPBOX_NORMAL) {}
  long onRadio(FXObject*o,FXSelector sel,void*p) {
    for (FXWindow*w=getFirst(); w; w=w->getNext()) {
      if (IsRadio(w)) { ((FXRadioButton*)w)->setCheck(w==o); }
    }
    return 1;
  }
  enum {
    ID_RADIO=FXGroupBox::ID_LAST
  };
};



FXDEFMAP(GroupBox) GroupBoxMap[] = {
  FXMAPFUNC(SEL_COMMAND,GroupBox::ID_RADIO,GroupBox::onRadio)
};

FXIMPLEMENT(GroupBox,FXGroupBox,GroupBoxMap,ARRAYNUMBER(GroupBoxMap));






int FxAsqWin::Run(FxAsqItem**results)
{
  Results=NULL;
  dlg->create();
  dlg->setWidth(dlg->getDefaultWidth());
  dlg->setHeight(dlg->getDefaultHeight());
  if (UsedSlotsInDict(&keylist)>0) for (FXint i=0; i<TotalSlotsInDict(&keylist); ++i)
  {
    focused_btn=-1; // Focusing a button doesn't make much sense for interactive dialogs.
    FXWindow*obj=(FXWindow*)(keylist.data(i));
    if (IsList(obj)) {
      FXListBox* list=(FXListBox*)obj;
      FXint n=list->getNumItems();
      list->setNumVisible(n<8?n:8);
      const char*dval=(const char*)list->getUserData();
      if (dval) {
        for (FXint j=0; j<n; j++) {
          const char*val=(const char*)list->getItemData(j);
          if ( val && (strcmp(val,dval)==0) ) {
            list->setCurrentItem(j);
            break;
          }
        }
      }
    } else if (IsGroup(obj)) {
      GroupBox* grp=(GroupBox*)obj;
      const char*dval=(const char*)grp->getUserData();
      if (dval) {
        for (FXWindow*w=grp->getFirst(); w; w=w->getNext()) {
          if (IsRadio(w)) {
            const char*val=(const char*)w->getUserData();
            ((FXRadioButton*)w)->setCheck( val && (strcmp(val,dval)==0) );
          }
        }
      }
    }
  }
  if (focused_btn>=0) {
    FXint i=0;
    for (FXWindow*btn=btnbox->getFirst(); btn; btn=btn->getNext()) {
      if (i==focused_btn) {
        btn->setFocus();
        break;
      }
      i++;
    }
  }
  FXint rv=dlg->execute(PLACEMENT_SCREEN);
  if (results) {
    PutResults();
    *results=Results;
  }
  return rv-1;
}



void FxAsqWin::PutResults()
{
  for (FXint i=0; i<TotalSlotsInDict(&keylist); ++i) {
    const char*k=DictKeyName(keylist,i);
    FXWindow*obj=(FXWindow*)keylist.data(i);
    if (k&&obj) {
      if (IsGroup(obj)) {
         GroupBox*grp=(GroupBox*)obj;
         for (FXWindow* w=grp->getFirst(); w; w=w->getNext()) {
           if (IsRadio(w)) {
             if ( ((FXRadioButton*)w)->getCheck() ) {
               AppendResult(k, (const char*)(w->getUserData()));
               break;
             }
           }
         }
      } else if (IsList(obj)) {
          FXListBox* list=(FXListBox*) obj;
          AppendResult(k, (const char*)list->getItemData(list->getCurrentItem()));
      } else if (IsCheck(obj)) {
        AppendResult(k, ((FXCheckButton*)obj)->getCheck()?"1":"0");
      } else if (IsTextField(obj)) {
        AppendResult(k, ((FXTextField*)obj)->getText().text());
      } else if (IsTextArea(obj)) {
        AppendResult(k, ((FXText*)obj)->getText().text());
      }
    }
  }
}



void FxAsqWin::AppendResult(const char*key, const char*value)
{
  if (key) {
    FxAsqItem*fi=(FxAsqItem*)calloc(1,sizeof(FxAsqItem));
    fi->key=strdup(key);
    fi->value=strdup(value?value:"");
    if (Results) {
      FxAsqItem*p=Results;
      while (p->next) { p=p->next; }
      p->next=fi;
    } else {
      Results=fi;
    }
  }
}



void FxAsqWin::TextBox(const char*key, const char*value, const char*label, bool secret)
{
  FXHorizontalFrame*frm=new FXHorizontalFrame(userbox,LAYOUT_FILL_X);
  if (label) { new FXLabel(frm, label); }
  FXTextField *txt=new FXTextField(frm,32,NULL,0,LAYOUT_FILL_X|TEXTFIELD_NORMAL|(secret?TEXTFIELD_PASSWD:0));
  if (value) { txt->setText(value); }
  keylist.insert(key,txt);
}



void FxAsqWin::Text(const char*key, const char*value, const char*label)
{
  TextBox(key,value,label,false);
}



void FxAsqWin::Password(const char*key, const char*value, const char*label)
{
  TextBox(key,value,label,true);
}


void FxAsqWin::HR()
{
  new FXHorizontalSeparator(userbox);
}



void FxAsqWin::Label(const char*text, int justify)
{
  new FXLabel(userbox,text,NULL,LABEL_NORMAL|justify);
}



void FxAsqWin::TextArea(const char*key, const char*value, const char*label)
{
  FXVerticalFrame*frm=new FXVerticalFrame(userbox, LAYOUT_FILL);
  if (label) { new FXLabel(frm, label); }
  frm=new FXVerticalFrame(frm, FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL);
  SetPad(frm,0);
  FXText*txt=new FXText(frm,
    NULL, 0, LAYOUT_FIX_HEIGHT|LAYOUT_FILL_X|TEXT_WORDWRAP ,0,0,240,120);
  if (value) { txt->setText(value); }
  keylist.insert(key,txt);
}



void FxAsqWin::CheckBox(const char*key, const bool value, const char*label)
{
  FXCheckButton *chk = new FXCheckButton(userbox,label);
  chk->setCheck(value);
  keylist.insert(key,chk);
}



void FxAsqWin::Select(const char*key, const char*value, const char*label)
{
  FXHorizontalFrame *frm = new FXHorizontalFrame(userbox,LAYOUT_FILL_X);
  new FXLabel(frm,label,NULL);
  FXListBox*list=(FXListBox*)(LookupInDict(&keylist,key));
  if (list) {
    list->reparent(frm);
    void*p=list->getUserData();
    if (p) { free(p); }
  } else {
    list=new FXListBox(frm,NULL,0,LISTBOX_OPTS);
    keylist.insert(key,list);
  }
  list->setUserData(value?strdup(value):NULL);
}



void FxAsqWin::Option(const char*key, const char*value, const char*label)
{
  FXListBox*list=(FXListBox*)(LookupInDict(&keylist,key));
  if (!IsList(list)) {
    list=new FXListBox(userbox,NULL,0,LISTBOX_OPTS);
    keylist.insert(key,list);
  }
  list->appendItem(label,NULL,value?strdup(value):NULL);
}



void FxAsqWin::Group(const char*key, const char*value, const char*label)
{
  GroupBox*grp=(GroupBox*)(LookupInDict(&keylist,key));
  if (grp) {
    ((FXWindow*)grp)->reparent(userbox);
    grp->setText(label);
  } else {
    grp=new GroupBox(userbox,label);
    keylist.insert(key,grp);
  }
  grp->setUserData(value?strdup(value):NULL);
}


void FxAsqWin::Radio(const char*key, const char*value, const char*label)
{
  GroupBox*grp=(GroupBox*)(LookupInDict(&keylist,key));
  if (!IsGroup(grp)) {
    grp=new GroupBox(userbox,NULL);
    keylist.insert(key,grp);
  }
  FXRadioButton*rb= new FXRadioButton(grp,label,grp,GroupBox::ID_LAST);
  rb->setUserData(value?strdup(value):NULL);
}



class ActionBox: public FXHorizontalFrame {
  FXDECLARE(ActionBox);
  ActionBox(){}
  FXTextField*txt;
public:
  ActionBox(FxAsqWin*o, const char*key, const char*value, const char*label, FXSelector action);
  long onButton(FXObject*o,FXSelector sel,void*p);
  enum {
    ID_FILE=FXHorizontalFrame::ID_LAST,
    ID_COLOR,
    ID_FONT
  };
};



ActionBox::ActionBox(FxAsqWin*o, const char*key, const char*value, const char*label, FXSelector action)
  :FXHorizontalFrame(o->userbox,LAYOUT_FILL_X)
{
  new FXLabel(this,label);
  txt=new FXTextField(this,24,NULL,0,TEXTFIELD_NORMAL|LAYOUT_FILL_X);
  txt->setText(value);
  new FXButton(this,"...",NULL,this,action);
  o->keylist.insert(key,txt);
}



#ifdef FOX_1_6
  static long colorFromName(const char* rgb)
  {
    int r=0, g=0, b=0;
    sscanf(rgb+1,"%2x%2x%2x",&r,&g,&b);
    return FXRGB(r, g, b);
  }
# define SetDlgFnt(dlg,dsc) dlg.setFontSelection(dsc)
# define freeElms(e) FXFREE(&e)
#else
# define SetDlgFnt(dlg,dsc) dlg.setFontDesc(dsc)
#endif



long ActionBox::onButton(FXObject*o,FXSelector sel,void*p)
{
  switch (FXSELID(sel)) {
    case ID_FILE:{
      FXFileDialog dlg(getShell(),_("Select file"));
      dlg.setFilename(txt->getText());
      if (dlg.execute(PLACEMENT_SCREEN)) {
        txt->setText(dlg.getFilename());
      }
      break;
    }
    case ID_COLOR:{
      FXColorDialog dlg(getShell(),_("Pick a color"));
      dlg.setOpaqueOnly(true);
      dlg.setRGBA(colorFromName(txt->getText().text()));
      if (dlg.execute(PLACEMENT_SCREEN)) {
        FXColor rgb=dlg.getRGBA();
        char clr[8];
        snprintf(clr, 8, "#%02x%02x%02x", FXREDVAL(rgb), FXGREENVAL(rgb), FXBLUEVAL(rgb));
        txt->setText(clr);
      }
      break;
    }
    case ID_FONT:{
      FXFontDialog dlg(getShell(), _("Choose font"));
      FXString name=txt->getText().trim();
      FXFont fnt(getApp(),name);
      FXFontDesc dsc;
#ifdef FOX_1_6
      fnt.getFontDesc(dsc);
      dlg.setFontSelection(dsc);
      dlg.getFontSelection(dsc);
#else
      dlg.setFontDesc(fnt.getFontDesc());
      dsc=dlg.getFontDesc();
#endif
      if (strncmp(name.text(),dsc.face,strlen(dsc.face))!=0) {
        FXFontDesc*fonts=NULL;
        FXuint numfonts=0;
        if (FXFont::listFonts(fonts,numfonts,name)) {
          if (numfonts>0) {
            SetDlgFnt(dlg,*fonts);
          }
          freeElms(fonts);
        } else if (FXFont::listFonts(fonts,numfonts,FXString::null)) {
          name.lower();
          if (numfonts>0) {
            for (FXuint i=0; i<numfonts; i++) {
              FXString test=fonts[i].face;
              if (test.lower().contains(name)) {
                SetDlgFnt(dlg,fonts[i]);
                break;
              }
            }
          }
          freeElms(fonts);
        }
      }
      if (dlg.execute(PLACEMENT_SCREEN)) {

#ifdef FOX_1_6
        dlg.getFontSelection(dsc);
        fnt.setFontDesc(dsc);
#else
        fnt.setFontDesc(dlg.getFontDesc());
#endif
        txt->setText(fnt.getFont());
      }
      break;
    }
  }
  return 1;
}




FXDEFMAP(ActionBox) ActionBoxMap[] = {
  FXMAPFUNCS(SEL_COMMAND,ActionBox::ID_FILE,ActionBox::ID_FONT,ActionBox::onButton)
};

FXIMPLEMENT(ActionBox,FXHorizontalFrame,ActionBoxMap,ARRAYNUMBER(ActionBoxMap));




void FxAsqWin::File(const char*key, const char*value, const char*label)
{
  new ActionBox(this,key,value,label,ActionBox::ID_FILE);
}



void FxAsqWin::Color(const char*key, const char*value, const char*label)
{
  new ActionBox(this,key,value,label,ActionBox::ID_COLOR);
}



void FxAsqWin::Font(const char*key, const char*value, const char*label)
{
  new ActionBox(this,key,value,label,ActionBox::ID_FONT);
}



FxAsqWin::~FxAsqWin()
{
  void*p=NULL;
  for (FXint i=0; i<TotalSlotsInDict(&keylist); ++i) {
    FXWindow*obj=(FXWindow*)(keylist.data(i));
    if (IsList(obj)) {
      FXListBox*listbox=(FXListBox*)obj;
      p=listbox->getUserData();
      if (p) { free(p); }
      FXint n=listbox->getNumItems();
      for (FXint j=0; j<n; j++) {
        p=listbox->getItemData(j);
        if (p) { free(p); }
      }
    } else if (IsGroup(obj)) {
      p=((GroupBox*)obj)->getUserData();
      if (p) {
        free(p);
      }
      for (FXWindow*w=obj->getFirst(); w; w=w->getNext()) {
        if (IsRadio(w)) {
          p=w->getUserData();
          if (p) { free(p); }
        }
      }
    }
  }
  keylist.clear();
  delete dlg;
}

