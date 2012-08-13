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


class DocTab;

class TabCallbacks {
public:
  static bool ZoomStepCB(FXint index, DocTab*tab, FXWindow*page, void*user_data);
  static bool ZoomSpecCB(FXint index, DocTab*tab, FXWindow*page, void*user_data);
  static bool LineNumsCB(FXint index, DocTab*tab, FXWindow*page, void*user_data);
  static bool WhiteSpaceCB(FXint index, DocTab*tab, FXWindow*page, void*user_data);
  static bool ShowMarginCB(FXint index, DocTab*tab, FXWindow*page, void*user_data);
  static bool ShowIndentCB(FXint index, DocTab*tab, FXWindow*page, void*user_data);
  static bool ShowCaretLineCB(FXint index, DocTab*tab, FXWindow*page, void*user_data);
  static bool FileAlreadyOpenCB(FXint index, DocTab*tab, FXWindow*page, void*user_data);
  static bool ResetUndoLevelCB(FXint index, DocTab*tab, FXWindow*page, void*user_data);
  static bool PrefsCB(FXint index, DocTab*tab, FXWindow*page, void*user_data);
  static bool AutoSaveCB(FXint index, DocTab*tab, FXWindow*page, void*user_data);
  static bool BookmarkCB(FXint index, DocTab*tab, FXWindow*page, void*user_data);
  static bool FileStillOpenCB(FXint index, DocTab*tab, FXWindow*page, void*user_data);
  static bool StyleNextDocCB(FXint index, DocTab*tab, FXWindow*page, void*user_data);
};
