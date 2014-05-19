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

#ifndef FXITE_SCIDOC_UTIL_H
#define FXITE_SCIDOC_UTIL_H


class SciDoc;
class Settings;

class SciDocUtils {
public:
  static void CharAdded(SciDoc*sci, long line, long pos, int ch, Settings*prefs, SciDoc*recording);
  static void AdjustIndent(SciDoc*sci, char ch, Settings*prefs, SciDoc*recording);
  static void OpenSelected(FXMainWindow*tw, SciDoc*sci);
  static bool InsertFile(SciDoc *sci, const FXString &filename);
  static void SetSciDocPrefs(SciDoc*sci, Settings*prefs);
  static void CycleSplitter(SciDoc*sci, Settings*prefs);
  static void Cut(SciDoc*sci);
  static void Copy(SciDoc*sci);
  static void Paste(SciDoc*sci);
  static void Indent(SciDoc*sci, bool forward, bool single_space, int indent_width);
  static SciDoc* NewSci(FXComposite*p, FXObject*trg, Settings*prefs);
  static void DoneSci(SciDoc*sci, SciDoc*recording);
  static void SetScintillaSelector(FXSelector sel);
  static void SetMacroRecordSelector(FXSelector sel);
  static FXString Filename(SciDoc*sci);
  static bool SaveToFile(SciDoc*sci, const char*filename, bool as_itself=true);
  static bool Reload(SciDoc*sci);
  static bool Dirty(SciDoc*sci);
  static void SetFocus(SciDoc*sci);
  static void KillFocus(SciDoc*sci);
  static FXival ID(SciDoc*sci);
  static void NeedBackup(SciDoc*sci, bool need);
  static void CopyText(SciDoc*sci, const FXString &txt);
  static const FXString GetLastError(SciDoc*sci);
};

#endif

