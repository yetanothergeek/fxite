/*
  FXiTe - The Free eXtensIble Text Editor
  Copyright (c) 2009 Jeffrey Pohlmeyer <yetanothergeek@gmail.com>

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


class SciDoc;

#define MAX_CAPTURES 9


class SciSearch {
private:
  FXint begs[MAX_CAPTURES],ends[MAX_CAPTURES];
  const char*content;
  SciDoc*sci;
  void SelectTarget(bool forward);
  void EnsureAnchorDirection(bool forward);
  FXRexError rxerr;
  bool CheckRegex(const FXRex &rx, bool quiet=false);
public:
  SciSearch(SciDoc*scidoc) { sci=scidoc; }
  int FindTextNoSel(const FXString &what, FXuint sciflags, long &beg, long &end);
  bool FindText(const FXString &what, FXuint sciflags, bool forward, bool wrap);
  void ReplaceSelection(const FXString &replacewith, FXuint opts);
  long ReplaceAllInDoc(const FXString &searchfor, const FXString &replacewith, FXuint opts);
  long ReplaceAllInSel(const FXString &searchfor, const FXString &replacewith, FXuint opts);
};

