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

#include "appwin.h"
#include "appwin_pub.h"

#define TW TopWindow

static TopWindow* tw = NULL;


bool TopWinPub::OpenFile(const char*caption, const char*rowcol, bool readonly, bool hooked)
{
  return tw->OpenFile(caption, rowcol, readonly, hooked);
}



bool TopWinPub::CloseFile(bool close_last, bool hooked)
{
  return tw->CloseFile(close_last, hooked);
}



SciDoc* TopWinPub::ControlDoc()
{
  return tw->ControlDoc();
}



SciDoc* TopWinPub::FocusedDoc()
{
  return tw->FocusedDoc();
}



void TopWinPub::ActiveWidget(FXID aw)
{
  tw->ActiveWidget(aw);
}



void TopWinPub::ShowOutputPane(bool showit)
{
  tw->ShowOutputPane(showit);
}



FXSelector TopWinPub::KillCmdID()
{
  return TW::ID_KILL_COMMAND;
}



FXSelector TopWinPub::LastID()
{
  return TW::ID_LAST;
}



void TopWinPub::SetKillCommandAccelKey(FXHotKey acckey)
{
  tw->SetKillCommandAccelKey(acckey);
}



UserMenu** TopWinPub::UserMenus()
{
  return tw->UserMenus();
}



void TopWinPub::SaveClipboard()
{
  tw->SaveClipboard();
}



const FXString& TopWinPub::Connector()
{
  return TW::Connector();
}


const FXString& TopWinPub::ConfigDir()
{
  return TW::ConfigDir();
}


void TopWinPub::ParseCommands(FXString &commands)
{
  tw->ParseCommands(commands);
}



FXMainWindow* TopWinPub::instantiate(FXApp*a)
{
  tw=new TopWindow(a);
  return tw;
}



void TopWinPub::create()
{
  tw->create();
}



FXbool TopWinPub::close(FXbool notify)
{
  return tw->close(notify);
}




// Prints the names of the compiled-in lexers to stdout,
// along with the number of word lists per lexer
// ( Only used during development, to help with the
//   syntax-highlighting interface. )

class WordList;
class Accessor;

#include <SciLexer.h>
#include <ILexer.h>
#include <LexerModule.h>
#include <Catalogue.h>

void TopWinPub::DumpLexers() {
  for (int i=0; i<=SCLEX_AUTOMATIC; i++) {
    const LexerModule*lex=Catalogue::Find(i);
    if (lex) {
      fprintf(stdout, "%d %s %d\n", i, lex->languageName, lex->GetNumWordLists());
    }
  }
}



void TopWinPub::FindText(const char*searchfor, FXuint searchmode, bool forward)
{
  tw->FindText(searchfor,searchmode,forward);
}



void TopWinPub::FindAndReplace(const char*searchfor, const char*replacewith, FXuint searchmode, bool forward)
{
  tw->FindAndReplace(searchfor,replacewith,searchmode,forward);
}



void TopWinPub::ReplaceAllInSelection(const char*searchstring, const char*replacewith,  FXuint searchmode)
{
  tw->ReplaceAllInSelection(searchstring,replacewith,searchmode);
}



void TopWinPub::ReplaceAllInDocument(const char*searchstring, const char*replacewith,  FXuint searchmode)
{
  tw->ReplaceAllInDocument(searchstring,replacewith,searchmode);
}



void TopWinPub::Paste()
{
  tw->Paste();
}



void TopWinPub::AdjustIndent(SciDoc*sci,char ch)
{
  tw->AdjustIndent(sci,ch);
}


DocTabs* TopWinPub::Tabs()
{
  return tw->Tabs();
}



FileDialogs* TopWinPub::FileDlgs()
{
  return tw->FileDlgs();
}



void TopWinPub::update()
{
  tw->update();
}



bool TopWinPub::IsMacroCancelled()
{
  return tw->IsMacroCancelled();
}



bool TopWinPub::Closing()
{
  return tw->Closing();
}



bool TopWinPub::Destroying()
{
  return tw->Destroying();
}



void TopWinPub::CloseWait()
{
  tw->getApp()->addChore(tw,TW::ID_CLOSEWAIT,NULL);
}


bool TopWinPub::SetReadOnly(SciDoc*sci, bool rdonly)
{
  return tw->SetReadOnly(sci,rdonly);
}



void TopWinPub::SetWordWrap(SciDoc*sci, bool wrapped)
{
  tw->SetWordWrap(sci,wrapped);
}



FXMainWindow* TopWinPub::instance()
{
  return TW::instance();
}



FXMenuCaption* TopWinPub::TagFiles()
{
  return tw->TagFiles();
}



void TopWinPub::AddOutput(const FXString&line)
{
  tw->AddOutput(line);
}


int TopWinPub::IsFileOpen(const FXString &filename, bool activate)
{
  return tw->IsFileOpen(filename,activate);
}



FXString* TopWinPub::NamedFiles()
{
  return tw->NamedFiles();
}

