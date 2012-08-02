/*
  fxasq_c.cpp - Plain "C" wrapper for fxasq.cpp
  Copyright (c) 2007-2009 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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

#include "fxasq.h"

void fxasq_free_results(FxAsqItem*results)
{
  FxAsqItem*p1=results;
  FxAsqItem*p2=NULL;
  while (p1) {
    if (p1->key) { free(p1->key); }
    if (p1->value) { free(p1->value); }
    p2=p1->next;
    free(p1);
    p1=p2;
  }
}




FxAsqDlg* fxasq_new(const char*title, const char **buttons)
{
  return (FxAsqDlg*)(new FxAsqWin(title,buttons));
}


#define Dlg ((FxAsqWin*)(dlg))

FxAsqItem* fxasq_run(FxAsqDlg*dlg, int*btn, void *user_data)
{
  FxAsqItem*results=NULL;
  int rv=Dlg->Run(&results);
  if (btn) { *btn=rv; }
  return results;
}



void fxasq_free(FxAsqDlg*dlg)
{
  delete Dlg;
}



void fxasq_text(FxAsqDlg *dlg, FxAsqStr key, FxAsqStr value, FxAsqStr label)
{
  Dlg->Text(key,value,label);
}



void fxasq_password(FxAsqDlg *dlg, FxAsqStr key, FxAsqStr value, FxAsqStr label)
{
  Dlg->Password(key,value,label);
}



void fxasq_textarea(FxAsqDlg *dlg, FxAsqStr key, FxAsqStr value, FxAsqStr label)
{
  Dlg->TextArea(key,value,label);
}



void fxasq_file(FxAsqDlg *dlg, FxAsqStr key, FxAsqStr value, FxAsqStr label)
{
  Dlg->File(key,value,label);
}



void fxasq_color(FxAsqDlg *dlg, FxAsqStr key, FxAsqStr value, FxAsqStr label)
{
  Dlg->Color(key,value,label);
}



void fxasq_font(FxAsqDlg *dlg, FxAsqStr key, FxAsqStr value, FxAsqStr label)
{
  Dlg->Font(key,value,label);
}




void fxasq_group(FxAsqDlg *dlg, FxAsqStr key, FxAsqStr value, FxAsqStr label)
{
  Dlg->Group(key,value,label);
}



void fxasq_radio(FxAsqDlg *dlg, FxAsqStr key, FxAsqStr value, FxAsqStr label)
{
  Dlg->Radio(key,value,label);
}



void fxasq_select(FxAsqDlg *dlg, FxAsqStr key, FxAsqStr value, FxAsqStr label)
{
  Dlg->Select(key,value,label);
}



void fxasq_option(FxAsqDlg *dlg, FxAsqStr key, FxAsqStr value, FxAsqStr label)
{
  Dlg->Option(key,value,label);
}




void fxasq_checkbox(FxAsqDlg *dlg, FxAsqStr key, int value, FxAsqStr label)
{
  Dlg->CheckBox(key,value,label);
}




void fxasq_label(FxAsqDlg *dlg, FxAsqStr text)
{
  Dlg->Label(text);
}



void fxasq_heading(FxAsqDlg *dlg, FxAsqStr text)
{
  Dlg->HR();
  Dlg->Label(text);
}



void fxasq_hr(FxAsqDlg *dlg)
{
  Dlg->HR();
}

