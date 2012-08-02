/*
  fxasq_c.h - Plain "C" API for fxasq.cpp
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

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _FXasqItem {
  char* key;
  char* value;
  struct _FXasqItem*next;
} FxAsqItem;


typedef struct _FXasqDlg FxAsqDlg;

typedef const char* FxAsqStr;


void fxasq_free_results(FxAsqItem*results);

FxAsqDlg* fxasq_new(const char*title, const char **buttons);
FxAsqItem* fxasq_run(FxAsqDlg*dlg, int*btn, void *user_data);
void fxasq_free(FxAsqDlg*dlg);

void fxasq_text(     FxAsqDlg *dlg, FxAsqStr key, FxAsqStr value, FxAsqStr label);
void fxasq_password( FxAsqDlg *dlg, FxAsqStr key, FxAsqStr value, FxAsqStr label);

void fxasq_textarea( FxAsqDlg *dlg, FxAsqStr key, FxAsqStr value, FxAsqStr label);
void fxasq_file(     FxAsqDlg *dlg, FxAsqStr key, FxAsqStr value, FxAsqStr label);
void fxasq_color(    FxAsqDlg *dlg, FxAsqStr key, FxAsqStr value, FxAsqStr label);
void fxasq_font(     FxAsqDlg *dlg, FxAsqStr key, FxAsqStr value, FxAsqStr label);

void fxasq_group(    FxAsqDlg *dlg, FxAsqStr key, FxAsqStr value, FxAsqStr label);
void fxasq_radio(    FxAsqDlg *dlg, FxAsqStr key, FxAsqStr value, FxAsqStr label);
void fxasq_select(   FxAsqDlg *dlg, FxAsqStr key, FxAsqStr value, FxAsqStr label);
void fxasq_option(   FxAsqDlg *dlg, FxAsqStr key, FxAsqStr value, FxAsqStr label);

void fxasq_checkbox( FxAsqDlg *dlg, FxAsqStr key, int value, FxAsqStr label);

void fxasq_label(    FxAsqDlg *dlg, FxAsqStr text);
void fxasq_heading(  FxAsqDlg *dlg, FxAsqStr text);
void fxasq_hr(       FxAsqDlg *dlg);

#ifdef __cplusplus
}
#endif

