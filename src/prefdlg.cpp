/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2009-2014 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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

#include <cctype>

#include <fxkeys.h>
#include <fx.h>

#include "compat.h"
#include "prefs.h"
#include "appname.h"
#include "appwin_pub.h"
#include "search.h"
#include "histbox.h"
#include "menuspec.h"
#include "desclistdlg.h"
#include "prefdlg_ext.h"
#include "prefdlg_sntx.h"
#include "cfg_shortcut.h"
#include "cfg_keybind.h"
#include "cfg_popmenu.h"
#include "cfg_toolbar.h"
#include "cfg_theme.h"
#include "shady_tabs.h"

#include "intl.h"
#include "prefdlg.h"


#define PACK_UNIFORM   ( PACK_UNIFORM_WIDTH | PACK_UNIFORM_HEIGHT )
#define LAYOUT_CENTER  ( LAYOUT_CENTER_Y | LAYOUT_CENTER_X )
#define SPIN_OPTS      ( SPIN_NORMAL | FRAME_SUNKEN | FRAME_THICK )
#define LIST_BOX_OPTS  ( FRAME_SUNKEN | FRAME_THICK | LISTBOX_NORMAL | LAYOUT_FILL_X )

static const char*color_hint = _("Drag a color square to FG/BG, or double-click FG/BG to edit");

static const char* hint_list[] = {
  _("General application settings"),
  _("Editor specific settings"),
  color_hint,
  _("Configure keyboard shortcuts"),
  _("Edit toolbar buttons"),
  _("Edit popup menu items"),
  _("User interface colors and font"),
  NULL
};

static FXival whichtab=0;


static const char* sntx_hint_list[] = {
  color_hint,
  _("Language-specific options"),
  NULL
};

static FXival whichsyntab=0;


FXDEFMAP(PrefsDialog) PrefsDialogMap[]={
  FXMAPFUNC(SEL_COMMAND,PrefsDialog::ID_TAB_SWITCHED,PrefsDialog::onTabSwitch),
  FXMAPFUNC(SEL_COMMAND, PrefsDialog::ID_FILTERS_EDIT,PrefsDialog::onFiltersEdit),
  FXMAPFUNC(SEL_COMMAND, PrefsDialog::ID_ERRPATS_EDIT,PrefsDialog::onErrPatsEdit),
  FXMAPFUNC(SEL_COMMAND, PrefsDialog::ID_SYSINCS_EDIT,PrefsDialog::onSysIncsEdit),
  FXMAPFUNC(SEL_COMMAND, PrefsDialog::ID_CHOOSE_FONT,PrefsDialog::onChooseFont),
};

FXIMPLEMENT(PrefsDialog,FXDialogBox,PrefsDialogMap,ARRAYNUMBER(PrefsDialogMap));



void PrefsDialog::MakeToolbarTab()
{
  new FXTabItem(tabs,_("toolbar"));
  new ToolbarPrefs(tabs, TopWinPub::Toolbar(), TopWinPub::UserMenus(), mnumgr);
}



void PrefsDialog::MakePopupTab()
{
  new FXTabItem(tabs,_("popup"));
  new PopupPrefs(tabs, TopWinPub::UserMenus(), mnumgr);
}



class MyShortcutList: public ShortcutList {
protected:
  virtual bool ConfirmOverwrite(FXHotKey acckey, MenuSpec*spec) { 
    MenuSpec*killcmd=mnumgr->LookupMenu(TopWinPub::KillCmdID());
    if ( (acckey==parseAccel(killcmd->accel)) && (spec!=killcmd)) {
      FXMessageBox::error(getShell(), MBOX_OK, _("Conflicting keybinding"),
        _("Keybinding for \"%s\" must not conflict with \"%s\""),spec->pref,killcmd->pref);
      return false;
    }
    return ShortcutList::ConfirmOverwrite(acckey,spec);
  }
  virtual bool DeleteShortcut(MenuSpec*spec) {
        if (spec->sel==(FXint)TopWinPub::KillCmdID()) {
     FXMessageBox::error(getShell(), MBOX_OK, _("Empty keybinding"), "%s \"%s\"",
        _("You cannot remove the keybinding for"), spec->pref);
     return false;
    }
    return ShortcutList::DeleteShortcut(spec);
  }
  virtual void ApplyShortcut(FXHotKey acckey, MenuSpec*spec){
    if (spec->sel==(FXint)TopWinPub::KillCmdID()) {
      TopWinPub::SetKillCommandAccelKey(acckey);
    } else {
      ShortcutList::ApplyShortcut(acckey,spec);
    }
  }
public:
  MyShortcutList(FXComposite*o, MenuMgr*mmgr, FXWindow*w, FXuint opts=0):ShortcutList(o,mmgr,w,opts){}
};



void PrefsDialog::MakeKeybindingsTab()
{
  new FXTabItem(tabs,_("keys"));
  new MyShortcutList(tabs,mnumgr,main_win,FRAME_RAISED|LAYOUT_FILL);
}



void PrefsDialog::MakeSyntaxTab()
{
  new FXTabItem(tabs,_("syntax"));
  langs=new LangGUI(tabs,prefs,this,ID_TAB_SWITCHED);

}



long PrefsDialog::onFiltersEdit(FXObject*o,FXSelector sel,void*p)
{
  FileFiltersDlg(this).execute();
#ifdef WIN32
  hide();
  show();
  ((FXWindow*)o)->setFocus();
#endif
  return 1;
}



long PrefsDialog::onErrPatsEdit(FXObject*o,FXSelector sel,void*p)
{
  ErrPatDlg(this).execute();
#ifdef WIN32
  hide();
  show();
  ((FXWindow*)o)->setFocus();
#endif
  return 1;
}



long PrefsDialog::onSysIncsEdit(FXObject*o,FXSelector sel,void*p)
{
  SysIncDlg(this).execute();
#ifdef WIN32
  hide();
  show();
  ((FXWindow*)o)->setFocus();
#endif
  return 1;
}



void PrefsDialog::MakeGeneralTab()
{
  new FXTabItem(tabs,_("general"));
  FXHorizontalFrame* hframe;
  FXVerticalFrame* vframe;
  FXVerticalFrame* left_column;
  FXVerticalFrame* right_column;
  FXCheckButton*chk;
  ClipTextField *tf;
  FXSpinner *spin;
  vframe=new FXVerticalFrame(tabs,FRAME_RAISED|LAYOUT_FILL,0,0,0,0,0,0,0,0,0,0);
  hframe=new FXHorizontalFrame(vframe,FRAME_NONE|LAYOUT_FILL|PACK_UNIFORM_WIDTH);
  left_column=new FXVerticalFrame(hframe,FRAME_SUNKEN|LAYOUT_FILL/*|PACK_UNIFORM_HEIGHT*/);
  right_column=new FXVerticalFrame(hframe,FRAME_SUNKEN|LAYOUT_FILL/*|PACK_UNIFORM_HEIGHT*/);

  chk=new FXCheckButton(left_column, _("Watch files for external changes."), prefs, Settings::ID_TOGGLE_WATCH_EXTERN);
  chk->setCheck(prefs->WatchExternChanges, false);
  chk->setPadBottom(8);

  hframe=new FXHorizontalFrame(left_column);
  SetPad(hframe,0);
  chk=new FXCheckButton(hframe, _("Backup files every "), prefs, Settings::ID_TOGGLE_AUTOSAVE);
  chk->setCheck(prefs->Autosave, false);
  chk->setPadBottom(12);
  spin=new FXSpinner(hframe, 3, prefs, Settings::ID_SET_AUTOSAVE_INT,SPIN_OPTS);
  spin->setRange(15,900);
  spin->setValue(prefs->AutosaveInterval);

  new FXLabel(hframe, _(" seconds."));
  if (!chk->getCheck()) { spin->disable(); }
  chk->setUserData(spin);

  new FXLabel(left_column,  _("Ask before closing multiple files:"));
  chk=new FXCheckButton(left_column, _("From menu"), prefs, Settings::ID_TOGGLE_ASK_CLOSE_MULTI_MENU);
  chk->setCheck(prefs->PromptCloseMultiMenu, false);
  chk->setPadLeft(12);
  chk=new FXCheckButton(left_column, _("On exit"), prefs, Settings::ID_TOGGLE_ASK_CLOSE_MULTI_EXIT);
  chk->setCheck(prefs->PromptCloseMultiExit, false);
  chk->setPadLeft(12);
  chk->setPadBottom(12);

  hframe=new FXHorizontalFrame(left_column);
  hframe->setPadBottom(8);
  tf=new ClipTextField(hframe, 3, prefs, Settings::ID_SET_MAX_FILES,TEXTFIELD_LIMITED|TEXTFIELD_INTEGER|TEXTFIELD_NORMAL);
  char maxfiles[8]="\0\0\0\0\0\0\0";
  snprintf(maxfiles, sizeof(maxfiles)-1, "%d", prefs->MaxFiles);
  tf->setText(maxfiles);
  new FXLabel(hframe, _("Maximum number of open files."));

  (new FXHorizontalSeparator(left_column,LAYOUT_SIDE_TOP|LAYOUT_FILL_X|SEPARATOR_GROOVE))->setPadBottom(12);
  hframe=new FXHorizontalFrame(left_column);
  hframe->setPadLeft(12);
  new FXButton(hframe, _(" File dialog filters... "), NULL, this, ID_FILTERS_EDIT);

  hframe=new FXHorizontalFrame(left_column, LAYOUT_FILL_X);
  (new FXLabel(hframe, _("Remember\nlast-used filter :")))->setJustify(JUSTIFY_LEFT);
  FXListBox* list=new FXListBox(hframe,prefs,Settings::ID_SET_KEEP_FILE_FILTER,LIST_BOX_OPTS|LAYOUT_BOTTOM);
  list->appendItem(_("never"),NULL,NULL);
  list->appendItem(_("during session"),NULL,NULL);
  list->appendItem(_("across sessions"),NULL,NULL);
  list->setNumVisible(list->getNumItems());
  list->setCurrentItem(prefs->KeepFileFilter);

  new FXLabel(right_column,_("Save open files before executing:"));
  chk=new FXCheckButton(right_column, _("Tools->Filter Selection"), prefs, Settings::ID_SAVE_ON_FILTER_SEL);
  chk->setCheck(prefs->SaveBeforeFilterSel, false);
  chk->setPadLeft(12);

  chk=new FXCheckButton(right_column, _("Tools->Insert Command"), prefs, Settings::ID_SAVE_ON_INS_CMD);
  chk->setCheck(prefs->SaveBeforeInsCmd, false);
  chk->setPadLeft(12);

  chk=new FXCheckButton(right_column, _("Tools->Execute Command"), prefs, Settings::ID_SAVE_ON_EXEC_CMD);
  chk->setCheck(prefs->SaveBeforeExecCmd, false);
  chk->setPadLeft(12);

#ifndef WIN32
  hframe=new FXHorizontalFrame(right_column);
  hframe->setPadTop(16);
  new FXLabel(hframe, _("Shell command: "));
  tf=new ClipTextField(hframe,18,prefs,Settings::ID_SET_SHELL_CMD,TEXTFIELD_NORMAL);
  tf->setText(prefs->ShellCommand);
#endif

  new FXHorizontalSeparator(right_column,LAYOUT_SIDE_TOP|LAYOUT_FILL_X|SEPARATOR_GROOVE);

  hframe=new FXHorizontalFrame(right_column);
  hframe->setPadTop(12);
  spin=new FXSpinner(hframe, 2, prefs, Settings::ID_SET_WHEEL_LINES,SPIN_OPTS);
  spin->setRange(1,32);
  spin->setValue(prefs->WheelLines);
  new FXLabel(hframe, _("Mouse wheel acceleration."));

  hframe=new FXHorizontalFrame(right_column);
  hframe->setPadTop(12);
  spin=new FXSpinner(hframe, 4, prefs, Settings::ID_SET_TAB_TITLE_MAX_WIDTH,SPIN_OPTS);
  spin->setRange(0,prefs->ScreenWidth);
  spin->setValue(prefs->TabTitleMaxWidth);
  new FXLabel(hframe, _("Max width for tab titles.\n( in pixels;  0 = no limit. )"));

  right_column=new FXVerticalFrame(right_column,FRAME_NONE|PACK_UNIFORM_WIDTH|LAYOUT_CENTER_X|LAYOUT_BOTTOM);
  right_column->setVSpacing(8);
  new FXButton(right_column, _("System header locations... "),NULL,this,ID_SYSINCS_EDIT,BUTTON_NORMAL|LAYOUT_CENTER_X);
  new FXButton(right_column, _("Output pane line matching... "),NULL,this,ID_ERRPATS_EDIT,BUTTON_NORMAL|LAYOUT_CENTER_X);
}



long PrefsDialog::onChooseFont(FXObject*o,FXSelector sel,void*p)
{
  prefs->handle(this, FXSEL(SEL_COMMAND, Settings::ID_CHOOSE_FONT), NULL);
  langs->handle(langs,FXSEL(SEL_COMMAND,LangGUI::ID_LANG_SWITCH),(void*)((FXival)(-1)));
  return 1;
}



void PrefsDialog::MakeEditorTab()
{
  new FXTabItem(tabs,_("editor"));
  FXHorizontalFrame *frame;
  FXVerticalFrame *column;
  FXHorizontalFrame *spinframe;
  FXCheckButton*chk;
  FXSpinner *spin;
  FXListBox*list;

  frame=new FXHorizontalFrame(tabs,FRAME_RAISED|LAYOUT_FILL);
  column=new FXVerticalFrame(frame,FRAME_SUNKEN|LAYOUT_FILL);

  spinframe=new FXHorizontalFrame(column);
  new FXLabel(spinframe, _("Auto indent:"));
  list=new FXListBox(spinframe,prefs,Settings::ID_SET_AUTO_INDENT,LIST_BOX_OPTS);
  list->appendItem(_("off"),NULL,NULL);
  list->appendItem(_("basic"),NULL,NULL);
  list->appendItem(_("smart"),NULL,NULL);
  list->setNumVisible(list->getNumItems());
  list->setCurrentItem(prefs->AutoIndent);

  chk=new FXCheckButton(column, _("Smart home key"), prefs, Settings::ID_TOGGLE_SMART_HOME);
  chk->setCheck(prefs->SmartHome, false);

  chk=new FXCheckButton(column, _("Smooth scrolling"), prefs, Settings::ID_TOGGLE_SMOOTH_SCROLL);
  chk->setCheck(prefs->SmoothScroll, false);

  chk=new FXCheckButton(column, _("Allow caret beyond end of line"), prefs, Settings::ID_TOGGLE_CARET_PAST_EOL);
  chk->setCheck(prefs->CaretPastEOL, false);

  chk=new FXCheckButton(column,_("Turn line wrapping on by default"),prefs,Settings::ID_TOGGLE_WORD_WRAP);
  chk->setCheck(prefs->WordWrap);

  chk=new FXCheckButton(column, _("Wrap-aware home/end keys"), prefs, Settings::ID_TOGGLE_WRAP_AWARE);
  chk->setCheck(prefs->WrapAwareHomeEnd, false);

  spinframe=new FXHorizontalFrame(column);
  spin=new FXSpinner(spinframe, 2, prefs, Settings::ID_SET_CARET_WIDTH, SPIN_OPTS);
  spin->setRange(1,3);
  spin->setValue(prefs->CaretWidth);
  new FXLabel(spinframe, _("Caret width"));


  spinframe=new FXHorizontalFrame(column);
  spin=new FXSpinner(spinframe, 4, prefs, Settings::ID_SET_RIGHT_EDGE, SPIN_OPTS);
  spin->setRange(1,1024);
  spin->setValue(prefs->RightEdgeColumn);
  new FXLabel(spinframe, _("Right Margin Indicator"));

  spinframe=new FXHorizontalFrame(column);
  new FXLabel(spinframe, _("Split views:"));
  list=new FXListBox(spinframe,prefs,Settings::ID_SET_SPLIT_VIEW,LIST_BOX_OPTS);
  list->appendItem(_("cycle"),NULL,NULL);
  list->appendItem(_("below"),NULL,NULL);
  list->appendItem(_("beside"),NULL,NULL);
  list->setNumVisible(list->getNumItems());
  list->setCurrentItem(prefs->SplitView);

  new FXLabel(column, " ");
  new FXButton(column, _(" Editor font... "), NULL, this, ID_CHOOSE_FONT, BUTTON_NORMAL|LAYOUT_CENTER_X);

  column=new FXVerticalFrame(frame,FRAME_SUNKEN|LAYOUT_FILL);

  spinframe=new FXHorizontalFrame(column);
  new FXLabel(spinframe, _("Highlight matching braces\n(when caret is...)"));
  list=new FXListBox(spinframe,prefs,Settings::ID_SET_BRACE_MATCHING,LIST_BOX_OPTS|LAYOUT_CENTER_Y);
  list->appendItem(_("never"),NULL,NULL);
  list->appendItem(_("inside"),NULL,NULL);
  list->appendItem(_("outside"),NULL,NULL);
  list->appendItem(_("on either side"),NULL,NULL);
  list->appendItem(_("after"),NULL,NULL);
  list->setNumVisible(list->getNumItems());
  list->setCurrentItem(prefs->BraceMatch);

  chk=new FXCheckButton(column, _("Use tabs for indent"), prefs, Settings::ID_TOGGLE_USE_TABS);

  spinframe=new FXHorizontalFrame(column);
  spin=new FXSpinner(spinframe, 2, prefs, Settings::ID_SET_TAB_WIDTH, SPIN_OPTS);
  spin->setRange(1,16);
  spin->setValue(prefs->TabWidth);
  (new FXLabel(spinframe, _("Tab width")))->setPadRight(16);

  spin=new FXSpinner(spinframe, 2, prefs, Settings::ID_SET_INDENT_WIDTH, SPIN_OPTS);
  spin->setRange(1,16);
  spin->setValue(prefs->IndentWidth);
  new FXLabel(spinframe, _("Indent width"));

  chk->setUserData((void*)spin);
  chk->setCheck(prefs->UseTabs, true);

  new FXHorizontalSeparator(column,LAYOUT_SIDE_TOP|LAYOUT_FILL_X|SEPARATOR_GROOVE);
  new FXLabel(column, _("Default search options:"));
  srchopts=new SciSearchOptions(column, prefs,Settings::ID_SET_SEARCH_OPTS);
  srchopts->SetSciFlags(prefs->SearchOptions);
  chk=new FXCheckButton(column, _("Verbose search messages"), prefs, Settings::ID_TOGGLE_SEARCH_VERBOSE);
  chk->setCheck(prefs->SearchVerbose, false);
  chk->setPadLeft(6);

  spinframe=new FXHorizontalFrame(column);

  new FXLabel(spinframe, _("Search wrap:"));
  list=new FXListBox(spinframe,prefs,Settings::ID_SET_SEARCH_WRAP,LIST_BOX_OPTS);
  list->appendItem(_("never"),NULL,NULL);
  list->appendItem(_("always"),NULL,NULL);
  list->appendItem(_("prompt"),NULL,NULL);
  list->setNumVisible(list->getNumItems());
  list->setCurrentItem(prefs->SearchWrap);

  new FXLabel(spinframe, _(" GUI:"));
  list=new FXListBox(spinframe,prefs,Settings::ID_SET_SEARCH_GUI,LIST_BOX_OPTS);
  list->appendItem(_("above"),NULL,NULL);
  list->appendItem(_("below"),NULL,NULL);
  list->appendItem(_("float"),NULL,NULL);
  list->setNumVisible(list->getNumItems());
  list->setCurrentItem(prefs->SearchGui);

  new FXHorizontalSeparator(column,LAYOUT_SIDE_TOP|LAYOUT_FILL_X|SEPARATOR_GROOVE);
  chk=new FXCheckButton(column, _("Open 7-bit ASCII files as UTF-8"), prefs, Settings::ID_TOGGLE_ASCII_DEFAULT);
  chk->setCheck(!prefs->DefaultToAscii, false);
  chk->setPadBottom(4);
  chk=new FXCheckButton(column, _("Treat other files as broken UTF-8"), prefs, Settings::ID_TOGGLE_SBCS_DEFAULT);
  chk->setCheck(!prefs->DefaultToSbcs, false);
  chk->setPadBottom(8);
  spinframe=new FXHorizontalFrame(column);
  new FXLabel(spinframe, _("Default file format:"));
  list=new FXListBox(spinframe,prefs,Settings::ID_SET_FILE_FORMAT,LIST_BOX_OPTS);
  list->appendItem(_("DOS  [CRLF]"),NULL,NULL);
  list->appendItem(_("Mac  [CR]"),NULL,NULL);
  list->appendItem(_("Unix [LF]"),NULL,NULL);
  list->setNumVisible(list->getNumItems());
  list->setCurrentItem(prefs->DefaultFileFormat);

  chk=new FXCheckButton(column, _("Show line endings with \"View Whitespace\""),
                          prefs, Settings::ID_TOGGLE_VIEW_WHITESPACE_EOL);
  chk->setCheck(prefs->WhitespaceShowsEOL, false);
  chk->setPadLeft(6);
}



void PrefsDialog::MakeThemeTab()
{
  new FXTabItem(tabs,_("theme"));
  new ThemeGUI(tabs,main_win,TopWinPub::ThemeID());
}



long PrefsDialog::onTabSwitch(FXObject*o,FXSelector sel,void*p)
{

  if (o==tabs) {
    whichtab=(FXival)p;
    hint_lab->setText(hint_list[whichtab]);
  } else {
    whichsyntab=(FXival)p;
    hint_lab->setText(sntx_hint_list[whichsyntab]);
  }
  return 0;
}



void PrefsDialog::create()
{
  FXDialogBox::create();
  SetupXAtoms(this, "prefs", APP_NAME);
  show(PLACEMENT_DEFAULT);
  
  FXRootWindow*root=getApp()->getRootWindow();
  setX(main_win->getX()+16+getWidth()<root->getWidth()?main_win->getX()+16:root->getWidth()-getWidth()-32);
  setY(main_win->getY()+24+getHeight()<root->getHeight()?main_win->getY()+24:root->getHeight()-getDefaultHeight()-48);

}



PrefsDialog::PrefsDialog(FXMainWindow* w, Settings* aprefs, MenuMgr* mmgr):FXDialogBox(w->getApp(), "Preferences")
{
  mnumgr=mmgr;
  prefs=aprefs;
  main_win=w;
  setWidth(620);
  SetPad(this,2);

  FXHorizontalFrame* buttons=new FXHorizontalFrame(this,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X);
  new FXButton(buttons,_("  &Close  "), NULL,this,ID_ACCEPT,FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_Y);

  hint_lab=new FXLabel(buttons, hint_list[0], NULL, LAYOUT_FILL_X);

  new FXHorizontalSeparator(this,SEPARATOR_GROOVE|LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X);
  tabs=new ShadyTabs(this, this,ID_TAB_SWITCHED,FRAME_NONE|PACK_UNIFORM|LAYOUT_FILL);
  MakeGeneralTab();
  MakeEditorTab();
  MakeSyntaxTab();
  MakeKeybindingsTab();
  MakeToolbarTab();
  MakePopupTab();
  MakeThemeTab();
  tabs->setCurrent(whichtab,true);
  tabs->childAtIndex(whichtab*2)->setFocus();
  setIcon(w->getIcon());
}

