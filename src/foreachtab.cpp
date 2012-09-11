/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2009-2012 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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
#include "doctabs.h"
#include "scidoc.h"
#include "backup.h"
#include "prefs.h"
#include "appwin.h"

#include "foreachtab.h"


// Check each document to see if any of them should be auto-saved.
bool TabCallbacks::AutoSaveCB(FXint index, DocTab*tab, FXWindow*page, void*user_data)
{
  SciDoc*sci=(SciDoc*)page->getFirst();
  if (sci->NeedBackup()) {
    ((BackupMgr*)user_data)->SaveBackup(sci);
  }
  return true;
}


/* Zoom each document In/Out/Close/Far/Default */
bool TabCallbacks::ZoomStepCB(FXint index, DocTab*tab, FXWindow*page, void*user_data)
{
  SciDoc*sci=(SciDoc*)page->getFirst();
  sci->ZoomStep(*((int*)user_data));
  return true;
}



/* Zoom each document to a specified amount (Used by Lua) */
bool TabCallbacks::ZoomSpecCB(FXint index, DocTab*tab, FXWindow*page, void*user_data)
{
  SciDoc*sci=(SciDoc*)page->getFirst();
  sci->SetZoom(*((int*)user_data));
  return true;
}



void TabCallbacks::SetZoom(SciDoc*sci, FXSelector sel, void*p, DocTabs*tabbook, Settings*prefs)
{
  FXival z;
  if (sel) {
    switch (FXSELID(sel)) {
      case TopWindow::ID_ZOOM_IN:   { z =  1;  break; }
      case TopWindow::ID_ZOOM_OUT:  { z = -1; break; }
      case TopWindow::ID_ZOOM_FAR:  { z = -2; break; }
      case TopWindow::ID_ZOOM_NEAR: { z =  2;  break; }
      case TopWindow::ID_ZOOM_NONE: { z =  0;  break; }
    }
    tabbook->ForEachTab(ZoomStepCB, &z);
  } else {
    z=(FXival)p;
   tabbook->ForEachTab(ZoomSpecCB, &z);
  }
  prefs->ZoomFactor=sci->GetZoom();  
}



bool TabCallbacks::LineNumsCB(FXint index, DocTab*tab, FXWindow*page, void*user_data)
{
  SciDoc*sci=(SciDoc*)page->getFirst();
  sci->ShowLineNumbers((bool)user_data);
  return true;
}



bool TabCallbacks::WhiteSpaceCB(FXint index, DocTab*tab, FXWindow*page, void*user_data)
{
  SciDoc*sci=(SciDoc*)page->getFirst();
  sci->ShowWhiteSpace((bool)user_data);
  return true;
}



bool TabCallbacks::ShowMarginCB(FXint index, DocTab*tab, FXWindow*page, void*user_data)
{
  for (SciDoc*sci=(SciDoc*)page->getFirst(); sci; sci=(SciDoc*)sci->getNext()) {
    sci->SetShowEdge((bool)user_data);
  }
  return true;
}



bool TabCallbacks::ShowIndentCB(FXint index, DocTab*tab, FXWindow*page, void*user_data)
{
  for (SciDoc*sci=(SciDoc*)page->getFirst(); sci; sci=(SciDoc*)sci->getNext()) {
    sci->SetShowIndent((bool)user_data);
  }
  return true;
}



bool TabCallbacks::ShowCaretLineCB(FXint index, DocTab*tab, FXWindow*page, void*user_data)
{
  Settings*p=(Settings*)user_data;
  for (SciDoc*sci=(SciDoc*)page->getFirst(); sci; sci=(SciDoc*)sci->getNext()) {
    sci->CaretLineBG(p->ShowCaretLine?p->CaretLineBG():NULL);
  }
  return true;
}



/* Mark all open documents as needing their settings updated. */
bool TabCallbacks::PrefsCB(FXint index, DocTab*tab, FXWindow*page, void*user_data)
{
  ((SciDoc*)page->getFirst())->NeedStyled(true);
  return true;
}



/*
  For each tab callback to add a chore to update the settings of the first
  document it finds that needs updating -
  CheckStyle() invokes this callback, and in turn, this callback adds a chore
  that invokes CheckStyle(). This exchange continues until all documents have
  had their settings updated.
*/
bool TabCallbacks::StyleNextDocCB(FXint index, DocTab*tab, FXWindow*page, void*user_data)
{
  SciDoc*sci=(SciDoc*)page->getFirst();
  if (sci->NeedStyled()) {
    TopWindow*tw=(TopWindow*)user_data;
    tw->getApp()->addChore(tw, TopWindow::ID_CHECK_STYLE, sci);
    return false;
  }
  return true;
}



/*
  ForEachTab callback that checks to see if a file is already open.
  The user_data is an array of 3 pointers: 
     { FXString*filename, bool*activate, SciDoc*sci=NULL }
  If we find the document, we set the NULL SciDoc* to the
  found document and (optionally) activate that tab.
  .
*/
bool TabCallbacks::FileAlreadyOpenCB(FXint index, DocTab*tab, FXWindow*page, void*user_data)
{
  void**p=(void**)user_data;
  FXString*filename=(FXString*)p[0];
  bool activate=*((bool*)p[1]);
  SciDoc*sci=(SciDoc*)page->getFirst();
  if (FX::compare(sci->Filename(),*filename)==0) {
    DocTabs*tabbook=(DocTabs*)tab->getParent();
    if (activate) { tabbook->ActivateTab(index); }
    p[2]=sci;
    return false;
  }
  return true;
}



/*
  ForEachTab callback that checks to see if a file is still open
  If we find the document, then we set the SciDoc** pointed to
  by the user_data to NULL as a sign that we found it.
*/
bool TabCallbacks::FileStillOpenCB(FXint index, DocTab*tab, FXWindow*page, void*user_data)
{
  SciDoc**sci=(SciDoc**)user_data;
  if (*sci==page->getFirst()) {
    *sci=NULL;
    return false;
  }
  return true;
}



// In case a Lua script left the SCI_*UNDOACTION level in an unbalanced state.
bool TabCallbacks::ResetUndoLevelCB(FXint index, DocTab*tab, FXWindow*page, void*user_data)
{
  ((SciDoc*)(page->getFirst()))->SetUserUndoLevel(0);
  return true;
}



bool TabCallbacks::BookmarkCB(FXint index, DocTab*tab, FXWindow*page, void*user_data)
{
  TopWindow*tw=(TopWindow*)user_data;
  return !tw->FoundBookmarkedTab(tab);
}



/* Return true if the document is still open */
bool TabCallbacks::IsDocValid(SciDoc*sci, DocTabs*tabbook)
{
  if (sci) {
    SciDoc*closed=sci;
    tabbook->ForEachTab(FileStillOpenCB,&closed);
    return (!closed);
  } else {
    return false;
  }
}



bool TabCallbacks::NamedFilesCB(FXint index, DocTab*tab, FXWindow*page, void*user_data)
{
  const FXString filename=((SciDoc*)(page->getFirst()))->Filename();
  if (!filename.empty()) {
    FXString*p=(FXString*)user_data;
    while (!p->empty()) { p++; }
    *p=filename;
  }
  return true;
}
