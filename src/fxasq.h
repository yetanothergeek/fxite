/*
  fxasq.h - Header file for fxasq.cpp
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


#include "fxasq_c.h"


class FxAsqWin: public FXObject {
  private:
    friend class ActionBox;

    FxAsqItem*Results;
    FXDialogBox*dlg;
    FXHorizontalFrame *btnbox;
    void TextBox(const char*key, const char*value, const char*label, bool secret);
    void AppendResult(const char*key, const char*value);
    void PutResults();
  protected:
    FXDict keylist;
    FXVerticalFrame *userbox;
  public:

    FxAsqWin(const char*title, const char **buttons);
    ~FxAsqWin();
    int Run(FxAsqItem**results=NULL);

    void HR();
    void Label(    const char*text);
    void CheckBox( const char*key, const bool value, const char*label);
    void Text(     const char*key, const char*value, const char*label);
    void Password( const char*key, const char*value, const char*label);
    void Select(   const char*key, const char*value, const char*label);
    void Option(   const char*key, const char*value, const char*label);
    void Group(    const char*key, const char*value, const char*label);
    void Radio(    const char*key, const char*value, const char*label);
    void TextArea( const char*key, const char*value, const char*label);
    void File(     const char*key, const char*value, const char*label);
    void Color(    const char*key, const char*value, const char*label);
    void Font(     const char*key, const char*value, const char*label);
};

