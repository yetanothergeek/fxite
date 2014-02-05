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


class TagHandler: public FXObject {
public:
  static bool FindTag(SciDoc*sci, FXMenuCaption*tagfiles, FXString &outfile, FXString &outcoords, FXString &outpat);
  static void GoToTag(SciDoc*sci, FXString &pattern);
  static void ShowCallTip(SciDoc*sci, FXMenuCaption*tagfiles);
};



class AutoCompleter:public Dictionary {
private:
  void Show(SciDoc*sci);
  void Parse(char startchar, const char*filename);
public:
  void Start(SciDoc*sci, FXMenuCaption*tagfiles);
  bool Continue(SciDoc*sci);
  ~AutoCompleter();
};

